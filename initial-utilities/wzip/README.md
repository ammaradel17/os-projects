### ZIP Project

This repository holds the source code and a set of test cases for a program named `wzip` that simulates the Unix `zip` command logically

> Description

The `zip`  is the most widely used archive file format that  supports lossless data compression. A ZIP file may contain one or more files or directories that may have been compressed.

A Zip file is a data container containing one or more compressed files or directories. Compressed (zipped) files take up less disk space and can be transferred from one to another machine more quickly than uncompressed files. Zip files can be easily extracted in Windows, macOS, and Linux using the utilities available for all operating systems.

The type of compression used here is a simple form of compression called   **Run-length encoding (RLE)**\
**Run-length encoding (RLE)**   is a basic form of data compression that converts consecutive identical values into a code consisting of the character and the number marking the length of the run. The more similar values there are, the more values can be compressed. The sequence of data is stored as a single value and count.


For Example : \
if we have a file containing the text\
 `hhhhhhhnnmiiw`\
the zip should turn it into\
 `7h2n1m2i1w` 

In order to encode a string of data, you need to loop through each character of the data and count the occurrences. Once you see a character that is different from the previous character, you will append the number of occurrences and the character to your encoding.

> Syntax

Before going into how to use the `wzip` command, let’s start by reviewing the basic code syntax.

```c
$ zip   files_list > zipfile
```



> Usage

Firstly we compile the code file as follows

``` bash
prompt> gcc -o wzip wzip.c -Wall -Werror 
```

this will produce a binary file named `wzip` which we will use.

To run the tests we firstly have to give a running permission for the bash script `test-wzip.sh`

```bash
prompt> chmod u+x ./test-wzip.sh
prompt> ./test-wzip.sh
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

The wzip utility expression takes the following form:

```sh
./wzip [FILES_NAME] > [zipfile]
```
you should use the UNIX redirection sign `>` to redirect the output to the zipfile

