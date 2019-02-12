//
//  Qub2.ino
//  Qub2 – 3D LED Cube Library
//
//  Created by Janik Schmidt on 22.11.18
//  Copyright © 2018 Team FESTIVAL. All rights reserved.
//

#include "Qub.h"
String matNr = "70454016"; // 70453310, 70453510

unsigned long cMillis = 0;
byte btnPort = 0x13;
ButtonState buttonState = Unpressed;
OperatingMode opMode = Default;
int layerStore[][9] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
int numberDefs[][9] = {
	{ 9, 0, 0, 0, 0, 0, 0, 0, 0 },	// 0
	{ 0, 0, 0, 0, 1, 0, 0, 0, 0 },	// 1
	{ 0, 0, 3, 0, 0, 0, 7, 0, 0 },	// 2
	{ 0, 0, 3, 0, 1, 0, 7, 0, 0 },	// 3
	{ 9, 0, 3, 0, 0, 0, 7, 0, 5 },	// 4
	{ 9, 0, 3, 0, 1, 0, 7, 0, 5 },	// 5
	{ 9, 0, 3, 8, 0, 4, 7, 0, 5 },	// 6
	{ 9, 0, 3, 8, 1, 4, 7, 0, 5 },	// 7
	{ 9, 2, 3, 8, 0, 4, 7, 6, 5 },	// 8
	{ 9, 2, 3, 8, 1, 4, 7, 6, 5 }	// 9
};

void setup() {
	Qub::setup();
	pinMode(btnPort, INPUT_PULLUP);
	
	mode0_setup();
}

void loop() {
	int buttonIn = analogRead(btnPort);
	
	if (buttonIn < 512 && buttonState == Unpressed) {
		buttonState = Pressed;
		cMillis = millis();
		
		if (Qub::inRange(buttonIn, 20, 24)) {
			opMode = Brightness;
			mode1_setup();
		}
		if (Qub::inRange(buttonIn, 27, 31)) {
			opMode = RegNumber;
			mode2_setup();
		}
		if (Qub::inRange(buttonIn, 34, 38)) {
			opMode = RandomNumer;
			mode3_setup(rand() % 6 + 1);
		}
	} else if (buttonIn > 512) {
		buttonState = Unpressed;
	}
	
	switch (opMode) {
		case Brightness:
			mode1_loop();
			break;
		case RegNumber:
			mode2_loop();
			break;
		case RandomNumer:
			mode3_loop();
			break;
		default:
			mode0_loop();
			break;
	}
}



#pragma mark - Runtime

/**
 * Mode "0"
 * 
 * @brief This is the default mode which rotates a pattern around X/Y/Z axes
 */
int cLayer = 1;
int cLed = 0;
int rotateCounter = 0;
int setIndex = 0;

void mode0_setup() {
	for (int l = 0; l < 3; l++) {
		int layer[9] = { 1, 2, 3, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 9; i++) {
			layerStore[l][i] = layer[i];
		}
	}
}

void mode0_loop() {
	Qub::changeSet(setIndex);
	renderLayerStore();
	
	if (millis() > cMillis + currentInterval(64, 512)) {
		if (setIndex < 4) {
			int store[] = { layerStore[0][0], layerStore[0][1], layerStore[0][2] };

			for (int i = 0; i < 7; i++) {
				shift(cLayer, cLed);
				shift(cLayer, cLed + 1);
				shift(cLayer, cLed + 2);

				if (cLed == 0 && cLayer < 2) {
					cLayer++;
				} else if (cLayer == 2 && cLed < 6) {
					cLed += 3;
				} else if (cLed == 6 && cLayer > 0) {
					cLayer--;
				} else if (cLayer == 0 && cLed > 3) {
					cLed -= 3;
				} else if (cLayer == 0 && cLed == 3) {
					cLayer = 1;
					cLed = 0;
				}
			}

			layerStore[0][3] = store[0] + (store[0] == 0 ? 0 : 3);
			layerStore[0][4] = store[1] + (store[1] == 0 ? 0 : 3);
			layerStore[0][5] = store[2] + (store[2] == 0 ? 0 : 3);
		} else {
			for (int l = 0; l < 3; l++) {
				for (int i = 0; i < 8; i++) {
					int n = layerStore[l][i];
					if (n > 0) {
						n++;
						if (n == 9) {
							n = 1;
						}
					}
					layerStore[l][i] = n;
				}
			}
		}

		cMillis = millis();
		rotateCounter++;
	}
	
	if (rotateCounter == 8) {
		Qub::disableLayers();
		Qub::disableColumns();

		rotateCounter = 0;
		setIndex = (++setIndex % 5);
	}
}



/**
 * Mode 1
 * 
 * @brief This mode uses the potentiometer on A7 to change the brightness of every LED
 */

void mode1_setup() {
	Qub::enableColumns();
}

void mode1_loop() {
	float frequency = 100;
	float cycleLength = 1000000 / frequency;
	float dutyCycle;

	int timeOn;
	int timeOff;

	float vOut;

	int val = analogRead(A7);
	vOut = map(val, 0, 1024, 0, 255);
	dutyCycle = vOut / 255;

	timeOn = dutyCycle * cycleLength;
	timeOff = cycleLength - timeOn;

	if (timeOn > 0) {
		Qub::enableLayers();
		delayMicroseconds(timeOn);
	}

	Qub::disableLayers();
	delayMicroseconds(timeOff);
}



/**
 * Mode 2
 * @brief This mode shows the configured student registration number
 */

int layerIndex = 0;
int blinkStepCount = 0;
int layerNumbers[3] = { -1, -1 ,-1 };

void mode2_setup() {
	Qub::changeSet(5);
	clearLayerStore();
	
	layerIndex = 0;
	blinkStepCount = 0;
	
	for (int i = 0; i < 3; i++) {
		layerNumbers[i] = -1;
	}
}

void mode2_loop() {
	if (layerIndex < matNr.length() + 3) {
		renderLayerStore();
		
		if (millis() > cMillis + currentInterval(256, 1024)) {
			cMillis = millis();
			
			layerNumbers[2] = layerNumbers[1];
			layerNumbers[1] = layerNumbers[0];
			
			int parsedNumber;
			if (matNr.substring(layerIndex, layerIndex + 1) == "") {
				parsedNumber = -1;
			} else {
				parsedNumber = matNr.substring(layerIndex, layerIndex + 1).toInt();
			}
			layerNumbers[0] = parsedNumber;
			
			for (int l = 0; l < 3; l++) {
				for (int i = 0; i < 9; i++) {
					if (layerNumbers[l] >= 0) {
						layerStore[l][i] = numberDefs[layerNumbers[l]][i];
					} else {
						layerStore[l][i] = 0;
					}
				}
			}
			
			layerIndex++;
		}
    } else {
		if (blinkStepCount < 6) {
			Qub::enableLayers();
			if (millis() > cMillis + 250) {
				cMillis = millis();
				
				if (blinkStepCount % 2 == 0) {
					Qub::enableColumns();
				} else {
					Qub::disableColumns();
				}
				
				blinkStepCount++;
			}
        } else {
			Qub::disableLayers();
			layerIndex = 0;
            blinkStepCount = 0;
        }
	}
}



/**
 * Mode 3
 * @brief This mode rotates a random number (1-6) around z axis in 45° steps
 * 
 * @param n Defines the random number to display
 */

void mode3_setup(int n) {
	Qub::changeSet(5);
	
	clearLayerStore();
	
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			layerStore[i][j] = numberDefs[n][(i * 3) + j];
		}
	}
}

void mode3_loop() {
	renderLayerStore();

	if (millis() >= cMillis + currentInterval(64, 1024)) {
		for (int l = 0; l < 3; l++) {
			for (int i = 0; i < 9; i++) {
				int n = layerStore[l][i];
				if (n > 1) {
					n++;
					if (n == 10) {
						n = 2;
					}
				}
				layerStore[l][i] = n;
			}
		}
		
		cMillis = millis();
	}
}



#pragma mark - Helper Methods

/**
 * @brief This method shifts indices around and is used to rotate a pattern around Z axis
 * 
 * @param from The base value that will be shifted
 * @param index The direction to shift the value to
 */

void shift(int from, int index) {
	if (from == 0 && index < 6) {
		// Move South
		if (layerStore[from][index] > 0) {
			layerStore[from][index + 3] = layerStore[from][index] + 3;
		} else {
			layerStore[from][index + 3] = layerStore[from][index];
		}
	} else if (from < 2 && index >= 6) {
		// Shift Up
		layerStore[from + 1][index] = layerStore[from][index];
	} else if (from == 2 && index > 2) {
		// Move North
		if (layerStore[from][index] > 0) {
			layerStore[from][index - 3] = layerStore[from][index] - 3;
		} else {
			layerStore[from][index - 3] = layerStore[from][index];
		}
	} else if (from > 0 && index <= 2) {
		// Shift Down
		layerStore[from - 1][index] = layerStore[from][index];
	}
	
	layerStore[from][index] = 0;
}


/**
 * @brief This method renders the current layer store by turning LEDs on or off using PWM
 */

void renderLayerStore() {
	for (int i = 0; i < 3; i++) {
		Qub::enableLayer(i + 1);
		for (int l : layerStore[i]) {
			if (l > 0) {
				Qub::enableColumn(l);
			}
		}

		delayMicroseconds(255);
		Qub::disableLayers();
		Qub::disableColumns();
	}
}


/**
 * @description This method calculates and returns the delta value on port A7
 * so it can be used as a speed controller instead of a delay controller
 * (higher value = higher speed)
 * 
 * @param lowerBound The lower bound to map the value to
 * @param upperBound The upper bound to map the value to
 */

int currentInterval(int lowerBound, int upperBound) {
	return map(1024 - analogRead(A7), 0, 1024, lowerBound, upperBound);
}


/**
 * @brief Clears the layer store
 */
void clearLayerStore() {
	for (int l = 0; l < 3; l++) {
		for (int i = 0; i < 9; i++) {
			layerStore[l][i] = 0;
		}
	}
}