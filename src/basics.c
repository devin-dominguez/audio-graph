#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "basics.h"

///////////////////////////////////////////////////////////
// Phasor /////////////////////////////////////////////////
///////////////////////////////////////////////////////////
static void Phasor_dsp(UGen* ugen)
{
  Phasor* self = (Phasor*) ugen;

  double frequency = Inlet_calc(self->frequency);
  double phaseIncrement = frequency / (double) self->ugen.audioContext->samplerate;

  self->_phase += phaseIncrement;
  self->_phase = wrap(self->_phase);

  self->phase->value = self->_phase;
}

Phasor* Phasor_create(AudioContext* audioContext)
{
  Phasor* self = (Phasor*) malloc(sizeof(Phasor));
  self->ugen = UGen_create(audioContext);
  self->ugen.dsp = &Phasor_dsp;

  self->_phase = 0.0;

  self->frequency = Inlet_create();
  self->phase = Outlet_create(&self->ugen);

  return self;
}

void Phasor_destroy(Phasor* self)
{
  Inlet_destroy(self->frequency);
  Outlet_destroy(self->phase);
  UGen_destroy(&self->ugen);

  free(self);
}

///////////////////////////////////////////////////////////
// VCA ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
static void VCA_dsp(UGen* ugen)
{
  VCA* self = (VCA*) ugen;

  double input = Inlet_calc(self->input);
  double gain = Inlet_calc(self->gain);

  self->output->value = input * gain;
}

VCA* VCA_create(AudioContext* audioContext)
{
  VCA* self = (VCA*) malloc(sizeof(VCA));
  self->ugen = UGen_create(audioContext);
  self->ugen.dsp = &VCA_dsp;

  self->input = Inlet_create();
  self->gain = Inlet_create();
  self->output = Outlet_create(&self->ugen);;

  return self;
}

void VCA_destroy(VCA* self)
{
  Inlet_destroy(self->input);
  Inlet_destroy(self->gain);
  Outlet_destroy(self->output);
  UGen_destroy(&self->ugen);

  free(self);
}
