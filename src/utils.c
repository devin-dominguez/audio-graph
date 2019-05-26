#include "utils.h"

static unsigned int isRandomSeeded = 0;

double rnd()
{
  if (!isRandomSeeded) {
    srand(time(0));
    isRandomSeeded = 1;
  }
  double value = (double) rand() / (double) RAND_MAX;
  return value * 2 - 1;
}

double clamp(double value, double min, double max)
{
  if (value > max) {
    value = max;
  }

  if (value < min) {
    value = min;
  }

  return value;
}

double wrap(double value)
{
  while (value >= 1.0) {
    value -= 1.0;
  }

  while (value <= 0.0) {
    value += 1.0;
  }

  return value;
}

