// utils.c
// Implementation of utility functions
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#include "utils.h"

#include <assert.h>
#include <stdint.h>

// #define _BSD_SOURCE
#include <endian.h>

double pcmToDouble(snd_pcm_format_t format, void* data) {
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
    unsigned int channels, snd_pcm_format_t format,
    double*** data) {
  int rc; // return code

  rc = snd_pcm_format_physical_width(format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    exit(EXIT_FAILURE);
  }
  size_t sampleSize = rc / 8; // sample count in bytes

  // Allocate space
  *data = malloc(sizeof(double*) * channels);
  unsigned int c;
  for (c = 0; c < channels; c++) {
    (*data)[c] = malloc(sizeof(double) * sampleSize * count);
  }

  // Read data
  size_t bufferSize = sampleSize * channels;
  char* buffer = malloc(bufferSize);
  unsigned long int readCount = 0;
  for (readCount = 0; readCount < count; readCount++) {
    size_t rs = fread(buffer, 1, bufferSize, file);
    if (rs < bufferSize) { // !NOTE : Ignore incomplete samples
      break;
    }
    for (c = 0; c < channels; c++) {
      (*data)[c][readCount] = pcmToDouble(format, buffer + (c * sampleSize));
    }
  }
  free(buffer);

  return readCount;
}
