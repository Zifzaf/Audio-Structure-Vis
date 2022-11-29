/*
  ==============================================================================

    Loudness.cpp
    Created: 1 Nov 2022 5:05:44pm
    Author:  andreas

  ==============================================================================
*/

#include "Loudness.h"

const double Loudness::f[] = {20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500};
const double Loudness::af[] = {0.532, 0.506, 0.480, 0.455, 0.432, 0.409, 0.387, 0.367, 0.349, 0.330, 0.315, 0.301, 0.288, 0.276, 0.267, 0.259, 0.253, 0.250, 0.246, 0.244, 0.243, 0.243, 0.243, 0.242, 0.242, 0.245, 0.254, 0.271, 0.301};
const double Loudness::Lu[] = {-31.6, -27.2, -23.0, -19.1, -15.9, -13.0, -10.3, -8.1, -6.2, -4.5, -3.1, -2.0, -1.1, -0.4, 0.0, 0.3, 0.5, 0.0, -2.7, -4.1, -1.0, 1.7, 2.5, 1.2, -2.1, -7.1, -11.2, -10.7, -3.1};
const double Loudness::Tf[] = {78.5, 68.7, 59.5, 51.1, 44.0, 37.5, 31.5, 26.5, 22.1, 17.9, 14.4, 11.4, 8.6, 6.2, 4.4, 3.0, 2.2, 2.4, 3.5, 1.7, -1.3, -4.2, -6.0, -5.4, -1.5, 6.0, 12.6, 13.9, 12.3};

void Loudness::getLevelArray(double *levels, double phon)
{
  for (auto i = 0; i < 29; i++)
  {
    double Af = 4.47E-3 * (std::pow(10.0, (0.025 * phon)) - 1.15) + std::pow(0.4 * std::pow(10.0, (((Tf[i] + Lu[i]) / 10) - 9)), af[i]);
    levels[i] = ((10.0 / af[i]) * std::log10(Af)) - Lu[i] + 94;
  }
}

double Loudness::getScaleFactor(double phon, double freq)
{
  if (freq < 20.0)
  {
    freq = 20.0;
  }
  if (freq > 12500.0)
  {
    freq = 12500.0;
  }
  double levels[29];
  getLevelArray(&levels[0], phon);

  /*
  juce::FloatVectorOperations::multiply(levels, 0.1, 29);
  for (auto i = 0; i < 29; i++)
  {
    levels[i] = std::pow(10.0, levels[i]);
  }
  double base = levels[17];
  for (auto i = 0; i < 29; i++)
  {
    levels[i] = base / levels[i];
  }
  */

  int i = 0;
  while (f[i + 1] < freq)
  {
    i++;
  }
  double base = levels[17];

  for (auto i = 0; i < 29; i++)
  {
    levels[i] = 2 * base - levels[i];
  }
  double max = juce::FloatVectorOperations::findMaximum(levels, 29);
  double min = juce::FloatVectorOperations::findMinimum(levels, 29);
  // std::cout << levels[i] + (freq - f[i]) * (levels[i + 1] - levels[i]) / (f[i + 1] - f[i]) << std::endl;
  return (levels[i] + (freq - f[i]) * (levels[i + 1] - levels[i]) / (f[i + 1] - f[i])) / max;
}

double Loudness::getLevel(double phon, double freq)
{
  if (freq < 20.0)
  {
    freq = 20.0;
  }
  if (freq > 12500.0)
  {
    freq = 12500.0;
  }
  double levels[29];
  getLevelArray(&levels[0], phon);
  int i = 0;
  while (f[i + 1] < freq)
  {
    i++;
  }
  return levels[i] + (freq - f[i]) * (levels[i + 1] - levels[i]) / (f[i + 1] - f[i]);
}