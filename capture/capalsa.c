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

void showDevices() {
  char** hints;
  /* Enumerate sound devices */
  int rc = snd_device_name_hint(-1, "pcm", (void***)&hints);
  if (rc < 0) {
    fprintf(stderr, "Error (at %s:%d) : %s\n",
        __FILE__, __LINE__, snd_strerror(rc));
  }

  char** n = hints;
  while (*n != NULL) {
    char* name = snd_device_name_get_hint(*n, "NAME");
    if (name != NULL && 0 != strcmp("null", name)) {
      free(name);
    }
    n++;
  }

  snd_device_name_free_hint((void**)hints);
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
  int showhelp = 0;

  int opt;
  while ((opt = getopt(argc, argv, "hD:c:d:r:")) != -1) {
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
      default: /* '?' */
        showhelp = 1;
        break;
    }
  }

  if (showhelp) {
    fprintf(stderr, "Usage: %s [-D device] [-c channels] "
        "[-d duration] [-r rate]\n", argv[0]);
    showDevices();
    return 0;
  }

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n",
        snd_strerror(rc));
    exit(1);
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

  /* Signed 16-bit little-endian format */
  rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
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
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  snd_pcm_format_t format;
  rc = snd_pcm_hw_params_get_format(params, &format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  rc = snd_pcm_format_physical_width(format);
  if (rc < 0) {
    fprintf(stderr, "Error at %s:%d : %s\n", __FILE__, __LINE__, snd_strerror(rc));
    return EXIT_FAILURE;
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

  printState(handle);
  rc = snd_pcm_close(handle);
  if (rc < 0) {
    fprintf(stderr, "Error (at %s:%d) : %s\n",
        __FILE__, __LINE__, snd_strerror(rc));
    snd_pcm_recover(handle, rc, 0);
  }

  free(buffer);

  return 0;
}

