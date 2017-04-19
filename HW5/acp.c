#define _GNU_SOURCE

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

// #include <string.h>
#define BUF_SIZE 4096


int main(int argc, char** argv) {

    if(argc<3)
    {
        write(2,"Too few arguments!\n", 19);
        return EXIT_FAILURE;
    }
    int olength=0;// strlen(argv[1])
    while(argv[1][olength]!='\0')
    {
        ++olength;
    }

    // create a temporary file
    char* tmpStr = (char*)malloc(sizeof(char)*(olength+6));
    for(int i=0; i<olength; i++)
    {// strcpy
        tmpStr[i]=argv[1][i];
    }
    for(int i=0; i<6; i++)
    {// XXXXXX
        tmpStr[i+olength]='X';
    }
    tmpStr[olength+6]='\0';
	mkstemp(tmpStr);

    write(1,"The name of the temporary file is ",34);
    write(1,tmpStr,olength+6+1);
    write(1,"\n",1);

    // open both files
    int inp = open (argv[1], O_RDONLY);
    int tmpFile = open(tmpStr, O_WRONLY, 0660);
	if (tmpFile == -1 || inp == -1)
	{
        write(2,"error: ",7);
        return EXIT_FAILURE;
    }

    // resize tmpFile to nothing
    ftruncate(tmpFile, 0);

    // necessary variables when copying files
    ssize_t numIn, numOut;
    char buffer[BUF_SIZE];
    off_t begin=0, end=0;
    int fileSize, blockSize, pos=0;

    // get the size of the input file
    fileSize = lseek(inp, 0, SEEK_END);
    lseek(inp, 0, SEEK_SET);

    // copy
    while (1) {
        // find the start position of the next data block
        pos = lseek(inp, pos, SEEK_DATA);
        begin = pos;
        // find the start position of the next hole
        pos = lseek(inp, pos, SEEK_HOLE);
        end = pos;
        blockSize=end-begin;

        // goto the start position of the next data block
        lseek(inp, begin, SEEK_SET);
        lseek(tmpFile, begin, SEEK_SET);

        // copy data
        while((numIn = read (inp, buffer, BUF_SIZE)) > 0 && blockSize) {
            numOut = write (tmpFile, buffer, (ssize_t) numIn);
            if (numIn != numOut)
                write(2,"numIn != numOut\n", 16);
            blockSize-=numIn;
        }

        // stop when the end has reached
        if (lseek(tmpFile, 0, SEEK_CUR) == fileSize)
            break;
    }

    // confirm the temporary file is created
    write(1,"tmpFile: ",9);
    write(1,tmpStr,olength+6+1);
    write(1,"is created!\n",12);
    write(1,"press Enter to continue...\n", 27);
    char* check=(char*)malloc(sizeof(char));
    while(read(0,check,1))
        if(check[0]=='\n')
            break;

    // move the temporary file to the assigned place
    rename(tmpStr,argv[2]);
    // delete the temporary file
    remove(tmpStr);

    // finish
	return EXIT_SUCCESS;
}
