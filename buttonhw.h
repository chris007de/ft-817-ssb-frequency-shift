/*
	 This file is part of FT-817 SSB Frequency Shift.

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this project.  If not, see http://www.gnu.org/licenses/;.	  

    Copyright 2020 Christian Obersteiner, DL1COM
*/

#pragma once

#include "Arduino.h"


class ButtonHW {
    public:
	ButtonHW(uint32_t pin);

	bool isPressed();
	bool isReleased();
	bool isPressedEdge();
	bool isReleasedEdge();
	void update();

    private:
	int pin_;
	bool buttonState;
	bool lastButtonState = LOW;
	bool stateChanged    = false;

	unsigned long lastDebounceTime =
		0; // the last time the output pin was toggled
	unsigned long debounceDelay =
		50; // the debounce time; increase if the output flickers
};
