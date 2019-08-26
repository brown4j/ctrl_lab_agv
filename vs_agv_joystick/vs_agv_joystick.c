#include <SoftwareSerial.h>
#define HC_06_RX 13
#define HC_06_TX 12
#define HC_06_VOLT 11

SoftwareSerial BluetoothSerial(HC_06_RX, HC_06_TX);
char BLUETOOTH_POWER = 0; // 0: OFF, 1: ON
short OPERATION_MODE = 0; // 0: BT message mode, 1: Serial forward mode
volatile bool msg_sent = false;

// hc-06 mac addr : 20:15:05:29:52:61

void INTERRUPT_RESET() {
	bt_off();
	delay(100);
	bt_initialise();
}

void bt_initialise() {
	digitalWrite(HC_06_VOLT, HIGH);
	delay(100);
	BluetoothSerial.begin(9600);
	BLUETOOTH_POWER = 1;
}

void bt_off() {
	BluetoothSerial.end();
	digitalWrite(HC_06_VOLT, LOW);
	BLUETOOTH_POWER = 0;
}

void setup() {
	attachInterrupt(1, INTERRUPT_RESET, FALLING);
	Serial.begin(9600);
	pinMode(HC_06_VOLT, OUTPUT);

	bt_initialise();
}

void loop() {
	int r_wheel_speed[] = { 0,0,0,0 };
	
	int joyx = analogRead(A0);
	int joyy = analogRead(A1);
	int joya = digitalRead(7);
	int joyb = digitalRead(6);
	int joyc = digitalRead(5);
	int joyd = digitalRead(4);
	int joye = digitalRead(3);
	int joyf = digitalRead(8);
	int joyg = digitalRead(9);
	char serial_buf = '0';
	char x_arr[5], y_arr[5];
	int tx,ty;

	tx = sprintf(x_arr, "%04X", joyx);
	ty = sprintf(y_arr, "%04X", joyy);

	if (!joyf) { 
		Serial.print('<');
		Serial.print(x_arr);
		Serial.print('X');
		Serial.print(y_arr);
		Serial.print('Y');
		Serial.print(joya);
		Serial.print(joyb);
		Serial.print(joyc);
		Serial.print(joyd);
		Serial.print(joye);
		Serial.print(joyf);
		Serial.print(joyg);
		Serial.println('>');
	}
	else {

	}

	if (BLUETOOTH_POWER) {
	//BluetoothSerial.print("<03FFX01F4Y1111101>");

		BluetoothSerial.print('<');
		BluetoothSerial.print(x_arr);
		BluetoothSerial.print('X');
		BluetoothSerial.print(y_arr);
		BluetoothSerial.print('Y');
		BluetoothSerial.print(joya);
		BluetoothSerial.print(joyb);
		BluetoothSerial.print(joyc);
		BluetoothSerial.print(joyd);
		BluetoothSerial.print(joye);
		BluetoothSerial.print(joyf);
		BluetoothSerial.print(joyg);
		BluetoothSerial.print('>');

		if (BluetoothSerial.available()) {
			r_wheel_speed[0] = BluetoothSerial.read()-48;
			r_wheel_speed[1] = BluetoothSerial.read()-48;
			r_wheel_speed[2] = BluetoothSerial.read()-48;
			r_wheel_speed[3] = BluetoothSerial.read()-48;

			Serial.print(r_wheel_speed[0]);
			Serial.print(' ');
		}
	}

	//if (Serial.available()) {
	//	serial_buf = Serial.read();
	//	BluetoothSerial.write(serial_buf);
	//	//if (serial_buf == 'b') {
	//	//	bt_initialise();
	//	//}
	//}
}
