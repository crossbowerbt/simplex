#include "matrix.h"

Matrix::Matrix (int m, int n, double *buff)
  : _m(m), _n(n)
{
  size_t size = m * n * sizeof(*buffer);

  if (buff) {
    buffer = (double *) malloc(size);
    memcpy(buffer, buff, size);
  } else {
    buffer = (double *) calloc(m * n, sizeof(*buffer));
  }
}

Matrix::~Matrix ()
{
  free(buffer);
}

/* elementary row operations */

void Matrix::swap_rows (int row1, int row2)
{
  assert( row1 >= 0   &&  row2 >= 0    &&
	  row1 <  m() &&  row2 <  m()  );
  assert(row1 != row2);

  for (int j = 0; j < n(); j++) {
    double temp = at(row1, j);
    at(row1, j, at(row2, j));
    at(row2, j, temp);
  }
}

void Matrix::swap_columns (int col1, int col2)
{
  assert( col1 >= 0    && col2 >= 0    &&
	  col1 <  n()  && col2 <  n()  );
  assert(col1 != col2);

  for (int i = 0; i < m(); i++) {
    double temp = at(i, col1);
    at(i, col1, at(i, col2));
    at(i, col2, temp);
  }
}

void Matrix::scale_row (int row, double k)
{
  assert( row >= 0 && row < m() );

  for (int j = 0; j < n(); j++) {
    at(row, j, at(row, j) * k);
  }
}

void Matrix::scale_column (int col, double k)
{
  assert( col >= 0 && col < n() );

  for (int i = 0; i < m(); i++) {
    at(i, col, at(i, col) * k);
  }
}

void Matrix::add_premultiplied_row (int src, double k, int dst)
{
  assert( src >= 0    &&  dst >= 0    &&
	  src <  m()  &&  dst <  m()  );
  assert(src != dst);

  for (int j = 0; j < n(); j++) {
    at(dst, j, at(dst, j) + at(src, j) * k);
  }
}

void Matrix::add_premultiplied_column (int src, double k, int dst)
{
  assert( src >= 0    &&  dst >= 0    &&
	  src <  n()  &&  dst <  n()  );
  assert(src != dst);

  for (int i = 0; i < m(); i++) {
    at(i, dst, at(i, dst) + at(i, src) * k);
  }
}

/* matrix operations */

void Matrix::invert ()
{
  /*
    The strategy adopted here is a simple one:
   
    First we prepare an identity matrix of the same
    size of the input matrix, then we transform the
    input matrix in the identity, using only elementary
    row operations.
   
    Repeating the same operations on the identity matrix
    we created, transforms it in the inverse of
    the input matrix.
   */ 

  assert(m() == n());

  /* prepare an identity matrix */

  Matrix *identity = new Matrix(m(), n(), NULL);

  for (int j = 0; j < n(); j++) // ones on the diagonal
    identity->at(j, j, 1.0);

  /* for every column of the matrix... */

  for (int j = 0; j < n(); j++) {
    double pivot = 0.0;

    /* ...search a pivot */

    for (int i = j; i < m(); i++) {
      double value = at(i, j);

      if (value != 0.0) {
	if (j != i) { // move the pivot in the correct position
	  swap_rows(j, i);
	  identity->swap_rows(j, i);
	}
	
	pivot = value;
	break;
      }
    }

    /* no pivot in a column means that the matrix is not invertible */

    if (pivot == 0.0) {
      fprintf(stderr, "Error: tried to invert a singular matrix.");
      return; // fix this
    }

    scale_row(j, 1.0 / pivot); // scale the pivot to assume value 1.0
    identity->scale_row(j, 1.0 / pivot);

    /* nullify every element in the column that is not the pivot */

    for (int i = 0; i < m(); i++) {
      if (i == j) continue;

      double value = at(i, j);
      double multiplier = - 1.0 * value;

      add_premultiplied_row(j, multiplier, i); // nullify the element
      identity->add_premultiplied_row(j, multiplier, i);
    }
  }

  memcpy(buffer, identity->buffer, m() * n() * sizeof(*buffer));

  delete(identity);
}

Matrix *Matrix::multiply_by (Matrix *mat)
{
  /* 
    Naive matrix multiplication, with
    asymptotic cost of O(n^3), for square matrices.
   */

  assert(n() == mat->m());

  /* prepare result matrix */

  Matrix *result = new Matrix(m(), mat->n(), NULL);

  /* fill every element of the result matrix using a simple dot-product:
     the corresponding row of the first matrix "dot" the corresponding
     column of the second matrix */

  for (int i = 0; i < result->m(); i++) {
    for (int j = 0; j < result->n(); j++) {

      for (int z = 0; z < n(); z++) // dot-product
	result->at(i, j, result->at(i, j) + at(i, z) * mat->at(z, j));

    }
  }

  return result;
}

/* other stuff... */

void Matrix::print ()
{
  for (int i = 0; i < m(); i++) {

    for (int j = 0; j < n(); j++) {
      printf("%.5f ", at(i, j));
    }

    putchar('\n');
  }
}

Matrix *Matrix::clone ()
{
  return new Matrix(m(), n(), buffer);
}

/* unit tests */
void Matrix::test ()
{
  Matrix *m1, *m2, *m3;

  double b1[] = { 1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0 };

  double b2[] = { 0, 0, 3,
                  0, 3, 0,
                  3, 0, 0 };

  double b3[] = { 0, 0, 3,
                  0, 3, 0,
                  3, 0, 0 };
  
  m1 = new Matrix(3, 4, b1);
  m2 = new Matrix(3, 3, b2);
  m3 = new Matrix(3, 3, b3);

  puts("Matrix: Elementary row/column operations:");

  puts("\nMatrix: matrix 1:");
  m1->print();

  m1->swap_rows(0, 2);

  puts("\nMatrix: swap first and last rows:");
  m1->print();

  m1->swap_columns(0, 3);

  puts("\nMatrix: swap first and last columns:");
  m1->print();

  m1->add_premultiplied_row(2, 2.5, 0);

  puts("\nMatrix: add last row to first (mult. by 2.5):");
  m1->print();

  m1->add_premultiplied_column(3, 2.5, 0);

  puts("\nMatrix: add last column to first (mult. by 2.5):");
  m1->print();

  m1->scale_row(0, 0.5);

  puts("\nMatrix: scale first row by 0.5:");
  m1->print();

  m1->scale_column(0, 0.5);

  puts("\nMatrix: scale first column by 0.5:");
  m1->print();

  puts("\nMatrix: Matrix operations:");

  puts("\nMatrix: matrix 2:");
  m2->print();

  m2->invert();

  puts("\nMatrix: inverse:");
  m2->print();

  Matrix *m4 = m3->multiply_by(m2);

  puts("\nMatrix: original matrix multiplied by its inverse:");
  m4->print();

  delete m1;
  delete m2;
  delete m3;
  delete m4;
}
