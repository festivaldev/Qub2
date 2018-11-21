//
//  Qub.ino
//  Qub2 – 3D LED Cube Library
//
//  Created by Janik Schmidt on 22.10.18
//  Copyright © 2018 Team FESTIVAL. All rights reserved.
//

#include "Qub.h"

void setup()
{
	Qub::setup();

	//stage1_setup();
	//stage2_setup();

	//stageR_setup(6);
}

void loop()
{
	//stage1_loop();
	//stage2_loop();
	stage3_loop();

	//stageR_loop();
}

/**
 * @brief Stage 1
 * 
 */

void stage1_setup()
{
	for (int l = 1; l <= 3; l++)
	{
		for (int c = 1; c <= 9; c++)
		{
			Qub::enableLEDInLayer(c, l);
			delay(1000);
		}
	}

	Qub::disableLEDs();
}
void stage1_loop()
{
	delay(500);

	Qub::enableLayers();
	for (int i = 0; i < 3; i++)
	{
		Qub::disableColumns();
		Qub::enableColumn(1 + i);
		Qub::enableColumn(4 + i);
		Qub::enableColumn(7 + i);

		delay(125);
	}

	Qub::disableColumns();
	delay(500);

	for (int i = 0; i < 3; i++)
	{
		Qub::disableColumns();
		Qub::enableColumn(1 + (i * 3));
		Qub::enableColumn(2 + (i * 3));
		Qub::enableColumn(3 + (i * 3));

		delay(125);
	}

	Qub::disableColumns();
	delay(500);

	for (int i = 0; i < 3; i++)
	{
		Qub::disableColumns();
		Qub::enableColumn(3 - i);
		Qub::enableColumn(6 - i);
		Qub::enableColumn(9 - i);

		delay(125);
	}

	Qub::disableColumns();
	delay(500);

	for (int i = 0; i < 3; i++)
	{
		Qub::disableColumns();
		Qub::enableColumn(7 - (i * 3));
		Qub::enableColumn(8 - (i * 3));
		Qub::enableColumn(9 - (i * 3));

		delay(125);
	}

	Qub::disableColumns();
	delay(500);

	Qub::disableLayers();
	Qub::enableColumns();

	for (int i = 1; i <= 3; i++)
	{
		Qub::disableLayers();
		Qub::enableLayer(i);

		delay(125);
	}

	Qub::disableLayers();
	delay(500);

	Qub::enableColumns();

	for (int i = 3; i >= 1; i--)
	{
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

void stage2_setup()
{
	pinMode(A5, INPUT_PULLUP);
}

void stage2_loop()
{
	// Qub::disableLEDs();
	//Qub::enableColumns();

	if (layer1Enabled)
	{
		Qub::enableLayer(1);
	}
	else
	{
		Qub::disableLayer(1);
	}
	if (layer2Enabled)
	{
		Qub::enableLayer(2);
	}
	else
	{
		Qub::disableLayer(2);
	}
	if (layer3Enabled)
	{
		Qub::enableLayer(3);
	}
	else
	{
		Qub::disableLayer(3);
	}

	int buttonIn = analogRead(A5);

	if (buttonIn < 512 && !isChangingLayer)
	{
		isChangingLayer = true;
		if (buttonIn >= 20 && buttonIn <= 24)
		{
			layer1Enabled = !layer1Enabled;
		}

		if (buttonIn >= 27 && buttonIn <= 31)
		{
			layer2Enabled = !layer2Enabled;
		}

		if (buttonIn >= 34 && buttonIn <= 38)
		{
			layer3Enabled = !layer3Enabled;
		}

		//while (analogRead(A5) < 512);
	}
	else if (buttonIn > 512)
	{
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
	dutyCycle = vOut / 255;

	timeOn = dutyCycle * cycleLength;
	timeOff = cycleLength - timeOn;

	if (timeOn > 0)
	{
		Qub::enableColumns();
		delayMicroseconds(timeOn);
	}
	Qub::disableColumns();
	delayMicroseconds(timeOff);
}

int cLayer = 1;
int cLed = 0;
int layerStore[][9] = {
	{1, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0},
	{1, 0, 0, 0, 0, 0, 0, 0, 0}
};
unsigned long rotateTime = millis();
int rotateCounter = 0;
int setI = 0;

void stage3_loop() {
	Qub::changeSet(setI);
	Qub::enableLayer(1);
	for (int l1 : layerStore[0]) {
		if (l1 > 0) {
			Qub::enableColumn(l1);
		}
	}

	delayMicroseconds(255);
	Qub::disableLayers();
	Qub::disableColumns();

	Qub::enableLayer(2);
	for (int l2 : layerStore[1]) {
		if (l2 > 0) {
			Qub::enableColumn(l2);
		}
	}

	delayMicroseconds(255);
	Qub::disableLayers();
	Qub::disableColumns();

	Qub::enableLayer(3);
	for (int l3 : layerStore[2]) {
		if (l3 > 0) {
			Qub::enableColumn(l3);
		}
	}

	delayMicroseconds(255);
	Qub::disableLayers();
	Qub::disableColumns();

	if (millis() > rotateTime + 200) {
		if (setI < 4) {

			// WORKS FLAWLESSLY

			int store[] = {layerStore[0][0], layerStore[0][1], layerStore[0][2]};

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

			// DOESN'T WORK

			/*for (int l = 0; l < 3; l++) {

				int s = layerStore[l][0];

				for (int i = 1; i < 8; i++) {
					int n = layerStore[l][i];
					if (n > 0) {
						n++;
						if (n == 10) {
							n = 1;
						}
					}
					int ii = i+1;   // SRSLY?!
					if (ii == 8) {
						ii = 0;
					}
					layerStore[l][ii] = n;
					layerStore[l][i] = 0;
				}

				layerStore[l][1] = s;
			}*/
			
			// WORKS BETTER, BUT NOT PERFECT YET
			for (int l=0; l<3; l++) {
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

		rotateTime = millis();
		rotateCounter++;
	}

	if (rotateCounter == 8) { // THIS SHOULD BE +1, BUT BREAKS STUFF AND LOOKS LIKE +2
		Qub::disableLayers();
		Qub::disableColumns();

		rotateCounter = 0;
		// Qub::changeSet(++setI);

		setI = (++setI % 5);
		// if (setI == 5) {
		// 	setI = 0;
		// }
	}
}

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

int numberDefs[][9] = {
	{0, 0, 0, 0, 1, 0, 0, 0, 0},
	{0, 0, 3, 0, 0, 0, 7, 0, 0},
	{0, 0, 3, 0, 1, 0, 7, 0, 0},
	{9, 0, 3, 0, 0, 0, 7, 0, 5},
	{9, 0, 3, 0, 1, 0, 7, 0, 5},
	{9, 0, 3, 8, 0, 4, 7, 0, 5}
};

void stageR_setup(int n) {
	layerStore[0][0] = numberDefs[n - 1][0];
	layerStore[0][1] = numberDefs[n - 1][1];
	layerStore[0][2] = numberDefs[n - 1][2];
	layerStore[1][0] = numberDefs[n - 1][3];
	layerStore[1][1] = numberDefs[n - 1][4];
	layerStore[1][2] = numberDefs[n - 1][5];
	layerStore[2][0] = numberDefs[n - 1][6];
	layerStore[2][1] = numberDefs[n - 1][7];
	layerStore[2][2] = numberDefs[n - 1][8];
}

void stageR_loop() {
	Qub::enableLayer(1);
	for (int l1 : layerStore[0]) {
		if (l1 > 0) {
			Qub::enableColumn(l1);
		}
	}

	delayMicroseconds(255);
	Qub::disableLayers();
	Qub::disableColumns();

	Qub::enableLayer(2);
	for (int l2 : layerStore[1]) {
		if (l2 > 0) {
			Qub::enableColumn(l2);
		}
	}

	delayMicroseconds(255);
	Qub::disableLayers();
	Qub::disableColumns();

	Qub::enableLayer(3);
	for (int l3 : layerStore[2]) {
		if (l3 > 0) {
			Qub::enableColumn(l3);
		}
	}

	delayMicroseconds(255);
	Qub::disableLayers();
	Qub::disableColumns();

	if (millis() >= rotateTime + 200) {
		for (int i = 0; i < 9; i++) {
			int n = layerStore[0][i];
			if (n > 1) {
				n++;
				if (n == 10) {
					n = 2;
				}
			}
			layerStore[0][i] = n;
		}

		for (int i = 0; i < 9; i++) {
			int n = layerStore[1][i];
			if (n > 1) {
				n++;
				if (n == 10) {
					n = 2;
				}
			}
			layerStore[1][i] = n;
		}

		for (int i = 0; i < 9; i++) {
			int n = layerStore[2][i];
			if (n > 1) {
				n++;
				if (n == 10) {
					n = 2;
				}
			}
			layerStore[2][i] = n;
		}
		
		rotateTime = millis();
	}
}
