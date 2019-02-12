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

// Define a space where we can store layer states
int layerStore[][9] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};
// Define the layout of dice-inspired numbers
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
	// Run Qub's setup routine which sets the column and layer ports as output
	Qub::setup();
	// Define the port where our buttons are connected as a pulled up input
	pinMode(btnPort, INPUT_PULLUP);
	
	// Run the setup routine for our default mode
	mode0_setup();
}

void loop() {
	// Read the current resistance value on our button port
	int buttonIn = analogRead(btnPort);
	
	// Check if the button is cuurently pressed and it hasn't been pressed in a previous loop
	if (buttonIn < 512 && buttonState == Unpressed) {
		buttonState = Pressed;
		cMillis = millis();
		
		// Check the range of the button input
		// Since it's an analog value, it may fluctuate, so we check if it's in a certain offset range
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
	
	// Check in which mode we're currently in and call it method accordingly
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
	// Set the default pattern that is shown in the first frame of the animation
	for (int l = 0; l < 3; l++) {
		int layer[9] = { 1, 2, 3, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 9; i++) {
			layerStore[l][i] = layer[i];
		}
	}
}

void mode0_loop() {
	// Change the set to the current index
	Qub::changeSet(setIndex);
	// Render whatever is currently stored inside the layer store
	renderLayerStore();
	
	// Check if the current processor time is larger than our speed interval
	if (millis() > cMillis + currentInterval(64, 512)) {
		// Do this as long as we're still doing the relatively simple animation steps
		if (setIndex < 4) {
			// Create a temporary store for the first three items of our base layer
			int store[] = { layerStore[0][0], layerStore[0][1], layerStore[0][2] };

			// Perfom some magic
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

			// Replace the next three items in the layer store with the shifted indices of our LEDs
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

		// Reset the stored processor time and increase our rotation counter
		cMillis = millis();
		rotateCounter++;
	}
	
	// If we've reached the maximum amount of rotations, we're resetting the whole thing and advance to the next animation
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
	// For this next trick, we need all columns enabled
	Qub::enableColumns();
}

void mode1_loop() {
	// Here we're getting the calculated duty cycle with the value from our potentiometer
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

	// Instead of toggling 9 columns, we're just toggling 3 layers, which should improve efficiency
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
	// For some reason, set 5 is the one to go with for dice numbers
	Qub::changeSet(5);
	// Clear whatever is stored in the layer store
	clearLayerStore();
	
	// Reset the counters
	layerIndex = 0;
	blinkStepCount = 0;
	
	// Clear the numbers stored in the render definitions
	for (int i = 0; i < 3; i++) {
		layerNumbers[i] = -1;
	}
}

void mode2_loop() {
	if (layerIndex < matNr.length() + 3) {
		// First we display the whole registration number
		renderLayerStore();
		
		// The potentiometer is again used as a speed controller
		// Even though it's not supposed to be, but while we're at it using it as such everywhere...
		if (millis() > cMillis + currentInterval(256, 1024)) {
			cMillis = millis();
			
			// Move the previous number to parse to the next index
			layerNumbers[2] = layerNumbers[1];
			layerNumbers[1] = layerNumbers[0];
			
			// Parse a digit from matNr
			int parsedNumber;
			if (matNr.substring(layerIndex, layerIndex + 1) == "") {
				// If we find an empty string, we probably reached the end of the string
				// To not render anything, we return -1 as the parsed number
				parsedNumber = -1;
			} else {
				// We found a non-empty string, so we can parse it to a number
				parsedNumber = matNr.substring(layerIndex, layerIndex + 1).toInt();
			}
			// We store the parsed number at the very beginning of our store
			layerNumbers[0] = parsedNumber;
			
			for (int l = 0; l < 3; l++) {
				for (int i = 0; i < 9; i++) {
					if (layerNumbers[l] >= 0) {
						// Here we copy render definition of our parsed number into the layer store
						layerStore[l][i] = numberDefs[layerNumbers[l]][i];
					} else {
						// If we've got a -1, we set every value to 0 (off)
						layerStore[l][i] = 0;
					}
				}
			}
			
			// Increase the layer index so the numbers move up in the next step
			layerIndex++;
		}
    } else {
		// After we displayed the registration number, we're supposed to blink 3 times
		// Every on/off step counts, so we're using 3*2=6 steps total
		if (blinkStepCount < 6) {
			Qub::enableLayers();
			
			// Toggle the state every 250ms
			if (millis() > cMillis + 250) {
				cMillis = millis();
				
				// Decide by the current step count if we should enable or disable the LEDs
				if (blinkStepCount % 2 == 0) {
					Qub::enableColumns();
				} else {
					Qub::disableColumns();
				}
				
				// Increase the step counter
				blinkStepCount++;
			}
        } else {
			// We've passed the final step, so reset everything
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
	// Still using dice numbers, so we still need set 5
	Qub::changeSet(5);
	// Clear whatever is stored in our layer store
	clearLayerStore();
	
	// Load the render definition for the random number n in the layer store
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			layerStore[i][j] = numberDefs[n][(i * 3) + j];
		}
	}
}

void mode3_loop() {
	// Render the layer store
	renderLayerStore();

	// Use the potentiometer as a speed controller
	if (millis() >= cMillis + currentInterval(64, 1024)) {
		// Perform some magic to make shift indices
		for (int l = 0; l < 3; l++) {
			for (int i = 0; i < 9; i++) {
				int n = layerStore[l][i];
				if (n > 1) {
					n++;
					if (n == 10) {
						n = 2;
					}
				}
				
				// Store the shifted indices in the layer store
				layerStore[l][i] = n;
			}
		}
		
		// As always, reset the processor time after each step
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