// readdata.c
// Read Raw PCM Data and convert to double arrays
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include <assert.h>
#include <stdint.h>

// redefined ?
// #define _BSD_SOURCE
#include <endian.h>

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

double convToDouble(snd_pcm_format_t format, void* data) {
  int format_bits = snd_pcm_format_width(format);
  int bps = format_bits / 8; /* bytes per sample */
  int big_endian = snd_pcm_format_big_endian(format) == 1;
  int is_unsigned = snd_pcm_format_unsigned(format) == 1;
  int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
      format == SND_PCM_FORMAT_FLOAT_BE);

  double value = 0.0;
  double maxval = 1.0;

  if (bps == 1) {
    // no endian problem
    int8_t* vp = (int8_t*) data;
    int8_t vc = *vp;
    if (is_unsigned) {
      assert(format == SND_PCM_FORMAT_U8);
      vc ^= 1U << (format_bits - 1);
    }
    else {
      assert(format == SND_PCM_FORMAT_S8);
    }
    int8_t im = INT8_MAX;
    maxval = (double) im;
    value = (double) vc / maxval;
  }
  else if (bps == 2) {
    uint16_t vcu = *((uint16_t*) data);
    if (big_endian) {
      vcu = be16toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le16toh(vcu);
    }

    assert(!is_float);
    if (is_unsigned) {
      vcu ^= 1U << (format_bits - 1);
    }
    else {
      assert(snd_pcm_format_signed(format) == 1);
    }

    int16_t im = INT16_MAX;
    maxval = (double) im;
    int16_t vcs = *((int16_t*) &vcu);
    value = (double) vcs / maxval;
  }
  else if (bps == 4) {
    uint32_t vcu = *((uint32_t*) data);
    if (big_endian) {
      vcu = be32toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le32toh(vcu);
    }

    if (!is_float) {
      if (is_unsigned) {
        vcu ^= 1U << (format_bits - 1);
      }
      else {
        assert(snd_pcm_format_signed(format) == 1);
      }

      int32_t im = INT32_MAX;
      maxval = (double) im;
      int32_t vcs = *((int32_t*) &vcu);
      value = (double) vcs / maxval;
    }
    else {
      float vcs = *((float*) &vcu);
      value = (double) vcs;
    }
  }
  else if (bps == 8) {
    assert(is_float);
    uint64_t vcu = *((uint64_t*) data);
    if (big_endian) {
      vcu = be64toh(vcu);
    }
    else {
      assert(snd_pcm_format_little_endian(format) == 1);
      vcu = le64toh(vcu);
    }

    value = *((double*) &vcu);
  }
  else {
    // bps == 3 ?? ignore
    // TODO : fix it
    assert(0);
  }

  return value;
}

unsigned long int readFile(FILE* file, unsigned long int count,
    unsigned int channel, snd_pcm_format_t format,
    double*** data) {
  int rc; // return code

  rc = snd_pcm_format_physical_width(format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    return EXIT_FAILURE;
  }
  size_t sampleSize = rc / 8; // sample count in bytes

  // Allocate space
  *data = malloc(sizeof(double*) * channel);
  unsigned int c;
  for (c = 0; c < channel; c++) {
    (*data)[c] = malloc(sizeof(double) * sampleSize * count);
  }

  // Read data
  size_t bufferSize = sampleSize * channel;
  char* buffer = malloc(bufferSize);
  unsigned long int readCount = 0;
  for (readCount = 0; readCount < count; readCount++) {
    size_t rs = fread(buffer, 1, bufferSize, file);
    if (rs < bufferSize) { // !NOTE : Ignore incomplete samples
      break;
    }
    for (c = 0; c < channel; c++) {
      (*data)[c][readCount] = convToDouble(format, buffer + (c * sampleSize));
    }
  }
  free(buffer);

  return readCount;
}

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

  if (format == SND_PCM_FORMAT_UNKNOWN) {
    fprintf(stderr, "Unknown format\n");
    exit(EXIT_FAILURE);
  }

  if (showhelp || optind >= argc) {
    fprintf(stderr, "Usage: %s [-c channels] [-f format] "
        "[-C count] <file>\n", argv[0]);
    exit(EXIT_SUCCESS);
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

  for (c = 0; c < channels; c++) {
    free(data[c]);
  }
  free(data);

  exit(EXIT_SUCCESS);
}

