#include <stdlib.h>
#include "timers.h"

static void Metro_dsp(UGen* ugen)
{
  Metro* self = (Metro*) ugen;
  double interval = Inlet_calc(self->interval);
  // TODO figure out the rammifications of this cast
  // See how pd does it
  unsigned int active = Inlet_calc(self->active);
  unsigned int reset = Inlet_calc(self->reset);

  if (reset > 0) {
    self->t = 0;
  }

  if (!(active > 0)) {
    return;
  }

  double msPerSample = 1000.0 / (double) ugen->audioContext->samplerate;
  double timeElapsed = (double) self->t * msPerSample;
  if (timeElapsed >= interval) {
    self->t = 0;
    self->callback(self, self->userData);
  }

  self->t++;
}

Metro* Metro_create(AudioContext* audioContext, MetroCallback callback, void* userData)
{
  Metro* self = (Metro*) malloc(sizeof(Metro));
  self->ugen = UGen_create(audioContext);
  self->ugen.dsp = Metro_dsp;
  self->callback = callback;
  self->userData = userData;

  self->interval = Inlet_create();
  self->active = Inlet_create();
  self->reset = Inlet_create();

  self->timer = Outlet_create(&self->ugen);

  self->t = 0;

  connect(self->timer, audioContext->timer);

  return self;
}

void Metro_destroy(struct Metro* self)
{
  UGen_destroy(&self->ugen);
  Inlet_destroy(self->interval);
  Inlet_destroy(self->active);
  Inlet_destroy(self->reset);
  Outlet_destroy(self->timer);

  free(self);
}
