#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int a = getpid();
  printf("Parent PID : %5d\n", a);
  printf("Before forking...\n");
  pid_t pid = fork();
  if (getpid() == a) {
    printf("This is the parent process with PID : %5d, Child PID : %5d\n",
           getpid(), pid);
  } else {
    printf(
        "THis is the child process with the PID : % 5d, Parent PID is : %5d\n",
        getpid(), getppid());
  }

  printf("Done.\n");
}
