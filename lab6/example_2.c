// Corrected example_2.c with Assertions.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Improved ASSERT macro to include file name and line number
#define ASSERT(expr)                                                           \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed: %s\n", #expr);                        \
      fprintf(stderr, "File: %s, Line: %d\n", __FILE__, __LINE__);             \
      exit(1);                                                                 \
    }                                                                          \
  }

#define TEST(expr)                                                             \
  {                                                                            \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Test failed: %s\n", #expr);                             \
      exit(1);                                                                 \
    }                                                                          \
  }

typedef struct node {
  uint64_t data;
  struct node *next;
} node_t;

typedef struct info {
  uint64_t sum;
} info_t;

node_t *head = NULL;
info_t info = {0};

// new function to calculate sum of all elements in the list
uint64_t calculate_sum() {
  uint64_t sum = 0;
  node_t *curr = head;
  
  while (curr != NULL) {
    sum += curr->data;
    curr = curr->next;
  }
  
  return sum;
}

void insert_sorted(uint64_t data) {
  node_t *new_node = malloc(sizeof(node_t));
  new_node->data = data;
  new_node->next = NULL;

  if (head == NULL) {
    head = new_node;
  } else if (data < head->data) {
    new_node->next = head;
    head = new_node;
  } else {
    node_t *curr = head;
    node_t *prev = NULL;

    while (curr != NULL) {
      if (data < curr->data) {
        break;
      }

      prev = curr;
      curr = curr->next;
    }

    prev->next = new_node;
    // fix: Changed from curr->next to curr
    if (curr != NULL) {
      new_node->next = curr;
    }
  }

  info.sum += data;
  
  // REQUIRED: Assertion to check sum is updated correctly after items are added
  ASSERT(info.sum == calculate_sum());
}

int index_of(uint64_t data) {
  node_t *curr = head;
  int index = 0;

  while (curr != NULL) {
    if (curr->data == data) {
      return index;
    }

    curr = curr->next;
    index++;
  }

  return -1;
}

int main() {
  insert_sorted(1);
  insert_sorted(3);
  insert_sorted(5);
  insert_sorted(2);

  TEST(info.sum == 1 + 3 + 5 + 2);
  TEST(index_of(2) == 1);
  
  //Assert to check that info.sum equals sum function
  ASSERT(info.sum == calculate_sum());

  return 0;
}
