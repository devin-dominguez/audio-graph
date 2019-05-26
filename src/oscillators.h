#pragma once

#include "audio-graph.h"
#include "basics.h"

struct VCO;

///////////////////////////////////////////////////////////
// VCO ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
typedef struct VCO {
  UGen ugen;

  Phasor* phasor;

  Inlet* frequency;
  Inlet* pwm;
  Outlet* saw;
  Outlet* pulse;
  Outlet* sine;
} VCO;

VCO* VCO_create(AudioContext* audioContext);
void VCO_destroy(VCO* self);
