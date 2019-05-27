#pragma once

#include "audio-graph.h"

struct Metro;

typedef void (*MetroCallback)(struct Metro* metro, void* data);
typedef struct Metro {
  UGen ugen;

  Inlet* interval;
  Inlet* active;
  Inlet* reset;

  Outlet* timer;

  unsigned long t;
  MetroCallback callback;
  void* userData;
} Metro;

Metro* Metro_create(AudioContext* audioContext, MetroCallback callback, void* userData);
void Metro_destroy(struct Metro* self);
