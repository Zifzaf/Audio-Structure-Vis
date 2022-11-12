/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 8 Nov 2022 2:50:20pm
    Author:  andreas

  ==============================================================================
*/

#include "WaveformDisplay.h"

WaveformDisplay ::WaveformDisplay()
{
  data = new float[dataLength];
  juce::FloatVectorOperations::fill(data, 0.0, dataLength);
  dataReady.set(true);
}

WaveformDisplay ::~WaveformDisplay()
{
}

void WaveformDisplay::update()
{
  repaint();
}

void WaveformDisplay ::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  g.drawImage(WaveformDisplayImage, 0, 0, widthAvailable, heightAvailable, 0, 0, widthAvailable, heightAvailable);
}

void WaveformDisplay::processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop)
{
  if (clipSTDBottom == 0.0 && clipSTDTop == 0.0)
  {
    auto minMax = juce::FloatVectorOperations::findMinAndMax(data, len);
    juce::FloatVectorOperations::add(data, -minMax.getStart(), len);
    juce::FloatVectorOperations::multiply(data, 1.0 / (minMax.getEnd() - minMax.getStart()), len);
    return;
  }
  double part = 1.0 / (double)len;
  double mean = 0.0;
  for (auto i = 0; i < len; i++)
  {
    mean += part * data[i];
  }
  juce::FloatVectorOperations::add(data, -mean, len);
  double var = 0.0;
  for (auto i = 0; i < len; i++)
  {
    var += part * data[i] * data[i];
  }
  double sd = std::sqrt(var);
  juce::FloatVectorOperations::multiply(data, 1.0 / sd, len);
  if (clipSTDBottom == 0.0)
  {
    juce::FloatVectorOperations::clip(data, data, std::numeric_limits<float>::min(), clipSTDTop, len);
    auto minMax = juce::FloatVectorOperations::findMinAndMax(data, len);
    juce::FloatVectorOperations::add(data, -minMax.getStart(), len);
    juce::FloatVectorOperations::multiply(data, 1.0 / (minMax.getEnd() - minMax.getStart()), len);
    return;
  }
  if (clipSTDTop == 0.0)
  {
    juce::FloatVectorOperations::clip(data, data, -clipSTDBottom, std::numeric_limits<float>::max(), len);
    auto minMax = juce::FloatVectorOperations::findMinAndMax(data, len);
    juce::FloatVectorOperations::add(data, -minMax.getStart(), len);
    juce::FloatVectorOperations::multiply(data, 1.0 / (minMax.getEnd() - minMax.getStart()), len);
    return;
  }
  juce::FloatVectorOperations::clip(data, data, -clipSTDBottom, clipSTDTop, len);
  auto minMax = juce::FloatVectorOperations::findMinAndMax(data, len);
  juce::FloatVectorOperations::add(data, -minMax.getStart(), len);
  juce::FloatVectorOperations::multiply(data, 1.0 / (minMax.getEnd() - minMax.getStart()), len);
  return;
}

void WaveformDisplay::replaceData(const float *inData, size_t inDataLength, bool normalized)
{
  float *temp = new float[inDataLength];
  dataReady.set(false);
  juce::FloatVectorOperations::copy(temp, inData, inDataLength);
  if (!normalized)
  {
    processDataArray(temp, inDataLength, 0.0, 0.0);
  }
  float *swap = data;
  dataLength = inDataLength;
  data = temp;
  delete swap;
  dataReady.set(true);
  resized();
}

void WaveformDisplay::showRMS(bool val)
{
  showRMSVal = val;
}

void WaveformDisplay::resized()
{
  if (dataReady.get())
  {
    widthAvailable = getWidth();
    heightAvailable = getHeight();
    if (widthAvailable > 0 && heightAvailable > 0)
    {
      int samplesPerPixel = 1;
      int pixelPerSample = 1;
      if (dataLength > widthAvailable)
      {
        samplesPerPixel = dataLength / widthAvailable + 1;
      }
      else
      {
        pixelPerSample = widthAvailable / dataLength;
      }
      WaveformDisplayImage = juce::Image(juce::Image::RGB, widthAvailable, heightAvailable, true);
      float part = 1.0 / (float)samplesPerPixel;
      juce::Graphics g(WaveformDisplayImage);
      g.setColour(juce::Colours::darkgrey);
      g.fillRect(0, 0, widthAvailable, heightAvailable);
      float *meanArray = new float[widthAvailable];
      float *rmsArray = new float[widthAvailable];
      for (auto i = 0; i < widthAvailable; i++)
      {
        float mean = 0.0;
        float rms = 0.0;
        if ((i + 1) * samplesPerPixel < dataLength)
        {
          for (auto j = i * samplesPerPixel; j < (i + 1) * samplesPerPixel; j++)
          {
            mean += part * std::abs(data[j]);
            rms += part * data[j] * data[j];
          }
          rms = std::sqrt(rms);
        }
        else
        {
          if (dataLength - i * samplesPerPixel > 0.0)
          {
            float sPart = 1.0 / (float)(dataLength - i * samplesPerPixel);
            for (auto j = i * samplesPerPixel; j < dataLength; j++)
            {
              mean += sPart * std::abs(data[j]);
              rms += sPart * data[j] * data[j];
            }
            rms = std::sqrt(rms);
          }
          else
          {
            mean = 0.5;
            rms = 0.5;
          }
        }
        meanArray[i] = mean;
        rmsArray[i] = rms;
      }
      /*
      for(auto i = 0; i < widthAvailable; i++){
        meanArray[i] = std::log10(1.0 + meanArray[i]);
        rmsArray[i] = std::log10(1.0 + rmsArray[i]);
      }
      processDataArray(meanArray, widthAvailable, 0.0, 0.0);
      processDataArray(rmsArray, widthAvailable, 0.0, 0.0);
      */
      juce::FloatVectorOperations::add(meanArray, -0.33, widthAvailable);
      juce::FloatVectorOperations::clip(meanArray, meanArray, 0.0, 1.0, widthAvailable);
      juce::FloatVectorOperations::add(rmsArray, -0.33, widthAvailable);
      juce::FloatVectorOperations::clip(rmsArray, rmsArray, 0.0, 1.0, widthAvailable);
      for (auto i = 0; i < widthAvailable; i++)
      {

        if (showRMSVal)
        {
          int heightBar = heightAvailable * rmsArray[i];
          int startY = (heightAvailable - heightBar) / 2;
          g.setColour(juce::Colours::whitesmoke);
          g.fillRect(i, startY, pixelPerSample, heightBar);
        }
        else
        {
          int heightBar = heightAvailable * meanArray[i];
          int startY = (heightAvailable - heightBar) / 2;
          g.setColour(juce::Colours::whitesmoke);
          g.fillRect(i, startY, pixelPerSample, heightBar);
        }
      }
      delete meanArray;
      delete rmsArray;
    }
  }
}
