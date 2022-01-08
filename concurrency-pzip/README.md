### PZIP Project

This repository holds the source code and a set of test cases for a program named `pzip` that simulates the Unix `zip` command logically using `Threads`

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

Using `Threads` we first opened the files passed by the user  and mapped the files contents from the disk to the virtual memory using `mmap`then we put a page size content from the file into a shared `Queue Buffer ` after checking that the buffer isn’t full using a thread we called `Producer Thread `.
After done mapping we close the opened file. 

Then after putting the page size content into that buffer we start compressing it using a multible
`consumer Threads` given its number by the return int from `get_nprocs() `.
The `consumer Threads` first checks that the buffer isn’t empty and then get a page size content from it and calculate its address using a function we called `calculateOutputPosition() ` and start compress that page by looping for every  character and count its occurrences and occurrences will increase by one if the following character is the same like the current one .
Once you see a character that is different from the previous character, you will append the number of occurrences and the character to your encoding.The function we used for compression is ` RLECompress`.
After compression we check similarity for the last character in a page and the first character in the following page if they are similar we combine its occurences .

Last we print our compressed file and free the allocated memory .

We can redirect our printed compressed file to a `zip file`

We provided `mutual exclusion` to the `critical siction` in our code represented by `enqueuing and dequeuing from the shared buffer`. using `pthread_mutex_t lock ` lock  , `pthread_cond_t empty & pthread_cond_t fill` conditions variable that indicates the status of our buffer whether it is empty or full and the state variable `producer_isComplete` that indicate that the producer has done mapping. 
. .

> Syntax

Before going into how to use the `pzip` , let’s start by reviewing the basic code syntax.

```c
$ pzip   files_list > zipfile
```



> Usage

Firstly we compile the code file as follows

``` bash
prompt> gcc -o pzip pzip.c -pthread -Wall -Werror 
```

this will produce a binary file named `pzip` which we will use.

To run the tests we firstly have to give a running permission for the bash script `test-pzip.sh`

```bash
prompt> chmod u+x ./test-pzip.sh
prompt> ./test-pzip.sh
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

The pzip expression takes the following form:

```sh
./pzip [FILES_NAME] > [zipfile]
```
you should use the UNIX redirection sign `>` to redirect the output to the zipfile

