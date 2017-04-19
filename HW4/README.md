<h1><center>File Lock</center></h1>
<br>

## Environment

x86_64, Fedora 25, Linux kernel 4.10.8-200

<br>

## Compiler

*   g++ (GCC) 6.3.1 20161221 (Red Hat 6.3.1-1)

<br>

## Compile options

*   gcc: `-Wall -Wextra -std=gnu11`

<br>

## About addGrade.c

1.  Handles SIGINT in order to close "Grade" properly.
2.  Input can have up to 80 characters in a single line.
3.  Can set lock-delay with an integer argument provided. e.g.`addGrade.exe 12` sets delay time to 12 seconds.

<br>

## makefile

*   `make`: Compiles all files if changed.
*   `make run`: Run addGrade.exe.
*   `make run-debug`: Run addGrade.exe with lock-delay for 7 seconds.
