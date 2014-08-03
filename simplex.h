/* 
 * Simple symplex implementation.
 * Written in summer 2014,
 * after taking an operational rersearch course.
 *
 * Emanuele Acri - crossbower@gmail.com - 2014
 */

#ifndef PRIMAL_SIMPLEX_H
#define PRIMAL_SIMPLEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tableau.h"

namespace PrimalSimplex {

  //public:

  /* Primal simplex, full-tableau implementation */
  double simplex (Tableau *tab);

  /* Two-Phase Method */
  double two_phase (Tableau *tab);

  /* Unit tests */
  void test ();

  //private:

  /* Test the optimality of the current solution */
  int test_optimality (Tableau *tab);

  /* Select the entering column */
  int select_entering_column (Tableau *tab);

  /* Test if the chosen next solution is unlimited */
  int test_unlimited (Tableau *tab, int entering_column);

  /* Select the exiting column */
  int select_exiting_column (Tableau *tab, int j);

  /* Search variable already usable for the initial basis */
  int search_usable_variables (Tableau *tab);

  /* Create artificial tableau, adding the artificial columns */
  Tableau *create_artificial_tableau (Tableau *orig_tab, int art_columns);

}

#endif
