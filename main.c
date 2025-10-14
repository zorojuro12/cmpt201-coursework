#define _POSIX_C_SOURCE 200809L
#include "../include/task0.h"
#include "../include/task1.h"
#include "../include/task2.h"
#include <string.h>
#include <unistd.h>

int main(void) {
  char input[INPUT_SIZE];
  char saved_input[INPUT_SIZE];
  char *args[ARG_MAX];
  int background;

  // setup the signal handlers
  setup_shell();

  while (1) {
    collect_background();
    print_prompt();

    int status = read_line(input, INPUT_SIZE);

    if (status == -2)
      continue;

    if (status == 0) { // EOF
      write(STDOUT_FILENO, "\n", 1);
      break;
    }

    if (status < 0) // Error
      continue;

    // save orig input
    strncpy(saved_input, input, INPUT_SIZE - 1);
    saved_input[INPUT_SIZE - 1] = '\0';

    // check for recall
    if (is_recall(input)) {
      char *recalled = get_recalled_cmd(input);
      if (!recalled) {
        continue;
      }

      // use the recalled command
      strncpy(input, recalled, INPUT_SIZE - 1);
      input[INPUT_SIZE - 1] = '\0';
      strncpy(saved_input, recalled, INPUT_SIZE - 1);
      saved_input[INPUT_SIZE - 1] = '\0';
    }

    // parse the args
    int count = split_args(input, args, &background);
    if (count == 0)
      continue;

    if (background) {
      size_t len = strlen(saved_input);
      if (len + 2 < INPUT_SIZE) {
        strncat(saved_input, " &", INPUT_SIZE - len - 1);
      }
    }

    // add it to history before execution;
    store_command(saved_input);

    // handle command
    if (is_internal(args[0])) {
      if (strcmp(args[0], "exit") == 0) {
        run_exit(args);
      } else if (strcmp(args[0], "pwd") == 0) {
        run_pwd(args);
      } else if (strcmp(args[0], "cd") == 0) {
        run_cd(args);
      } else if (strcmp(args[0], "help") == 0) {
        run_help(args);
      } else if (strcmp(args[0], "history") == 0) {
        show_history(args);
      } else {
        execute(args, background);
      }
    }
  }
  return 0;
}
