/* 
 * Simple symplex implementation.
 * Written in summer 2014,
 * after taking an operational rersearch course.
 *
 * Emanuele Acri - crossbower@gmail.com - 2014
 */

#ifndef TABLEAU_H
#define TABLEAU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "matrix.h"

class TableauException {
 public: int code;
};

class InvalidFormException : public TableauException {};
class UnlimitedException   : public TableauException {};
class ImpossibleException  : public TableauException {};

class Tableau : public Matrix {
  
 public:
  Tableau (int m, int n, double *buffer, int *basis_indices);
  virtual ~Tableau ();

  /* getters and setters */

  inline int basis_at(int i) {           // get the column of the i-th basis variable
    assert( i >= 0 && i < m() - 1 );
    return basis_indices[i];
  }

  inline int basis_at(int i, int col) {  // set the column of the i-th basis variable
    assert( i   >= 0 && i   < m() - 1  &&
	    col >= 0 && col < n() - 1  );
    basis_indices_set[i] = 1;
    return basis_indices[i] = col;
  }

  inline int basis_set_at(int i) {           // check if the i-th basis variable is set
    assert( i >= 0 && i < m() - 1 );
    return basis_indices_set[i];
  }

  /* delete row and columns */

  void delete_row    (int row);
  void delete_column (int col);

  /* tableau operations */

  void pivot (int row, int col); // pivot operation on the given element
  void canonicalize (); // put in canonical form using the basis indices

  /* other stuff... */

  virtual void print (); // pretty print the tableau
  virtual Tableau *clone (); // create a copy

  /* unit tests */
  static void test ();

 protected:
  int *basis_indices;
  int *basis_indices_set;

};

#endif
