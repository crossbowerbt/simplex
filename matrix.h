/* 
 * Simple symplex implementation.
 * Written in summer 2014,
 * after taking an operational rersearch course.
 *
 * Emanuele Acri - crossbower@gmail.com - 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef MATRIX_H
#define MATRIX_H

class Matrix {

 public:
  Matrix (int m, int n, double *buffer);
  virtual ~Matrix ();

  /* getters and setters */

  inline int m ()      { return _m; };
  inline int n ()      { return _n; };

  inline double at (int i, int j) {             // get element at position
    assert ( i >= 0  &&  j >= 0  &&
	     i < _m  &&  j < _n );
    return buffer[i * _n + j];
  }

  inline double at (int i, int j, double val) { // set element at position
    assert ( i >= 0  &&  j >= 0  &&
	     i < _m  &&  j < _n );
    return buffer[i * _n + j] = val;
  }
 
  /* elementary row operations */

  void swap_rows    (int row1, int row2);
  void swap_columns (int col1, int col2);

  void scale_row    (int row, double k);
  void scale_column (int col, double k);

  void add_premultiplied_row    (int src, double k, int dst);
  void add_premultiplied_column (int src, double k, int dst);

  /* matrix operations */

  void invert ();
  Matrix *multiply_by (Matrix *mat);

  /* other stuff... */

  virtual void print (); // pretty print the matrix 
  virtual Matrix *clone (); // create a copy

  /* unit tests */
  static void test ();

 protected:
  int _m, _n;
  double *buffer;

  /* setters */

  inline int m (int v) { return _m = v; };
  inline int n (int v) { return _n = v; };

};

#endif
