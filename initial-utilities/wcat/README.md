### Cat Project

This repository holds the source code and a set of test cases for a program named `wcat` that simulates the Unix `cat` command logically

> Description

Cat is a standard Unix utility that reads files sequentially, writing them to standard output. The name is derived from its function to concatenate files. It has been ported to a number of operating systems..

Cat can be used to pipe a file to a program that expects plain text or binary data on its input stream. cat does not destroy non-text bytes when concatenating and outputting. As such, its two main use cases are text files and certain format-compatible types of binary files.

Concatenation of text is limited to text files using the same legacy encoding, such as ASCII. cat does not provide a way to concatenate Unicode text files that have a Byte Order Mark or files using different text encodings from each other.

in this project we used `fopen()`, `fgets()`, and `fclose()` to open a file then read from it and close it.


To print out file contents, we used `printf()`. For example, after reading in a line with `fgets()` into a variable buffer, we printed out the buffer as follows:
```c
printf("%s", buffer);
```


> Syntax

Before going into how to use the `wcat` command, let’s start by reviewing the basic code syntax.

```c
$ cat   files_list 
```



> Usage

Firstly we compile the code file as follows

``` bash
prompt> gcc -o wcat wcat.c -Wall -Werror 
```

this will produce a binary file named `wcat` which we will use.

To run the tests we firstly have to give a running permission for the bash script `test-wcat.sh`

```bash
prompt> chmod u+x ./test-wcat.sh
prompt> ./test-wcat.sh
```

the output will be in the following form

```bash
test 1: passed
test 2: passed
test 3: passed
test 4: passed
test 5: passed
test 6: passed
test 7: passed
```

The wcat utility expression takes the following form:

```sh
./wcat [FILES_NAME] 
```


