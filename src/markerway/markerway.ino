/*
  markerway.ino --- control program for robot

  Copyright (C) 2012 Roman V. Prikhodchenko



  Author: Roman V. Prikhodchenko <chujoii@gmail.com>


  This file is part of markerway.

    markerway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    markerway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with markerway.  If not, see <http://www.gnu.org/licenses/>.



 Keywords: robot



 Usage:


 History:
 
 Project started at 2013.05(may).31



 Code:
*/

#include <math.h>
#include <Stepper.h>
#include <Servo.h>
#include <Wire.h>

// -------------------------------------- physical dimensions --------------------------------------

const int num_of_wheel = 4;
const int R_dimension_a = 70; // mm    half of distance from the robots center to the wheel along the axis x.    fixme 
const int R_dimension_b = 70; // mm    half of distance from the robots center to the wheel along the axis y.    fixme




// -------------------------------------- pins --------------------------------------
// http://arduino.cc/en/Main/ArduinoBoardDuemilanove
// On boards other than the Mega, use of the Servo library disables analogWrite() (PWM) functionality on pins 9 and 10.
//
//  rx    tx          pwm         pwm   pwm               {pwm} {pwm}   pwm         led                           sda   scl
//  0     1     2    [3]    4    [5]   [6]    7     8     [9]   [10]   [11]  [12]  [13]   A0    A1    A2    A3   [A4]  [A5]
//                    wheel       wheel wheel             servo servo   wheel servo servo                         i2c   i2c

// Servo
const int pin_servo_0 = 9;
const int pin_servo_1 = 10;
const int pin_servo_2 = 12;
const int pin_servo_3 = 13;
Servo servo_wheel[num_of_wheel];

// Wheel. this only PWM for speed control. direction - i2c port expander
const int i2c_wheel = 22; // fixme
//                                   0 coord x       1  coord y      2 pwm 3 pin_A(i2c) 4 pin_B(i2c)
const int wheel[num_of_wheel][5] = {{-R_dimension_a, -R_dimension_b, 3,    B00000001,   B00000010},  // 1,  2
                                    {-R_dimension_a, +R_dimension_b, 5,    B00000100,   B00001000},  // 4,  8
                                    {+R_dimension_a, -R_dimension_b, 6,    B00010000,   B00100000},  // 16, 32
                                    {+R_dimension_a, +R_dimension_b, 11,   B01000000,   B10000000}}; // 64, 128



// -------------------------------------- fixme --------------

// global interrupt pin
const int pin_global_interrupt = 2;

// optic line-sensors
const int analog_pin_optocouple = A0;
const int digital_pin_optocouple_led = 4; // current for modulated LED
const int delay_us_between_led_on_and_scan = 1;

// scanner (steper-motor, sensors)
const int digital_pin_scanner_a = 5;
const int digital_pin_scanner_b = 6;
const int digital_pin_scanner_c = 7;
const int digital_pin_scanner_d = 8;
const int maximum_number_of_steps = 100; // if sensor broken. fixme: need check number steps
const int digital_pin_scanner_left = 9;
const int digital_pin_scanner_right = 10;
const int scanner_steps_per_revolution = 10; // fixme
const int scanner_steps_speed = 1000; // fixmes
Stepper scanner_stepper(scanner_steps_per_revolution, digital_pin_scanner_a, digital_pin_scanner_b, digital_pin_scanner_c, digital_pin_scanner_d);




const byte pwm_min = 0;
const byte pwm_max = 255;

const int delay_us_between_step = 1;




int sign(int x){
	if (x>=0) {return 1;} else {return -1;}
}

int remainder(int a, int b)
{
	// exist -330%180 = -150
	// want  -330%180 = 30
	
	return (b+(a%b))%b;
}

boolean xorr(boolean a, boolean b)
{
	return ((!a) && b) || (a && (!b));
}

boolean check_odd_turn(int angle, int oneturn)
{
	//return abs(x/2);
	return xorr((angle/oneturn)%2 == 0, angle>0);
}

int set_wheels (int xt, int yt, int desirable_angular_velocity) 
{
	/*
	  n - number of wheel
	  x,y - center of rotation
	  desirable angular velocity // fixme: for motion without rotation R=infinity => speed = ?  linear velocity 
	  velocity <0 counter-clockwise
	  velocity >0 clockwise

	  return real angular velocity
	 */

	int angle[num_of_wheel];
	double x[num_of_wheel];
	double y[num_of_wheel];
	double r[num_of_wheel];
	int n; // wheel_counter
	
	byte wheel_state = 0;
	

	int wheel_collector_a, wheel_collector_b; // number colum in array "wheel"
	if (sign(desirable_angular_velocity) > 0) {
		wheel_collector_a = 3;
		wheel_collector_b = 4;
	}else{
		wheel_collector_a = 4;
		wheel_collector_b = 3;
	}

	for (n=0; n<num_of_wheel; n++){
		// See drawings/chassis.svg
		x[n] = xt + wheel[n][0];
		y[n] = yt + wheel[n][1];
		
		r[n] = sqrt((x[n]*x[n]) + (y[n]*y[n]));


		angle[n] = int(atan(y[n]/x[n])*(180.0/3.1415926));   //   *(180/3.14) convert rad->gradus
		
		// See drawings/motion.svg
		angle[n] = remainder(angle[n], 180);
		if (check_odd_turn(i,180)) {
			wheel_state = wheel_state | byte(wheel[n][wheel_collector_a]);          // 1 power
			wheel_state = wheel_state & (0xff - byte(wheel[n][wheel_collector_b])); // 0 gnd
		}else{
			wheel_state = wheel_state & (0xff - byte(wheel[n][wheel_collector_a])); // 0 gnd
			wheel_state = wheel_state | byte(wheel[n][wheel_collector_b]);          // 1 power
		}
		servo_wheel[n].write(angle[n]);
	}

	
	Wire.beginTransmission(i2c_wheel);
	Wire.write(wheel_state);
	Wire.endTransmission();




	// calculate maximum radius => maximum linear speed
	double max_R = max(max(max(r[0], r[1]), r[2]), r[3]);
	// maximum linear_velocity = 255; (pwm)
	int real_angular_velocity = 255 / max_R; // fixme strange dimension of 255: rad/sec OR gradus/sec OR rpm ?
	real_angular_velocity = constrain(abs(desirable_angular_velocity), 0, real_angular_velocity);

	
	for (n=0; n<num_of_wheel; n++){
		digitalWrite(wheel[n][2], constrain(r[n]*real_angular_velocity, 0, 255)); // linear_velocity = R * angular_velocity
	}


	return real_angular_velocity;
}




void setup() 
{
	delay(1000);


	pinMode(digital_pin_optocouple_led, OUTPUT);
	digitalWrite(digital_pin_optocouple_led, LOW);

		
	pinMode(digital_pin_scanner_left, INPUT);
	pinMode(digital_pin_scanner_right, INPUT);


	servo_wheel[0].attach(pin_servo_0);
	servo_wheel[1].attach(pin_servo_1);
	servo_wheel[2].attach(pin_servo_2);
	servo_wheel[3].attach(pin_servo_3);

	Wire.begin(); // join i2c bus
}


/*
int scanner_to_start(a, b, c, d, s_left, s_right, s_max)
{
	int scanner_step_conuter = 0;
	while (digitalRead(s_left)<>LOW) && (scanner_setp_counter<)){
		scanner_stepper.step(1);
		scanner_step_counter++;
		delayMicroseconds(scanner_steps_speed);
	}
}
*/

int ground_scanner()
{
	//int oc;
	//oc = optocouple();
	return 0;
}


void loop()
{
	delayMicroseconds(delay_us_between_step);
}

