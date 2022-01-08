#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wgrep: searchterm [file ...]\n");
        exit(1);
    }

    char *term = argv[1];

    // Read stdin
    if (argc == 2) {
        char buffer[1024];
        while(fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (strstr(buffer, term) != NULL) {
                printf("%s", buffer);
            }
        }
        return 0;
    }

    // Read files
    for (int i=2; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printf("wgrep: cannot open file\n");
            exit(1);
        }
        /*
         getline()  reads  an  entire  line  from  stream fp,  storing  the address of the buffer containing the text into
         lineptr.
         If *lineptr is set to NULL and *len is set 0 before the call, then getline() will allocate a buffer for  storing
         the line.
         In  either case, on a successful call, *lineptr and *len will be updated to reflect the buffer address and allo‐
         cated size respectively.
        */
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, fp) != -1) {
            if (strstr(line, term) != NULL) {
                printf("%s", line);
            }
        }
        free(line);
        fclose(fp);
    }
    return 0;
}