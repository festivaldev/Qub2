//
//  Qub.cpp
//  Qub2 – 3D LED Cube Library
//
//  Created by Janik Schmidt on 18.10.18
//  Copyright © 2018 Team FESTIVAL. All rights reserved.
//

#include "Qub.h"

// const int columns[]	= { 2, 3, 4, 5, 6, 7, 8, 9, 10 };
const int columns[] = { 6, 3, 4, 7, 10, 9, 8, 5, 2 };
const int layers[]	= { 11, 12, 13 };

/**
 * @brief Setup Qub and configure the required pins
 * 
 */
void Qub::setup() {
	// Configure columns as output
	for (int c: columns) {
		pinMode(c, OUTPUT);
	}
	
	// Configure layers as output
	for (int l: layers) {
		pinMode(l, OUTPUT);
	}
}

/**
 * @brief Enables one of the nine columns of the cube
 * 
 * @param column The column of the cube to enable
 */
void Qub::enableColumn(int column) {
	// Check if we have a valid value
	if (Qub::inRange(column, 1, 9)) {
		// Set the column to HIGH
		digitalWrite(columns[column - 1], HIGH);
	}
}

/**
 * @brief Disables one of the nine columns of the cube
 * 
 * @param column The column of the cube to disable
 */
void Qub::disableColumn(int column) {
	// Check if we have a valid value
	if (Qub::inRange(column, 1, 9)) {
		// Set the column to LOW
		digitalWrite(columns[column - 1], LOW);
	}
}

/**
 * @brief Enables all columns at once
 * 
 */
void Qub::enableColumns() {
	for (int c: columns) {
		digitalWrite(c, HIGH);
	}
}

/**
 * @brief Disables all columns at once
 * 
 */
void Qub::disableColumns() {
	for (int c: columns) {
		digitalWrite(c, LOW);
	}
}

/**
 * @brief Enables one of the three layers of the cube by switching a transistor
 * 
 * @param column The layer of the cube to enable
 */
void Qub::enableLayer(int layer) {
	// Check if we have a valid value
	if (Qub::inRange(layer, 1, 3)) {
		// Set the layer to HIGH
		digitalWrite(layers[layer - 1], HIGH);
	}
}

/**
 * @brief Disables one of the three layers of the cube by switching a transistor
 * 
 * @param column The layer of the cube to disable
 */
void Qub::disableLayer(int layer) {
	// Check if we have a valid value
	if (Qub::inRange(layer, 1, 3)) {
		// Set the layer to LOW
		digitalWrite(layers[layer - 1], LOW);
	}
}

/**
 * @brief Enables all layers at once
 * 
 */
void Qub::enableLayers() {
	for (int l: layers) {
		digitalWrite(l, HIGH);
	}
}

/**
 * @brief Disables all layers at once
 * 
 */
void Qub::disableLayers() {
	for (int l: layers) {
		digitalWrite(l, LOW);
	}
}

/**
 * @brief Enables all columns and layers at once
 * 
 */
void Qub::enableLEDs() {
	Qub::enableLayers();
	Qub::enableColumns();
}

/**
 * @brief Disables all columns and layers at once
 * 
 */
void Qub::disableLEDs() {
	Qub::disableLayers();
	Qub::disableColumns();
}

/**
 * @brief Enables a single LED in a specified column, disabling any other LED
 * 
 * @param column The column to enable
 * @param layer The layer to enable
 */
void Qub::enableLEDInLayer(int column, int layer) {
	Qub::disableLEDs();
	
	Qub::enableColumn(column);
	Qub::enableLayer(layer);
}

/**
 * @brief Enables any LED in a specified layer, disabling any other layer
 * 
 * @param layer 
 */
void Qub::enableLEDsInLayer(int layer) {
	Qub::disableLEDs();
	
	Qub::enableColumns();
	Qub::enableLayer(layer);
}



/**
 * @brief Checks if a value is in a defined range
 * 
 * @param value The value to check
 * @param min The lower bound of the range
 * @param max The upper bound of the range
 * @return true If the value is in between the defined range
 * @return false If the value is outside the defined range
 */
bool Qub::inRange(int value, int min, int max) {
	return (value >= min && value <= max);
}
