/*
  ==============================================================================

    Waveogram.cpp
    Created: 24 Nov 2022 8:30:59am
    Author:  andreas

  ==============================================================================
*/

#include "Waveogram.h"

Waveogram ::Waveogram()
{
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

  rawAudioData = NULL;
  rawAudioDataLength = 0;
  rawAudioDataBufferSize = 0;

  timeBinSize = 1024;
  frequencyBinNum = 32;

  fftOutput = NULL;
  fftBlockNum = 0;
  fftBlockSize = 0;
  fftBufferSize = 0;

  valueArray = NULL;
  valueArrayBufferSize = 0;

  frequencyBorderValues = NULL;

  zoom = 1.0;

  widthAvailable = 0;
  heightAvailable = 0;
  heightData = 0;
  xAxisSize = 28;

  verticalPixelMap = NULL;

  heightBinBorders = NULL;
  widthBinBorders = NULL;

  samplesPerPixel = 256;
  samplesPerPixelDefault = 256;

  selectionCoordinates[0] = 0;
  selectionCoordinates[1] = 0;
  selectionCoordinates[2] = 0;
  selectionCoordinates[3] = 0;

  horizontalLinesIn = false;
  horizontalLablesIn = false;
  verticalLables = false;

  dataAvailable = false;
  fftAvailable = false;
  valueArrayAvailable = false;
  imageCalculated = false;

  drawEllipse = false;

  normalizeFrequencyDim = false;
  normalizeTimeDim = false;

  scaleVertical = false;
  scaleHorizontal = false;

  threshhold = 0.0;
  clip = 1.0;

  levelBinNum = -1;
  levelBinLogScale = false;

  selectionInfo = true;

  loudnessCorrection = false;

  centered = true;

  frequencyLabels = false;

  yAxisSize = 40;
}

Waveogram ::~Waveogram()
{
  delete rawAudioData;
  delete fftOutput;
  delete valueArray;
  delete frequencyBorderValues;
  delete verticalPixelMap;
  delete heightBinBorders;
  delete widthBinBorders;
}

void Waveogram ::setRawAudio(const float *audioData, size_t audioDataLength, double audioDataSampleRate)
{
  // allocate buffer and move data to it
  dataAvailable = false;
  if (audioDataLength > rawAudioDataBufferSize)
  {
    delete rawAudioData;
    rawAudioData = new float[audioDataLength];
    rawAudioDataBufferSize = audioDataLength;
  }
  juce::FloatVectorOperations::copy(rawAudioData, audioData, audioDataLength);
  rawAudioDataLength = audioDataLength;
  sampleRate = audioDataSampleRate;

  fftAvailable = false;
  valueArrayAvailable = false;
  dataAvailable = true;

  auto start = std::chrono::high_resolution_clock::now();

  calculateFFT();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "FFT: " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();

  calculateValueArray();

  stop = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "ValueArray: " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();

  recalculateImage();

  updateImage();

  stop = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "Image: " << duration.count() << std::endl;
}

void Waveogram ::calculateFFT()
{
  if (dataAvailable.get())
  {
    calculateFFTBlockSize();

    // allocate buffer
    fftBlockNum = (int)std::ceil((double)rawAudioDataLength / (double)timeBinSize);
    if (fftBlockNum * fftBlockSize > fftBufferSize)
    {
      delete fftOutput;
      fftOutput = new kfr::complex<float>[fftBlockNum * fftBlockSize];
      fftBufferSize = fftBlockNum * fftBlockSize;
    }

    // prepare FFT
    kfr::dft_plan_real<float> dft(fftBlockSize);
    kfr::u8 *temp = new kfr::u8[dft.temp_size];
    juce::dsp::WindowingFunction<float> window(timeBinSize, juce::dsp::WindowingFunction<float>::kaiser, true, 1.0);
    float *audioDataBlock = new float[fftBlockSize];

    // calculate FFT
    for (int i = 0; i < fftBlockNum - 1; i++)
    {
      juce::FloatVectorOperations::fill(audioDataBlock, 0.0, fftBlockSize);
      juce::FloatVectorOperations::copy(audioDataBlock, &rawAudioData[i * timeBinSize], timeBinSize);
      window.multiplyWithWindowingTable(audioDataBlock, timeBinSize);
      dft.execute(&fftOutput[i * fftBlockSize], audioDataBlock, temp);
    }

    // do last iteration which doesn't have the full size
    juce::FloatVectorOperations::fill(audioDataBlock, 0.0, fftBlockSize);
    juce::FloatVectorOperations::copy(audioDataBlock, &rawAudioData[(fftBlockNum - 1) * timeBinSize], rawAudioDataLength - (fftBlockNum - 1) * timeBinSize);
    window.multiplyWithWindowingTable(audioDataBlock, timeBinSize);
    dft.execute(&fftOutput[(fftBlockNum - 1) * fftBlockSize], audioDataBlock, temp);

    delete temp;
    delete audioDataBlock;
    fftAvailable = true;
  }
}

void Waveogram ::calculateFFTBlockSize()
{
  // We use 9.5 Octaves, so calculate FFT size such that when breaking up
  // freq bins in factors based on log2 the lowest bin includes at least
  // one value
  double stepFactor = std::pow(2.0, 9.5 / (double)frequencyBinNum);
  double minBlockSize = sampleRate / (lowFreqCut * stepFactor - lowFreqCut);
  minBlockSize = std::pow(2.0, std::ceil(std::log2(minBlockSize)));
  // also at least use the size of the time bining as the FFT size
  double timeBinSizePower2 = timeBinSize;
  timeBinSizePower2 = std::pow(2.0, std::ceil(std::log2(timeBinSizePower2)));
  fftBlockSize = std::max((int)minBlockSize, (int)timeBinSizePower2);
}

void Waveogram ::calculateValueArray()
{
  if (fftAvailable.get())
  {
    // allocate valueArray
    if (frequencyBinNum * fftBlockNum > valueArrayBufferSize)
    {
      delete valueArray;
      valueArray = new float[frequencyBinNum * fftBlockNum];
      valueArrayBufferSize = frequencyBinNum * fftBlockNum;
    }

    calculateFrequencyBorders();

    int *fftBinMap = new int[fftBlockSize];
    float *loudnessCorrectionParameters = new float[fftBlockSize];
    double freqStep = sampleRate / fftBlockSize;
    // Calculation for binning
    auto index = 0;
    while (index * freqStep < frequencyBorderValues[0])
    {
      fftBinMap[index] = -1;
      loudnessCorrectionParameters[index] = 0.0;
      index++;
    }
    auto binNumber = 0;
    while (index * freqStep < frequencyBorderValues[frequencyBinNum])
    {
      if (index * freqStep < frequencyBorderValues[binNumber + 1])
      {
        fftBinMap[index] = binNumber;
      }
      else
      {
        binNumber++;
        fftBinMap[index] = binNumber;
      }
      loudnessCorrectionParameters[index] = Loudness::getScaleFactor(80.0, index * freqStep);
      index++;
    }
    for (auto i = index; i < fftBlockSize; i++)
    {
      fftBinMap[i] = -1;
      loudnessCorrectionParameters[index] = 0.0;
    }

    auto complexBins = new kfr::complex<double>[frequencyBinNum];

    for (auto i = 0; i < fftBlockNum; i++)
    {
      for (auto j = 0; j < frequencyBinNum; j++)
      {
        complexBins[j] = 0.0;
      }
      for (auto j = 0; j < fftBlockSize; j++)
      {
        if (fftBinMap[j] > -1)
        {
          if (loudnessCorrection)
          {
            complexBins[fftBinMap[j]] = complexBins[fftBinMap[j]] + loudnessCorrectionParameters[j] * fftOutput[i * fftBlockSize + j] * 1.0 / (double)fftBlockSize;
          }
          else
          {
            complexBins[fftBinMap[j]] = complexBins[fftBinMap[j]] + fftOutput[i * fftBlockSize + j] * 1.0 / (double)fftBlockSize;
          }
        }
      }
      for (auto j = 0; j < frequencyBinNum; j++)
      {
        valueArray[i * frequencyBinNum + j] = kfr::cabs(complexBins[j]);
      }
    }

    processDataArray(valueArray, frequencyBinNum * fftBlockNum, 0.0, 0.0);

    delete complexBins;
    delete fftBinMap;
    delete loudnessCorrectionParameters;
    valueArrayAvailable = true;
  }
}

void Waveogram ::calculateFrequencyBorders()
{
  // allocate frequencyBorderValues
  delete frequencyBorderValues;
  frequencyBorderValues = new float[frequencyBinNum + 1];
  // set given top and bottom fequencies
  frequencyBorderValues[0] = lowFreqCut;
  frequencyBorderValues[frequencyBinNum] = highFreqCut;
  // calcualte step for 9.5 octaves and intialise the rest
  double stepFactor = std::pow(2.0, 9.5 / (double)frequencyBinNum);
  for (auto i = 1; i < frequencyBinNum; i++)
  {
    frequencyBorderValues[i] = stepFactor * frequencyBorderValues[i - 1];
    // std::cout << frequencyBorderValues[i] << " : " << Loudness::getScaleFactor(80.0, frequencyBorderValues[i]) << std::endl;
  }
}

void Waveogram ::calculateVerticalPixelMap()
{
  delete verticalPixelMap;
  verticalPixelMap = new double[heightData];
  verticalPixelMap[0] = lowFreqCut;
  verticalPixelMap[heightData - 1] = highFreqCut;
  double range = (highFreqCut - lowFreqCut);

  double stepFactor = std::pow(2.0, 9.5 / (double)heightData);
  for (auto i = 1; i < heightData; i++)
  {
    verticalPixelMap[i] = std::pow(stepFactor, (double)i) * lowFreqCut;
  }
}

void Waveogram::setFrequencyBins(int newFrequencyBins)
{
  frequencyBinNum = newFrequencyBins;
}

void Waveogram::setTimeBinSize(int newTimeBinSize)
{
  timeBinSize = newTimeBinSize;
}

inline bool Waveogram::overlap(int startA, int endA, int startB, int endB)
{
  return (startA <= startB && endA >= startB) || (startB <= startA && endB >= startA);
}

void Waveogram::mouseUp(const juce::MouseEvent &event)
{
  auto relEvent = event.getEventRelativeTo(this);
  selectionCoordinates[0] = relEvent.getMouseDownX() + viewer.getViewPositionX() - yAxisSize;
  selectionCoordinates[2] = relEvent.getMouseDownY() + viewer.getViewPositionY();
  selectionCoordinates[1] = relEvent.x + viewer.getViewPositionX() - yAxisSize;
  selectionCoordinates[3] = relEvent.y + viewer.getViewPositionY();
  redrawImage();
  updateImage();
  repaint();
}

void Waveogram::setZoom(double newZoom)
{
  zoom = newZoom;
}

double Waveogram::getZoom()
{
  return zoom;
}

void Waveogram::zoomInClicked()
{
  zoom = zoom * 2.0;
  recalculateImage();
  updateImage();
}

void Waveogram::zoomOutClicked()
{
  zoom = std::max(zoom * 0.5, 1.0 / 32.0);
  recalculateImage();
  updateImage();
}

void Waveogram::setHorizontalLines(bool in)
{
  horizontalLinesIn = in;
}

void Waveogram::setHorizontalLables(bool in)
{
  horizontalLablesIn = in;
}

void Waveogram::setVerticalLables(bool in)
{
  verticalLables = in;
}

void Waveogram::setDrawEllipse(bool in)
{
  drawEllipse = in;
}

void Waveogram::setNormalizeFrequencyDim(bool in)
{
  normalizeFrequencyDim = in;
}

void Waveogram::setNormalizeTimeDim(bool in)
{
  normalizeTimeDim = in;
}

void Waveogram::setScaleVertical(bool in)
{
  scaleVertical = in;
}

void Waveogram::setScaleHorizontal(bool in)
{
  scaleHorizontal = in;
}

void Waveogram::setThreshhold(float in)
{
  threshhold = in;
}

void Waveogram::setClip(float in)
{
  clip = in;
}

void Waveogram::setLevelBinNum(int in)
{
  levelBinNum = in;
}

void Waveogram::setLevelBinLogScale(bool in)
{
  levelBinLogScale = in;
}

void Waveogram::setLoudnessCorrection(bool in)
{
  loudnessCorrection = in;
}

void Waveogram::setCentered(bool in)
{
  centered = in;
}

void Waveogram::setFrequencyLabels(bool in)
{
  frequencyLabels = in;
}

void Waveogram::redrawImageCall()
{
  redrawImage();
  updateImage();
}

void Waveogram::recalculateImageCall()
{
  recalculateImage();
  updateImage();
}

void Waveogram::calculateValueArrayCall()
{
  auto start = std::chrono::high_resolution_clock::now();

  calculateValueArray();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "ValueArray: " << duration.count() << std::endl;

  recalculateImage();
  updateImage();
}

void Waveogram::calculateFTTCall()
{
  auto start = std::chrono::high_resolution_clock::now();

  calculateFFT();

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "FFT: " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();

  calculateValueArray();

  stop = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "ValueArray: " << duration.count() << std::endl;

  start = std::chrono::high_resolution_clock::now();

  recalculateImage();

  updateImage();
}

void Waveogram::getSelection(float *selectionOut, int *selectionOutPixel)
{
  if (valueArrayAvailable.get())
  {
    int indexStartX = 0;
    int indexStartY = 0;
    int indexEndX = 0;
    int indexEndY = 0;
    for (auto i = 0; i < frequencyBinNum; i++)
    {
      if (heightBinBorders[i] <= selectionCoordinates[2] && selectionCoordinates[2] < heightBinBorders[i + 1])
      {
        indexStartY = i;
      }
      if (heightBinBorders[i] < selectionCoordinates[3] && selectionCoordinates[3] <= heightBinBorders[i + 1])
      {
        indexEndY = i + 1;
      }
    }
    for (auto i = 0; i < fftBlockNum; i++)
    {
      if (widthBinBorders[i] <= selectionCoordinates[0] && selectionCoordinates[0] < widthBinBorders[i + 1])
      {
        indexStartX = i;
      }
      if (widthBinBorders[i] < selectionCoordinates[1] && selectionCoordinates[1] <= widthBinBorders[i + 1])
      {
        indexEndX = i + 1;
      }
    }
    selectionOut[0] = indexStartX * timeBinSize / sampleRate;
    selectionOut[1] = indexEndX * timeBinSize / sampleRate;
    selectionOut[2] = frequencyBorderValues[frequencyBinNum - indexStartY];
    selectionOut[3] = frequencyBorderValues[frequencyBinNum - indexEndY];
    selectionOutPixel[0] = widthBinBorders[indexStartX];
    selectionOutPixel[1] = widthBinBorders[indexEndX];
    selectionOutPixel[2] = heightBinBorders[indexStartY];
    selectionOutPixel[3] = heightBinBorders[indexEndY];
  }
  else
  {

    selectionOut[0] = 0.0;
    selectionOut[1] = 0.0;
    selectionOut[2] = 0.0;
    selectionOut[3] = 0.0;
    selectionOutPixel[0] = 0;
    selectionOutPixel[1] = 0;
    selectionOutPixel[2] = 0;
    selectionOutPixel[3] = 0;
  }
}

juce::Colour Waveogram::levelToColour(float level, bool selection)
{
  if (level == 1.0)
  {
    if (selection)
    {
      return juce::Colour::fromRGB(48, 255, 48);
    }
    return juce::Colour::fromRGB(255, 85, 0);
  }
  if (level == 0.0)
  {
    if (selection)
    {
      return juce::Colour::fromRGB(48, 48, 48);
    }
    return juce::Colour::fromRGB(0, 64, 143);
  }
  if (selection)
  {
    return juce::Colour::fromRGB(96, 96 + level * (255 - 96), 96);
  }
  return juce::Colour::fromRGB(level * 255, 143, 255 - level * 255);
}

void Waveogram ::paint(juce::Graphics &g)
{
  // (Our component is opaque, so we must completely fill the background with a solid colour)
  g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void Waveogram ::paintOverChildren(juce::Graphics &g)
{
  if (frequencyLabels && imageCalculated.get())
  {
    float labelFrequencies[8] = {50.0, 100.0, 200.0, 500.0, 1000.0, 2000.0, 5000.0, 10000.0};
    g.setColour(juce::Colours::whitesmoke);
    int labelIndex = 0;
    for (auto i = 0; i < heightData; i++)
    {
      if (labelIndex < 8 && verticalPixelMap[i] > labelFrequencies[labelIndex])
      {
        g.drawText(std::to_string((int)labelFrequencies[labelIndex]), 0, (heightData - i) - 5, yAxisSize - 4, 10, juce::Justification::centredRight, false);
        g.fillRect(yAxisSize - 2, (heightData - i), std::min(widthAvailable - yAxisSize, widthBinBorders[fftBlockNum] - widthBinBorders[0]) + 2, 1);
        labelIndex++;
      }
    }
  }
  if (horizontalLablesIn && imageCalculated.get())
  {
    g.setColour(juce::Colours::whitesmoke);
    for (auto i = 1; i < frequencyBinNum; i++)
    {
      g.drawText(std::to_string((int)frequencyBorderValues[i]), 0, heightData - heightBinBorders[i] - 5, yAxisSize - 4, 10, juce::Justification::centredRight, false);
    }
  }
  if (selectionInfo)
  {
    float currentSelection[4];
    int currentSelectionPixel[4];
    getSelection(&currentSelection[0], &currentSelectionPixel[0]);
    g.setColour(juce::Colours::white);
    g.drawText("From: " + floatToString(currentSelection[0]) + " s", widthAvailable - 102, 26, 100, 20, juce::Justification::centredLeft, false);
    g.drawText("To: " + floatToString(currentSelection[1]) + " s", widthAvailable - 102, 46, 100, 20, juce::Justification::centredLeft, false);
    g.drawText("dT: " + floatToString((currentSelection[1] - currentSelection[0]) * 1000.0) + " ms", widthAvailable - 102, 66, 100, 20, juce::Justification::centredLeft, false);
    g.drawText("Low: " + std::to_string((int)currentSelection[3]) + " Hz", widthAvailable - 102, 86, 100, 20, juce::Justification::centredLeft, false);
    g.drawText("Heigh: " + std::to_string((int)currentSelection[2]) + " Hz", widthAvailable - 102, 106, 100, 20, juce::Justification::centredLeft, false);
  }
}

inline std::string Waveogram::floatToString(float a)
{
  std::string num_text = std::to_string(a);
  return num_text.substr(0, num_text.find(".") + 3);
}

void Waveogram::processDataArray(float *data, size_t len, double clipSTDBottom, double clipSTDTop)
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

void Waveogram::recalculateImage()
{
  if (valueArrayAvailable.get())
  {
    imageCalculated = false;
    delete heightBinBorders;
    heightBinBorders = new int[frequencyBinNum + 1];
    heightBinBorders[0] = 0;
    heightBinBorders[frequencyBinNum] = heightData;
    int indexBorder = 1;
    for (auto i = 0; i < heightData - 1; i++)
    {
      if (verticalPixelMap[i] > frequencyBorderValues[indexBorder])
      {
        heightBinBorders[indexBorder] = i;
        indexBorder++;
      }
    }

    delete widthBinBorders;
    widthBinBorders = new int[fftBlockNum + 1];
    samplesPerPixel = samplesPerPixelDefault * zoom;
    while (samplesPerPixel > timeBinSize / 4)
    {
      zoom = 0.5 * zoom;
      samplesPerPixel = samplesPerPixelDefault * zoom;
    }
    int widthBinSize = timeBinSize / samplesPerPixel;

    for (auto i = 0; i < fftBlockNum + 1; i++)
    {
      widthBinBorders[i] = i * widthBinSize;
    }
    WaveogramImage = juce::Image(juce::Image::RGB, widthBinSize * fftBlockNum, heightAvailable, true);
    imageCalculated = true;
    redrawImage();
  }
}

void Waveogram::redrawImage()
{
  if (imageCalculated.get())
  {

    juce::Graphics g(WaveogramImage);
    g.fillAll(juce::Colour::fromRGB(10, 10, 10));
    g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillRect(widthBinBorders[0], heightAvailable - xAxisSize, widthBinBorders[fftBlockNum] - widthBinBorders[0], 40);

    float currentSelection[4];
    int currentSelectionPixel[4];
    getSelection(&currentSelection[0], &currentSelectionPixel[0]);
    g.setColour(juce::Colours::white);
    g.drawRect(currentSelectionPixel[0], currentSelectionPixel[2], currentSelectionPixel[1] - currentSelectionPixel[0], currentSelectionPixel[3] - currentSelectionPixel[2]);

    float *levelBinBorderValues = NULL;
    if (levelBinNum > 0)
    {
      levelBinBorderValues = new float[levelBinNum + 1];
      levelBinBorderValues[0] = 0.0;
      if (levelBinLogScale)
      {
        double part = 1.0 / (std::exp2(levelBinNum + 1) - 1.0);
        for (auto i = 0; i < levelBinNum; i++)
        {
          double exponent = levelBinNum - i;
          levelBinBorderValues[i + 1] = part * std::exp2(exponent);
        }
        for (auto i = 1; i < levelBinNum + 1; i++)
        {
          levelBinBorderValues[i] += levelBinBorderValues[i - 1];
        }
      }
      else
      {
        double part = 1.0 / (double)levelBinNum;
        for (auto i = 0; i < levelBinNum; i++)
        {
          levelBinBorderValues[i + 1] = (i + 1) * part;
        }
      }
    }

    float *maxFrequencyBinValue = new float[frequencyBinNum];
    juce::FloatVectorOperations::fill(maxFrequencyBinValue, 0.0f, frequencyBinNum);
    for (auto i = 0; i < fftBlockNum; ++i)
    {
      for (auto j = 0; j < frequencyBinNum; j++)
      {
        maxFrequencyBinValue[j] = std::max(maxFrequencyBinValue[j], valueArray[i * frequencyBinNum + (frequencyBinNum - (j + 1))]);
      }
    }

    for (auto j = 0; j < frequencyBinNum; j++)
    {
      double rawTimeNomalizationValue = maxFrequencyBinValue[j];
      if (rawTimeNomalizationValue > clip)
      {
        rawTimeNomalizationValue = clip;
      }
      if (rawTimeNomalizationValue < threshhold)
      {
        rawTimeNomalizationValue = 0.0;
      }
      else
      {
        rawTimeNomalizationValue = rawTimeNomalizationValue - threshhold;
      }
      rawTimeNomalizationValue = rawTimeNomalizationValue / (clip - threshhold);
      maxFrequencyBinValue[j] = rawTimeNomalizationValue;
    }

    for (auto i = 0; i < fftBlockNum; ++i)
    {
      int lowerBorderW = widthBinBorders[i];
      int upperBorderW = widthBinBorders[i + 1];

      float maxColumn = juce::FloatVectorOperations::findMaximum(&valueArray[i * frequencyBinNum], frequencyBinNum);
      float frequencyNormalizationValue = 1.0;
      if (normalizeFrequencyDim)
      {
        double rawFrequencyNormalizationValue = maxColumn;
        if (rawFrequencyNormalizationValue > clip)
        {
          rawFrequencyNormalizationValue = clip;
        }
        if (rawFrequencyNormalizationValue < threshhold)
        {
          rawFrequencyNormalizationValue = 0.0;
        }
        else
        {
          rawFrequencyNormalizationValue = rawFrequencyNormalizationValue - threshhold;
        }
        rawFrequencyNormalizationValue = rawFrequencyNormalizationValue / (clip - threshhold);
        frequencyNormalizationValue = std::isfinite(1.0 / rawFrequencyNormalizationValue) ? 1.0 / rawFrequencyNormalizationValue : 0.0;
      }

      for (auto j = 0; j < frequencyBinNum; j++)
      {
        float timeNomalizationValue = 1.0;
        if (normalizeTimeDim)
        {
          timeNomalizationValue = std::isfinite(1.0 / maxFrequencyBinValue[j]) ? 1.0 / maxFrequencyBinValue[j] : 0.0;
        }
        int lowerBorderH = heightData - heightBinBorders[(frequencyBinNum - j)];
        int upperBorderH = heightData - heightBinBorders[(frequencyBinNum - (j + 1))];
        float level = valueArray[i * frequencyBinNum + (frequencyBinNum - (j + 1))];
        if (level > clip)
        {
          level = clip;
        }
        if (level < threshhold)
        {
          level = 0.0;
        }
        else
        {
          level = level - threshhold;
        }
        level = level / (clip - threshhold);
        double levelTimesFrequencyNormalizationValue = level * frequencyNormalizationValue;
        double levelTimesTimeNomalizationValue = level * timeNomalizationValue;
        if (levelBinNum > 0 && level > 0.0)
        {
          double part = 1.0 / (double)levelBinNum;
          for (auto i = 1; i < levelBinNum + 1; i++)
          {
            if (levelBinBorderValues[i - 1] < level && level <= levelBinBorderValues[i])
            {
              level = i * part;
            }
            if (levelBinBorderValues[i - 1] < levelTimesFrequencyNormalizationValue && levelTimesFrequencyNormalizationValue <= levelBinBorderValues[i])
            {
              levelTimesFrequencyNormalizationValue = i * part;
            }
            if (levelBinBorderValues[i - 1] < levelTimesTimeNomalizationValue && levelTimesTimeNomalizationValue <= levelBinBorderValues[i])
            {
              levelTimesTimeNomalizationValue = i * part;
            }
          }
        }

        if (overlap(selectionCoordinates[0], selectionCoordinates[1], lowerBorderW, upperBorderW) && overlap(selectionCoordinates[2], selectionCoordinates[3], lowerBorderH, upperBorderH))
        {
          g.setColour(levelToColour(levelTimesFrequencyNormalizationValue, true));
        }
        else
        {
          g.setColour(levelToColour(levelTimesFrequencyNormalizationValue));
        }

        float spaceW = (upperBorderW - lowerBorderW);
        float spaceH = (upperBorderH - lowerBorderH);
        float sizeW = spaceW - 2;
        float sizeH = spaceH - 2;
        if (scaleVertical && scaleHorizontal)
        {
          sizeW = (spaceW - 2) * sqrt(levelTimesFrequencyNormalizationValue);
          sizeH = (spaceH - 2) * sqrt(levelTimesTimeNomalizationValue);
        }
        else if (scaleVertical)
        {
          sizeW = spaceW - 2;
          sizeH = (spaceH - 2) * levelTimesTimeNomalizationValue;
        }
        else if (scaleHorizontal)
        {
          sizeW = (spaceW - 2) * level;
          sizeH = spaceH - 2;
        }
        if (centered)
        {
          if (drawEllipse)
          {
            g.fillEllipse(lowerBorderW + (spaceW - sizeW) / 2.0, lowerBorderH + (spaceH - sizeH) / 2.0, sizeW + 1, sizeH + 1);
          }
          else
          {
            g.fillRect(lowerBorderW + (spaceW - sizeW) / 2.0, lowerBorderH + (spaceH - sizeH) / 2.0, sizeW + 1, sizeH + 1);
          }
        }
        else
        {
          if (drawEllipse)
          {
            g.fillEllipse(lowerBorderW, lowerBorderH + (spaceH - sizeH), sizeW + 1, sizeH + 1);
          }
          else
          {
            g.fillRect((float)lowerBorderW, lowerBorderH + (spaceH - sizeH), sizeW + 1, sizeH + 1);
          }
        }
      }
    }
    if (horizontalLinesIn)
    {
      g.setColour(juce::Colours::grey);
      for (auto i = 1; i < frequencyBinNum; i++)
      {
        g.fillRect(widthBinBorders[0], heightData - heightBinBorders[i], widthBinBorders[fftBlockNum] - widthBinBorders[0], 1);
      }
    }
    if (verticalLables)
    {
      float range = (double)rawAudioDataLength / sampleRate;
      double step = 0.000000001;
      float maxMarkers = (widthBinBorders[fftBlockNum] - widthBinBorders[0]) / 80;
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
      float pixelBetweenMarkers = (widthBinBorders[fftBlockNum] - widthBinBorders[0]) / numMarkers;
      double rangePerPixel = range / (double)(widthBinBorders[fftBlockNum] - widthBinBorders[0]);
      for (float i = 0.0; i < widthBinBorders[fftBlockNum]; i = i + pixelBetweenMarkers)
      {
        g.setColour(juce::Colours::whitesmoke);
        g.fillRect((int)i, 0, 1, heightAvailable - xAxisSize + 5);
        int k = (i - (float)widthBinBorders[0]) / pixelBetweenMarkers;
        g.setColour(juce::Colours::white);
        g.drawText(std::to_string(stepInUnit * k) + " " + unit, (int)i - 40, heightAvailable - xAxisSize + 5, 80, 35, juce::Justification::centredTop, false);
      }
    }

    delete levelBinBorderValues;
    delete maxFrequencyBinValue;
  }
}

void Waveogram::updateImage()
{
  if (imageCalculated.get())
  {
    juce::ImageComponent *image = new juce::ImageComponent();
    image->setBounds(0, 0, widthBinBorders[fftBlockNum] - widthBinBorders[0], heightAvailable);
    image->setImage(WaveogramImage);
    image->addMouseListener(this, true);
    auto current = viewer.getViewPosition();
    viewer.setViewedComponent(image, true);
    viewer.setViewPosition(current);
    repaint();
  }
}

void Waveogram::resized()
{
  if (heightAvailable != getHeight())
  {
    heightAvailable = getHeight();
    widthAvailable = getWidth();
    heightData = heightAvailable - xAxisSize;
    calculateVerticalPixelMap();
    recalculateImage();
  }
  else
  {
    widthAvailable = getWidth();
  }
  viewer.setBounds(yAxisSize, 0, widthAvailable - yAxisSize, heightAvailable);
  viewer.setScrollBarsShown(false, true, false, true);
  viewer.setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::never);
  zoomIn.setBounds(widthAvailable - 44, 2, 20, 20);
  zoomOut.setBounds(widthAvailable - 22, 2, 20, 20);
  updateImage();
}
