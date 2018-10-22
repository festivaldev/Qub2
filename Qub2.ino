//
//  Qub.ino
//  Qub2 – 3D LED Cube Library
//
//  Created by Janik Schmidt on 22.10.18
//  Copyright © 2018 Team FESTIVAL. All rights reserved.
//

#include "Qub.h"

void setup() {
	Qub::setup();

	//stage1_setup();
	//stage2_setup();
}

void loop() {
	stage1_loop();
	//stage2_loop();
}



/**
 * @brief Stage 1
 * 
 */

void stage1_setup() {
	for (int l=1; l<=3; l++) {
		for (int c=1; c<=9; c++) {
			Qub::enableLEDInLayer(c, l);
			delay(1000);
		}
	}
	
	Qub::disableLEDs();
}
void stage1_loop() {
	delay(500);
	
	Qub::enableLayers();
	for (int i=0; i<3; i++) {
		Qub::disableColumns();
		Qub::enableColumn(1+i);
		Qub::enableColumn(4+i);
		Qub::enableColumn(7+i);
		
		delay(125);
	}
	
	Qub::disableColumns();
	delay(500);
	
	for (int i=0; i<3; i++) {
		Qub::disableColumns();
		Qub::enableColumn(1+(i*3));
		Qub::enableColumn(2+(i*3));
		Qub::enableColumn(3+(i*3));
		
		delay(125);
	}
	
	Qub::disableColumns();
	delay(500);
	
	for (int i=0; i<3; i++) {
		Qub::disableColumns();
		Qub::enableColumn(3-i);
		Qub::enableColumn(6-i);
		Qub::enableColumn(9-i);
		
		delay(125);
	}
	
	Qub::disableColumns();
	delay(500);
	
	for (int i=0; i<3; i++) {
		Qub::disableColumns();
		Qub::enableColumn(7-(i*3));
		Qub::enableColumn(8-(i*3));
		Qub::enableColumn(9-(i*3));
		
		delay(125);
	}
	
	Qub::disableColumns();
	delay(500);
	
	Qub::disableLayers();
	Qub::enableColumns();
	
	for (int i=1; i<=3; i++) {
		Qub::disableLayers();
		Qub::enableLayer(i);
		
		delay(125);
	}
	
	Qub::disableLayers();
	delay(500);
	
	Qub::enableColumns();
	
	for (int i=3; i>=1; i--) {
		Qub::disableLayers();
		Qub::enableLayer(i);
		
		delay(125);
	}
	
	Qub::disableLayers();
	Qub::disableColumns();
}



/**
 * @brief Stage 2
 * 
 */

bool layer1Enabled = true;
bool layer2Enabled = true;
bool layer3Enabled = true;
bool isChangingLayer = false;

void stage2_setup() {
	pinMode(A5, INPUT_PULLUP);
}

void stage2_loop() {
	// Qub::disableLEDs();
	//Qub::enableColumns();
	
	if (layer1Enabled) {
		Qub::enableLayer(1);
	} else {
		Qub::disableLayer(1);
	}
	if (layer2Enabled) {
		Qub::enableLayer(2);
	} else {
		Qub::disableLayer(2);
	}
	if (layer3Enabled) {
		Qub::enableLayer(3);
	} else {
		Qub::disableLayer(3);
	}
	
	int buttonIn = analogRead(A5);
	
	if (buttonIn < 512 && !isChangingLayer) {
		isChangingLayer = true;
		if (buttonIn >= 20 && buttonIn <= 24) {
			layer1Enabled = !layer1Enabled;
		}
		
		if (buttonIn >= 27 && buttonIn <= 31) {
			layer2Enabled = !layer2Enabled;
		}
		
		if (buttonIn >= 34 && buttonIn <= 38) {
			layer3Enabled = !layer3Enabled;
		}
		
		//while (analogRead(A5) < 512);
	} else if (buttonIn > 512) {
		isChangingLayer = false;
	}
	
	float frequency = 100;
    float cycleLength = 1000000 / frequency;
    float dutyCycle;

    int timeOn;
    int timeOff;

    float vOut;

    int val = analogRead(A7);
	Serial.println(val);
    vOut = map(val, 0, 1024, 0, 255);
    dutyCycle = vOut/255;

    timeOn = dutyCycle * cycleLength;
    timeOff = cycleLength - timeOn;

    if (timeOn > 0) {
        Qub::enableColumns();
        delayMicroseconds(timeOn);
    }
    Qub::disableColumns();
    delayMicroseconds(timeOff);
}