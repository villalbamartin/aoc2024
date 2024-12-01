Advent of Code 2024
===================
Repository for the Advent of Code (AoC) 2024.

This year I decided to write it in "modern" C, using as many pre-existing
libraries as possible and using known tools to ensure that the code is robust.

Running
-------
You can compile the code using the provided Makefile. You can use either
`make day1` to build the code for a certain day or `make all` to build all
code at once.

Clang-format
------------
You can manually format your file using `clang` and the following command:

```
clang-format -i <file>.c
```

Valgrind
--------
You can check for memory leaks with Valgrind and the command

```
valgrind --leak-check=yes ./day1
```

References
----------
GLib reference: 
https://docs.gtk.org/glib/

IBM intro to GLib:
https://developer.ibm.com/tutorials/l-glib/#sorting-with-gcomparefunc21

Valgrind:
https://valgrind.org/docs/manual/quick-start.html#quick-start.prepare
