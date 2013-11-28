// convert.c
// Read raw PCM data, convert formatf and write to file
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include "utils.h"

int main(int argc, char** argv) {
  int showhelp = 0;
  unsigned int channels = 1;
  snd_pcm_format_t formatf = SND_PCM_FORMAT_UNKNOWN;
  snd_pcm_format_t formatt = SND_PCM_FORMAT_UNKNOWN;

  int opt;
  while ((opt = getopt(argc, argv, "hc:f:t:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
      case 'c':
        channels = atoi(optarg);
        break;
      case 'f':
        formatf = snd_pcm_format_value(optarg);
        break;
      case 't':
        formatt = snd_pcm_format_value(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

 if (showhelp || argc - optind < 2) {
    fprintf(stderr, "Usage: %s [-c channels] [-f fromFormat] "
        "[-t toFormat] <inputFile> <outputFile>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  if (formatf == SND_PCM_FORMAT_UNKNOWN || formatt == SND_PCM_FORMAT_UNKNOWN) {
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

    rc = readFile(input, bsize, channels, formatf, &data);
    rc = writeFile(output, rc, channels, formatt, data);
    count += rc;
    
    unsigned int c;
    for (c = 0; c < channels; c++) {
      free(data[c]);
    }
    free(data);
  }
  fprintf(stderr, "%lu samples converted\n", count);

  fclose(input);
  fclose(output);

  exit(EXIT_SUCCESS);
}

