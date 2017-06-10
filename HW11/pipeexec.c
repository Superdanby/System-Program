#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
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
        execv("/bin/cat", argv);
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
            execv("/bin/wc",argv);
        }
    }
}
