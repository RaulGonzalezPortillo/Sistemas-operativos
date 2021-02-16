#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int t = 5;

int main()
{
  int pid = fork();
  if (pid > 0) {
    sleep(1);
    printf("hello\nfather here %d\n",t);
    wait(&pid);
  } else {
    t = 20;
    printf("hello\nchild here %d\n",t);
  }
  return 0;
}
