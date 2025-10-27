#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();

  if (pid == 0) {
    printf("Child process : %5d\n", getpid());
    char *args[] = {"/bin/ls", "-a", "-l", "-h", NULL};
    execv("/bin/ls", args);
  } else {
    printf("Parent process : %5d and Child Process: %5d\n", getpid(), pid);
    execl("/bin/ls", "/bin/ls", NULL);
  }
  printf("end\n");
}
