#pragma once

struct Outlet;
struct Inlet;
struct AudioContext;
struct OutletList;
struct OutletListNode;
struct UGen;

void connect(struct Outlet* outlet, struct Inlet* inlet);
///////////////////////////////////////////////////////////
// UGen ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct UGen {
  struct AudioContext* audioContext;
  unsigned long sampleNumber;
  void (*dsp)(struct UGen*);
} UGen;

UGen UGen_create(struct AudioContext* audioContext);
void UGen_destroy(struct UGen* self);
void UGen_calc(UGen* self);

///////////////////////////////////////////////////////////
// OutletListNode /////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct OutletListNode {
  struct Outlet* outlet;
  struct OutletListNode* next;
} OutletListNode;

OutletListNode* OuletListNode_create(struct Outlet* outlet);
void OutletListNode_destroy(struct OutletListNode* self);

///////////////////////////////////////////////////////////
// OutletList /////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct OutletList {
  struct OutletListNode* head;
} OutletList;

OutletList* OutletList_create();
void OutletList_destroy(struct OutletList* self);
OutletListNode* OutletList_getTail(struct OutletList* self);
void OutletList_append(struct OutletList* self, struct Outlet* outlet);

///////////////////////////////////////////////////////////
// Outlet /////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct Outlet {
  double value;
  struct UGen* ugen;
} Outlet;

Outlet* Outlet_create(struct UGen* ugen);
void Outlet_destroy(struct Outlet* self);
void Outlet_connect(struct Outlet* self, struct Inlet* inlet);
double Outlet_calc(struct Outlet* self);

///////////////////////////////////////////////////////////
// Inlet //////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct Inlet {
  struct OutletList* connectedOutlets;
  double value;
} Inlet;

void Inlet_destroy(struct Inlet* self);
Inlet* Inlet_create();
double Inlet_calc(struct Inlet* self);

///////////////////////////////////////////////////////////
// AudioContext ///////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct AudioContext {
  unsigned int samplerate;
  unsigned long sampleNumber;
  unsigned int numChannels;
  struct Inlet** inlets;
  double* sampleData;
} AudioContext;

AudioContext* AudioContext_create(unsigned int samplerate, unsigned int numChannels);
void AudioContext_destroy(struct AudioContext* self);
double* AudioContext_calc(AudioContext* self);
