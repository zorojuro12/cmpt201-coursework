#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { N = 5 }; // keep the last 5 lines

static void free_history(char *history[N]) {
  for (int i = 0; i < N; i++) {
    free(history[i]);
    history[i] = NULL;
  }
}

// Takes the owneship from the ownes_line and overwritest the oldest entry
static void push_history_take(char *history[N], int *next, int *count,
                              char *owned_line) {
  if (history[*next] != NULL) {
    free(history[*next]);
  }
  history[*next] = owned_line;
  *next = (*next + 1) % N;
  if (*count < N)
    (*count)++;
}

// prints from oldest entry to newest entry.
static void print_history(char *history[N], int next, int count) {
  int start = (next - count + N) % N;
  for (int i = 0; i < count; i++) {
    int idx = (start + i) % N;
    puts(history[idx]);
  }
}

int main(void) {
  char *history[N] = {NULL};
  int next = 0;  // the next index to overwrite
  int count = 0; // keeps the count of current valid entries.

  char *line = NULL;
  size_t cap = 0;

  for (;;) {
    printf("Enter input: ");
    fflush(stdout);

    ssize_t n = getline(&line, &cap, stdin);
    if (n == -1) {
      free_history(history);
      free(line);
      return 0;
    }

    // removes the traiing \n character from getline()
    if (n > 0 && line[n - 1] == '\n') {
      line[n - 1] = '\0';
    }

    push_history_take(history, &next, &count, line);

    line = NULL;
    cap = 0;

    int last = (next - 1 + N) % N;

    // display the entries if the last entry is 'print'
    if (strcmp(history[last], "print") == 0) {
      print_history(history, next, count);
    }
  }

  return 0;
}
