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
typedef void (*UGenDspCallback)(struct UGen* ugen);
typedef struct UGen {
  struct AudioContext* audioContext;
  unsigned long sampleNumber;
  UGenDspCallback dsp;
} UGen;

UGen UGen_create(struct AudioContext* audioContext);
void UGen_destroy(struct UGen* self);
void UGen_calc(struct UGen* self);

///////////////////////////////////////////////////////////
// Connection /////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct Connection {
  struct Outlet* outlet;
  struct Connection* next;
} Connection;

Connection* Connection_create(struct Outlet* outlet);
void Connection_destroy(struct Connection* self);
void Connection_append(struct Outlet* outlet, struct Inlet* inlet);
Connection* Connection_getTail(struct Connection* self);

///////////////////////////////////////////////////////////
// Outlet /////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct Outlet {
  double value;
  struct UGen* ugen;
} Outlet;

Outlet* Outlet_create(struct UGen* ugen);
void Outlet_destroy(struct Outlet* self);
double Outlet_calc(struct Outlet* self);

///////////////////////////////////////////////////////////
// Inlet //////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct Inlet {
  struct Connection* connectedOutlets;
  double value;
  double gain;
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
  Inlet* timer;
  double* sampleData;
} AudioContext;

AudioContext* AudioContext_create(unsigned int samplerate, unsigned int numChannels);
void AudioContext_destroy(struct AudioContext* self);
double* AudioContext_calc(struct AudioContext* self);
