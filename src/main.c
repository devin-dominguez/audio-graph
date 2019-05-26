#include <stdlib.h>
#include "portaudio.h"
#include "config.h"
#include "app.h"

static int callback(
    const void *inputBuffer,
    void *outputBuffer,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *data)
{
  float* out = (float*) outputBuffer;
  unsigned int frame;

  for (frame = 0; frame < frameCount; frame++) {
    double* samples = App_calc();
    out[frame * 2] = samples[0];
    out[frame * 2 + 1] = samples[1];
  }

  return 0;
}

int main(void)
{
  App_setup();

  PaStream *stream;
  PaError error;

  error = Pa_Initialize();
  if (error != paNoError) {
    return error;
  }

  error = Pa_OpenDefaultStream(
      &stream,
      0,
      NUMBER_OF_CHANNELS,
      paFloat32,
      SAMPLE_RATE,
      BLOCK_SIZE,
      callback,
      NULL);
  if (error != paNoError) {
    return error;
  }

  error = Pa_StartStream(stream);
  if (error != paNoError) {
    return error;
  }

  // loop until we say sooooooo!
  while (!App_shouldStop()) {}

  error = Pa_StopStream( stream );
  if (error != paNoError) {
    return error;
  }

  App_destroy();

  return 0;
}
