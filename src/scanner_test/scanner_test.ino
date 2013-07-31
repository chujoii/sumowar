/*
  created 2013.07.31
  by Roman V. Prikhodchenko chujoii@gmail.com
  license GPL
*/



// strange but lcd.print(12);	not work - after upload only led blinking 
//             lcd.print(String(12)); not work - blinking
// wokr lcd.print("12"); work
//
//	char buf[12]; // "-2147483648\0"
//	itoa(7, buf, 10);
//	lcd.print(buf);

#include <XBee.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <addr.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display

XBee xbee = XBee();

float ftest;

char buf[9]; // menu_max_value_length



// Set DIO0 (pin 20) to Analog Input
uint8_t d0Cmd[] = { 'D', '0' };
uint8_t d0Value[] = { 0x2 };



// SH + SL of your remote radio
//XBeeAddress64 remoteAddress = XBeeAddress64(0x123456, 0xABCDEF01); // see addr.h

// Create a remote AT request with the IR command
RemoteAtCommandRequest remoteAtRequest = RemoteAtCommandRequest(remoteAddress, irCmd, irValue, sizeof(irValue));
  
// Create a Remote AT response object
RemoteAtCommandResponse remoteAtResponse = RemoteAtCommandResponse();






int strLength(char *buffer)
{
	int i=0;
	while(buffer[i]){
		i++;
	}
	return i;
}




void setup ()
{
	lcd.init();                      // initialize the lcd 
	
	// Print a message to the LCD.
	lcd.backlight();
	lcd.print("Hello, world!");



	delay(1000);
	
	Serial.begin(9600);
	xbee.begin(Serial);
	delay(5000); // delay for join network
	
}



void loop ()
{
	lcd.clear();
	lcd.setCursor(0,0);
	
        ftest = 10.0;
	itoa(ftest, buf, 10); // fixme need use sprintf ?
	
	lcd.print(buf);
	delay(1000);	
}
