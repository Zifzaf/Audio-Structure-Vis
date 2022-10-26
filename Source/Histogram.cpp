/*
  ==============================================================================

    Histogram.cpp
    Created: 20 Oct 2022 9:08:59am
    Author:  andreas

  ==============================================================================
*/

#include "Histogram.h"

Histogram ::Histogram(int dataLevels) : dataLevels(dataLevels), fixedLengthData(false)
{
  data = new float[dataLength * dataLevels];
}

Histogram ::Histogram(float *inData, int dataLevels, int dataLength) : dataLevels(dataLevels), dataLength(dataLength), fixedLengthData(true)
{
  data = new float[dataLength * dataLevels];
std:
  memcpy(data, inData, sizeof(float) * dataLength * dataLevels);
}

Histogram ::~Histogram()
{
}

void Histogram::update()
{
  repaint();
}

void Histogram ::addDataLine(float dataLine[], bool nomalized)
{
  if (!nomalized)
  {
    auto minMax = juce::FloatVectorOperations::findMinAndMax(dataLine, dataLevels);
    juce::FloatVectorOperations::add(dataLine, -minMax.getStart(), dataLevels);
    juce::FloatVectorOperations::multiply(dataLine, 1.0 / (minMax.getEnd() - minMax.getStart()), dataLevels);
  }
  histogramImage.moveImageSection(levelWidth, 0, 0, 0, widthAvailable - levelWidth, heightAvailable);
  for (auto j = 0; j < levelWidth; ++j)
  {
    for (auto k = heightAvailable - 1; k >= 0; --k)
    {
      histogramImage.setPixelAt(j, k, juce::Colour::fromHSV(0.0f, 1.0f, dataLine[heightDataMap[k]], 1.0f));
    }
  }
  for (auto i = dataLength - 1; i > 0; --i)
  {
    std::memcpy(&data[i * dataLevels], &data[(i - 1) * dataLevels], sizeof(float) * dataLevels);
  }
  std::memcpy(data, dataLine, sizeof(float) * dataLevels);
}

void Histogram ::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  g.drawImage(histogramImage, 0, 0, widthAvailable, heightAvailable, 0, 0, widthAvailable, heightAvailable);
}

void Histogram::resized()
{

  widthAvailable = getWidth();
  heightAvailable = getHeight();
  histogramImage = juce::Image(juce::Image::RGB, widthAvailable, heightAvailable, true);
  delete heightDataMap;
  heightDataMap = new int[heightAvailable];
  int pixelPerLevel = heightAvailable / dataLevels;
  int pixelLeftOver = heightAvailable % dataLevels;
  int oldDataLength = dataLength;
  if (!fixedLengthData)
  {
    levelWidth = std::min(pixelPerLevel + 1, 20);
    dataLength = widthAvailable / levelWidth + 1;
  }
  else
  {
    levelWidth = std::max(widthAvailable / dataLength, 1);
  }
  int level = 0;
  int pixelsLeft = pixelPerLevel;
  for (auto i = 0; i < heightAvailable; i++)
  {
    if (pixelsLeft == 0)
    {
      if (pixelLeftOver > 0)
      {
        heightDataMap[i] = level;
        level++;
        pixelLeftOver--;
        pixelsLeft = pixelPerLevel;
      }
      else
      {
        level++;
        pixelsLeft = pixelPerLevel;
        heightDataMap[i] = level;
        pixelsLeft--;
      }
    }
    else
    {
      heightDataMap[i] = level;
      pixelsLeft--;
    }
  }
  int dataLengthCP = std::min(oldDataLength, dataLength);
  float *newData = new float[dataLevels * dataLength];
  std::memcpy(newData, data, sizeof(float) * dataLengthCP * dataLevels);
  float *temp = data;
  data = newData;
  delete temp;
  for (auto i = 0; i < dataLengthCP - 1; ++i)
  {
    for (auto j = 0; j < levelWidth; ++j)
    {
      for (auto k = heightAvailable - 1; k >= 0; --k)
      {
        int yCord = (i * levelWidth + j);
        histogramImage.setPixelAt(yCord, k, juce::Colour::fromHSV(0.0f, 1.0f, data[i * dataLevels + heightDataMap[k]], 1.0f));
      }
    }
  }
  int i = dataLengthCP - 1;
  for (auto j = 0; j < levelWidth; ++j)
  {
    for (auto k = heightAvailable - 1; k >= 0; --k)
    {
      int yCord = (i * levelWidth + j);
      if (yCord < widthAvailable)
      {
        histogramImage.setPixelAt(yCord, k, juce::Colour::fromHSV(0.0f, 1.0f, data[i * dataLevels + heightDataMap[k]], 1.0f));
      }
    }
  }
}
