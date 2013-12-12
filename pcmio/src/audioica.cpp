// audioica.cpp
// ICA of audio files
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

extern "C" {
  #include "utils.h"
}

#include <itpp/itbase.h>
#include <itpp/signal/fastica.h>

using itpp::abs;
using itpp::mat;
using itpp::max;
using itpp::Fast_ICA;

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

 if (showhelp || argc - optind < 2) {
    fprintf(stderr, "Usage: %s [-c channels] [-f format] "
        "<inputFile> <outputFile>\n", argv[0]);
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

  // Load Audio File
  double** data;
  count = readFile(input, count, channels, format, &data);
  fprintf(stderr, "%lu samples read\n", count);

  fclose(input);

  mat sources(channels, count);
  for (int i = 0; i < channels; i++) {
    for (int j = 0; j < count; j++) {
      sources.set(i, j, data[i][j]);
    }
  }
 
  Fast_ICA fastica(sources);
  fastica.set_nrof_independent_components(sources.rows());
  fastica.set_non_linearity(FICA_NONLIN_TANH);
  fastica.set_approach(FICA_APPROACH_DEFL);
  fastica.separate();
  mat ics = fastica.get_independent_components();
  ics /= max(max(abs(ics)));

  for (int i = 0; i < channels; i++) {
    for (int j = 0; j < count; j++) {
      data[i][j] = ics.get(i, j);
    }
  }

  count = writeFile(output, count, channels, format, data);

  fclose(output);
  freeData(data, channels);

  exit(EXIT_SUCCESS);
}

