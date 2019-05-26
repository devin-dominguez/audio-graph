#pragma once

#include "audio-graph.h"

struct SVF;
struct SKF;

///////////////////////////////////////////////////////////
// SVF ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct SVF {
  UGen ugen;

  Inlet* input;
  Inlet* frequency;
  Inlet* q;

  Outlet* low;
  Outlet* high;
  Outlet* band;
  Outlet* notch;
  Outlet* peak;
  Outlet* all;

  double ic1eq;
  double ic2eq;
} SVF;

SVF* SVF_create(AudioContext* audioContext);
void SVF_destroy(SVF* self);
