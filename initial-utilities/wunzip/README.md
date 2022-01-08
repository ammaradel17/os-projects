### UNZIP Project

This repository holds the source code and a set of test cases for a program named `unzip` that simulates the Unix `unzip` command logically

> Description

The `unzip`  provides decompression for classic zip formats. Includes supports for Unicode and encryption if appropriate USE flags have been selected.

unzip will list, test, or extract files from a ZIP archive, commonly found on MS-DOS systems. The default behavior (with no options) is to extract into the current directory (and subdirectories below it) all files from the specified ZIP archive. A companion program, zip(1L), creates ZIP archives; both programs are compatible with archives created by PKWARE's PKZIP and PKUNZIP for MS-DOS, but in many cases the program options or default behaviors differ.

Because The type of compression used in ZIP Project   is **Run-length encoding (RLE)**\
we will decompose files based on this type of encoding


For Example : \
if we have a file containing the text\
 `7h2n1m2i1w`\
the unzip should turn it into\
 `hhhhhhhnnmiiw` 

In order to decompose a string of data, you need to loop through each character of the data and print that character for its number of occurrences. 

> Syntax

Before going into how to use the `wunzip` command, let’s start by reviewing the basic code syntax.

```c
$ unzip   files_list > any_file_format
```



> Usage

Firstly we compile the code file as follows

``` bash
prompt> gcc -o wunzip wunzip.c -Wall -Werror 
```

this will produce a binary file named `wunzip` which we will use.

To run the tests we firstly have to give a running permission for the bash script `test-wunzip.sh`

```bash
prompt> chmod u+x ./test-wunzip.sh
prompt> ./test-wunzip.sh
```

the output will be in the following form

```bash
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed
```

The wunzip utility expression takes the following form:

```sh
./wunzip [FILES_NAME] > [any file format]
```
you should use the UNIX redirection sign `>` to redirect the output to any file format you want

