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
#include <black_white_sensor.h>
#include <encoder.h>

encoder enc(2, 3, 1000);

// global interrupt pin
int G_pin_interrupt = 2;

// optic line-sensors
int G_analog_pin_optocouple = A0;
int G_digital_pin_optocouple_led = 4; // current for modulated LED
int G_delay_us_between_led_on_and_scan = 1;
int G_ground_sensor_threshold = 50; // fixme
black_white_sensor bw_sensor(G_digital_pin_optocouple_led, G_analog_pin_optocouple, G_delay_us_between_led_on_and_scan);


// gnd_scanner (steper-motor, sensors)
int G_digital_pin_gnd_scanner_a = 5;
int G_digital_pin_gnd_scanner_b = 6;
int G_digital_pin_gnd_scanner_c = 7;
int G_digital_pin_gnd_scanner_d = 8;
const int G_maximum_number_of_steps = 1000; // if sensor broken. fixme: need check number steps
int G_digital_pin_gnd_scanner_left = 9;
int G_digital_pin_gnd_scanner_right = 10;
int gnd_scanner_steps_per_revolution = 10; // fixme
int gnd_scanner_steps_speed = 200; // fixme
Stepper gnd_scanner_stepper(gnd_scanner_steps_per_revolution, G_digital_pin_gnd_scanner_a, G_digital_pin_gnd_scanner_b, G_digital_pin_gnd_scanner_c, G_digital_pin_gnd_scanner_d);




const byte pwm_min = 0;
const byte pwm_max = 255;

int delay_us_between_step = 1;


boolean prev_scan[G_maximum_number_of_steps];
boolean now_scan[G_maximum_number_of_steps];

boolean changes = true;

void eintrptwrapper()
{
	enc.encoderhalf();
	changes = true;
}


void setup()
{
		
	pinMode(G_digital_pin_gnd_scanner_left, INPUT);
	pinMode(G_digital_pin_gnd_scanner_right, INPUT);
	digitalWrite(G_digital_pin_gnd_scanner_left, HIGH);
	digitalWrite(G_digital_pin_gnd_scanner_right, HIGH);



	attachInterrupt(0, eintrptwrapper, CHANGE);
	attachInterrupt(1, eintrptwrapper, CHANGE);


	delay(1000);
	Serial.begin(9600);
}



void scanner_to_start(int s_left, int s_max, int scanner_steps_speed)
{
	int scanner_step_conuter = 0;
	// move scanner to left
	while ((digitalRead(s_left) != LOW) && (scanner_step_conuter < s_max)){
		gnd_scanner_stepper.step(-1); //fixme global function
		scanner_step_conuter++;
		delayMicroseconds(scanner_steps_speed);
	}
	
	/*
	// remove backlash (backlash)
	while ((digitalRead(s_left)<>HIGH) && (scanner_setp_counter<scanner_backlash)){
		scanner_stepper.step(1);
		scanner_step_counter++;
		delayMicroseconds(scanner_steps_speed);
	}
	*/
}


int ground_scanner(int sensor_threshold, int maximum_number_of_steps, int sensor_left)
{
	int i;
	for (i=0; i<maximum_number_of_steps; i++){
		prev_scan[i] = now_scan[i];
	}
	
	scanner_to_start(sensor_left, maximum_number_of_steps, gnd_scanner_steps_speed);
	int gnd_scanner_step_conuter = 0;
	
	while ((digitalRead(G_digital_pin_gnd_scanner_right) != LOW) && (gnd_scanner_step_conuter < maximum_number_of_steps)){
		gnd_scanner_stepper.step(-1);
		gnd_scanner_step_conuter++;
		delayMicroseconds(gnd_scanner_steps_speed);
		
		now_scan[i] = bw_sensor.read_black_white(sensor_threshold);
	}
}


//aout simple_ocr(ain, n){
// ain [0][0][0][1][1][1][0][0][0][0][1][1][0][0]
//      0  1  2  3  4  5  6  7  8  9  10 11 12 13
//      _______/--------\___________/-----\_____
// aout [2][5]
//      [9][11]
//
	
//}

void loop()
{
	//ground_scanner(G_ground_sensor_threshold, G_maximum_number_of_steps, G_digital_pin_gnd_scanner_left);
	//delay(delay_us_between_step);



	gnd_scanner_stepper.step(-1); //fixme global function
	delayMicroseconds(gnd_scanner_steps_speed);
	


	if (changes){
		if (enc.get_angle() >= 0) {gnd_scanner_steps_speed = enc.get_angle();}
		Serial.println(enc.get_angle());
		changes = false;
	}
}

