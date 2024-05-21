#include "time_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

FILE* mark_lat_logfd = NULL;

// ---- BEGIN PRIVATE FUNCTIONS ----

static uint64_t get_micros() {
  struct timespec tv;
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return tv.tv_sec * 1e6 + tv.tv_nsec / 1e3;
}

void mark_to_file(char op, uint64_t time, void* ptr) {
  if (mark_lat_logfd == NULL) {
    return;
  }

  fwrite(&op, sizeof(op), 1, mark_lat_logfd);
  fwrite(&time, sizeof(time), 1, mark_lat_logfd);
  fwrite(&ptr, sizeof(ptr), 1, mark_lat_logfd);
}

// ---- BEGIN PUBLIC FUNCTIONS ----

void mark_init() {
  const char* lat_dir = getenv("PSM_LAT_LOGDIR");
  if (lat_dir == NULL) {
    mark_lat_logfd = NULL;
    return;
  }

  char filename[2048];
  char hostname[1024];
  gethostname(hostname, sizeof(hostname));
  snprintf(filename, sizeof(filename), "%s/latency.%s.%d", lat_dir, hostname, getpid());

  mark_lat_logfd = fopen(filename, "w+");
  if (mark_lat_logfd == NULL) {
    perror("fopen");
    exit(1);
  }
}

void mark_destroy() {
  if (mark_lat_logfd != NULL) {
    fclose(mark_lat_logfd);
  }
}

void mark_begin(void* ptr) {
  if (mark_lat_logfd == NULL) {
    return;
  }

  uint64_t begin_time = get_micros();
  char op = 0; // op for begin
  mark_to_file(op, begin_time, ptr);
}

void mark_end(void* ptr) {
  if (mark_lat_logfd == NULL) {
    return;
  }

  uint64_t end_time = get_micros();
  char op = 1;
  mark_to_file(op, end_time, ptr);
}
