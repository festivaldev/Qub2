//
//  Qub.h
//  Qub2 – 3D LED Cube Library
//
//  Created by Janik Schmidt on 18.10.18
//  Copyright © 2018 Team FESTIVAL. All rights reserved.
//

#include <Arduino.h>

#ifndef Qub_h
#define Qub_h

enum ButtonState {
	Pressed,
	Unpressed
};

enum OperatingMode {
	Default,
	Brightness,
	RegNumber,
	RandomNumer
};

class Qub {
	public:
		static void setup();

        static void changeSet(int set);

        static void enableColumn(int column);
		static void disableColumn(int column);
		static void enableColumns();
		static void disableColumns();
		
		static void enableLayer(int layer);
		static void disableLayer(int layer);
		static void enableLayers();
		static void disableLayers();
		
		static void enableLEDs();
		static void disableLEDs();
		static void enableLEDInLayer(int column, int layer);
		static void enableLEDsInLayer(int layer);
		
		static bool inRange(int value, int min, int max);
};

#endif
