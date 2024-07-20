#define _GNU_SOURCE

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#include <assert.h>
#include <time.h>

#define TIMESPEC2NS(ts) (((ts).tv_sec * 1000000000ULL) + (ts).tv_nsec)

#include "lz.h"

#define pr_info(fmt, ...) fprintf(stdout, fmt "\n", ##__VA_ARGS__)
#define pr_err(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)

int main(int argc, char *argv[]) {
  if (argc < 3) {
    pr_err("Usage: %s INPUT_FILE_PATH MAX_SIZE", argv[0]);
    return -EINVAL;
  }

  ssize_t szLimit = atoll(argv[2]);
  const char *nameInput = basename(argv[1]);

  size_t lenPath = 5 /* /tmp/ */ + strlen(nameInput) + 4 /* .(lz|zl)\0 */;

  char *pathEncoded = malloc(lenPath);
  if (!pathEncoded) {
    perror("Failed to allocate memory for the encoded filename");
    goto free_en;
  }
  snprintf(pathEncoded, lenPath, "/tmp/%s.lz", nameInput);

  char *pathDecoded = malloc(lenPath);
  if (!pathDecoded) {
    perror("Failed to allocate memory for the decoded filename");
    goto free_de;
  }
  snprintf(pathDecoded, lenPath, "/tmp/%s.zl", nameInput);

  struct stat s;
  if (stat(argv[1], &s)) {
    perror("Failed to get input file stat");
    goto free_de;
  }

  // input file may be special file, use szLimit instead
  ssize_t szRaw = s.st_size > 0 ? MIN(s.st_size, szLimit) : szLimit;

  // allocate buffers for raw data, encoded data, decoded data
  void *datRaw = malloc(szRaw);
  if (!datRaw) {
    perror("Failed to allocate memory for input data");
    goto free_de;
  }

  void *datEnc = malloc(szRaw * 3UL + 5);
  if (!datEnc) {
    perror("Failed to allocate memory for encoded data");
    goto free_in_buf;
  }
  void *datDec = malloc(szRaw);
  if (!datDec) {
    perror("Failed to allocate memory for decoded data");
    goto free_enc_buf;
  }

  // get raw data
  int fd;
  if (!(fd = open(argv[1], O_RDONLY))) {
    perror("Failed to open input file");
    goto free_dec_buf;
  }

  size_t nsLoadTime;
  struct timespec tsRawBeg, tsRawEnd;
  assert(0 == clock_gettime(CLOCK_REALTIME, &tsRawBeg));
  if (szRaw != read(fd, datRaw, szRaw)) {
    perror("Failed to read data from the input file");
    goto close_in;
  }
  assert(0 == clock_gettime(CLOCK_REALTIME, &tsRawEnd));
  nsLoadTime = TIMESPEC2NS(tsRawEnd) - TIMESPEC2NS(tsRawBeg);

  pr_info("Compressing %ld bytes... (load time: %lu ns)", szRaw, nsLoadTime);
  for (uint8_t bits = 1; bits < 16; ++bits) {
    struct timespec tsBeg, tsEnd;
    size_t time_ns;

    assert(0 == clock_gettime(CLOCK_REALTIME, &tsBeg));
    ssize_t szEnc = encode(datRaw, szRaw, datEnc, bits);
    assert(0 == clock_gettime(CLOCK_REALTIME, &tsEnd));
    time_ns = TIMESPEC2NS(tsEnd) - TIMESPEC2NS(tsBeg);

    ssize_t szDec = decode(datEnc, datDec);
    pr_info("\t[%02hu]: Compressed to %ld B (%lu ns)", bits, szEnc, time_ns);

    // verify
    if ((szDec != szRaw) || memcmp(datRaw, datDec, szRaw)) {
      pr_err("Data corrupted...");
      break;
    }
  }

close_in:
  close(fd);
free_dec_buf:
  free(datDec);
free_enc_buf:
  free(datEnc);
free_in_buf:
  free(datRaw);
free_de:
  free(pathDecoded);
free_en:
  free(pathEncoded);
  return 0;
}
