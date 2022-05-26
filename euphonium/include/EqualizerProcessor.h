#ifndef EUPHOINIUM_EQUALIZERPROCESSOR_H
#define EUPHOINIUM_EQUALIZERPROCESSOR_H

#include "AudioProcessors.h"
#include <cmath>
#include "BiquadFilter.h"
#include <memory>
#include <vector>
#include "BerryBind.h"
#include <functional>


using namespace std;

#define SAMPLE_FREQUENCY 44100.0 // Hz

#define EQUALIZER_LOW_SHELF_FREQUENCY 250   // Hz
#define EQUALIZER_NOTCH_FREQUENCY 1000      // Hz
#define EQUALIZER_HIGH_SHELF_FREQUENCY 4000 // Hz

#define DEFAULT_Q_FACTOR sqrt(2) / 2 // 0.7071067812
#define EQUALIZER_GAIN 3     // dB

#define MAX_INT16 32767

#define DOWN_MIX_TO_MONO 1

#define DEFAULT_CHANNEL_LENGTH 1024


class EqualizerProcessor : public AudioProcessor
{
private:
    uint32_t logVolume;

public:
    shared_ptr<BiquadFilter> lowShelf;
    shared_ptr<BiquadFilter> highShelf;
    shared_ptr<BiquadFilter> notch;

    //Voicing filters
    shared_ptr<BiquadFilter> lowEndProtectionFilter;
    shared_ptr<BiquadFilter> lowEndProtectionFilterNew;
    shared_ptr<BiquadFilter> hsNegative1;
    shared_ptr<BiquadFilter> hsNegative2;
    shared_ptr<BiquadFilter> hsNegative3;
    shared_ptr<BiquadFilter> hsPositive1;
    shared_ptr<BiquadFilter> peak1;
    shared_ptr<BiquadFilter> peak2;
    shared_ptr<BiquadFilter> peak3;
    shared_ptr<BiquadFilter> peak4;
    shared_ptr<BiquadFilter> peak5;
    shared_ptr<BiquadFilter> peak6;
    shared_ptr<BiquadFilter> peak7;
    shared_ptr<BiquadFilter> peak8;
    shared_ptr<BiquadFilter> allGain;


    //Crossover filters
    shared_ptr<BiquadFilter> lowPassCrossover;

    // A list/array of objects
    vector<reference_wrapper<shared_ptr<BiquadFilter>>> dspFilters;
    // = vector<reference_wrapper<shared_ptr<BiquadFilter>>>(15);
//    vector<reference_wrapper<BiquadFilter>> dspFilters;
//    vector<reference_wrapper<BiquadFilter>> crossoverFilterRight;
//    vector<reference_wrapper<BiquadFilter>> crossoverFilterLeft;
    vector<reference_wrapper<shared_ptr<BiquadFilter>>> crossoverFilterRight;
    vector<reference_wrapper<shared_ptr<BiquadFilter>>> crossoverFilterLeft;

    vector<float> dataLeft = vector<float>(DEFAULT_CHANNEL_LENGTH);
    vector<float> dataRight = vector<float>(DEFAULT_CHANNEL_LENGTH);

    EqualizerProcessor()
    {
        lowShelf = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
        highShelf = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
        notch = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);

        setBands(0, 0, 0);
        configureVoicing();
//        configureCrossover();
    };
    ~EqualizerProcessor(){};


    void processCrossover(vector<float>& channel_left, vector<float>& channel_right, int& channelLength) {

      // Process the right channel
      for (int i=0; i<crossoverFilterRight.size(); i++) {
        shared_ptr<BiquadFilter> filter = crossoverFilterRight[i];
        filter->processSamples(channel_right.data(), channelLength);
      }

      // Process the left channel
      for (int i=0; i<crossoverFilterRight.size(); i++) {
        shared_ptr<BiquadFilter> filter = crossoverFilterLeft[i];
        filter->processSamples(channel_left.data(), channelLength);
      }

    }

    void processVoicing(vector<float>& channelData, int& channelLength) {
      for (int i=0; i<dspFilters.size(); i++) {
        shared_ptr<BiquadFilter> filter = dspFilters[i];
        filter->processSamples(channelData.data(), channelLength);
      }

/*      for (int i=0; i<dspFilters.size(); i++) {
        BiquadFilter &filter = dspFilters[i];
        filter.processSamples(channelData.data(), channelLength);
      }*/
    }

    void setBands(float low, float mid, float high)
    {
        // configure filter, then add it to the filter array

/*
        //BiquadFilter lowShelf(SAMPLE_FREQUENCY);
        lowShelf->generateLowShelfCoEffs(EQUALIZER_LOW_SHELF_FREQUENCY, (low) * EQUALIZER_GAIN, DEFAULT_Q_FACTOR);
        dspFilters.push_back(lowShelf);
        //BiquadFilter notch(SAMPLE_FREQUENCY);
        notch->generateNotchCoEffs(EQUALIZER_NOTCH_FREQUENCY, (mid) * EQUALIZER_GAIN, DEFAULT_Q_FACTOR / 2);
        dspFilters.push_back(notch);
        //BiquadFilter highShelf(SAMPLE_FREQUENCY);
        highShelf->generateHighShelfCoEffs(EQUALIZER_HIGH_SHELF_FREQUENCY, (high) * EQUALIZER_GAIN, DEFAULT_Q_FACTOR);
        dspFilters.push_back(highShelf);
*/

        // Ad filters to be processed
        /*voicingFilters.push_back(lowShelf);
        voicingFilters.push_back(notch);
        voicingFilters.push_back(highShelf);
*/
        EUPH_LOG(info, "eq", "setBands: low: %.2f, mid: %.2f, high: %.2f", low, mid, high);
    }

    void configureVoicing()
    {

      // Initializing filters
      // How can be make this into a locally created variable?
      lowEndProtectionFilter  = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      hsNegative1             = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      hsNegative2             = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      hsNegative3             = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      hsPositive1             = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak1                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak2                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak3                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak4                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak5                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak6                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak7                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      peak8                   = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      allGain                 = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);

      // Set coeffs and add to processing list
      //BiquadFilter lowEndProtectionFilter(SAMPLE_FREQUENCY);
      lowEndProtectionFilter->generateHighPassCoEffs(45, DEFAULT_Q_FACTOR);
      dspFilters.push_back(lowEndProtectionFilter);
      //dspFilters.push_back(lowEndProtectionFilter); // let's add it again to provide a steeper slobe -> untill we have a proper way of performing a higher order filter


      // FIXME: this gain thing should be done differently
      allGain->generateHighShelfCoEffs(50, -5, DEFAULT_Q_FACTOR);
      dspFilters.push_back(allGain);

      // Voicing
      hsNegative1->generateHighShelfCoEffs(100, -10.0, DEFAULT_Q_FACTOR);
      dspFilters.push_back(hsNegative1);
      hsPositive1->generateHighShelfCoEffs(1800, 9, DEFAULT_Q_FACTOR);
      dspFilters.push_back(hsPositive1);
      hsNegative2->generateHighShelfCoEffs(3230, -3, DEFAULT_Q_FACTOR);
      dspFilters.push_back(hsNegative2);
      hsNegative3->generateHighShelfCoEffs(350, -0.75, DEFAULT_Q_FACTOR);
      dspFilters.push_back(hsNegative3);

      peak1->generatePeakCoEffs(78,8,3);
      dspFilters.push_back(peak1);
      peak2->generatePeakCoEffs(185,-3.8,2);
      dspFilters.push_back(peak2);
      peak3->generatePeakCoEffs(650,4,6);
      dspFilters.push_back(peak3);
      peak4->generatePeakCoEffs(529,-3.5,6);
      dspFilters.push_back(peak4);
      peak5->generatePeakCoEffs(1415,-2,2.5);
      dspFilters.push_back(peak5);
      peak6->generatePeakCoEffs(60,6,5);
      dspFilters.push_back(peak6);
      peak7->generatePeakCoEffs(3200,4,5);
      dspFilters.push_back(peak7);
      peak8->generatePeakCoEffs(6600,3,3);
      dspFilters.push_back(peak8);
    }

    void configureCrossover()
    {
      // How can be make this into a locally created variable?
      lowPassCrossover = make_shared<BiquadFilter>(SAMPLE_FREQUENCY);
      lowPassCrossover->generateLowPassCoEffs(3500, DEFAULT_Q_FACTOR);
      crossoverFilterRight.push_back(lowPassCrossover);
    }




    void process(uint8_t *data, size_t nBytes)
    {

        int16_t *data_16 = (int16_t *)data;

        // Why divide by 4?
        // And how long is it?
        // Debug info -> how long is this loop?
        // It is = 1024

        // And why not hardcode to 1024? As this is the length of the vector
        int channel_length_16 = nBytes / 4;



        // down mix
        //if (true) {
          for (size_t i = 0; i < channel_length_16; i++)
          {
            //EUPH_LOG(info, "eq", "process: processing element: %u", i);
            dataLeft[i] = ((data_16[i * 2] + data_16[i * 2 + 1]) / 2) / (float) MAX_INT16; // Normalize and sum
            dataRight[i] = dataLeft[i];
          }
          //dataLeft[i] = data_16[i * 2] / (float) MAX_INT16;
          //dataRight[i] = dataLeft[i];
        //}
        /*else {
          for (size_t i = 0; i < channel_length_16; i++)
          {
            dataLeft[i] = data_16[i * 2] / (float) MAX_INT16;      // Normalize left
            dataRight[i] = data_16[i * 2 + 1] / (float) MAX_INT16; // Normalize right
          }
        }*/
//        EUPH_LOG(info, "eq", "process: start processing voicing filters");

        // process samples
        // we need to read some config file here, as hardcoding all of this isn't great
        //processCrossover(dataLeft, dataRight, channel_length_16);

        // Process the left channel
        processVoicing(dataLeft, channel_length_16);
        // Process the right channel
        processVoicing(dataRight, channel_length_16);

        // Returning data to it's orignal format
        for (size_t i = 0; i < channel_length_16; i++)
        {
            // Denormalize
            data_16[i * 2] = dataLeft[i] * MAX_INT16;     // Denormalize left
            data_16[i * 2 + 1] = dataRight[i] * MAX_INT16; // Denormalize right
        }
    }


  /*
    void processOld(uint8_t *data, size_t nBytes)
    {

        int16_t *data_16 = (int16_t *)data;

        // Why divide by 4?
        // And how long is it?
        int channel_length_16 = nBytes / 4;

        for (size_t i = 0; i < channel_length_16; i++)
        {
            dataLeft[i] = data_16[i * 2] / (float) MAX_INT16;      // Normalize left
            dataRight[i] = data_16[i * 2 + 1] / (float) MAX_INT16; // Normalize right
        }

        lowShelfLeft->processSamples(dataLeft.data(), channel_length_16);
        lowShelfRight->processSamples(dataRight.data(), channel_length_16);

        notchLeft->processSamples(dataLeft.data(), channel_length_16);
        notchRight->processSamples(dataRight.data(), channel_length_16);

        highShelfLeft->processSamples(dataLeft.data(), channel_length_16);
        highShelfRight->processSamples(dataRight.data(), channel_length_16);

        for (size_t i = 0; i < channel_length_16; i++)
        {
            data_16[i * 2] = dataLeft[i] * MAX_INT16;     // Denormalize left
            data_16[i * 2 + 1] = dataRight[i] * MAX_INT16; // Denormalize right
        }
    }
*/
    void setBindings(shared_ptr<berry::VmState> berry)
    {
        berry->export_this("set_eq", this, &EqualizerProcessor::setBands, "playback");
    }
};

#endif
