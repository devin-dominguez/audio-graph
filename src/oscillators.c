#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "oscillators.h"
#include "basics.h"

///////////////////////////////////////////////////////////
// VCO ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
static double getPolyBlep(double phase, double dt)
{
  double polyBlep = 0.0;
  if (phase < dt) {
    double pdt = phase / dt;
    polyBlep = (pdt * 2.0) - (pdt * pdt) - 1.0;
  } else if (phase > 1.0 - dt) {
    double pdt = (phase - 1.0) / dt;
    polyBlep = (pdt * pdt) + (pdt * 2.0) + 1.0;
  }

  return polyBlep;
}

static void VCO_dsp(UGen* ugen)
{
  VCO* self = (VCO*) ugen;

  double pwmInput = clamp(Inlet_calc(self->pwm), -1.0, 1.0);
  double pwm = (pwmInput + 1.0) * 0.5;;

  double frequency = Inlet_calc(self->frequency);
  self->phasor->frequency->value = frequency;

  double phase = Outlet_calc(self->phasor->phase);
  double phase2 = wrap(phase + pwm);

  double dt = frequency / self->ugen.audioContext->samplerate;
  double polyBlep = getPolyBlep(phase, dt);
  double polyBlep2 = getPolyBlep(phase2, dt);

  double saw = phase * 2.0 - 1.0 - polyBlep;
  double saw2 = phase2 * 2.0 - 1.0 - polyBlep2;

  self->saw->value = saw;
  self->pulse->value = saw - saw2 + pwmInput;
  self->sine->value = sin(M_PI * 2.0 * phase);
}

VCO* VCO_create(AudioContext* audioContext)
{
  VCO* self = (VCO*) malloc(sizeof(VCO));
  self->ugen = UGen_create(audioContext);
  self->ugen.dsp = VCO_dsp;

  self->phasor = Phasor_create(audioContext);

  self->frequency = Inlet_create();
  self->pwm = Inlet_create();

  self->saw = Outlet_create(&self->ugen);;
  self->pulse = Outlet_create(&self->ugen);;
  self->sine = Outlet_create(&self->ugen);;

  return self;
}

void VCO_destroy(VCO* self)
{
  Inlet_destroy(self->frequency);
  Inlet_destroy(self->pwm);
  Outlet_destroy(self->saw);
  Outlet_destroy(self->pulse);
  Outlet_destroy(self->sine);

  UGen_destroy(&self->ugen);

  free(self);
}
