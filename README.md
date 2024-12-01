Advent of Code 2024
===================
Repository for the Advent of Code (AoC) 2024.

This year I decided to write it in "modern" C, using as many pre-existing
libraries as possible and using known tools to ensure that the code is robust.

Valgrind
--------
You can check for memory leaks with the command

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
