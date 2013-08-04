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
//#include <SoftwareSerial.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#include "addr.h"



LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display


// Define SoftSerial TX/RX pins
// Connect Arduino pin 8 to TX of usb-serial device
//uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
//uint8_t ssTX = 9;
// Remember to connect all devices to a common Ground: XBee, Arduino and USB-Serial device
//SoftwareSerial nss(ssRX, ssTX);




XBee xbee = XBee();

float ftest;

char buf[9]; // menu_max_value_length


// Turn on I/O sampling
uint8_t irCmd[] = {'I','R'};
// Set sample rate to 65 seconds (0xffff/1000)
uint8_t irValue[] = { 0xff, 0xff };


// Set DIO0 (pin 20) to Analog Input
uint8_t d0Cmd[] = { 'D', '0' };
uint8_t d0Value[] = { 0x2 };


//ZBRxIoSampleResponse ioSample = ZBRxIoSampleResponse();


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



	// start terminal
	delay(1000);
	
        // xbee
	Serial.begin(9600);
	//nss.begin(9600);
	xbee.setSerial(Serial);
	xbee.setSerial(Serial);
	
	delay(5000); // delay for join network
	

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("start");

}



void loop ()
{
	
	sendRemoteAtCommand();
	
	// now reuse same object for DIO0 command
	remoteAtRequest.setCommand(d0Cmd);
	remoteAtRequest.setCommandValue(d0Value);
	remoteAtRequest.setCommandValueLength(sizeof(d0Value));
	
	sendRemoteAtCommand();
	
	// it's a good idea to clear the set value so that the object can be reused for a query
	remoteAtRequest.clearCommandValue();
	
	
	
/*
	
	//attempt to read a packet    
	xbee.readPacket();
	
	if (xbee.getResponse().isAvailable()) {
		// got something
		
		if (xbee.getResponse().getApiId() == ZB_IO_SAMPLE_RESPONSE) {
			xbee.getResponse().getZBRxIoSampleResponse(ioSample);
			
			lcd.print("Received I/O Sample from: ");
			
			lcd.print(ioSample.getRemoteAddress64().getMsb(), HEX);  
			lcd.print(ioSample.getRemoteAddress64().getLsb(), HEX);  
			lcd.println("");
			
			if (ioSample.containsAnalog()) {
				lcd.println("Sample contains analog data");
			}
			
			if (ioSample.containsDigital()) {
				lcd.println("Sample contains digtal data");
			}      
			
			// read analog inputs
			for (int i = 0; i <= 4; i++) {
				if (ioSample.isAnalogEnabled(i)) {
					lcd.print("Analog (AI");
					lcd.print(i, DEC);
					lcd.print(") is ");
					lcd.println(ioSample.getAnalog(i), DEC);
				}
			}
			
			// check digital inputs
			for (int i = 0; i <= 12; i++) {
				if (ioSample.isDigitalEnabled(i)) {
					lcd.print("Digital (DI");
					lcd.print(i, DEC);
					lcd.print(") is ");
					lcd.println(ioSample.isDigitalOn(i), DEC);
				}
			}
			
			// method for printing the entire frame data
			//for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
			//  lcd.print("byte [");
			//  lcd.print(i, DEC);
			//  lcd.print("] is ");
			//  lcd.println(xbee.getResponse().getFrameData()[i], HEX);
			//}
		} 
		else {
			lcd.print("Expected I/O Sample, but got ");
			lcd.print(xbee.getResponse().getApiId(), HEX);
		}    
	} else if (xbee.getResponse().isError()) {
		lcd.print("Error reading packet.  Error code: ");  
		lcd.println(xbee.getResponse().getErrorCode());
	}



*/


  delay(1000);

}



void sendRemoteAtCommand() {
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.println("Sending command to the XBee");
	
	// send the command
	xbee.send(remoteAtRequest);
	
	// wait up to 5 seconds for the status response
	if (xbee.readPacket(5000)) {
		// got a response!
		
		// should be an AT command response
		if (xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
			xbee.getResponse().getRemoteAtCommandResponse(remoteAtResponse);
			
			if (remoteAtResponse.isOk()) {
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Command [");
				lcd.print(remoteAtResponse.getCommand()[0]);
				lcd.print(remoteAtResponse.getCommand()[1]);
				lcd.println("] was successful!");
				
				if (remoteAtResponse.getValueLength() > 0) {
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("Command value length is ");
					lcd.println(remoteAtResponse.getValueLength(), DEC);
					
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("Command value: ");
					
					for (int i = 0; i < remoteAtResponse.getValueLength(); i++) {
						lcd.print(remoteAtResponse.getValue()[i], HEX);
						lcd.print(" ");
					}
					
					lcd.println("");
				}
			} else {
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Command returned error code: ");
				lcd.println(remoteAtResponse.getStatus(), HEX);
			}
		} else {
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Expected Remote AT response but got ");
			lcd.print(xbee.getResponse().getApiId(), HEX);
		}    
	} else if (xbee.getResponse().isError()) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Error reading packet.  Error code: ");  
		lcd.println(xbee.getResponse().getErrorCode());
	} else {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("No response from radio");  
	}
}
