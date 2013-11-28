// utils.h
// Utility Functions for Audio Processing
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

#ifndef UTILS_H_
#define UTILS_H_

// Use the newer ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

/**
 * Convert one PCM format data to double.
 * Supported formats are U8, S8, U16_LE, U16_BE, S16_LE, S16_BE,
 * U32_LE, U32_BE, S32_LE, S32_BE, FLOAT_LE, FLOAT_BE,
 * FLOAT64_LE, FLOAT64_BE.
 *
 * @return value of PCM data, ranging from -1.0 to 1.0.
 */
double pcmToDouble(snd_pcm_format_t format, void* data);

/**
 * Convert a double value to PCM format.
 * Supported formats are U8, S8, U16_LE, U16_BE, S16_LE, S16_BE,
 * U32_LE, U32_BE, S32_LE, S32_BE, FLOAT_LE, FLOAT_BE,
 * FLOAT64_LE, FLOAT64_BE.
 * Memory should be allocated to data before calling the function.
 *
 * @return 0 on seccuess, other if error occured.
 */
int doubleToPCM(snd_pcm_format_t format, double value, void* data);

/**
 * Read pcm data from file and convert to double arrays.
 * @return number of samples read.
 */
unsigned long int readFile(FILE* file, unsigned long int count,
    unsigned int channels, snd_pcm_format_t format, double*** data);

/**
 * Convert double value data to pcm and write to file.
 * @return number of samples write.
 */
unsigned long int writeFile(FILE* file, unsigned long int count,
    unsigned int channels, snd_pcm_format_t format, double** data);

#endif  // UTILS_H_

