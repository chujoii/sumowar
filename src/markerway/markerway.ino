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

//#include "ocr.ino"

#include "logic.h"
#include "logic.c" // very strange

//#include "geometry.h"
//#include "geometry.c" // very strange



#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


// -------------------------------------- physical dimensions --------------------------------------

const int num_of_wheel = 4;
const int R_dimension_a = 62; // 62mm    half of distance from the robots center to the wheel along the axis x.    fixme 
const int R_dimension_b = 62; // 62mm    half of distance from the robots center to the wheel along the axis y.    fixme




// -------------------------------------- pins --------------------------------------
// http://arduino.cc/en/Main/ArduinoBoardDuemilanove
// On boards other than the Mega, use of the Servo library disables analogWrite() (PWM) functionality on pins 9 and 10.
//
//  rx    tx          pwm         pwm   pwm               {pwm} {pwm}   pwm         led                           sda   scl
//  [0]   [1]    2    [3]    4    [5]   [6]    7     8     [9]   [10]   [11]  [12]  [13]   A0    A1    A2    A3   [A4]  [A5]
//                    wheel       wheel wheel             servo servo   wheel servo servo                         i2c   i2c



// -------------------------------------- pins --------------------------------------
/* http://arduino.cc/en/Main/ArduinoBoardMega
   ATmega 1280
   On boards other than the Mega, use of the Servo library disables analogWrite() (PWM) functionality on pins 9 and 10.

   pin - capability - function   (if pin without [] == NotConnected)
   [0]  - rx             - rx
   [1]  - tx             - tx
   [2]  - pwm, ext_int 0 - mousecam
   [3]  - pwm, ext_int 1 - mousecam
   [4]  - pwm            - mousecam
   [5]  - pwm            - wheel0
   [6]  - pwm            - wheel2  // fixme 6->7 перепутал ножки?
   [7]  - pwm            - wheel1  // fixme 7->6 перепутал ножки?
   [8]  - pwm            - wheel3
   9    - pwm            - 
   10   - pwm            - 
   11   - pwm            - 
   12   - pwm            - 
   13   - pwm            - 
   14   - tx3            - 
   15   - rx3            - 
   16   - tx2            - 
   17   - rx2            - 
   18   - tx1, ext_int 5 - 
   19   - rx1, ext_int 4 - 
   [20] - sda, ext_int 3 - sda
   [21] - scl, ext_int 2 - scl
   [22] -                - servo0
   [23] -                - servo1
   [24] -                - servo2
   [25] -                - servo3
   26   -                - 
   27   -                - 
   28   -                - 
   29   -                - 
   30   -                - 
   31   -                - 
   32   -                - 
   33   -                - 
   34   -                - 
   35   -                - 
   36   -                - 
   37   -                - 
   38   -                - 
   39   -                - 
   40   -                - 
   41   -                - 
   42   -                - 
   43   -                - 
   44   -                - 
   45   -                - 
   46   -                - 
   47   -                - 
   48   -                - 
   49   -                - 
   50   -                - 
   51   -                - 
   52   -                - 
   [53] -                - start_button

   A0   -                - 
   A1   -                - 
   A2   -                - 
   A3   -                - 
   A4   -                - 
   A5   -                - 
   A6   -                - 
   A7   -                - 
   A8   -                - 
   A9   -                - 
   A10  -                - 
   A11  -                - 
   A12  -                - 
   A13  -                - 
   A14  -                - 
   A15  -                - 
*/

// Servo
const int pin_servo_0 = 22;
const int pin_servo_1 = 23;
const int pin_servo_2 = 24;
const int pin_servo_3 = 25;
Servo servo_wheel[num_of_wheel];

// Wheel. this only PWM for speed control. direction - i2c port expander
const int i2c_wheel_addr = 0x21;
//                                   0 coord x       1  coord y      2 pwm 3 pin_A(i2c) 4 pin_B(i2c)
const int wheel[num_of_wheel][5] = {{-R_dimension_a, -R_dimension_b, 5,    B00000010,   B00000001},  //0   2,  1
                                    {-R_dimension_a, +R_dimension_b, 7,    B00001000,   B00000100},  //1   8,  4 // fixme 6->7 перепутал ножки?
                                    {+R_dimension_a, -R_dimension_b, 6,    B00100000,   B00010000},  //2  32, 16 // fixme 7->6 перепутал ножки?
                                    {+R_dimension_a, +R_dimension_b, 8,    B10000000,   B01000000}}; //3 128, 64




// -------------------------------------- mouse cam ----------------
// Number of pixels in row and column
#define imgWidth 16

byte img[imgWidth][imgWidth];
byte bwimg[imgWidth][imgWidth];

// -------------------------------------- fixme --------------

// global interrupt pin
//const int pin_global_interrupt = 2;

// optic line-sensors
//const int analog_pin_optocouple = A0;
//const int digital_pin_optocouple_led = 4; // current for modulated LED
//const int delay_us_between_led_on_and_scan = 1;

// scanner (steper-motor, sensors)
//const int digital_pin_scanner_a = 5;
//const int digital_pin_scanner_b = 6;
//const int digital_pin_scanner_c = 7;
//const int digital_pin_scanner_d = 8;
//const int maximum_number_of_steps = 100; // if sensor broken. fixme: need check number steps
//const int digital_pin_scanner_left = 9;
//const int digital_pin_scanner_right = 10;
//const int scanner_steps_per_revolution = 10; // fixme
//const int scanner_steps_speed = 1000; // fixmes
//Stepper scanner_stepper(scanner_steps_per_revolution, digital_pin_scanner_a, digital_pin_scanner_b, digital_pin_scanner_c, digital_pin_scanner_d);

const int pin_start_button = 53;

// -------------------------------------- const --------------


const byte pwm_min = 0;
const byte pwm_max = 255;

const int delay_us_between_step = 1;


// -------------------------------------- other --------------


const int i2c_lcd_addr = 0x20;
LiquidCrystal_I2C lcd(i2c_lcd_addr,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

const int i2c_btn_addr = 0x22;

//const int i2c_???_addr = 0x23; // fixme new

// -------------------------------------- code --------------





void wait_for_start_button(){
	while(digitalRead(pin_start_button) == HIGH){}
}


void setup() 
{



	//pinMode(digital_pin_optocouple_led, OUTPUT);
	//digitalWrite(digital_pin_optocouple_led, LOW);

		
	//pinMode(digital_pin_scanner_left, INPUT);
	//pinMode(digital_pin_scanner_right, INPUT);


	pinMode(pin_start_button, INPUT);
	digitalWrite(pin_start_button, HIGH); // internal pull-up resistor
	
	servo_wheel[0].attach(pin_servo_0);
	servo_wheel[1].attach(pin_servo_1);
	servo_wheel[2].attach(pin_servo_2);
	servo_wheel[3].attach(pin_servo_3);
	
	
	set_wheels_straight(90, 0); // stop motor

	
	
	
	
	Wire.begin(); // join i2c bus
	
	
	lcd.init();                      // initialize the lcd 
	lcd.backlight();
	
	// Print a message to the LCD.
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Hello");
	

	delay(1000);
	set_wheels_straight(90, 0); // stop motor
	Serial.begin(115200);
	set_wheels_straight(90, 0); // stop motor

	//Serial.println("Hello");
	


	//delay(1000);
	
	
	

	
	
	
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("wait for start");
	
	wait_for_start_button();
	
	//test_diameter_set_wheels_rotate(100);
	//test_diameter_set_wheels_rotate(-100);
	//
	//wait_for_start_button();
	
	
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("start");
	




	mouse_cam_init();




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
	//delayMicroseconds(delay_us_between_step);
	/*
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("wait...");
	*/
	/*
	Wire.requestFrom(i2c_btn_addr, 1);    // request 1 bytes from slave device 0x21

	while(Wire.available())    // slave may send less than requested
	{ 
		byte c = Wire.read(); // receive a byte as character
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("readed ");
		lcd.setCursor(0,1);
		lcd.print("bin ");
		lcd.print(byte(c), BIN);         // print the character
	}
	
	

	Wire.beginTransmission(i2c_wheel_addr); // transmit to device #4
	Wire.write(B10101010);              // sends one byte  
	Wire.endTransmission();    // stop transmitting


	
	delay(1000);
	*/






}

