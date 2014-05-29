/*
  linetracer.ino --- control program for robot

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

void linetracer() {

	get_img();
	//SerialPrintImg(img);
	binarization_img(img);
	//SerialPrintImg(bwimg);
	
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print(wave(bwimg, 8, 8));
	SerialPrintImg(bwimg);
	Serial.print("start_line=");Serial.println(simple_line_finder(bwimg, 8, 8, imgWidth-1)); // top of robot
	Serial.print("endeo_line=");Serial.println(simple_line_finder(bwimg, 8, 8, 0)); // down of robot
	
}
