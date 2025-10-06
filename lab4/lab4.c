#define _DEFAULT_SOURCE
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define HEAP_SIZE 256
#define BLOCK_SIZE 128
#define BUF_SIZE 256

struct header {
  uint64_t size;
  struct header *next;
};

static void handle_error(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

static void print_out(const char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  int len;

  if (data_size == sizeof(void *)) {
    void *pv = *(void **)data;
    len = snprintf(buf, sizeof buf, format, pv);
  } else {
    unsigned long long v = (unsigned long long)(*(uint64_t *)data);
    len = snprintf(buf, sizeof buf, format, v);
  }

  if (len < 0)
    handle_error("snprintf");
  (void)write(STDOUT_FILENO, buf, (size_t)len);
}

static void print_block_data(const char *block_start) {
  size_t data_bytes = BLOCK_SIZE - sizeof(struct header);
  const unsigned char *p =
      (const unsigned char *)block_start + sizeof(struct header);
  for (size_t i = 0; i < data_bytes; ++i) {
    uint64_t val = (uint64_t)p[i];
    print_out("%llu\n", &val, sizeof(val));
  }
}

int main(void) {
  void *base = sbrk(HEAP_SIZE);
  if (base == (void *)-1)
    handle_error("sbrk");

  char *start = (char *)base;
  struct header *block1 = (struct header *)(start + 0);
  struct header *block2 = (struct header *)(start + BLOCK_SIZE);

  block1->size = BLOCK_SIZE;
  block1->next = NULL;
  block2->size = BLOCK_SIZE;
  block2->next = block1;

  {
    void *p1 = block1, *p2 = block2, *n1 = block1->next, *n2 = block2->next;
    uint64_t s1 = block1->size, s2 = block2->size;

    print_out("first block : %p\n", &p1, sizeof p1);
    print_out("second block : %p\n", &p2, sizeof p2);

    print_out("first block size : %llu\n", &s1, sizeof s1);
    print_out("first block next : %p\n", &n1, sizeof n1);
    print_out("second block size : %llu\n", &s2, sizeof s2);
    print_out("second block next : %p\n", &n2, sizeof n2);
  }

  memset(block1 + 1, 0x00, BLOCK_SIZE - sizeof(struct header));
  memset(block2 + 1, 0x01, BLOCK_SIZE - sizeof(struct header));

  {
    void *n1 = block1->next, *n2 = block2->next;
    uint64_t s1 = block1->size, s2 = block2->size;

    print_out("first block size: %llu\n", &s1, sizeof s1);
    print_out("first block next : %p\n", &n1, sizeof n1);
    print_out("second block size:%llu\n", &s2, sizeof s2);
    print_out("second block next : %p\n", &n2, sizeof n2);
  }

  print_block_data((const char *)block1);
  print_block_data((const char *)block2);

  return 0;
}
