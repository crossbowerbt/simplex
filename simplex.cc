#include "simplex.h"

/* Test the optimality of the current solution */
int PrimalSimplex::test_optimality (Tableau *tab)
{
  for (int j = 0; j < tab->n() - 1; j++) { /* n - 1 to exclude the last column containing
					      the cost of the current solution */
    
    if (tab->at(tab->m() - 1, j) < 0) return 0; // check if the j-th reduced cost is negative
  }

  // if no reduced cost is negative, the current solution is optimal
  return 1;
}

/* Select the entering column

   Uses Bland's rule, i.e. select the negative reduced cost having
   the smallest position (smallest subscript) in the vector
*/
int PrimalSimplex::select_entering_column (Tableau *tab)
{
  for (int j = 0; j < tab->n() - 1; j++) { /* n - 1 to exclude the last column containing
					      the cost of the current solution */

    if (tab->at(tab->m() - 1, j) < 0) return j; // return the first negative reduced cost
  }

  assert(0); // execution should not reach here
}

/* Test if the chosen next solution is unlimited */
int PrimalSimplex::test_unlimited (Tableau *tab, int entering_column)
{
  for (int i = 0; i < tab->m() - 1; i++) { // m - 1 to exclude the reduced costs row

    if (tab->at(i, entering_column) >= 0) return 0; /* check if the i-th component of
						       the entering column is positive */
  }

  // if no element of the entering column is positive, the problem is unlimited
  return 1;  
}

/* Select the exiting column

   Uses Bland's rule, i.e. select the smallest ratio, and,
   when multiple variables in base give the same ratio,
   select the one having the smallest subscript
   (a.k.a. the one associated with the smallest column position)
*/
int PrimalSimplex::select_exiting_column (Tableau *tab, int j)
{
  double min_ratio = 0;
  int min_ratio_position = -1;

  for (int i = 0; i < tab->m() - 1; i++) { /* m - 1 to exclude the reduced costs row */
    if (tab->at(i, j) <= 0) continue;

    double ratio = tab->at(i, tab->n() - 1) / tab->at(i, j);

    if (min_ratio_position == -1 ||
	ratio < min_ratio  ||
	(ratio == min_ratio && tab->basis_at(i) < tab->basis_at(min_ratio_position) ) ) {

      min_ratio = ratio;
      min_ratio_position = i;
    }
  }

  return min_ratio_position;
}

/* 
   Primal simplex, full-tableau implementation.
   
   A single iteration of the method:

   1) Start with a tableau associated to
      a basis B and basic feasible solution x

   2) Check if the reduced cost row is >= 0 for every element.
      If positive: the algorith terminates, with x as optimal solution;
      else: choose a negative reduced cost (using some criterion)

   3) Check if the column of the chosen reduced cost has all negative elements.
      If that is the case, the problem is unlimited (optimal cost = minus infinity)
      and the algorithm terminates.

   4) For every positive element of the reduced cost column, compute the ratio
      "component of x" / "corresponding element of the column".
      Select the smallest ratio and drive the corresponding variable (in the basis)
      out of the basis. The column of the selected reduced cost enter the basis.

   5) Normalize the entered column (standard pivot procedure). Goto 2.

 */
double PrimalSimplex::simplex (Tableau *tab)
{
  // step 1
  int i, j;
  // ... nothing to do ...

 step_2:
  if (test_optimality(tab)) {
    printf("Optimal solution found!\n");

    // extract cost from the tableau (the sign is inverted)
    double cost = - tab->at(tab->m() - 1, tab->n() - 1);
    return cost;
  }

  else {
    j = select_entering_column(tab);
    printf("Selected pivot: j = %d, ", j);
  }
  
  // step 3
  if (test_unlimited(tab, j)) {
    printf("The problem is unlimited!\n");
    throw new UnlimitedException();
  }
  
  // step 4
  i = select_exiting_column(tab, j);
  printf("i = %d\n", i);
  tab->basis_at(i, j);

  // step 5
  tab->pivot(i, j);

  tab->print();

  goto step_2;
}

/* Search variable already usable for the initial basis */
int PrimalSimplex::search_usable_variables (Tableau *tab)
{
  int found_indices = 0;

  for (int j = 0; j < tab->n() - 1; j++) { // n - 1 to skip the variables vector

    if (found_indices >= tab->m() - 1)
      break;

    /* 
       NOTE: this is only a small optimization, can be further refined
       to include variable that can enter in basis using some elementary
       row operations (respecting the constraint that the variables vector
       must remain positive).

       Here we only select columns that have a single positive element,
       and all other elements null.
    */

    int elem_row = -1;
    int positive_elements = 0;

    for (int i = 0; i < tab->m() - 1; i++) { // m - 1 to skip the reduced costs row
      
      if (tab->at(i, j) < 0) { // a negative element invalids the column
	positive_elements = 0; // reset
	break;
      }

      if (tab->at(i, j) > 0) {
	elem_row = i;
	positive_elements++;
      }
      
      if (positive_elements > 1) break;
    }

    if (positive_elements == 1) { // can be used as a variable in basis
      
      // check if in that row there are still no basis variable
      if (tab->basis_set_at(elem_row) == 0) {
	tab->basis_at(elem_row, j);
	found_indices++;
	
	double value = tab->at(elem_row, j);
	tab->scale_row(elem_row, 1.0 / value);
      }
      
    }

  }

  return found_indices;
}

/* Create artificial tableau, adding the artificial columns */
Tableau *PrimalSimplex::create_artificial_tableau (Tableau *orig_tab, int art_columns)
{
  
  // fill the artificial tableau

  Tableau *art_tab = new Tableau(orig_tab->m(),
				 orig_tab->n() + art_columns,
				 NULL, NULL);

  for (int i = 0; i < orig_tab->m() - 1; i++) // fill the basis indices
    if (orig_tab->basis_set_at(i)) art_tab->basis_at(i, orig_tab->basis_at(i));

                                                 // fill the matrix:
  for (int i = 0; i < orig_tab->m() - 1; i++)    // m - 1 to skip the reduced costs row
    for (int j = 0; j < orig_tab->n() - 1; j++)  // n - 1 to skip the artificial columns
      art_tab->at(i, j, orig_tab->at(i, j));
  
  for (int i = 0; i < orig_tab->m() - 1; i++) {        // m - 1 to skip the reduced costs row
    for (int j = orig_tab->n() - 1;
	 j < (orig_tab->n() - 1) + art_columns;        // only the artificial columns
	 j++) {
      
      if (art_tab->basis_set_at(i) == 0 && j - (orig_tab->n() - 1) == i) {
	
	art_tab->at(i, j, 1);
	art_tab->basis_at(i, j);
	
      }
    }
  }
  
  for (int j = orig_tab->n() - 1;    // set the cost function for artificial variables
       j < (orig_tab->n() - 1) + art_columns;
       j++)
    art_tab->at(orig_tab->m() - 1, j, 1);
  
                                               // set the values of the variables column 
  for (int i = 0; i < orig_tab->m() - 1; i++)  // m - 1 to skip the reduced costs row
    art_tab->at(i, (orig_tab->n() - 1) + art_columns,
		orig_tab->at(i, orig_tab->n() - 1));

  return art_tab;
}

/* 
   Two-phase simplex method.

   Phase I:

   1) By multiplying some of the rows by -1, change the problem
      so that b >= 0

   2) Introduce artificial variables (if necessary), canonicalize the artificial tableau,
      and apply the simplex method to the auxiliary problem, with the sum of
      the artificial variables as cost function.

   3) Three cases after solving the artificial problem:
   
   3.1) If the optimal cost is positive the problem is infeasible

   3.2) If the optimal cost is zero, and no artificial variables are
        in the final basis, the corresponding columns can be eliminated
	and a feasible basis for the original problem has been found

   3.3) If the optimal cost is zero, and an artificial variable is
        in basis, examine the elements of the row of its pivot element:
	
	3.3.1) If all the entries are zero the row is redundant
	       and can be eliminated

	3.3.2) If an entry is not zero, let that column enter the basis
	       using a pivot operation on that element

	3.3.3) goto 3

   Phase II:

   1) Let the final basis and tableau from phase I be the initial
      basis and tableau for phase II

   2) Compute the reduced costs for this initial basis, using the
      coefficients of the original problem (i.e. canonicalize the tableau)

   3) Apply the simplex method to the obtained tableau

 */
double PrimalSimplex::two_phase (Tableau *tab)
{
  /* Phase I */

  // step 1

  for (int i = 0; i < tab->m() - 1; i++) // m - 1 to skip the reduced costs row
    // all the variables must be positive
    if (tab->at(i, tab->n() - 1) < 0) tab->scale_row(i, -1.0);

  puts("\nafter step 1:");
  tab->print();

  // step 2

  // search variable already usable for the initial basis
  int found_indices = search_usable_variables(tab);

  puts("\nafter step 2 (already available variables):");
  tab->print();

  /* at this point some valid variables in base should have been selected:
     we introduce artificial variables only for the rows that still doesn't
     have a variable in basis (a pivot) */
    
  int art_columns = (tab->m() - 1) - found_indices;

  Tableau *art_tab = create_artificial_tableau(tab, art_columns);

  puts("\nthe artificial tableau:");
  art_tab->print();

  art_tab->canonicalize();

  puts("\ncanonicalized artificial tableau:");
  art_tab->print();
  putchar('\n');

  double cost = simplex(art_tab);

  puts("\nsolution to the artificial problem:");
  art_tab->print();
  
  // step 3

 step_3:
  
  if (cost > 0.0) { // case 3.1
    puts("The problem is impossible!");
    throw new ImpossibleException();
  }

  else {
    int art_var_row = -1;
  
    for (int i = 0; i < art_tab->m() - 1; i++) { // m - 1 to skip the reduced costs row
      if (art_tab->basis_at(i) >= tab->n()) { // search artificial variables in basis
	art_var_row = i;
	break;
      }
    }

    if (art_var_row == -1) { // case 3.2
      goto phase_II; // no artificial variables in basis
    }

    // check if the row is null

    int null_row = 1;
    int not_null_elem_column;

    for (int j = 0; j < tab->n() - 1; j++) { // only original variable columns
      if (art_tab->at(art_var_row, j) != 0) { // we need a positive "not-artificial" element
	null_row = 0;
	not_null_elem_column = j;
	break;
      }
    }

    if (null_row) { // case 3.3.1
      art_tab->delete_row(art_var_row);
      tab->delete_row(art_var_row);
    }

    else { // case 3.3.2
      art_tab->pivot(art_var_row, not_null_elem_column);
    }

    goto step_3;

  }

  // Phase II

 phase_II:

  // step 1

  puts("\ntableau, after phase I:");
  art_tab->print();

  /* use the obtained tableau, without the artificial columns,
     in the original problem */

  // copy the relevant rows into the original tableau
  for (int i = 0; i < tab->m() - 1; i++)
    for (int j = 0; j < tab->n(); j++)
      tab->at(i, j, art_tab->at(i, j));

  // set the found variables in basis
  for (int i = 0; i < tab->m() - 1; i++)
    tab->basis_at(i, art_tab->basis_at(i));

  delete art_tab;

  // step 2

  tab->canonicalize();

  puts("\nresulting tableau, canonicalized with the just found basis:");
  tab->print();
  putchar('\n');

  // step 3

  return simplex(tab);
}

/* Unit tests */
void PrimalSimplex::test()
{
  // simplex only

  double buffer[] = { 12,   8, 2, 0, /**/ 48,
		       6,  -4, 0, 2, /**/ 12,
		      /*--------------------*/
		      -1,  -1, 0, 0, /**/  0 };
  
  int indices[] = {2, 3};
  
  Tableau *tab  = new Tableau(3, 5, buffer, indices);

  puts("\nPrimal Simplex: original tableau 1:");
  tab->print();

  tab->canonicalize();

  puts("\nPrimal Simplex: canonicalized tableau 1:");
  tab->print();

  try {
    simplex(tab);
  } catch (TableauException *ex) {
    delete ex;
  }

  puts("\nPrimal Simplex: solved tableau 1 using only the simplex method:");
  tab->print();

  delete tab;

  // two-phase method only


  double buffer2[] = { 12,   8, 2, 0, /**/ 48,
		        6,  -4, 0, 2, /**/ 12,
		       18,   4, 2, 2, /**/ 60,
		      /*---------------------*/
		       -1,  -1, 0, 0, /**/  0 };

  Tableau *tab2 = new Tableau(4, 5, buffer2, NULL);

  puts("\nPrimal Simplex: original tableau 2:");
  tab2->print();

  try {
    two_phase(tab2);
  } catch (TableauException *ex) {
    delete ex;
  }

  puts("\nPrimal Simplex: solved tableau 2, using the two-phase method:");
  tab2->print();

  delete tab2;

  // impossible problem

  double buffer3[] = { 1, 2, 0, 1, /**/ -5,
		       0, 1, 2, 0, /**/  6,
		     /*-------------------*/
		       1, 0, 1, 0, /**/  0 };
  
  Tableau *tab3 = new Tableau(3, 5, buffer3, NULL);

  puts("\nPrimal Simplex: original tableau 3 (impossible):");
  tab3->print();

  try {
    two_phase(tab3);
  } catch (TableauException *ex) {
    delete ex;
  }

  puts("\nPrimal Simplex: final tableau 3 (impossible), after two-phase method:");
  tab3->print();

  delete tab3;
}
