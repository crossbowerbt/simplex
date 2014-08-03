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

#include <ctype.h>

#include "simplex.h"
#include "dual.h"

char *pname;

enum solver_method {
  SIMPLEX,
  TWO_PHASE,
  DUAL
};

enum parser_phases {
  METHOD_TO_READ,
  TABLEAU_TO_READ,
  READING_TABLEAU,
  VARS_TO_READ,
  PARSING_COMPLETED
};

struct parsed_file {
  int method;
  Tableau *tableau;
};

void usage ()
{
  puts("Simple simplex implementation, written in summer 2014,");
  puts("after taking an operational research course.");
  puts("Emanuele Acri - crossbower@gmail.com - 2014");
  printf("\nusage:\n\t %s -t | -f file\n", pname);
}

int count_word_in_line (char *line)
{
  int count = 0;
  int in_word = 0;

  for (int i = 0; line[i]; i++) {

    if (isalnum(line[i]) && !in_word) {
      in_word = 1;
      count++;
    }

    else if (isspace(line[i])) {
      in_word = 0;
    }

  }

  return count;
}

struct parsed_file *parse_file (char *filename)
{
  FILE *fp = NULL;
  struct parsed_file *parsed = NULL;
  
  char buffer[1024 * 4];
  
  int phase = METHOD_TO_READ;
  
  int method = -1;

  int m = -1, n = -1;

  double *matrix = NULL;
  int matrix_pos = 0;

  int *indices = NULL;
  int indices_pos = 0;

  int line = 0;

  int count;

  fp = fopen(filename, "r");
  if (!fp) {
    fprintf(stderr, "%s: error opening file: %s\n", pname, filename);
    goto error_exit;
  }

  parsed = (struct parsed_file *) malloc(sizeof(*parsed));

  // read file, line per line
  while (fgets(buffer, 1024 * 4 - 1, fp)) {
    line++;

    int i = 0;
    while (isspace((int) buffer[i]) && buffer[i] != '\0') i++; // search the first non-space char

    if (buffer[i] == '\0' || buffer[i] == '#') { // skip empty lines and comment lines

      // this kind of line mark the end of the tableau rows
      if (phase == READING_TABLEAU) phase = VARS_TO_READ;

      continue;
    }

    switch (phase) {

    case METHOD_TO_READ: // first of all read the solver method to apply

      if (!strncmp(&buffer[i], "SIMPLEX", strlen("SIMPLEX")))
	method = SIMPLEX;
      else if (!strncmp(&buffer[i], "TWO_PHASE", strlen("TWO_PHASE")))
	method = TWO_PHASE;
      else if (!strncmp(&buffer[i], "DUAL", strlen("DUAL")))
	method = DUAL;
      else {
	fprintf(stderr, "%s: invalid format for the file: %s, unknown method, line: %d\n", pname, filename, line);
	goto error_exit;
      }

      phase = TABLEAU_TO_READ;

      break;

    case TABLEAU_TO_READ: // begin reading the tableau

      count = count_word_in_line(buffer);

      m = 0;
      n = count;
      matrix = (double *) calloc(count * count, sizeof(*matrix));
      matrix_pos = 0;

      phase = READING_TABLEAU;

      // fallthrough...

    case READING_TABLEAU: // read tableau rows

      m++;

      if (m > n) {
	fprintf(stderr, "%s: invalid format for the file: %s, matrix rows >= matrix columns, line: %d\n", pname, filename, line);
	goto error_exit;
      }

      count = count_word_in_line(buffer);

      if (count != n) {
	fprintf(stderr, "%s: invalid format for the file: %s, invalid number of elements in row, line: %d\n", pname, filename, line);
	goto error_exit;
      }

      while (count) {
	int r = sscanf(&buffer[i], "%lf", &matrix[matrix_pos]);
	matrix_pos++;

	if (r != 1) {
	  fprintf(stderr, "%s: invalid format for the file: %s, invalid element in tableau, line: %d\n", pname, filename, line);
	  goto error_exit;
	}

	while (!isspace((int) buffer[i]) && buffer[i] != '\0') i++; // go to next word
	while (isspace((int) buffer[i]) && buffer[i] != '\0') i++;
	
	count--;

	assert(buffer[i] != '\0' ||  !count);
      }

      break;

    case VARS_TO_READ: // read indices of the variables in basis

      indices = (int *) malloc((m - 1) * sizeof(*indices));

      count = count_word_in_line(buffer);

      if (count != m - 1) {
	fprintf(stderr, "%s: invalid format for the file: %s, invalid number of variables in basis, line: %d\n", pname, filename, line);
	goto error_exit;
      }

      while (count) {
	int r = sscanf(&buffer[i], "%d", &indices[indices_pos]);
	indices_pos++;
	
	if (r != 1 || indices[indices_pos - 1] < 0 || indices[indices_pos - 1] >= n) {
	  fprintf(stderr, "%s: invalid format for the file: %s, invalid variable in basis, line: %d\n", pname, filename, line);
	  goto error_exit;
	}

	while (!isspace((int) buffer[i]) && buffer[i] != '\0') i++; // go to next word
	while (isspace((int) buffer[i]) && buffer[i] != '\0') i++;

	count--;

	assert(buffer[i] != '\0' || !count);
      }

      phase = PARSING_COMPLETED;

      break;

    case PARSING_COMPLETED: // parsing completed...
      break;

    }

  }

  // prepare the tableau
  parsed = (parsed_file *) malloc(sizeof(*parsed));
  parsed->method = method;

  parsed->tableau = new Tableau(m, n, matrix, indices);

  puts("Initial Tableau:");
  parsed->tableau->print();

  fclose(fp);
  free(matrix);
  free(indices);

  return parsed;
  
 error_exit:
  if (parsed) free(parsed);
  if (fp) fclose(fp);
  if (matrix) free(matrix);
  if (indices) free(indices);
  return NULL;
}

int main (int argc, char *argv[])
{
  pname = argv[0];

  if (argc < 2) {
    usage();
    return 0;
  }

  if (!strcmp(argv[1], "-t")) { // execute tests
    Matrix::test();
    PrimalSimplex::test();
    DualSimplex::test();
  }

  if (argc == 3 && !strcmp(argv[1], "-f")) { // solve file
    struct parsed_file *parsed= parse_file(argv[2]);

    if (!parsed) {
      //fprintf(stderr, "%s: error parsing file: %s\n", pname, argv[2]);
      return 1;
    }

    double solution;

    try {

      switch (parsed->method) { // solve with the specified method
      case SIMPLEX:
	solution = PrimalSimplex::simplex(parsed->tableau);
	break;
      case TWO_PHASE:
	solution = PrimalSimplex::two_phase(parsed->tableau);
	break;
      case DUAL:
	solution = DualSimplex::simplex(parsed->tableau);
	break;
      default:
	solution = PrimalSimplex::two_phase(parsed->tableau);
	break;
    }

    } catch (TableauException *ex) {

      puts("\nNo solution found.");
      goto end;

    }

    puts("\nFinal Tableau:");
    parsed->tableau->print();
    printf("Solution value: %lf\n", solution);

  end:
    delete parsed->tableau;
    free(parsed);
  }

  return 0;
}
