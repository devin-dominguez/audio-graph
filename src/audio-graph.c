#include <stdlib.h>
#include "audio-graph.h"

void connect(Outlet* outlet, Inlet* inlet)
{
  OutletList_append(inlet->connectedOutlets, outlet);
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
// OutletListNode /////////////////////////////////////////
///////////////////////////////////////////////////////////
OutletListNode* OuletListNode_create(Outlet* outlet)
{
  OutletListNode* outletListNode = (OutletListNode*) malloc(sizeof(OutletListNode));
  outletListNode->outlet = outlet;
  outletListNode->next = NULL;

  return outletListNode;
}

void OutletListNode_destroy(struct OutletListNode* self)
{
  free(self);
}

///////////////////////////////////////////////////////////
// OutletList /////////////////////////////////////////////
///////////////////////////////////////////////////////////
OutletList* OutletList_create()
{
  OutletList* outletList = (OutletList*) malloc(sizeof(OutletList));
  outletList->head = NULL;

  return outletList;
}

void OutletList_destroy(struct OutletList* self)
{
  OutletListNode* currentNode = self->head;

  while(currentNode) {
    OutletListNode* temp = currentNode;
    currentNode = currentNode->next;
    free(temp);
  }

  free(self);
}

OutletListNode* OutletList_getTail(OutletList* self)
{
  OutletListNode* currentNode = self->head;
  OutletListNode* previousNode = NULL;

  while(currentNode) {
    previousNode = currentNode;
    currentNode = currentNode->next;
  }

  return previousNode;
}

void OutletList_append(OutletList* self, Outlet* outlet)
{
  OutletListNode* node = OuletListNode_create(outlet);
  if (!self->head) {
    self->head = node;
  } else {
    OutletList_getTail(self)->next = node;
  }
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
  inlet->connectedOutlets = OutletList_create();
  inlet->value = 0.0;

  return inlet;
}

void Inlet_destroy(Inlet* self)
{
  OutletList_destroy(self->connectedOutlets);
  free(self);
}

double Inlet_calc(struct Inlet* self)
{
  double value = 0.0;
  OutletListNode* node = self->connectedOutlets->head;
  while (node) {
    value += Outlet_calc(node->outlet);
    node = node->next;
  }

  return value + self->value;
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

