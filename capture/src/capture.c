// capalsa.c
// Sound Capture using ALSA interfaces
//
// Author : Weipeng He <heweipeng@gmail.com>
// Copyright (c) 2013, All rights reserved.

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

int main(int argc, char** argv) {
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  int dir;
  char *buffer;

  char* device = "default";
  unsigned int channels = 1;
  unsigned int rate = 44100;
  unsigned int duration = 5;
  snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;
  int showhelp = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hD:c:d:r:f:")) != -1) {
    switch (opt) {
      case 'h':
        showhelp = 1;
        break;
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
      case 'f':
        format = snd_pcm_format_value(optarg);
        break;
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }
  
  if (format == SND_PCM_FORMAT_UNKNOWN) {
    format = SND_PCM_FORMAT_S16_LE;
  }

  if (showhelp) {
    fprintf(stderr, "Usage: %s [-D device] [-c channels] "
        "[-d duration] [-r rate] [-f format]\n", argv[0]);
    exit(EXIT_SUCCESS);
  }

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n",
        snd_strerror(rc));
    exit(EXIT_FAILURE);
  }

  /* Allocate a hardware parameters object. */
  rc = snd_pcm_hw_params_malloc(&params);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  /* Fill it in with default values. */
  rc = snd_pcm_hw_params_any(handle, params);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  /* Set the desired hardware parameters. */
  /* Interleaved mode */
  rc = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  /* set format */
  rc = snd_pcm_hw_params_set_format(handle, params, format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  /* channels */
  rc = snd_pcm_hw_params_set_channels_near(handle, params, &channels);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  /* sampling rate */
  rc = snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  /* Set period size to 32 frames. */
  snd_pcm_uframes_t frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
    exit(EXIT_FAILURE);
  }

  /* Use a buffer large enough to hold one period */
  rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  rc = snd_pcm_hw_params_get_format(params, &format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  rc = snd_pcm_format_physical_width(format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    exit(EXIT_FAILURE);
  }
  size_t size = frames * (rc / 8) * channels;
  buffer = (char*)malloc(size);

  /* set number of loops */
  unsigned int ptime;
  rc = snd_pcm_hw_params_get_period_time(params, &ptime, &dir);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }
  long loops = duration * 1000000 / ptime;

  fprintf(stderr, "Capture Sound\nDevice : %s\nChannels : %d\n"
      "Rate : %dHz\nFormat : %s\nDuration : %ds (%ld loops)\n",
      device, channels, rate, snd_pcm_format_name(format), duration, loops);

  while (loops > 0) {
    loops--;
    rc = snd_pcm_readi(handle, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means overrun */
      fprintf(stderr, "Overrun occurred\n");
      snd_pcm_recover(handle, rc, 0);
      snd_pcm_prepare(handle);
    }
    else if (rc < 0) {
      fprintf(stderr, "Error from read: %s\n", snd_strerror(rc));
      snd_pcm_recover(handle, rc, 0);
    }
    else if (rc != (int)frames) {
      fprintf(stderr, "Short read, read %d frames\n", rc);
    }
    rc = write(1, buffer, size);
    if (rc != size)
      fprintf(stderr, "Short write: wrote %d bytes\n", rc);
  }

  rc = snd_pcm_close(handle);
  if (rc < 0) {
    fprintf(stderr, "Error (at %s:%d) : %s\n",
        __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  free(buffer);

  exit(EXIT_SUCCESS);
}

