#define _POSIX_C_SOUCE 200809L
#include "../include/task2.h"
#include "../include/msgs.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// struct for simple history entry
typedef struct {
  int number;
  char text[INPUT_SIZE];
} cmd_record;

// storage
static cmd_record records[MAX_HIST];
static int next_num = 0;

// helper for number of commands stored
static int stored_count(void) {
  return (next_num < MAX_HIST) ? next_num : MAX_HIST;
}

// get oldest command number in history
static int oldest_num(void) {
  if (next_num < MAX_HIST) {
    return 0;
  }
  return next_num - MAX_HIST;
}

// helper function to find the command by number
static cmd_record *lookup(int num) {
  int count = stored_count();
  int start = oldest_num();

  if (num < start || num >= next_num) {
    return NULL;
  }

  // find it in array
  for (int i = 0; i < count; i++) {
    if (records[i].number == num) {
      return &records[i];
    }
  }
  return NULL;
}

// store a command
void store_command(const char *cmd_line) {
  if (!cmd_line || cmd_line[0] == '\0') {
    return;
  }

  // decide the slot
  int slot = next_num % MAX_HIST;

  // save it
  records[slot].number = next_num;
  strncpy(records[slot].text, cmd_line, INPUT_SIZE - 1);
  records[slot].text[INPUT_SIZE - 1] = '\0';

  next_num++;
}

void show_history(char **cmd_args) {
  // check for extra args
  if (cmd_args[1]) {
    write(STDERR_FILENO, FORMAT_MSG("history", TMA_MSG),
          strlen(FORMAT_MSG("history", TMA_MSG)));
    return;
  }

  int count = stored_count();
  if (count == 0) {
    return;
  }

  // show from newest to oldest
  for (int num = next_num - 1; num >= oldest_num(); num--) {
    cmd_record *rec = lookup(num);
    if (rec) {
      // buld the formatted string
      char output[INPUT_SIZE + 32];
      snprintf(output, sizeof(output), "%d\t%s\n", rec->number, rec->text);
      write(STDOUT_FILENO, output, strlen(output));
    }
  }
}

int is_recall(const char *input) {
  if (!input || input[0] != '!') {
    return 0;
  }
  return 1;
}

char *get_recalled_cmd(const char *input) {
  static char buffer[INPUT_SIZE];

  if (strcmp(input, "!!") == 0) {
    if (next_num == 0) {
      write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_NO_LAST_MSG),
            strlen(FORMAT_MSG("history", HISTORY_NO_LAST_MSG)));
      return NULL;
    }

    cmd_record *last = lookup(next_num - 1);
    if (!last) {
      return NULL;
    }

    // display what we're running
    write(STDOUT_FILENO, last->text, strlen(last->text));
    write(STDOUT_FILENO, "\n", 1);

    // return it
    strncpy(buffer, last->text, INPUT_SIZE - 1);
    buffer[INPUT_SIZE - 1] = '\0';
    return buffer;
  }

  // hyandle !n
  if (input[0] == '!' && input[1] != '\0') {
    const char *digits = input + 1;

    // validate its a number
    for (int i = 0; digits[i]; i++) {
      if (!isdigit(digits[i])) {
        write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_INVALID_MSG),
              strlen(FORMAT_MSG("history", HISTORY_INVALID_MSG)));
        return NULL;
      }
    }

    int target = atoi(digits);

    // check the range
    if (target < oldest_num() || target >= next_num) {
      write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_INVALID_MSG),
            strlen(FORMAT_MSG("history", HISTORY_INVALID_MSG)));
      return NULL;
    }

    cmd_record *found = lookup(target);
    if (!found) {
      write(STDERR_FILENO, FORMAT_MSG("history", HISTORY_INVALID_MSG),
            strlen(FORMAT_MSG("history", HISTORY_INVALID_MSG)));
      return NULL;
    }

    // display whats running
    write(STDOUT_FILENO, found->text, strlen(found->text));
    write(STDOUT_FILENO, "\n", 1);

    // return it
    strncpy(buffer, found->text, INPUT_SIZE - 1);
    buffer[INPUT_SIZE - 1] = '\0';
    return buffer;
  }

  return NULL;
}
