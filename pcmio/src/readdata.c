// readdata.c
// Read Raw PCM Data and convert to double arrays
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include "utils.h"

int main(int argc, char** argv) {
  int showhelp = 0;
  unsigned int channels = 1;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
  unsigned long int count = 1;

  int opt;
  while ((opt = getopt(argc, argv, "hc:f:C:")) != -1) {
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
      case 'C':
        count = atol(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

 if (showhelp || optind >= argc) {
    fprintf(stderr, "Usage: %s [-c channels] [-f format] "
        "[-C count] <file>\n", argv[0]);
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

  double** data;

  unsigned long int rc = readFile(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", rc);

  unsigned int c;
  unsigned long int i;
  for (i = 0; i < rc; i++) {
    for (c = 0; c < channels; c++) {
      printf("%f\t", data[c][i]);
    }
    printf("\n");
  }

  freeData(data, channels);
  fclose(input);

  exit(EXIT_SUCCESS);
}

