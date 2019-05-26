#pragma once

#include <math.h>
#include "config.h"
#include "audio-graph.h"
#include "ugens.h"
#include "utils.h"


static AudioContext* audioContext;
static Phasor* env;
static VCO* vco;
static SVF* filter;
static VCA* filterGain;
static VCA* gain;
static VCA* destination;

void App_setup()
{
  audioContext = AudioContext_create(SAMPLE_RATE, NUMBER_OF_CHANNELS);

  env = Phasor_create(audioContext);
  env->frequency->value = -0.125;

  vco = VCO_create(audioContext);
  vco->frequency->value = 75.0;

  filter = SVF_create(audioContext);
  filter->q->value = 0.5;

  filterGain = VCA_create(audioContext);
  filterGain->gain->value = 2000.0;

  gain = VCA_create(audioContext);

  destination = VCA_create(audioContext);
  destination->gain->value = 0.25;

  connect(vco->saw, filter->input);
  connect(filter->low, gain->input);

  connect(env->phase, gain->gain);
  connect(env->phase, filterGain->input);
  connect(filterGain->output, filter->frequency);

  connect(gain->output, destination->input);

  connect(destination->output, audioContext->inlets[0]);
  connect(destination->output, audioContext->inlets[1]);
}

void App_destroy()
{
}

unsigned int App_shouldStop()
{
  return 0;
  //return audioContext->sampleNumber > SAMPLE_RATE * 5;
}

double* App_calc()
{
  unsigned long t = audioContext->sampleNumber;
  if (t % (audioContext->samplerate / 12) == 0) {
    double pitch = rnd();
    pitch = fabs(pitch);
    double pitch2 = 1.0 - pitch;
    pitch = pow(2.0, pitch * 4.0) * 200.0;
    vco->frequency->value = pitch;

    pitch2 = pow(2.0, pitch2 * 4.0) * 200;
    filterGain->gain->value = pitch2 * 4.0;
  }

  return AudioContext_calc(audioContext);
}
