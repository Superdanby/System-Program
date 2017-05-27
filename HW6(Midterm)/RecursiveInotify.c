#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>

#define BUF_LEN (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))
typedef struct {
    char isdir;
    char permis[10];
    int hlink;
    char* uname;
    char* gname;
    int size;
    char* lasttime;
    char* name;
} opt;

static volatile sig_atomic_t run = 1;
void sigint(int a)
{//handles ^C
    run=0;
    write(2,"\nSIGINT caught\n", 15);
    write(2,"Terminating...\n", 15);
    return;
}

int cmp(const void * A, const void * B)
{//compare function for qsort
    if(!strcmp((*(opt *) A).name, (char*)"."))
        return 0;
    else if (!strcmp((*(opt *) B).name, (char*)"."))
        return 1;
    if(!strcmp((*(opt *) A).name, (char*)".."))
        return 0;
    else if (!strcmp((*(opt *) B).name, (char*)".."))
        return 1;
    if((*(opt *)A).isdir == 'd' && (*(opt *)B).isdir == '-')
        return 1;
    int length = sizeof((*(opt *) A).name) < sizeof((*(opt *) B).name) ? sizeof((*(opt *) A).name) : sizeof((*(opt *) B).name);
    for(int i = 0; i < length; i++)
    {
        char a = tolower(((*(opt *) A).name)[i]);
        char b = tolower(((*(opt *) B).name)[i]);
        if(a < b)
            return 0;
        else if(a > b)
            return 1;
    }
    if(sizeof((*(opt *) A).name) > sizeof((*(opt *) B).name))
        return 1;
    return 0;
}

void getall(opt* data, char* pathname, struct stat* buf, int perm, struct dirent* ent, char* inp, int const cnt)
{//record data
    strcpy(pathname, inp);
    data[cnt].name = (char*)malloc(sizeof(char)*(ent->d_reclen + 2));
    strcpy(data[cnt].name, ent->d_name);
    strcat(pathname, data[cnt].name);
    stat(pathname, buf);

    perm = (buf->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    for(int i = 8; i >= 0; i--)
    {
        char fill;
        switch (i % 3) {
            case 0:
                fill = 'r';
                break;
            case 1:
                fill = 'w';
                break;
            case 2:
                fill = 'x';
                break;
        }
        if(perm & 1)
            data[cnt].permis[i] = fill;
        else
            data[cnt].permis[i] = '-';
        perm >>= 1;
    }
    data[cnt].permis[9] = '\0';

    data[cnt].hlink = buf->st_nlink;

    data[cnt].lasttime = (char*)malloc(sizeof(char)*strlen(ctime(&buf->st_atime)) + 2);
    strcpy(data[cnt].lasttime, ctime(&buf->st_atime));
    data[cnt].lasttime[strlen(data[cnt].lasttime)-1] = 0;

    if(S_ISDIR(buf->st_mode))
        data[cnt].isdir = 'd';
    else
        data[cnt].isdir = '-';

    data[cnt].uname = (char*)malloc(sizeof(char)*strlen(getpwuid(buf->st_uid)->pw_name) + 2);
    strcpy(data[cnt].uname, getpwuid(buf->st_uid)->pw_name);

    data[cnt].gname = (char*)malloc(sizeof(char)*strlen(getgrgid(buf->st_gid)->gr_name) + 2);
    strcpy(data[cnt].gname, getgrgid(buf->st_gid)->gr_name);

    data[cnt].size = (int)buf->st_size;

    return;
}

void rec(char* inp, int const ifd, char*** whod, int* const whodMax){//recursion
    printf("%s:\n", inp);
    DIR* dir;
	struct dirent* ent;
	char pathname[512];
	struct stat* buf = (struct stat*)malloc(sizeof(struct stat));
    assert(buf!=NULL);
	int perm;

    opt* data = (opt*)malloc(sizeof(opt));
    assert(data!=NULL);
    int cnt = 0;
    int Max = 1;

	dir = opendir(inp);
	ent = readdir(dir);
	while(ent != NULL) {
        if(cnt == Max)
        {
            Max <<= 1;
            data = (opt*)realloc(data, sizeof(opt)*Max);
        }

		getall(data, pathname, buf, perm, ent, inp, cnt);

        ent = readdir(dir);

        ++cnt;
	}
    free(buf);
    closedir(dir);

    qsort(data, cnt, sizeof(opt), cmp);

    int optcnt = 0;

    while(optcnt<cnt)
    {
        if(data[optcnt].isdir == 'd' && strcmp(data[optcnt].name, ".") && strcmp(data[optcnt].name, ".."))
        {
            if(optcnt)
                printf("\n\n");
            char* next = (char*)malloc(sizeof(char)*strlen(inp) + strlen(data[optcnt].name) + 2 + 1);
            strcpy(next, inp);
            strcat(next, data[optcnt].name);
            free(data[optcnt].name);
            free(data[optcnt].uname);
            free(data[optcnt].gname);
            free(data[optcnt].lasttime);
            strcat(next, "/");
            rec(next, ifd, whod, whodMax);
            free(next);

        }
        else
        {
            printf("%c%s  %s  %s %8d %s %s\n", data[optcnt].isdir, data[optcnt].permis, data[optcnt].uname, data[optcnt].gname, data[optcnt].size, data[optcnt].lasttime, data[optcnt].name);
            free(data[optcnt].name);
            free(data[optcnt].uname);
            free(data[optcnt].gname);
            free(data[optcnt].lasttime);
        }

        ++optcnt;
    }
    //├──
    free(data);
    
    int ret = inotify_add_watch(ifd, inp, IN_ALL_EVENTS);
    if (ret < 0) {
        errno = EBADF;
        perror("error");
        exit (-1);
    }

    if(ret == *whodMax)
    {
        (*whodMax)<<=1;
        char** newwhod = (char**)realloc((*whod), sizeof(char*)*(*whodMax));
        if(!newwhod)
        {
            perror("newwhod failed");
            exit(-1);
        }
        else
        (*whod) = newwhod;
    }

    assert(ret<(*whodMax));
    assert(strlen(inp)>0);
    (*whod)[ret] = (char*)malloc(sizeof(char)*strlen(inp) + 2);
    assert((*whod)[ret]!=NULL);
    strcpy((*whod)[ret], inp);

    return;
}

int inot(char*** whod, int ifd)
{
    int num;
    char ibuf[BUF_LEN];
    char* p = NULL;
    struct inotify_event* event;

    struct pollfd fds;
    fds.fd = ifd;
    fds.events = POLLIN;
    int nfds = 1;
    const struct timespec timeout = { .tv_sec = 0, .tv_nsec = 1000 };
    sigset_t sigmask;
    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGINT);
    int rc = ppoll(&fds, nfds, &timeout, &sigmask);
    if(rc < 0)
    {
        perror("poll failed\n");
        exit(-1);
    }
    if(fds.revents != POLLIN)
        return 0;

    num = read(ifd, ibuf, BUF_LEN);
    if (num < 0) {
        perror("Error");
        exit(-1);
    }

    for (p = ibuf; p < ibuf + num; ) {
        event = (struct inotify_event *) p;
        if(event->wd < 0)
        {
            errno = EBADF;
            perror("Event lost!\n");
            run = 0;
            return 1;
        }
        char* optstr = (char*)malloc(sizeof(char)* 300 + 2);
        optstr[0]='\0';
        // printf("mask = %d\n", event->mask);
        strcat(optstr, "{");
        if (event->mask & IN_ACCESS)
            strcat(optstr, " IN_ACCESS,");
        if (event->mask & IN_ATTRIB)
            strcat(optstr, " IN_ATTRIB,");
        if (event->mask & IN_CLOSE_WRITE)
            strcat(optstr, " IN_CLOSE_WRITE,");
        if (event->mask & IN_CLOSE_NOWRITE)
            strcat(optstr, " IN_CLOSE_NOWRITE,");
        if (event->mask & IN_CREATE)
            strcat(optstr, " IN_CREATE,");
        if (event->mask & IN_DELETE)
            strcat(optstr, " IN_DELETE,");
        if (event->mask & IN_DELETE_SELF)
            strcat(optstr, " IN_DELETE_SELF,");
        if (event->mask & IN_MODIFY)
            strcat(optstr, " IN_MODIFY,");
        if (event->mask & IN_MOVE_SELF)
            strcat(optstr, " IN_MOVE_SELF,");
        if (event->mask & IN_MOVED_FROM)
            strcat(optstr, " IN_MOVED_FROM,");
        if (event->mask & IN_MOVED_TO)
            strcat(optstr, " IN_MOVED_TO,");
        if (event->mask & IN_OPEN)
            strcat(optstr, " IN_OPEN,");
        if (event->mask & IN_ISDIR)
            strcat(optstr, " IN_ISDIR,");
        strcat(optstr, "}");
        char* optname = NULL;
        if(event->len == 0)
        {
            optname = (char*)malloc(sizeof(char) * 5);
            strcpy(optname, "null");
        }
        else
        {
            optname = (char*)malloc(sizeof(char) * (event->len + 2));
            strcpy(optname, event->name);
        }
        char* optwho = (char*)malloc(sizeof(char) * (strlen((*whod)[event->wd]) + 5));
        optwho[0]='\0';
        strcat(optwho,"[");
        strcat(optwho,(*whod)[event->wd]);
        strcat(optwho,"]");
        printf("%s %scookie=%d name=%s\n", optwho, optstr, event->cookie, optname);
        p+=sizeof(struct inotify_event) + event->len;
        printf("\n");
        free(optstr);
        free(optname);
        free(optwho);
    }
    return 0;
}
int main(int argc, char** argv)
{
    if(argc < 2)
    {
        errno = EINVAL;
        perror("You Should enter at least 1 argument!\n");
        return 0;
    }

    int ifd = inotify_init();

    if (ifd < 0) {
        errno = EBADF;
        perror("error");
        exit (-1);
    }
    printf("DIR:");

    char** who = (char**)malloc(sizeof(char*));
    assert(who!=NULL);
    int cnt = 0;
    int Max = 1;

    for(int i = 1; i < argc; i++)
    {
        if(cnt == Max)
        {
            Max<<=1;
            who = (char**)realloc(who, sizeof(char*)*Max);
            // memcpy(newwho, who, sizeof(char*)*cnt);
            // free(who);
            // who = newwho;
        }
        char* fix = (char*)malloc(sizeof(char)*strlen(argv[i]) + 2);
        strcpy(fix, argv[i]);
        if(argv[i][strlen(argv[i])-1] != '/')
            strcat(fix, "/");

        DIR* temp = opendir(fix);
        if(temp == NULL)
            continue;
        closedir(temp);

        who[cnt] = (char*)malloc(sizeof(char)*strlen(fix) + 2);
        strcpy(who[cnt], fix);
        free(fix);
        ++cnt;
        printf(" %s", argv[i]);
    }
    printf("\n");

    int* whodMax = (int*)malloc(sizeof(int));
    *whodMax = 2;
    char** whod = (char**)malloc(sizeof(char*)*2);
    assert(whod!=NULL);

    for(int i = 0; i < cnt; i++)
    {
        rec(who[i], ifd, &whod, whodMax);
        free(who[i]);
    }
    free(who);
    printf("\n\n");

    signal(SIGINT,sigint);

    while(run){
        inot(&whod, ifd);
    }
    free(whodMax);
    free(whod);
    if( errno == 0)
        printf("\nSuccess!\n");
    return 0;
}
