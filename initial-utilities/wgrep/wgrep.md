### Grep Project

This repository holds the source code and a set of test cases for a program named `wgrep` that simulates the Unix `grep` command logically

> Description

grep is a command-line utility for searching plain-text data sets for lines that match a regular expression. Its name comes from the ed command g/re/p (globally search for a regular expression and print matching lines), which has the same effect.

The grep filter searches a file for a particular pattern of characters, and displays all lines that contain that pattern. The pattern that is searched in the file is referred to as the regular expression (grep stands for global search for regular expression and print out).

This tool looks through a file, line by line, trying to find a user-specified search term in the line. If a line has the word within it, the line is printed out, otherwise it is not.
so if we wanted to search for a word "there" in a file 1.txt
```c
prompt> ./wgrep there 1.txt
there is found in this line 

```


> Syntax

Before going into how to use the `wgrep` command, let’s start by reviewing the basic code syntax.

```c
$ grep word  file_name 
```



> Usage

Firstly we compile the code file as follows

``` bash
prompt> gcc -o wgrep wgrep.c -Wall -Werror 
```

this will produce a binary file named `wgrep` which we will use.

To run the tests we firstly have to give a running permission for the bash script `test-wgrep.sh`

```bash
prompt> chmod u+x ./test-wgrep.sh
prompt> ./test-wgrep.sh
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

The wgrep utility expression takes the following form:

```sh
./wgrep word file_name 
```


