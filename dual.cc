#include "dual.h"

/* Check if the tableau is in the correct form for the dual simplex method */
int DualSimplex::check_correct_form (Tableau *tab)
{
  for (int j = 0; j < tab->n() - 1; j++) { // skip the current cost
    if (tab->at(tab->m() - 1, j) < 0) { // found a negative reduced cost
      return 0;
    }
  }

  // if no reduced cost is negative, the tableau is in the correct for for the dual
  return 1;
}

/* Test the feasibility of the current solution */
int DualSimplex::test_feasibility (Tableau *tab)
{
  for (int i = 0; i < tab->m() - 1; i++) { /* m - 1 to exclude the reduced costs row */
    if (tab->at(i, tab->n() - 1) < 0) return 0; // check if the i-th variable is positive
  }

  // if no variable is negative, the current solution is feasible
  return 1;
}

/* Select the entering row

   Uses Bland's rule, i.e. select the negative variable
   having the smallest subscript
*/
int DualSimplex::select_pivot_row (Tableau *tab)
{
  int min_index = -1;
  int min_index_col = -1;

  for (int i = 0; i < tab->m() - 1; i++) { /* m - 1 to exclude the reduced costs row */
    if (tab->at(i, tab->n() - 1) < 0) {

      if (tab->basis_at(i) < min_index_col || min_index == -1) {
	min_index = i;
	min_index_col = tab->basis_at(i);
      }
      
    }
  }

  return min_index;
}

/* Test if the cost is plus infinity in the dual simplex */
int DualSimplex::test_unlimited (Tableau *tab, int entering_row)
{
  for (int j = 0; j < tab->n() - 1; j++) { // n - 1 to exclude the variable column
    if (tab->at(entering_row, j) < 0) return 0; /* check if the j-th component of
						   the entering row is negative */
  }

  // if no element of the entering row is negative, the problem is unlimited
  return 1;
}

/* Select the entering column

   Uses Bland's rule, i.e. select the smallest ratio, and,
   when multiple variables in base give the same ratio,
   select the one having the smallest subscript
   (a.k.a. the one associated with the smallest column position)
*/
int DualSimplex::select_pivot_column (Tableau *tab, int i)
{
  double min_ratio = 0;
  int min_ratio_position = -1;

  for (int j = 0; j < tab->n() - 1; j++) { /* n - 1 to exclude the variable row */
    if (tab->at(i, j) >= 0) continue;

    double ratio = tab->at(tab->m() - 1, j) / (- tab->at(i, j));

    if (min_ratio_position == -1 ||
	ratio < min_ratio) {

      min_ratio = ratio;
      min_ratio_position = j;
    }
  }

  return min_ratio_position;
}

/*
  Dual Simplex Method

  A single iteration of the method:

  1) Starts with a tableau associated with a basis B
     and with all reduced costs NOT negative.

  2) If the variable column contains only positive
     variables the solution is feasible, and thus optimal.
     Otherwise select a negative variable: its row is the
     new pivot row.

  3) If the pivot row contains only positive elements
     (excluding the variable) then the optimal cost is 
     plus infinity, and the algorithm terminates.

  4) Select the pivot column calculating the ratios
     between the reduced costs and the corresponding
     coefficients. The column having the mimimum ratio
     is chosen.

  5) Pivot operation on the individuated pivot.
     Go to 2.
*/

double DualSimplex::simplex (Tableau *tab)
{
  // step 1
  int i, j;

  if (!check_correct_form(tab)) {
    printf("Error: invalid tableau for dual simplex method: a reduced cost is negative\n");
    throw new InvalidFormException();
  }

 step_2:
  if (test_feasibility(tab)) {
    printf("Optimal solution found!\n");

    // extract cost from the tableau (the sign is inverted)
    double cost = - tab->at(tab->m() - 1, tab->n() - 1);

    return cost;
  }

  else {
    i = select_pivot_row(tab);
    printf("Selected pivot: i = %d, ", i);
  }
  
  // step 3
  if (test_unlimited(tab, i)) {
    printf("The problem is unlimited\n");
    throw new UnlimitedException();
  }
  
  // step 4
  j = select_pivot_column(tab, i);
  printf("j = %d\n", j);
  tab->basis_at(i, j);

  // step 5
  tab->pivot(i, j);

  goto step_2;
}

/* Unit tests */
void DualSimplex::test()
{
  double buffer[] = { -2, -2, -1, 1, 0, /**/ -6,
		      -1, -2, -3, 0, 1, /**/ -5,
		     /*--------------------------*/
		       3,  4,  5, 0, 0, /**/  0 };

  int indices[] = {3, 4};
  
  Tableau *tab = new Tableau(3, 6, buffer, indices);

  puts("\nDual Simplex: original tableau:");
  tab->print();
  
  try {
    simplex(tab);
  } catch (TableauException *ex) {
    delete ex;
  }

  puts("\nDual Simplex: final tableau, after the dual simplex method:");
  tab->print();

  delete tab;
}
