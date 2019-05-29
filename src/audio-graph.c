#include <stdlib.h>
#include "audio-graph.h"

void connect(Outlet* outlet, Inlet* inlet)
{
  Connection_append(outlet, inlet);
}
///////////////////////////////////////////////////////////
// UGen ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////
UGen UGen_create(AudioContext* audioContext)
{
  UGen ugen = {
    .audioContext = audioContext,
    .sampleNumber = 0
  };

  return ugen;
}

void UGen_destroy(UGen* self)
{
  // probably safe to leave this empty
}

void UGen_calc(UGen* self)
{
  if (self->sampleNumber < self->audioContext->sampleNumber) {
    self->dsp(self);
    self->sampleNumber = self->audioContext->sampleNumber;
  }
}
///////////////////////////////////////////////////////////
// Connection /////////////////////////////////////////////
///////////////////////////////////////////////////////////
Connection* Connection_create(struct Outlet* outlet)
{
  Connection* self = (Connection*) malloc(sizeof(Connection));
  self->outlet = outlet;
  self->next = NULL;

  return self;
}

void Connection_destroy(struct Connection* self)
{
  free(self);
}

void Connection_append(struct Outlet* outlet, struct Inlet* inlet)
{
  Connection* connection = Connection_create(outlet);
  if (!inlet->connectedOutlets) {
    inlet->connectedOutlets = connection;
  } else {
    Connection_getTail(inlet->connectedOutlets)->next = connection;
  }
}

Connection* Connection_getTail(struct Connection* self)
{
  Connection* currentConnection = self;
  Connection* previousConnection = NULL;

  while(currentConnection) {
    previousConnection = currentConnection;
    currentConnection = currentConnection->next;
  }

  return previousConnection;
}

///////////////////////////////////////////////////////////
// Outlet /////////////////////////////////////////////////
///////////////////////////////////////////////////////////
Outlet* Outlet_create(UGen* ugen)
{
  Outlet* outlet = (Outlet*) malloc(sizeof(Outlet));
  outlet->ugen = ugen;
  outlet->value = 0.0;

  return outlet;
}

void Outlet_destroy(struct Outlet* self)
{
  free(self);
}

double Outlet_calc(Outlet* self)
{
  UGen_calc(self->ugen);

  return self->value;
}

///////////////////////////////////////////////////////////
// Inlet //////////////////////////////////////////////////
///////////////////////////////////////////////////////////
Inlet* Inlet_create()
{
  Inlet* inlet = (Inlet*) malloc(sizeof(Inlet));
  inlet->connectedOutlets = NULL;
  inlet->value = 0.0;
  inlet->gain = 1.0;

  return inlet;
}

void Inlet_destroy(Inlet* self)
{
  Connection* connection = self->connectedOutlets;

  while(connection) {
    Connection* temp = connection;
    connection = connection->next;
    Connection_destroy(temp);
  }

  free(self);
}

double Inlet_calc(struct Inlet* self)
{
  double value = 0.0;
  Connection* connection = self->connectedOutlets;
  while (connection) {
    value += Outlet_calc(connection->outlet);
    connection = connection->next;
  }

  return self->gain * value + self->value;
}

///////////////////////////////////////////////////////////
// AudioContext ///////////////////////////////////////////
///////////////////////////////////////////////////////////
AudioContext* AudioContext_create(unsigned int samplerate, unsigned int numChannels)
{
  AudioContext* self = (AudioContext*) malloc(sizeof(AudioContext));
  self->samplerate = samplerate;
  self->numChannels = numChannels;
  self->sampleNumber = 0;

  self->inlets = (Inlet**) malloc(numChannels * sizeof(Inlet*));
  self->timer = Inlet_create();
  self->sampleData = (double*) malloc(numChannels * sizeof(double));

  unsigned int i;
  for (i = 0; i < numChannels; i++) {
    self->inlets[i] = Inlet_create();
    self->sampleData[i] = 0.0;
  }

  return self;
}

void AudioContext_destroy(AudioContext* self)
{
  unsigned int i;
  for (i = 0; i < self->numChannels; i++) {
    Inlet_destroy(self->inlets[i]);
  }

  free(self->inlets);
  Inlet_destroy(self->timer);
  free(self->sampleData);
  free(self);
}

double* AudioContext_calc(AudioContext* self)
{
  self->sampleNumber++;
  // causes all connected timers to calc
  Inlet_calc(self->timer);
  unsigned int i;
  for (i = 0; i < self->numChannels; i++) {
    self->sampleData[i] = Inlet_calc(self->inlets[i]);
  }

  return self->sampleData;
}

