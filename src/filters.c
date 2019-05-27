#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "audio-graph.h"
#include "filters.h"

///////////////////////////////////////////////////////////
// SVF ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
// adapted from https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
// all credit to Andy Simper because filter math may as well be magic to me
static void SVF_dsp(UGen* ugen)
{
  SVF* self = (SVF*) ugen;
  double cutoff = Inlet_calc(self->frequency);
  double q = clamp(Inlet_calc(self->q), 0.0, 1.0);
  double g = tan(M_PI * cutoff / self->ugen.audioContext->samplerate);
  double k = 2.0 - 2.0 * q;

  double a1 = 1.0 / (1.0 + g * (g+k));
  double a2 = g * a1;

  double v0 = Inlet_calc(self->input);
  double v1 = a1 * self->ic1eq + a2 * (v0 - self->ic2eq);
  double v2 = self->ic2eq + g * v1;

  self->ic1eq = 2 * v1 - self->ic1eq;
  self->ic2eq = 2 * v2 - self->ic2eq;

  double low = v2;
  double band = v1;
  double high = v0 - k * v1 - v2;
  double notch = low + high;
  double peak = low - high;
  double all = low + high - k * band;

  self->low->value = low;
  self->band->value = band;
  self->high->value = high;
  self->notch->value = notch;
  self->peak->value = peak;
  self->all->value = all;
}

SVF* SVF_create(AudioContext* audioContext)
{
  SVF* self = (SVF*) malloc(sizeof(SVF));
  self->ugen = UGen_create(audioContext);
  self->ugen.dsp = SVF_dsp;

  self->input = Inlet_create();
  self->frequency = Inlet_create();
  self->q = Inlet_create();

  self->low = Outlet_create(&self->ugen);
  self->high = Outlet_create(&self->ugen);
  self->band = Outlet_create(&self->ugen);
  self->notch = Outlet_create(&self->ugen);
  self->peak = Outlet_create(&self->ugen);
  self->all = Outlet_create(&self->ugen);

  self->ic1eq = 0.0;
  self->ic2eq = 0.0;

  return self;
}

void SVF_destroy(SVF* self)
{
  Inlet_destroy(self->input);
  Inlet_destroy(self->frequency);
  Inlet_destroy(self->q);

  Outlet_destroy(self->low);
  Outlet_destroy(self->high);
  Outlet_destroy(self->band);
  Outlet_destroy(self->notch);
  Outlet_destroy(self->peak);
  Outlet_destroy(self->all);

  UGen_destroy(&self->ugen);

  free(self);
}
