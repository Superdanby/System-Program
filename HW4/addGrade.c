#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#define Msize 100
static volatile int run = 1;
void sigint(int a)
{//handles ^C
    run=0;
    write(2,"^C caught\n", 10);
    write(2,"Press any key to quit.\n", 23);
    return;
}
int digit(int inp)
{//Counts the digit of inp
    int cnt=0;
    while(inp)
    {
        ++cnt;
        inp/=10;
    }
    return cnt;
}
char* tostring(int inp, int size)
{//Turns inp from integer to string
    char* opt=(char*)malloc(sizeof(char)*size);
    int cnt = 0;
    while(inp)
    {
        opt[cnt]=(char)inp%10+'0';
        inp/=10;
        ++cnt;
    }
    return opt;
}
int Read(int fd, int argc)
{//Prints file to screen
    char* inp;
    inp=(char*)malloc(Msize*sizeof(char));

    //Shows error when file is being written.
    flock(fd, LOCK_SH);
    write(1,"SHARE\n",6);//file locked: SHARE

    //Prints to standard output
    lseek(fd, 0, SEEK_SET);//Go to the head of the file.
    while(read(fd,inp,Msize))
    {
        write(1,inp,Msize);
    }

    //file unlocked
    if(argc>1)
        usleep(argc*1000000);
    flock(fd, LOCK_UN);
    write(1,"UNLOCK\n\n",8);

    return 0;
}
int Write(int fd, int argc)
{//Writes user input to file
    char* inp;
    inp=(char*)malloc(Msize*sizeof(char));
    read(0,inp,Msize);//Reads user input for at most Msize characters.
    int i=0;
    for(; i<Msize; i++)
        if(inp[i]=='\0')
            break;
    //Terminates if SIGINT received.
    if(!run)
        return 1;

    //Shows error when file in use.
    flock(fd, LOCK_EX);
    write(1,"EXCLUSIVE\n",10);//file locked: EXCLUSIVE

    write(fd, inp, i);//Writes to file.

    //file unlocked
    if(argc>1)
        usleep(argc*1000000);
    flock(fd, LOCK_UN);
    write(1,"UNLOCK\n\n",8);

    return 0;
}
int main(int argc, char* argv[]) {

    //Gets user defined delay time
    if(argc>1)
        argc=atol(argv[1]);

    int fd;
	fd = open ("Grade", O_RDWR| O_APPEND | O_CREAT, 0666);//Read, append, or create "Grade"

    //Insure Grade is opened.
    write(1,"filediscriptor = ", 17);
    int dig = digit(fd);
    write(1,tostring(fd,dig), dig);
    write(1," is opened\n", 11);

    //Catches ^C signal to close "Grade" properly.
    signal(SIGINT,sigint);

    char* mode = (char*)malloc(sizeof(char));
    // write(1,"Choose Mode: Read(r/R), Write(w/W)\n", 35);

    while(run)//Stops when ^C received.
    {
        write(1,"Choose Mode: Read(r/R), Write(w/W)\n", 35);
        read(0,mode,Msize);
        if(mode[0]=='w' || mode[0]=='W')
        {
            // while(run)
            Write(fd,argc);
            run=1;
            write(1,"Press Ctrl+C(again) to quit.\n",29);
        }
        else if(mode[0]=='r' || mode[0]=='R')
            Read(fd,argc);
    }
    close(fd);//Close "Grade"
	return 0;
}
