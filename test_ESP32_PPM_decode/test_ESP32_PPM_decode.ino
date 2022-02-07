/*
This example outputs values from all PPM channels to Serial
in a format compatible with Arduino IDE Serial Plotter
*/

#include "PPMReader_ESP32.h"

// Initialize a PPMReader on digital pin 3 with 6 expected channels.
byte interruptPin = 4;
byte channelAmount = 8;
//PPMReader ppm(interruptPin, channelAmount);


void setup() {
    Serial.begin(115200);

    PPMReaderInit(interruptPin, channelAmount);
}

void loop() {
    // Print latest valid values from all channels
    for (byte channel = 1; channel <= channelAmount; ++channel) {
        unsigned value = PPMReaderLatestValidChannelValue(channel, 0);
        Serial.print(value);
		if(channel < channelAmount) Serial.print('\t');
    }
    Serial.println();
	delay(20);
}
