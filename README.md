Simple Simplex Implementation
=============================

This is a small project I written in summer 2014, after taking an operational research course at my university.

The program doesn't require a lot of documentation, just a little knowledge of the simplex method, to write the problems to solve in the correct form.

See the example in the problems/ directory to start.

The program implements the *primal simplex* and *two-phase methods*, and the *dual simplex* method.

The code has been written to be clear and as a consolidation of the studied theory, so it is not super-optimized, but should be easy to modify. 

Usage
-----

To solve a problem:

```
./simplex -f problems/problem_file.txt
```

To run unit tests:

```
./simplex -t
```

Compile
-------

Just type:
```
make
```

The compiler g++ is the only requirement.

Should be easy to port to other platforms, if you replaces the calls to malloc() and calloc() with new and delete.

