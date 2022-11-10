/*
  ==============================================================================

    Loudness.h
    Created: 1 Nov 2022 5:05:44pm
    Author:  andreas

  ==============================================================================
*/
#include <JuceHeader.h>

#pragma once

class Loudness
{
public:
  static double getScaleFactor(double phon, double freq);
  static double getLevel(double phon, double freq);

private:
  static void getLevelArray(double* levels, double phon);
  const static double f[];
  const static double af[];
  const static double Lu[];
  const static double Tf[];
};
