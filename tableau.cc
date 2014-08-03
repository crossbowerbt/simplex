#include "tableau.h"

Tableau::Tableau (int m, int n, double *buffer, int *indices)
  : Matrix::Matrix(m, n, buffer)
{
  size_t size = (m - 1) * sizeof(*basis_indices);

  if (indices) {

    basis_indices = (int *) malloc(size);
    memcpy(basis_indices, indices, size);

    basis_indices_set = (int *) malloc(size);
    for (int i = 0; i < m - 1; i++)
      basis_indices_set[i] = 1;

  } else {
    basis_indices = (int *) calloc(m - 1, sizeof(*basis_indices));
    basis_indices_set = (int *) calloc(m - 1, sizeof(*basis_indices_set));
  }
}

Tableau::~Tableau ()
{
  free(basis_indices);
}

/* add/delete row and columns */

void Tableau::delete_row (int row)
{
  assert( row >= 0 && row <= m() - 1 );
  
  for (int i = row; i < m() - 1; i++) {
    swap_rows(i, i+1);

    if (i < m() - 2) basis_at(i, basis_at(i+1));
  }
  
  m(m() - 1);
}

void Tableau::delete_column (int col)
{
  assert( col >= 0 && col <= n() - 1 );
 
  for (int j = col; j < n() - 1; j++) {
    swap_columns(j, j+1);
  }

  double *tmp = (double *) malloc(m() * (n() - 1) * sizeof(*tmp));

  for (int i = 0; i < m(); i++)
    for (int j = 0; j < n() - 1; j++)
      tmp[i * (n() - 1) + j] = at(i, j);

  free(buffer);
  buffer = tmp;
  
  n(n() - 1);
}

/* tableau operations */

void Tableau::pivot (int row, int col)
{
  assert( row >= 0        &&  col >= 0       &&
	  row <  m() - 1  &&  col <  n() - 1 );

  double pivot = at(row, col);

  if (pivot != 1.0)
    scale_row(row, 1.0 / pivot); // scale the pivot to assume value 1.0
  
  /* nullify every element in the column that is not the pivot */

  for (int i = 0; i < m(); i++) {
    if (i == row) continue;

    double value = at(i, col);
    if (value == 0) continue;
    
    double multiplier = - 1.0 * value;
    add_premultiplied_row(row, multiplier, i); // nullify the element
  }
}

void Tableau::canonicalize ()
{
  for (int i = 0; i < m() - 1; i++) { /* only m - 1 basic variables
				       (skip the reduced costs row) */

    int j = basis_indices[i]; // column of the i-th basic variable
    pivot(i, j);
  }
}

/* other stuff... */

void Tableau::print ()
{
  Matrix::print();

  for (int i = 0; i < m() - 1; i++) {
    printf("index[%d] = %d", i, basis_indices[i]);
    
    if (basis_indices_set[i]) puts(" (set)");
    else puts(" (unset)");
  }

  putchar('\n');
}

Tableau *Tableau::clone ()
{
  return new Tableau(m(), n(), buffer, basis_indices);
}
