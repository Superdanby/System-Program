<h1><center>Copy and Move</center></h1>
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

## About acp

1.  Creates a temporary file in the same directory of the original file with mkstemp.
2.  Supports file holes.

<br>

## makefile

*   `make`: Compiles acp.c.
*   `make debug`: Run acp and take the path of the first file in the current folder as input and copies the file to \[original name\]opt.
*   `make clean`: Removes acp.
*   `make auto` = `make && make debug && make clean`
