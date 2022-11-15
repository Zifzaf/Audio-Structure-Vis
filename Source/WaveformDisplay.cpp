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
  addAndMakeVisible(viewer);

  addAndMakeVisible(&zoomIn);
  zoomIn.setButtonText("+");
  zoomIn.onClick = [this]
  { zoomInClicked(); };
  zoomIn.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);

  addAndMakeVisible(&zoomOut);
  zoomOut.setButtonText("-");
  zoomOut.onClick = [this]
  { zoomOutClicked(); };
  zoomOut.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
}

WaveformDisplay ::~WaveformDisplay()
{
}

void WaveformDisplay ::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
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

void WaveformDisplay::replaceData(const float *inData, size_t inDataLength, bool normalized, double newSampleRate)
{
  sampleRate = newSampleRate;
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
  recalculateImage();
  updateImage();
}

void WaveformDisplay::showRMS(bool val)
{
  showRMSVal = val;
}

void WaveformDisplay::recalculateImage()
{
  int samplesPerPixel = zoom * 40;
  widthImage = dataLength / samplesPerPixel + 1;
  WaveformDisplayImage = juce::Image(juce::Image::RGB, widthImage, heightAvailable, true);
  double part = 1.0 / (double)samplesPerPixel;
  juce::Graphics g(WaveformDisplayImage);
  g.setColour(juce::Colours::darkgrey);
  g.fillRect(0, 0, widthImage, heightAvailable);
  delete meanArray;
  delete rmsArray;
  meanArray = new float[widthImage];
  rmsArray = new float[widthImage];
  for (auto i = 0; i < widthImage; i++)
  {
    double mean = 0.0;
    double rms = 0.0;
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
    { /*
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
      }*/
    }
    meanArray[i] = mean;
    rmsArray[i] = rms;
  }
  juce::FloatVectorOperations::add(meanArray, -0.001, widthImage);
  juce::FloatVectorOperations::clip(meanArray, meanArray, 0.0, 1.0, widthImage);
  juce::FloatVectorOperations::add(rmsArray, -0.001, widthImage);
  juce::FloatVectorOperations::clip(rmsArray, rmsArray, 0.0, 1.0, widthImage);
  for (auto i = 0; i < widthImage; i++)
  {
    meanArray[i] = 3.32 * std::log10(1.0 + meanArray[i]);
    rmsArray[i] = 3.32 * std::log10(1.0 + rmsArray[i]);
  }
  redrawImage();
}

void WaveformDisplay::updateImage()
{
  juce::ImageComponent *image = new juce::ImageComponent();
  image->setBounds(0, 0, widthImage, heightAvailable);
  image->setImage(WaveformDisplayImage);
  image->addMouseListener(this, true);
  auto current = viewer.getViewPosition();
  viewer.setViewedComponent(image, true);
  viewer.setViewPosition(current);
}

void WaveformDisplay::redrawImage()
{
  juce::Graphics g(WaveformDisplayImage);
  int heightWave = heightAvailable - xAxisSize;

  for (auto i = 0; i < widthImage; i++)
  {
    if (showRMSVal)
    {
      int heightBar = heightWave * rmsArray[i];
      int startY = (heightWave - heightBar) / 2;
      g.setColour(juce::Colours::blue);
      g.fillRect(i, startY, 1, heightBar);
    }
    else
    {
      int heightBar = heightWave * meanArray[i];
      int startY = (heightWave - heightBar) / 2;
      g.setColour(juce::Colours::blue);
      g.fillRect(i, startY, 1, heightBar);
    }
  }
  float range = dataLength * 1.0 / sampleRate;
  double step = 0.000000001;
  float maxMarkers = widthImage / 80;
  while (range / step > maxMarkers)
  {
    step *= 10;
  }
  std::string unit = "";
  int stepInUnit = 0;
  if (step < 0.000001)
  {
    unit = "ns";
    stepInUnit = step * 1000000000;
  }
  else if (step < 0.001)
  {
    unit = "us";
    stepInUnit = step * 1000000;
  }
  else if (step < 1.0)
  {
    unit = "ms";
    stepInUnit = step * 1000;
  }
  else
  {
    unit = "s";
    stepInUnit = step;
  }
  float numMarkers = range / step;
  float pixelBetweenMarkers = widthImage / numMarkers;
  for (float i = 0; i < widthImage; i = i + pixelBetweenMarkers)
  {
    g.setColour(juce::Colours::whitesmoke);
    g.fillRect((int)i, 0, 1, heightAvailable - xAxisSize + 5);
    int k = i / pixelBetweenMarkers;
    g.setColour(juce::Colours::white);
    g.drawText(std::to_string(stepInUnit * k) + " " + unit, (int)i - 40, heightAvailable - xAxisSize + 5, 80, 35, juce::Justification::centredTop, false);
  }
}

void WaveformDisplay::setZoom(double newZoom)
{
  zoom = newZoom;
  recalculateImage();
  updateImage();
}

double WaveformDisplay::getZoom()
{
  return zoom;
}

void WaveformDisplay::zoomInClicked()
{
  zoom *= 1.4;
  recalculateImage();
  updateImage();
}

void WaveformDisplay::zoomOutClicked()
{
  zoom = std::max(1.0 / 39.0, zoom * 1.0 / 1.4);
  recalculateImage();
  updateImage();
}

void WaveformDisplay::resized()
{
  if (dataReady.get())
  {
    if (heightAvailable != getHeight())
    {
      heightAvailable = getHeight();
      widthAvailable = getWidth();
      recalculateImage();
    }
    else
    {
      widthAvailable = getWidth();
    }
    viewer.setBounds(0, 0, widthAvailable, heightAvailable);
    viewer.setScrollBarsShown(false, true, false, true);
    viewer.setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::never);
    zoomIn.setBounds(widthAvailable - 44, 2, 20, 20);
    zoomOut.setBounds(widthAvailable - 22, 2, 20, 20);
    updateImage();
  }
}
