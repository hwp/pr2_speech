// chanfilter.c
// Channel Filter, extract one channel out of all.
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include "utils.h"

int main(int argc, char** argv) {
  int showhelp = 0;
  unsigned int channels = 1;
  unsigned int index = 0;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hc:i:f:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'c':
        channels = atoi(optarg);
        break;
      case 'f':
        format = snd_pcm_format_value(optarg);
        break;
      case 'i':
        index = atoi(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp || argc - optind < 2) {
    fprintf(stderr, "Usage: %s [-c channels] [-i index] "
        "[-f format] <inputFile> <outputFile>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }

  FILE* input = fopen(argv[optind], "r");
  if (input == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", argv[optind], strerror(errno));
    exit(EXIT_FAILURE);
  }

  FILE* output = fopen(argv[optind + 1], "w");
  if (output == NULL) {
    fprintf(stderr, "Cannot Open File %s : %s\n", argv[optind + 1], strerror(errno));
    exit(EXIT_FAILURE);
  }

  unsigned long int count = 0;
  unsigned long int bsize = 1000;
  unsigned long int rc = bsize;
  while (rc == bsize) {
    double** data;

    rc = readFile(input, bsize, channels, format, &data);
    rc = writeFile(output, rc, 1, format, data + index);
    count += rc;
    
    freeData(data, channels);
  }
  fprintf(stderr, "%lu samples filtered\n", count);

  fclose(input);
  fclose(output);

  exit(EXIT_SUCCESS);
}

