// capalsa.c
// Sound Capture using ALSA interfaces
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

/*
This example reads from the default PCM device
and writes to standard output for 5 seconds of data.
*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

void printState(snd_pcm_t* handle) {
  int s = snd_pcm_state(handle);
  switch (s) {
    case SND_PCM_STATE_OPEN:
      fprintf(stderr, "PCM State : OPEN\n");
      break;
    case SND_PCM_STATE_SETUP:
      fprintf(stderr, "PCM State : SETUP\n");
      break;
    case SND_PCM_STATE_PREPARED:
      fprintf(stderr, "PCM State : PREPARED\n");
      break;
    case SND_PCM_STATE_RUNNING:
      fprintf(stderr, "PCM State : RUNNING\n");
      break;
    case SND_PCM_STATE_XRUN:
      fprintf(stderr, "PCM State : XRUN\n");
      break;
    case SND_PCM_STATE_DRAINING:
      fprintf(stderr, "PCM State : DRAINING\n");
      break;
    case SND_PCM_STATE_PAUSED:
      fprintf(stderr, "PCM State : PAUSED\n");
      break;
    case SND_PCM_STATE_SUSPENDED:
      fprintf(stderr, "PCM State : SUSPENDED\n");
      break;
    case SND_PCM_STATE_DISCONNECTED:
      fprintf(stderr, "PCM State : DISCONNECTED\n");
      break;
    default:
      fprintf(stderr, "PCM State : ELSE\n");
      break;
  }
}

int main(int argc, char* argv[]) {
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  int dir;
  char *buffer;

  char* device = "default";
  unsigned int channels = 1;
  unsigned int rate = 44100;
  unsigned int duration = 5;

  int opt;
  while ((opt = getopt(argc, argv, "D:c:d:r:")) != -1) {
    switch (opt) {
      case 'D':
        device = strdup(optarg);
        break;
      case 'c':
        channels = atoi(optarg);
        break;
      case 'd':
        duration = atoi(optarg);
        break;
      case 'r':
        rate = atoi(optarg);
        break;
      default: /* '?' */
        fprintf(stderr, "Usage: %s [-D device] [-c channels] "
            "[-d duration] [-r rate]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  }

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n",
        snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_malloc(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

  /* channels */
  snd_pcm_hw_params_set_channels(handle, params, channels);

  /* sampling rate */
  snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);

  /* Set period size to 32 frames. */
  snd_pcm_uframes_t frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  size_t size = frames * 2 * channels; /* 2 bytes/sample times #channels */
  buffer = (char*)malloc(size);

  /* We want to loop for 5 seconds */
  unsigned int ptime;
  snd_pcm_hw_params_get_period_time(params, &ptime, &dir);
  long loops = duration * 1000000 / ptime;

  fprintf(stderr, "Capture Sound\nDevice : %s\nChannels : %d\nRate : %dHz\n"
      "Duration : %ds (%ld loops)\n",
      device, channels, rate, duration, loops);

  while (loops > 0) {
    loops--;
    rc = snd_pcm_readi(handle, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr,
          "error from read: %s\n",
          snd_strerror(rc));
    } else if (rc != (int)frames) {
      fprintf(stderr, "short read, read %d frames\n", rc);
    }
    rc = write(1, buffer, size);
    if (rc != size)
      fprintf(stderr,
          "short write: wrote %d bytes\n", rc);
  }

  fprintf(stderr, "Finished #0\n");
  snd_pcm_drain(handle);
  printState(handle);
  snd_pcm_nonblock(handle, 0);
  printState(handle);
  fprintf(stderr, "Finished #1/2\n");
  snd_pcm_hw_free(handle);
  printState(handle);
  fprintf(stderr, "Finished #1\n");
  printState(handle);
  fprintf(stderr, "Finished #2\n");
  free(buffer);
  fprintf(stderr, "Finished #3\n");

  return 0;
}

