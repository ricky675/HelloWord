#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<string.h>
#include<time.h>
//#include<sys/types.h>
#define ERROR_EXIT(m) {perror(m); exit(EXIT_FAILURE);}

void myDeamon()
{
  pid_t fpid = 0;
  int i, fd, count = 0;
  /*signal(SIGCHLD, SIG_IGN);*/
  fpid = fork();/*第一次创建子进程*/
  if (fpid < 0)
    ERROR_EXIT("First fork failed!");
  
  if (fpid > 0)
    exit(EXIT_SUCCESS);
  /*变更会话，成为新的会话的进程组长*/
  if (setsid() == -1)
    ERROR_EXIT("setsid failed!");

  fpid = fork();/*第二次创建子进程*/
  if (fpid < 0)
    ERROR_EXIT("First fork failed!");

  if (fpid > 0)
    exit(EXIT_SUCCESS);

  /*关闭从父进程继承来的文件描述符*/
  #ifdef RLIMIT_NOFILE
  if (getrlimit(RLIMIT_NOFILE, &rl) == -1)
    ERROR_EXIT("getrlimit failed!");
  nfiles = rl.rlim_cur = rl.rlim_max;
  setrlimit(RLIMIT_NOFILE, &rl);
  for(i=3; i<nfiles; i++)
    close(i);
  #endif

  /*重定向标准的三个文件描述符*/
  if (fd = open("/dev/null", O_RDWR) < 0)
    ERROR_EXIT("open /dev/null failed!");
  for(i=0; i<3; i++)
    dup2(fd, i);
  if(fd > 2) close(fd);

  /* 改变工作目录和文件掩码常量 */
  chdir("/");
  umask(0);
}

int main()
{
  time_t t;
  int fd, i;
  myDeamon();
  fd = open("./home/sap/src/test/mydaemon.log", O_RDWR|O_CREAT, 0644);
  if(fd < 0)
    ERROR_EXIT("open /mydaemon.log failed!");
  for(i=0; i<3; i++)
  {
    time(&t);
    char *buf = asctime(localtime(&t));
    write(fd, buf, strlen(buf));
    //printf("%s\n", buf);
    //fflush(stdout);
    sleep(10);
  }
  //close(fd);
  return 0;
}
