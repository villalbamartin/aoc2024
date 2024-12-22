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

Current status
--------------
The eternal problem with AoC is that I barely have time to get a working
solution, much less a "proper" one. This table details what's missing from
every submission:

  * **Part 1/2:** whether the program solves this part
  * **Valgrind:** whether the program passes Valgrind memory check. A * indicates
    that the "still reachable" leak is non-zero. This is a known issue with
    GLib.
  * **Comments:** whether all functions are properly commented
  * **GLib structures:** whether the code uses as many data structures from GLib
    as possible instead of custom-defined linked structures.

| Day    | Part 1   | Part 2  | Valgrind | Comments | GLib structures |
|--------|----------|---------|----------|----------|-----------------|
| Day 1  | Yes      | Yes     | Yes*     | Yes      | Yes             |
| Day 2  | Yes      | Yes     | Yes      | Yes      | Yes             |
| Day 3  | Yes      | Yes     | Yes      | Yes!     | Yes             |
| Day 4  | Yes      | Yes     | Yes      | Yes      | Yes             |
| Day 5  | Yes      | Yes     | Yes*     | Kinda    |                 |
| Day 6  | Yes      | Yes     |          |          |                 |
| Day 7  | Yes      | Yes     |          |          |                 |
| Day 8  | Yes      | Yes     |          |          |                 |
| Day 9  | Yes      | Yes     | Yes*     | Yes      | Yes             |
| Day 10 | Yes      | Yes     |          |          |                 |
| Day 11 | Yes      | Yes     |          |          |                 |
| Day 12 | Yes      |         |          |          |                 |
| Day 13 | Yes      |         |          |          |                 |
| Day 14 | Yes      | Yes     |          |          |                 |


References
----------
GLib reference: 
https://docs.gtk.org/glib/

IBM intro to GLib:
https://developer.ibm.com/tutorials/l-glib/#sorting-with-gcomparefunc21

Valgrind:
https://valgrind.org/docs/manual/quick-start.html#quick-start.prepare
