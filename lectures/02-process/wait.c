#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();
  if (pid) {
    int wstatus = 0;
    waitpid(pid, &wstatus, 0);
  } else {
    execl("/bin/ls", "/bin/ls", "-a", "-l", NULL);
  }
}
