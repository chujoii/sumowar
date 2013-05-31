/*
  zuk.ino --- control program for robot

  Copyright (C) 2012 Roman V. Prikhodchenko



  Author: Roman V. Prikhodchenko <chujoii@gmail.com>


  This file is part of zuk.

    zuk is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    zuk is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with zuk.  If not, see <http://www.gnu.org/licenses/>.



 Keywords: robot



 Usage:


 History:
 
 Project started at 2013.05(may).31



 Code:
*/

#include <Stepper.h>


// global interrupt pin
int pin_global_interrupt 2;

// optic line-sensors
int analog_pin_optocouple = A0;
int digital_pin_optocouple_led = 4; // current for modulated LED
int delay_us_between_led_on_and_scan = 1;

// scanner (steper-motor, sensors)
int digital_pin_scanner_a = 5;
int digital_pin_scanner_b = 6;
int digital_pin_scanner_c = 7;
int digital_pin_scanner_d = 8;
int maximum_number_of_steps = 100; // if sensor broken. fixme: need check number steps
int digital_pin_scanner_left = 9;
int digital_pin_scanner_right = 10;
const int scanner_steps_per_revolution = 10; // fixme
conts int scanner_steps_speed = 1000; // fixmes
Stepper scanner_stepper(scanner_steps_per_revolution, digital_pin_scanner_a, digital_pin_scanner_b, digital_pin_scanner_c, digital_pin_scanner_d);




const byte pwm_min = 0;
const byte pwm_max = 255;

int delay_us_between_step = 1;







void setup()
{
	pinMode(digital_pin_optocouple_led, OUTPUT);
	digitalWrite(digial_pin_optocouple_led, LOW);

		
	pinMode(digital_pin_scanner_left, INPUT);
	pinMode(digital_pin_scanner_right, INPUT);



}



int scanner_to_start(a, b, c, d, s_left, s_right, s_max)
{
	int scanner_step_conuter = 0;
	while (digitalRead(s_left)<>LOW) && (scanner_setp_counter<)){
		scanner_stepper.step(1);
		scanner_step_counter++;
		delayMicroseconds(scanner_steps_speed);
	}
}


int ground_scanner()
{
	int oc;
	oc = optocouple();
}


void loop()
{
	delay(delay_between_step);
}

