/*
Copyright 2016 Aapo Nikkilä
Copyright 2021 Dmitry Grigoryev

This file is part of PPM Reader.

PPM Reader is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PPM Reader is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PPM Reader.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PPMReader_ESP32.h"

//static PPMReader *PPMReader::ppm;
//PPMReader *PPMReader::ppm;
struct PPMReader ppm;

void IRAM_ATTR PPM_ISR(void) {
  PPMReaderHandleInterrupt();
}


void PPMReaderInit(byte interruptPin, byte channelAmount){
    ppm.interruptPin = interruptPin;
    ppm.channelAmount = channelAmount;
    // Setup an array for storing channel values
    ppm.rawValues = new unsigned [channelAmount];
    ppm.validValues = new unsigned [channelAmount];
    for (int i = 0; i < channelAmount; ++i) {
        ppm.rawValues[i] = 0;
        ppm.validValues[i] = 0;
    }
    // Attach an interrupt to the pin
    pinMode(interruptPin, INPUT);
    //attachInterrupt(digitalPinToInterrupt(interruptPin), PPM_ISR, RISING);
    attachInterrupt(interruptPin, PPM_ISR, RISING);
}

void PPMReaderDelete(void) {
    //detachInterrupt(digitalPinToInterrupt(interruptPin));
    detachInterrupt(ppm.interruptPin);
    delete [] ppm.rawValues;
    delete [] ppm.validValues;
}

void PPMReaderHandleInterrupt(void) {
    // Remember the current micros() and calculate the time since the last pulseReceived()
    unsigned long previousMicros = ppm.microsAtLastPulse;
    ppm.microsAtLastPulse = micros();
    unsigned long time = ppm.microsAtLastPulse - previousMicros;

    if (time > ppm.blankTime) {
        /* If the time between pulses was long enough to be considered an end
         * of a signal frame, prepare to read channel values from the next pulses */
        ppm.pulseCounter = 0;
    }
    else {
        // Store times between pulses as channel values
        if (ppm.pulseCounter < ppm.channelAmount) {
            ppm.rawValues[ppm.pulseCounter] = time;
            if (time >= ppm.minChannelValue - ppm.channelValueMaxError && time <= ppm.maxChannelValue + ppm.channelValueMaxError) {
                ppm.validValues[ppm.pulseCounter] = constrain(time, ppm.minChannelValue, ppm.maxChannelValue);
            }
        }
        ++ppm.pulseCounter;
    }
}

unsigned PPMReaderRawChannelValue(byte channel) {
    // Check for channel's validity and return the latest raw channel value or 0
    unsigned value = 0;
    if (channel >= 1 && channel <= ppm.channelAmount) {
        noInterrupts();
        value = ppm.rawValues[channel-1];
        interrupts();
    }
    return value;
}

unsigned PPMReaderLatestValidChannelValue(byte channel, unsigned defaultValue) {
    // Check for channel's validity and return the latest valid channel value or defaultValue.
    unsigned value;
    if(micros() - ppm.microsAtLastPulse > ppm.failsafeTimeout) return defaultValue;
    if (channel >= 1 && channel <= ppm.channelAmount) {
        noInterrupts();
        value = ppm.validValues[channel-1];
        interrupts();
        if(value < ppm.minChannelValue) return defaultValue; // value cannot exceed maxChannelValue by design
    }
    return value;
}
