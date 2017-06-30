#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {
    int pipefd[2];
    char* buf = (char*)malloc(sizeof(char)*200);
    FILE *in_stream;
    int ret;
    pipe(pipefd);
    ret = fork();
    if (ret==0) {	/*child 1*/
        printf("%s\n", argv[1]);
        close(1); dup(pipefd[1]);
        close(pipefd[1]); close(pipefd[0]);
        char* pathname = (char*)malloc(sizeof(char) * 256);
        strcat(pathname, (char*)"/bin/");
        strcat(pathname, argv[1]);
        // char** f = (char**)malloc(sizeof(char*));
        // f = &argv[1];
        // char** s = (char**)malloc(sizeof(char*));
        // char** s = &argv[2];
        execl(pathname, pathname, argv[2], (char*)NULL);
    }
    if (ret>0) {
        ret = fork();
        if (ret==0) {	/*child 2*/
            close(0); dup(pipefd[0]);
            close(pipefd[1]); close(pipefd[0]);
            in_stream=fdopen(0, "r");
            assert(in_stream != NULL);
            while(fgets(buf, 200, in_stream))
                printf("%s", buf);

            char* pathname2 = (char*)malloc(sizeof(char) * 256);
            strcat(pathname2, (char*)"/bin/");
            strcat(pathname2, argv[3]);
            // printf("%s%s\n", pathname2, argv[4]);
            execl(pathname2 ,pathname2, argv[4], argv[2], (char*) NULL);
        }
    }
}
