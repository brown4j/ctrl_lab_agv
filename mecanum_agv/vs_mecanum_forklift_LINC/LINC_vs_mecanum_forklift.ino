#include <Wire.h>
//#include "./header/TimerThree.h" 
//#include "./header/MsTimer2.h"
#define TIMER_INTERVAL 100000

#define DEFAULT_JOYPAD_X_ZP 510
#define DEFAULT_JOYPAD_Y_ZP 500 
#define DEFAULT_JOYPAD_RANGE 505 // 1023 / 2에서 적당한 값으로 
#define DEFAULT_JOYPAD_ZERO 5 

#define FL_DIR_FORWARD HIGH 
#define FL_DIR_BACKWARD LOW   
#define FR_DIR_FORWARD HIGH
#define FR_DIR_BACKWARD LOW
#define RL_DIR_FORWARD LOW
#define RL_DIR_BACKWARD HIGH
#define RR_DIR_FORWARD LOW
#define RR_DIR_BACKWARD HIGH

// AGV size (unit: meter)
#define AGV_LENGTH 0.300 // b
#define AGV_WIDTH 0.280 // a

// Pin 5, 6 : timer0 에 의해 컨트롤
// Pin 9, 10 : timer1 에 의해 컨트롤
// Pin 11, 3 : timer2 에 의해 컨트롤

//#define HC_05_S 17 // disable
#define HC_05_V 16 // TX2
#define HC_05_E 14 // RX2


#define FL_PWM_VC 4
#define FL_PWM_LG 3
#define RL_PWM_VC 6
#define RL_PWM_LG 7
#define FR_PWM_VC 10
#define FR_PWM_LG 9
#define RR_PWM_VC 12
#define RR_PWM_LG 13 

const short WHEEL_PIN[] = { FL_PWM_VC, FL_PWM_LG, RL_PWM_VC, RL_PWM_LG,
							FR_PWM_VC, FR_PWM_LG, RR_PWM_VC, RR_PWM_LG };


enum agv_movement
{
	STOP, EMERGENCY_STOP, FORWARD, BACKWARD,
	LEFT, RIGHT, LEFT_FORWARD, RIGHT_FORWARD, 
	LEFT_BACKWARD, RIGHT_BACKWARD, CW, CCW
};
 

short OPERATION_MODE = 0; // 0: basic, 1: HC-05 ATMODE
char INPUT_MODE = 0; // 0: analog,  1: digital 
volatile bool enable_EMERGENCY_STOP = 0; 

char ch_movement;
volatile unsigned short MSG_RECEIVED = 0;
 
int wheel_speed[] = { 0,0,0,0 };


void init_serial_comm();
void init_pins(); 

void hc_05_on();
void hc_05_off();
void hc_05_atmode();

int msg_extract(const char* _str_msg, int* _arr);

void func_movement_auto(const enum agv_movement val_movement, const unsigned short mag);
void func_movement_manual(const int* joystick_input, int* wheel_spd);
void func_movement_joystick(const int* joystick_input);
void motor_rpm(const int* wheel_speed);

//void ISR_timer() {
//	en_TimerTick = true;
//
//	MSG_RECEIVED = 0;
//
//}
 
void setup(){
	init_pins();
	hc_05_on();
	init_serial_comm();
	 
}

void loop(){
	int i;
	static int iter = 0, total = 0;
	 
}

void init_pins() {
	int i;
	for (i = 0; i < 8; ++i) {
		pinMode(WHEEL_PIN[i], OUTPUT);
	}

	pinMode(HC_05_V, OUTPUT);  pinMode(HC_05_E, OUTPUT); // pinMode(HC_05_S, INPUT);
}

void init_serial_comm() {

	Serial.begin(9600);
	Serial3.begin(9600);
	//while (!Serial);
	//Serial.print("Serial0 OKAY!\r\n");
} 

void hc_05_on() {
	digitalWrite(HC_05_V, HIGH);
	digitalWrite(HC_05_E, HIGH);
	delay(100);
	Serial3.begin(9600);
	//Serial.println("HC_05 ON!");
}

void hc_05_off() {
	//Serial.println("HC_05 OFF!");
	Serial3.end();

	digitalWrite(HC_05_V, LOW);
	digitalWrite(HC_05_E, LOW);
}

void hc_05_atmode() {
	Serial3.end();

	//digitalWrite(HC_05_V, LOW);
	//digitalWrite(HC_05_E, LOW); 
	//digitalWrite(HC_05_V, HIGH);
	//digitalWrite(HC_05_E, HIGH);

	Serial3.begin(38400);
	//while (!Serial3);
	//Serial.println("ON!");
}
 
int msg_extract(const char* _str_msg, int* _arr) {
	char x_arr[5], y_arr[5];
	//char* inv_x, inv_y;
	//int iter = 11;
	int i = 0;
	//int j, k, lx, ly, la;

	if (_str_msg[0] != '<') {
		return 1;
	}

	if (_str_msg[5] != 'X') {
		return 1;
	}

	if (_str_msg[10] != 'Y') {
		return 1; 
	}

	//if (_str_msg[10] != 'A') {
	//	Serial.println('A');
	//	return 1;
	//}

	//inv_x = (char*)malloc(sizeof(char)*(5-0));
	//inv_y = (char*)malloc(sizeof(char)*(5-0));

	x_arr[0] = _str_msg[1];
	x_arr[1] = _str_msg[2];
	x_arr[2] = _str_msg[3];
	x_arr[3] = _str_msg[4];
	x_arr[4] = 0;

	y_arr[0] = _str_msg[6];
	y_arr[1] = _str_msg[7];
	y_arr[2] = _str_msg[8];
	y_arr[3] = _str_msg[9];
	y_arr[4] = 0;

	//_arr[i++] = atoi(inv_y);
	_arr[0] = (int)strtol(x_arr, NULL, 16) - DEFAULT_JOYPAD_X_ZP;
	_arr[1] = (int)strtol(y_arr, NULL, 16) - DEFAULT_JOYPAD_Y_ZP;

	_arr[2] = _str_msg[11] - 48;
	_arr[3] = _str_msg[12] - 48;
	_arr[4] = _str_msg[13] - 48;
	_arr[5] = _str_msg[14] - 48;
	_arr[6] = _str_msg[15] - 48;
	_arr[7] = _str_msg[16] - 48;
	_arr[8] = _str_msg[17] - 48;

	if (abs(_arr[0]) < DEFAULT_JOYPAD_ZERO) _arr[0] = 0;
	if (abs(_arr[1]) < DEFAULT_JOYPAD_ZERO) _arr[1] = 0;

	if (_arr[0] > DEFAULT_JOYPAD_RANGE) _arr[0] = DEFAULT_JOYPAD_RANGE;
	if (_arr[1] > DEFAULT_JOYPAD_RANGE) _arr[1] = DEFAULT_JOYPAD_RANGE;
	if (_arr[0] < -DEFAULT_JOYPAD_RANGE) _arr[0] = -DEFAULT_JOYPAD_RANGE;
	if (_arr[1] < -DEFAULT_JOYPAD_RANGE) _arr[1] = -DEFAULT_JOYPAD_RANGE;

	_arr[0] /= 4;
	_arr[1] /= 4;
	 
	MSG_RECEIVED++;
	return 0;
}

void func_movement_auto(const enum agv_movement val_movement, const unsigned short mag)
{
	const int HIGH_SPEED = 100;
	const int LOW_SPEED = 50;

	static short wheel_state[] = { 0,0,0,0 };
	int wheel_spd[] = { 0,0,0,0 };
	
	switch (val_movement)
	{
	case EMERGENCY_STOP:
		//Serial.write('F');
		motor_rpm(wheel_spd);
		break;
	case FORWARD:
		//Serial.write('W');
		wheel_spd[0] = HIGH_SPEED;
		wheel_spd[1] = HIGH_SPEED;
		wheel_spd[2] = HIGH_SPEED;
		wheel_spd[3] = HIGH_SPEED;
		 
		break;

	case BACKWARD:
		//Serial.write('X');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case LEFT:
		//Serial.write('A');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case RIGHT:
		//Serial.write('D');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case LEFT_FORWARD:
		//Serial.write('Q');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case RIGHT_FORWARD:
		//Serial.write('E');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case LEFT_BACKWARD:
		//Serial.write('Z');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case RIGHT_BACKWARD:
		//Serial.write('C');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case CCW:
		//Serial.write('R');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	case CW:
		//Serial.write('T');
		wheel_spd[0] = -HIGH_SPEED;
		wheel_spd[1] = -HIGH_SPEED;
		wheel_spd[2] = -HIGH_SPEED;
		wheel_spd[3] = -HIGH_SPEED;
		break;

	default: // or STOP
		//Serial.write('S');
		wheel_spd[0] = 0;
		wheel_spd[1] = 0;
		wheel_spd[2] = 0;
		wheel_spd[3] = 0;
		break;
	}
}

void func_movement_joystick(const int* joystick_input) {
	const float joy_sensitive = 1.5;
	int x_abs, y_abs, spd;
	x_abs = abs(joystick_input[0])/3;
	y_abs = abs(joystick_input[1])/3;
	spd = max(x_abs, y_abs);


	if (!joystick_input[2] && !joystick_input[3] && !joystick_input[4] && !joystick_input[5] && 
		!joystick_input[6] && !joystick_input[7] && !joystick_input[8] ) {
		 func_movement_auto(STOP, 0);	
		 return;
	}
	if (!joystick_input[2]) {
		 func_movement_auto(CCW, 0xFF);
		 return;
	}
	if (!joystick_input[3]) {
		 func_movement_auto(CW, 0xFF);
		 return;
	}
	if (x_abs < 3 && y_abs < 3) {
		func_movement_auto(STOP, 0);
		return;
	}
	
	if (joystick_input[0] > 0 && joystick_input[0] > joy_sensitive*joystick_input[1] 
		&& joystick_input[0] > -joy_sensitive*joystick_input[1]) {
		 func_movement_auto(FORWARD, joystick_input[0]);
	}
	else if (joystick_input[0] < 0 && joystick_input[0] < joy_sensitive*joystick_input[1] 
		&& joystick_input[0] < -joy_sensitive*joystick_input[1]) {
		 func_movement_auto(BACKWARD, -joystick_input[0]);
	}
	else if (joystick_input[1] < 0 && joystick_input[0] > joystick_input[1]  
		&& joystick_input[0] < -joystick_input[1] / joy_sensitive) {
		 func_movement_auto(LEFT, -joystick_input[1]);
	}
	else if (joystick_input[1] > 0 && joystick_input[0] < joystick_input[1] / joy_sensitive
		 && joystick_input[0] > -joystick_input[1]/ joy_sensitive) {
		 func_movement_auto(RIGHT, joystick_input[1]);
	}
	else if (joystick_input[0] > 0 && joystick_input[1] > 0) {
		 func_movement_auto(RIGHT_FORWARD, spd);
	}
	else if (joystick_input[0] > 0 && joystick_input[1] < 0) {
		 func_movement_auto(LEFT_FORWARD, spd);
	}
	else if (joystick_input[0] < 0 && joystick_input[1] < 0) {
		 func_movement_auto(LEFT_BACKWARD, spd);
	}
	else if (joystick_input[0] < 0 && joystick_input[1] > 0) {
		 func_movement_auto(RIGHT_BACKWARD, spd);
	}
	else {
		 func_movement_auto(STOP, 0);
	}
	return;
} 

void func_movement_manual(const int* joystick_input, int* wheel_spd)
{
	static short wheel_state[] = { 0,0,0,0 }; // 정지 0, 전진 1, 후진 2 
	const int rotational_spd = 200;
	int omega = 0, i = 0;
	int vy = joystick_input[0];
	int vx = joystick_input[1];

	if (!joystick_input[2]) {
		omega = rotational_spd;
		//abs(vx) < 10  && abs(vy) < 10 ? omega = 250:  omega = 120;
	}
	else if (!joystick_input[3]) {
		omega = -rotational_spd;
		//abs(vx) < 10 && abs(vy) < 10 ? omega = -250 : omega = -120;
	}
	//Serial.print(vx);
	//Serial.print(',');
	//Serial.print(vy);
	//Serial.print(',');
	//Serial.print(',');

	wheel_spd[0] = vx + vy - omega*(AGV_LENGTH + AGV_WIDTH);
	wheel_spd[1] = -vx + vy + omega*(AGV_LENGTH + AGV_WIDTH);
	wheel_spd[2] = -vx + vy - omega*(AGV_LENGTH + AGV_WIDTH);
	wheel_spd[3] = vx + vy + omega*(AGV_LENGTH + AGV_WIDTH);


	//for (i = 0; i < 4; i++) {
	//	wheel_abs_speed[i] = abs(wheel_speed[i]);
	//	if (wheel_abs_speed[i] < 10) {
	//		wheel_abs_speed[i] = 0;
	//		wheel_speed[i] = 0;
	//	}
	//}

	/////////////////////////////////////////////////////////////////////////

	//for (i = 0; i < 4; i++) {
	//	if (wheel_speed[i] > 2) {
	//		//digitalWrite(WHEEL_PWR_PIN[i], LOW);
	//		//digitalWrite(WHEEL_PWR_PIN[i], HIGH);
	//		digitalWrite(WHEEL_DIR_PIN[i], WHEEL_DIRECTION_FORWARD[i]);
	//		//wheel_state[i] = 1;
	//	}
	//	else if(wheel_speed[i] < -2) {
	//		//digitalWrite(WHEEL_PWR_PIN[i], LOW);
	//		//digitalWrite(WHEEL_PWR_PIN[i], HIGH);
	//		digitalWrite(WHEEL_DIR_PIN[i], WHEEL_DIRECTION_BACKWARD[i]);
	//		//wheel_state[i] = 2;
	//	}
	//	else {

	//if (wheel_state[i] == 2) {
	//	wheel_state[i] = 1;
	//}
	//else if (wheel_state[i] == 1) {
	//	wheel_state[i] = 2;
	//}
	//	}
	//}

	//for (i = 0; i < 4; i++) {
	//	//digitalWrite(WHEEL_PWR_PIN[i], HIGH);
	//	analogWrite(WHEEL_PWM_PIN[i], abs(wheel_speed[i]));
	//}

	//Serial.print(wheel_speed[0]);
	//Serial.print(',');
	//Serial.print(wheel_speed[1]);
	//Serial.print(',');
	//Serial.print(wheel_speed[2]);
	//Serial.print(',');
	//Serial.println(wheel_speed[3]);

	//analogWrite(FL_PWM, abs(wheel_speed[0]));
	//analogWrite(FR_PWM, abs(wheel_speed[1]));
	//analogWrite(RL_PWM, abs(wheel_speed[2]));
	//analogWrite(RR_PWM, abs(wheel_speed[3]));
}

void motor_rpm(const int* wheel_speed) {
	const int max_v = 250, min_v = 0;
	const int v_ref = 125;
	int i;

	for (i = 0; i < 4; ++i) {
		if (wheel_speed[i] > 2) {
			analogWrite(WHEEL_PIN[2 * i], wheel_speed[i] * max_v / v_ref);
			analogWrite(WHEEL_PIN[2 * i + 1], min_v);
		}
		else if(wheel_speed[i] < -2) {
			analogWrite(WHEEL_PIN[2 * i], min_v);
			analogWrite(WHEEL_PIN[2 * i + 1], -wheel_speed[i] * max_v / v_ref);
		}
		else {
			analogWrite(WHEEL_PIN[2 * i], min_v);
			analogWrite(WHEEL_PIN[2 * i + 1], min_v);
		}
	}
	
}

void serialEvent() {
	char comm_buf;
	static short pwm_val = 0;
	if (Serial.available()) {
		comm_buf = Serial.read();

		if (OPERATION_MODE == 0) {
			switch (comm_buf) {
			case '/':
				Serial.println("forwarding mode");
				OPERATION_MODE = 1; break;
			case 'q':
				func_movement_auto(LEFT_FORWARD, 0xFF); break;
			case 'w':
				func_movement_auto(FORWARD, 0xFF); break;
			case 'e':
				func_movement_auto(RIGHT_FORWARD, 0xFF); break;
			case 'a':
				func_movement_auto(LEFT, 0xFF); break;
			case 's':
				func_movement_auto(STOP, 0xFF); break;
			case 'd':
				func_movement_auto(RIGHT, 0xFF); break;
			case 'z':
				func_movement_auto(LEFT_BACKWARD, 0xFF); break;
			case 'x':
				func_movement_auto(BACKWARD, 0xFF); break;
			case 'c':
				func_movement_auto(RIGHT_BACKWARD, 0xFF); break;
			case 'r':
				func_movement_auto(CCW, 0xFF); break;
			case 't':
				func_movement_auto(CW, 0xFF); break;
			case 'l':
				func_movement_auto(FORWARD, ++pwm_val);
				Serial.println(pwm_val); break;
			case 'k':
				func_movement_auto(FORWARD, --pwm_val);
				Serial.println(pwm_val); break;
			//case 'n':
			//	hc_05_atmode();  break;
			default:
				break;
			}

		}
		else if (OPERATION_MODE == 1) {
			if (comm_buf == '/') {
				Serial.println("command mode");
				OPERATION_MODE = 0;
			}
			else {
				Serial3.write(comm_buf);
			}
			//int tSerial_input = Serial.read();
			//if (tSerial_input >= '0' && tSerial_input <= 'z') {
			//	Serial_input = tSerial_input;
			//	//Serial3.write(tSerial_input); // int type 
			//	//Serial3.println(tSerial_input); // char type
			//}

		}
	}

}

//////////////////////////////////////////////////// Serial Event 1
void serialEvent3() {
	//static short bad_cnt = 0;
	int joystick_input[9];
	int wheel_spd[] = { 0,0,0,0 };
	char bluetooth_buffer[30] = { 0, };

	//if (en_halt) { 
	//	digitalWrite(FL_DIR, FL_DIR_FORWARD);
	//	digitalWrite(FR_DIR, FR_DIR_FORWARD);
	//	digitalWrite(RL_DIR, RL_DIR_FORWARD);
	//	digitalWrite(RR_DIR, RR_DIR_FORWARD);
	//	analogWrite(FL_PWM, 0xFF);
	//	analogWrite(FR_PWM, 0xFF);
	//	analogWrite(RL_PWM, 0xFF);
	//	analogWrite(RR_PWM, 0xFF);

	//} else 
	if (Serial3.available()) {
		Serial3.readBytesUntil('>', bluetooth_buffer, 25);

		if (!msg_extract(bluetooth_buffer, joystick_input)) {
			//func_movement_joystick(joystick_input);
			func_movement_manual(joystick_input, wheel_spd);
			Serial.println(wheel_spd[0]);
			motor_rpm(wheel_spd);
			//bad_cnt = 0;
		}
		else {
			//if (++bad_cnt > 5) {
			//	func_movement_auto(STOP, 0);
			//}
		}

		if (OPERATION_MODE == 1) {
			Serial.write(bluetooth_buffer);
		}
	}
}  
