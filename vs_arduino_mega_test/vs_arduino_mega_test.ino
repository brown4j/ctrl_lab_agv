/*
 Name:		vs_arduino_mega_test.ino
 Created:	12/30/2016 12:43:56 AM
 Author:	purpl
*/

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600); 
	Serial3.begin(9600);
	pinMode(13, OUTPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	char tx_buf;
	char rx_buf;


	if (Serial.available())  {		
		tx_buf = Serial.read();
		Serial3.print(tx_buf);

	}


	if (Serial3.available()) {
		rx_buf = Serial3.read();
		Serial.print(rx_buf);
	}


	//digitalWrite(13, HIGH);
	//delay(500);
	//digitalWrite(13, LOW);
	//delay(100);
}
