/*
  created 2013.07.31
  by Roman V. Prikhodchenko chujoii@gmail.com
  license GPL
*/



#include <XBee.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#include "addr.h"



LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x20 for a 16 chars and 2 line display





XBee xbee = XBee();

float ftest;

char buf[9]; // menu_max_value_length

int wait_limit_ms = 5000;

// Set D0 to Analog Input
uint8_t set_ADC_ON_cmd[] = { 'D', '0' };
uint8_t set_ADC_ON_val[] = { 0x02 };


// Read from Analog Input
uint8_t get_ADC_cmd[] = { 'I', 'S' };
uint8_t get_ADC_val[] = {  };

// Set D4 to Digital ON                    // fixme need ATAC?
uint8_t set_LED_ON_cmd[] = { 'D', '4' };
uint8_t set_LED_ON_val[] = { 0x05 };

// Set D4 to Digital OFF                   // fixme need ATAC?
uint8_t set_LED_OFF_cmd[] = { 'D', '4' };
uint8_t set_LED_OFF_val[] = { 0x04 };


// SH + SL of your remote radio
//XBeeAddress64 remoteAddress = XBeeAddress64(0x123456, 0xABCDEF01); // see addr.h


// Create a remote AT request with the IR command
RemoteAtCommandRequest remoteAtRequest = RemoteAtCommandRequest(remoteAddress, set_ADC_ON_cmd, set_ADC_ON_val, sizeof(set_ADC_ON_val));
  
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
	
	countdown (15); // delay for join network
	

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("start");








	// fixme need check succes or not ADC=ON


	remoteAtRequest.setCommand(set_ADC_ON_cmd);
	remoteAtRequest.setCommandValue(set_ADC_ON_val);
	remoteAtRequest.setCommandValueLength(sizeof(set_ADC_ON_val));
	
	sendRemoteAtCommand();
	
	// it's a good idea to clear the set value so that the object can be reused for a query
	remoteAtRequest.clearCommandValue();

	delay(5000);
	
}



void loop ()
{

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("LED on");
	
	delay(1000);
	
	remoteAtRequest.setCommand(set_LED_ON_cmd);
	remoteAtRequest.setCommandValue(set_LED_ON_val);
	remoteAtRequest.setCommandValueLength(sizeof(set_LED_ON_val));
	
	sendRemoteAtCommand();
	
	// it's a good idea to clear the set value so that the object can be reused for a query
	remoteAtRequest.clearCommandValue();

	delay(5000);







	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("ADC");
	
	delay(1000);

	remoteAtRequest.setCommand(get_ADC_cmd);
	remoteAtRequest.setCommandValue(get_ADC_val);
	remoteAtRequest.setCommandValueLength(sizeof(get_ADC_val));
	
	sendRemoteAtCommand();
	
	// it's a good idea to clear the set value so that the object can be reused for a query
	remoteAtRequest.clearCommandValue();

	delay(5000);


	



	



	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("LED off");
	
	delay(1000);
	
	remoteAtRequest.setCommand(set_LED_OFF_cmd);
	remoteAtRequest.setCommandValue(set_LED_OFF_val);
	remoteAtRequest.setCommandValueLength(sizeof(set_LED_OFF_val));
	
	sendRemoteAtCommand();
	
	// it's a good idea to clear the set value so that the object can be reused for a query
	remoteAtRequest.clearCommandValue();

	delay(5000);







	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("ADC");
	
	delay(1000);

	remoteAtRequest.setCommand(get_ADC_cmd);
	remoteAtRequest.setCommandValue(get_ADC_val);
	remoteAtRequest.setCommandValueLength(sizeof(get_ADC_val));
	
	sendRemoteAtCommand();
	
	// it's a good idea to clear the set value so that the object can be reused for a query
	remoteAtRequest.clearCommandValue();

	delay(5000);











}



void countdown (int start_seconds)
{
	int counter;
	for (int counter = start_seconds; counter > 0 ; counter--) {
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print(counter);
			delay (1000);
	}	
}



void sendRemoteAtCommand() {
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("Sending command");
	
	// send the command
	xbee.send(remoteAtRequest);
	
	// wait up to 5 seconds for the status response
	if (xbee.readPacket(wait_limit_ms)) {
		// got a response!
		
		// should be an AT command response
		if (xbee.getResponse().getApiId() == REMOTE_AT_COMMAND_RESPONSE) {
			xbee.getResponse().getRemoteAtCommandResponse(remoteAtResponse);
			
			if (remoteAtResponse.isOk()) {
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Command [");
				
				lcd.print(remoteAtResponse.getCommand()[0]);
				lcd.setCursor(0,1);
				lcd.print(remoteAtResponse.getCommand()[1]);
				lcd.print("] was successful!");
				
				if (remoteAtResponse.getValueLength() > 0) {
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("Command value length is ");
					lcd.print(remoteAtResponse.getValueLength(), DEC);
					
					lcd.clear();
					lcd.setCursor(0,0);
					lcd.print("value: (n=");
					lcd.print(remoteAtResponse.getValueLength());					
					lcd.print(")");
					lcd.setCursor(0,1);
					for (int i = 0; i < remoteAtResponse.getValueLength(); i++) {
						lcd.print(remoteAtResponse.getValue()[i], HEX);
						lcd.print(" ");
					}
					
					lcd.print("");
				}
			} else {
				lcd.clear();
				lcd.setCursor(0,0);
				lcd.print("Cmd ret err code:");
				lcd.setCursor(0,1);
				lcd.print(remoteAtResponse.getStatus(), HEX);
			}
		} else {
			lcd.clear();
			lcd.setCursor(0,0);
			lcd.print("Expected Remote AT response but got ");
			lcd.setCursor(0,1);
			lcd.print(xbee.getResponse().getApiId(), HEX);
		}    
	} else if (xbee.getResponse().isError()) {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("Error reading packet:");  
		lcd.setCursor(0,1);
		lcd.print(xbee.getResponse().getErrorCode());
	} else {
		lcd.clear();
		lcd.setCursor(0,0);
		lcd.print("No response from radio");  
	}
}








