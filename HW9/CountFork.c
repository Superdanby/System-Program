#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char const *argv[]) {
    int cnt = 1, status = 0;
    pid_t p;
    pid_t pid = getpid();
    p = fork();
    waitpid(p, &status, WUNTRACED);
    if(p)
        cnt = WEXITSTATUS(status);
    p = fork();
    waitpid(p, &status, WUNTRACED);
    if(p)
        cnt = WEXITSTATUS(status);
    p = fork();
    waitpid(p, &status, WUNTRACED);
    if(p)
        cnt = WEXITSTATUS(status);
    p = fork();
    waitpid(p, &status, WUNTRACED);
    if(p)
        cnt = WEXITSTATUS(status);
    printf("pid: %d %d\n", getpid(), cnt);
    if(getpid() != pid)
        return cnt + 1;
    return 0;
}
