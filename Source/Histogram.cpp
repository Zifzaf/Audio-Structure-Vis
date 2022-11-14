/*
  ==============================================================================

    Histogram.cpp
    Created: 20 Oct 2022 9:08:59am
    Author:  andreas

  ==============================================================================
*/

#include "Histogram.h"

Histogram ::Histogram(int dataLevels) : dataLevels(dataLevels)
{
  data = new float[dataLength * dataLevels];
  juce::FloatVectorOperations::fill(data, 0.0, dataLength * dataLevels);
  heightBinBorders = new int[dataLevels + 1];
  dataReady.set(true);
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

  heightBorderValues = new float[dataLevels + 1];
}

Histogram ::~Histogram()
{
  delete heightBorderValues;
}

inline bool Histogram::overlap(int startA, int endA, int startB, int endB)
{
  return (startA <= startB && endA >= startB) || (startB <= startA && endB >= startA);
}

void Histogram::mouseUp(const juce::MouseEvent &event)
{
  auto relEvent = event.getEventRelativeTo(this);
  selction[0] = relEvent.getMouseDownX() + viewer.getViewPositionX();
  selction[2] = relEvent.getMouseDownY() + viewer.getViewPositionY();
  selction[1] = relEvent.x + viewer.getViewPositionX();
  selction[3] = relEvent.y + viewer.getViewPositionY();

  redrawImage();
  updateImage();
}

void Histogram::setZoom(double newZoom)
{
  zoom = newZoom;
  recalculateImage();
  updateImage();
}

double Histogram::getZoom()
{
  return zoom;
}

void Histogram::zoomInClicked()
{
  zoom = zoom + 0.1;
  recalculateImage();
  updateImage();
}

void Histogram::zoomOutClicked()
{
  zoom = std::max(zoom - 0.1, 0.1);
  recalculateImage();
  updateImage();
}

void Histogram::setVerticalLines(bool in)
{
  verticalLines = in;
}

void Histogram::setHorizontalLines(bool in)
{
  horizontalLines = in;
}

void Histogram::setVerticalLables(bool in)
{
  verticalLables = in;
}

void Histogram::setHorizontalLables(bool in)
{
  horizontalLables = in;
}

void Histogram::getSelection(float *selectionOut, bool borderValuesSet)
{
  int indexStartX = 0;
  int indexStartY = 0;
  int indexEndX = 0;
  int indexEndY = 0;
  for (auto i = 0; i < dataLevels; i++)
  {
    if (heightBinBorders[i] <= selction[2] && selction[2] < heightBinBorders[i + 1])
    {
      indexStartY = i;
    }
    if (heightBinBorders[i] < selction[3] && selction[3] <= heightBinBorders[i + 1])
    {
      indexEndY = i + 1;
    }
  }
  for (auto i = 0; i < widthBins; i++)
  {
    if (widthBinBorders[i] <= selction[0] && selction[0] < widthBinBorders[i + 1])
    {
      indexStartX = i;
    }
    if (widthBinBorders[i] < selction[1] && selction[1] <= widthBinBorders[i + 1])
    {
      indexEndX = i + 1;
    }
  }
  if (borderValuesSet && widthBorderValuesSet && heightBorderValuesSet)
  {
    selectionOut[0] = widthBorderValues[indexStartX];
    selectionOut[1] = widthBorderValues[indexEndX];
    selectionOut[2] = heightBorderValues[indexStartY];
    selectionOut[3] = heightBorderValues[indexEndY];
  }
  else
  {
    selectionOut[0] = indexStartX;
    selectionOut[1] = indexEndX;
    selectionOut[2] = indexStartY;
    selectionOut[3] = indexEndY;
  }
}

juce::Colour Histogram::levelToColour(float level, bool selection)
{
  if (selection)
  {
    return juce::Colour::fromHSV((1.0 - level) / 9.0 + 0.33, 0.9, level, 1.0);
  }
  return juce::Colour::fromHSV((1.0 - level) / 9.0 + 0.66, 0.9, level, 1.0);
}

void Histogram::addDataLine(const float dataLineIN[], bool nomalized)
{
  std::cout << "NOT IMPLEMENTED!" << std::endl;
  /*
  float dataLine[dataLevels];
  juce::FloatVectorOperations::copy(dataLine, dataLineIN, dataLevels);
  if (!nomalized)
  {
    auto minMax = juce::FloatVectorOperations::findMinAndMax(dataLine, dataLevels);
    juce::FloatVectorOperations::add(dataLine, -minMax.getStart(), dataLevels);
    juce::FloatVectorOperations::multiply(dataLine, 1.0 / (minMax.getEnd() - minMax.getStart()), dataLevels);
  }
  histogramImage.moveImageSection(0, 0, levelWidth, 0, widthAvailable - levelWidth, heightAvailable);
  for (auto j = widthAvailable - levelWidth; j < widthAvailable; ++j)
  {
    for (auto k = 0; k < heightAvailable; ++k)
    {
      histogramImage.setPixelAt(j, k, levelToColour(dataLine[heightDataMap[k]]));
    }
  }
  dataEdit.enter();
  dataReady.set(false);
  for (auto i = dataLength - 1; i > 0; --i)
  {
    std::memcpy(&data[i * dataLevels], &data[(i - 1) * dataLevels], sizeof(float) * dataLevels);
  }
  std::memcpy(data, dataLine, sizeof(float) * dataLevels);
  dataReady.set(true);
  dataEdit.exit();
  */
}

void Histogram ::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void Histogram ::paintOverChildren(juce::Graphics &g)
{
  if (horizontalLables && heightBorderValuesSet)
  {
    g.setColour(juce::Colours::grey);
    for (auto i = 1; i < dataLevels; i++)
    {
      g.drawText(std::to_string((int)heightBorderValues[i]), 12, heightBinBorders[i] - 10, 100, 10, juce::Justification::bottom, false);
    }
  }
}

void Histogram::processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop)
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

void Histogram::replaceData(const float *inData, size_t inDataLength, bool normalized, bool logScale, float *widthBorderValues, float *heightBorderValues)
{
  auto newData = new float[inDataLength * dataLevels];
std:
  memcpy(newData, inData, sizeof(float) * inDataLength * dataLevels);
  if (!normalized)
  {
    processDataArray(newData, dataLevels * inDataLength, 3.0, 3.0);
  }
  if (logScale)
  {
    juce::FloatVectorOperations::add(newData, 1.0, dataLevels * inDataLength);
    for (auto i = 0; i < dataLevels * inDataLength; i++)
    {
      newData[i] = std::log10(newData[i]);
    }
    auto minMax = juce::FloatVectorOperations::findMinAndMax(newData, dataLevels * inDataLength);
    juce::FloatVectorOperations::multiply(newData, 1.0 / minMax.getEnd(), dataLevels * inDataLength);
  }
  float *temp = data;
  dataReady.set(false);
  data = newData;
  dataLength = inDataLength;
  if (widthBorderValues != NULL)
  {
    widthBorderValuesSet = true;
    delete this->widthBorderValues;
    this->widthBorderValues = new float[dataLength + 1];
    juce::FloatVectorOperations::copy(this->widthBorderValues, widthBorderValues, dataLength + 1);
  }
  if (heightBorderValues != NULL)
  {
    heightBorderValuesSet = true;
    for (auto i = 0; i < dataLevels + 1; i++)
    {
      this->heightBorderValues[i] = heightBorderValues[dataLevels - i];
    }
  }
  dataReady.set(true);
  delete temp;
  recalculateImage();
  updateImage();
}

void Histogram::recalculateImage()
{
  if (dataReady.get())
  {
    int pixelPerLevel = (heightAvailable - 40) / dataLevels;
    int pixelLeftOver = (heightAvailable - 40) % dataLevels;
    heightBinBorders[0] = 0;
    for (auto i = 1; i < dataLevels - pixelLeftOver; i++)
    {
      heightBinBorders[i] = i * pixelPerLevel;
    }
    for (auto i = dataLevels - pixelLeftOver; i < dataLevels; i++)
    {
      heightBinBorders[i] = (dataLevels - pixelLeftOver) * pixelPerLevel + (i - (dataLevels - pixelLeftOver)) * (pixelPerLevel + 1);
    }
    heightBinBorders[dataLevels] = (heightAvailable - 40);
    levelWidth = zoom * pixelPerLevel;
    widthBins = std::max(widthAvailable / levelWidth + 1, dataLength);
    delete widthBinBorders;
    widthBinBorders = new int[widthBins + 1];
    for (auto i = 0; i < widthBins + 1; i++)
    {
      widthBinBorders[i] = i * levelWidth;
    }
    histogramImage = juce::Image(juce::Image::RGB, widthBins * levelWidth, heightAvailable, true);
    redrawImage();
  }
}

void Histogram::redrawImage()
{
  int spaceAvailable = std::min(dataLength, widthBins);
  juce::Graphics g(histogramImage);
  g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  g.fillRect(widthBinBorders[0], heightAvailable - 40, widthBinBorders[widthBins] - widthBinBorders[0], 40);
  for (auto i = 0; i < spaceAvailable; ++i)
  {
    int lowerBorderW = widthBinBorders[i];
    int upperBorderW = widthBinBorders[i + 1];
    for (auto j = 0; j < dataLevels; j++)
    {
      int lowerBorderH = heightBinBorders[j];
      int upperBorderH = heightBinBorders[j + 1];
      float level = data[i * dataLevels + (dataLevels - (j + 1))];
      if (overlap(selction[0], selction[1], lowerBorderW, upperBorderW) && overlap(selction[2], selction[3], lowerBorderH, upperBorderH))
      {
        g.setColour(levelToColour(level, true));
      }
      else
      {
        g.setColour(levelToColour(level));
      }
      g.fillRect(lowerBorderW, lowerBorderH, upperBorderW - lowerBorderW, upperBorderH - lowerBorderH);
    }
  }
  if (widthBins > dataLength)
  {
    g.setColour(juce::Colours::black);
    g.drawRect(widthBinBorders[dataLength], 0, widthBins * levelWidth - widthBinBorders[dataLength], heightAvailable);
  }
  if (horizontalLines)
  {
    g.setColour(juce::Colours::grey);
    for (auto i = 1; i < dataLevels; i++)
    {
      g.fillRect(widthBinBorders[0], heightBinBorders[i], widthBinBorders[dataLength] - widthBinBorders[0], 1);
    }
  }
  if ((verticalLables || verticalLines) && widthBorderValuesSet)
  {
    float range = widthBorderValues[dataLength] - widthBorderValues[0];
    double step = 0.000000001;
    float maxMarkers = (widthBinBorders[dataLength] - widthBinBorders[0]) / 80;
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
    float pixelBetweenMarkers = (widthBinBorders[dataLength] - widthBinBorders[0]) / numMarkers;
    double rangePerPixel = range / (double)(widthBinBorders[dataLength] - widthBinBorders[0]);
    std::cout << -widthBorderValues[0] / rangePerPixel << std::endl;
    for (float i = -widthBorderValues[0] / rangePerPixel; i < widthBinBorders[dataLength]; i = i + pixelBetweenMarkers)
    {
      if (verticalLines)
      {
        g.setColour(juce::Colours::whitesmoke);
        g.fillRect((int)i, 0, 1, heightAvailable - 35);
      }
      if (verticalLables)
      {
        int k = (i - (float)widthBinBorders[0]) / pixelBetweenMarkers;
        g.setColour(juce::Colours::white);
        g.drawText(std::to_string(stepInUnit * k) + " " + unit, (int)i - 40, heightAvailable - 35, 80, 35, juce::Justification::centredTop, false);
      }
    }
  }
}

void Histogram::updateImage()
{
  juce::ImageComponent *image = new juce::ImageComponent();
  image->setBounds(0, 0, widthBins * levelWidth, heightAvailable);
  image->setImage(histogramImage);
  image->addMouseListener(this, true);
  auto current = viewer.getViewPosition();
  viewer.setViewedComponent(image, true);
  viewer.setViewPosition(current);
}

void Histogram::resized()
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
