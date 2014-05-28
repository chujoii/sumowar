/*
  chassis.ino --- control program for robot chassis (4 wheel)

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


boolean check_odd_turn(int angle, int oneturn)
{
	//return abs(x/2);
	return (((angle/oneturn)%2 == 0) ^ (angle>0));
}

void test_check_odd_turn()
{
	int a;
	for (a = -360; a <= 360; a+=10){
		Serial.print("alpha = ");Serial.print(a);
		Serial.print("   check_odd_turn = ");Serial.println(check_odd_turn(a, 180));
	}
}


void display_wheel_state_angle (int n, int angle)
{
	if (0 == n) {lcd.clear();} // fixme


	// wheels:
	// 1 3        =     01 11
	// 0 2        =     00 10

	// screen:
	// 0 2        =     00 10
	// 1 3        =     01 11
	
	// need invert y 0->1 1->0
	byte y = bitRead(n, 0)? 0:1;

	lcd.setCursor(bitRead(n, 1)*8 + 0, y);
	lcd.print(angle);
	//lcd.print(n);
	//lcd.print("a");
}

void display_wheel_state_velocity (int n, int velocity)
{
	// wheels:
	// 1 3        =     01 11
	// 0 2        =     00 10

	// screen:
	// 0 2        =     00 10
	// 1 3        =     01 11
	
	// need invert y 0->1 1->0
	byte y = bitRead(n, 0)? 0:1;

	lcd.setCursor(bitRead(n, 1)*8 + 4, y);
	lcd.print(velocity);
	//lcd.print(n);
	//lcd.print("v");

}

int set_wheels_straight (int angle, int desirable_linear_velocity) 
{
	/*
	  n - number of wheel
	  angle - pitch (line coord) in degree (0-360)
	  desirable linear velocity 
	  velocity <0 front move
	  velocity >0 revrese move
	  
	  return real linear velocity
	*/
	
	int n; // wheel_counter
	
	byte wheel_state = 0;
	
	int real_angle;
	byte real_linear_velocity;

	real_angle = myremainder(angle, 180);
	real_linear_velocity = constrain(desirable_linear_velocity, 0, 255);


	int wheel_collector_a, wheel_collector_b; // 3, 4: number colum in array "wheel": 0 coord x       1  coord y      2 pwm 3 pin_A(i2c) 4 pin_B(i2c)

	if (desirable_linear_velocity > 0) {
		wheel_collector_a = 4;
		wheel_collector_b = 3;
	}else{
		wheel_collector_a = 3;
		wheel_collector_b = 4;
	}

	for (n=0; n<num_of_wheel; n++){
		// See drawings/chassis.svg
		
		// See drawings/motion.svg
		
		Serial.print(angle);
		
		if (!check_odd_turn(angle,180)) {
			Serial.print(angle);
			Serial.print("(  0-180) ");
			wheel_state = wheel_state | byte(wheel[n][wheel_collector_a]);          // 1 power
			wheel_state = wheel_state & (0xff - byte(wheel[n][wheel_collector_b])); // 0 gnd
		}else{
			Serial.print("(180-360) ");
			wheel_state = wheel_state & (0xff - byte(wheel[n][wheel_collector_a])); // 0 gnd
			wheel_state = wheel_state | byte(wheel[n][wheel_collector_b]);          // 1 power
		}
		
		servo_wheel[n].write(map(real_angle, 0, 180, 180, 0)); // map for inversion
		Serial.print("servo[");Serial.print(n);Serial.print("] = ");Serial.println(real_angle);
		display_wheel_state_angle(n, real_angle);
	}

	
	Wire.beginTransmission(i2c_wheel_addr);
	Wire.write(wheel_state);
	Wire.endTransmission();
	Serial.println(wheel_state, BIN);
	


	for (n=0; n<num_of_wheel; n++){
		analogWrite(wheel[n][2], real_linear_velocity);
		//lcd.print("speed[");lcd.print(n);lcd.print("] = ");lcd.println(real_linear_velocity);
		display_wheel_state_velocity(n, real_linear_velocity);
	}


	return real_linear_velocity;
}

void test_speed_set_wheels_straight()
{
	int i;
	
	for (i=0; i<255; i++){
		set_wheels_straight(90, i);
		delay(100);
	}
	set_wheels_straight(90, 0); // stop motor
}

void test_angle_set_wheels_straight()
{
	int i;

	for (i=0; i<360; i++){
		set_wheels_straight(i, 92);
		delay(100);
	}
	set_wheels_straight(90, 0); // stop motor
}



int set_wheels_rotate (int xt, int yt, int desirable_angular_velocity)
{
	/*
	  n - number of wheel
	  x,y - center of rotation mm
	  desirable angular velocity // [degree/sec]           fixme: for motion without rotation R=infinity => speed = ?  linear velocity 
	  velocity <0 counter-clockwise
	  velocity >0 clockwise
	  
	  return real angular velocity
	*/
	
	int angle[num_of_wheel];
	int real_angle[num_of_wheel];
	double x[num_of_wheel];
	double y[num_of_wheel];
	double r[num_of_wheel];
	int n; // wheel_counter
	
	byte wheel_state = 0;
	
	
	int wheel_collector_a, wheel_collector_b; // 3, 4: number colum in array "wheel": 0 coord x       1  coord y      2 pwm 3 pin_A(i2c) 4 pin_B(i2c)
	
	
	if (desirable_angular_velocity >= 0) {
		wheel_collector_a = 4;
		wheel_collector_b = 3;
	}else{
		wheel_collector_a = 3;
		wheel_collector_b = 4;
	}
	
	
	for (n=0; n<num_of_wheel; n++){

		// See drawings/chassis.svg
		x[n] = xt - wheel[n][0];
		y[n] = yt - wheel[n][1];
		
		r[n] = sqrt((x[n]*x[n]) + (y[n]*y[n]));
	

		angle[n] = int(atan(y[n]/x[n])*(180.0/3.1415926)) + 90;   //   "*(180/3.14)" convert radian->gradus       "+90" because: "axis of rotation of a wheel" and "plane of rotation of the wheel," they are perpendicular.
		
		// See drawings/motion.svg
		real_angle[n] = myremainder(angle[n], 180);
	
		if (check_odd_turn(angle[n],180)) {
			wheel_state = wheel_state | byte(wheel[n][wheel_collector_a]);          // 1 power 
			wheel_state = wheel_state & (0xff - byte(wheel[n][wheel_collector_b])); // 0 gnd
		}else{
			wheel_state = wheel_state & (0xff - byte(wheel[n][wheel_collector_a])); // 0 gnd
			wheel_state = wheel_state | byte(wheel[n][wheel_collector_b]);          // 1 power
		}
	
		servo_wheel[n].write(map(real_angle[n], 0, 180, 180, 0)); // map for inversion
		Serial.print("servo[");Serial.print(n);Serial.print("] = ");Serial.println(real_angle[n]);
		display_wheel_state_angle(n, real_angle[n]);
        }

	
	Wire.beginTransmission(i2c_wheel_addr);
	Wire.write(wheel_state);
	Wire.endTransmission();


	Serial.println("wheel_state[all] = 33221100");
	Serial.print("wheel_state[all] = "); if (wheel_state< B10000000) {Serial.print("0");}else{} Serial.println(wheel_state, BIN);



	// calculate maximum radius => maximum linear speed
	double max_R = max(max(max(r[0], r[1]), r[2]), r[3]);
	// maximum linear_velocity = 255; (pwm)
	int max_angular_velocity = 255 / max_R; // v=w*r  max_v=255  max_w=max_v/max_r    fixme strange dimension of 255: rad/sec OR gradus/sec OR rpm ?
	int real_angular_velocity = constrain(abs(desirable_angular_velocity), 0, max_angular_velocity);
	Serial.print("real_angular_velacity=");Serial.println(real_angular_velocity);

	
	for (n=0; n<num_of_wheel; n++){
		analogWrite(wheel[n][2], constrain(int(r[n]*real_angular_velocity), 0, 255)); // linear_velocity = R * angular_velocity
		Serial.print("speed[");Serial.print(n);Serial.print("] = ");Serial.println(constrain(int(r[n]*real_angular_velocity), 0, 255));
		display_wheel_state_velocity(n, constrain(int(r[n]*real_angular_velocity), 0, 255));
        }
	

	return real_angular_velocity;
}



void test_diameter_set_wheels_rotate(int cw_or_ccw)
{
	int x, y;


	// cw>0
	// ccw<0
	for (x = -3*(R_dimension_a/2); x<= 3*(R_dimension_a/2); x+=(R_dimension_a/2)){
		for (y = -3*(R_dimension_b/2); y<= 3*(R_dimension_b/2); y+=(R_dimension_b/2)){
			Serial.print("x = ");Serial.print(x);Serial.print("   y = ");Serial.println(y);
			set_wheels_rotate (x, y, 100);
			Serial.println();
			delay(500);
			wait_for_start_button();
		}
	}

	set_wheels_straight(90, 0); // stop motor
}



