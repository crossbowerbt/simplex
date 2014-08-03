/* 
 * Simple symplex implementation.
 * Written in summer 2014,
 * after taking an operational rersearch course.
 *
 * Emanuele Acri - crossbower@gmail.com - 2014
 */

#ifndef DUAL_SIMPLEX_H
#define DUAL_SIMPLEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tableau.h"

namespace DualSimplex {
  
  // public:

  /* Dual Simplex Method */
  double simplex (Tableau *tab);

  /* Unit tests */
  void test ();

  // private:

  /* Check if the tableau is in the correct form for the dual simplex method */
  int check_correct_form (Tableau *tab);

  /* Test the feasibility of the current solution */
  int test_feasibility (Tableau *tab);

  /* Select the entering row */
  int select_pivot_row (Tableau *tab);

  /* Test if the cost is plus infinity in the dual simplex */
  int test_unlimited (Tableau *tab, int entering_row);

  /* Select the entering column */
  int select_pivot_column (Tableau *tab, int i);

}

#endif
