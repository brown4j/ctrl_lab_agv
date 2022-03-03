#include <LiquidCrystal.h>
#include <Wire.h>
#include "./header/TimerThree.h" 
#include "./header/MECANUM.h" 
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

#define FL_PWM 9
#define RL_PWM 10
#define FR_PWM 11
#define RR_PWM 12

#define FL_DIR A11
#define RL_DIR A10
#define FR_DIR A9
#define RR_DIR A8

#define FL_ONOFF A12
#define FR_ONOFF A13
#define RL_ONOFF A14
#define RR_ONOFF A15

#define wheel_sensor_volt A0

#define FL_wheel_sensor A1
#define FR_wheel_sensor A2
#define RL_wheel_sensor A3
#define RR_wheel_sensor A4


enum agv_movement
{
	STOP, EMERGENCY_STOP, FORWARD, BACKWARD,
	LEFT, RIGHT, LEFT_FORWARD, RIGHT_FORWARD,
	LEFT_BACKWARD, RIGHT_BACKWARD, CW, CCW
};


const short WHEEL_PWM_PIN[] = { FL_PWM , FR_PWM , RL_PWM , RR_PWM };
const short WHEEL_PWR_PIN[] = { FL_ONOFF , FR_ONOFF , RL_ONOFF , RR_ONOFF };
const short WHEEL_DIR_PIN[] = { FL_DIR , FR_DIR , RL_DIR , RR_DIR };
const short WHEEL_DIRECTION_BACKWARD[] = { FL_DIR_BACKWARD , FR_DIR_BACKWARD ,
											RL_DIR_BACKWARD , RR_DIR_BACKWARD };
const short WHEEL_DIRECTION_FORWARD[] = { FL_DIR_FORWARD , FR_DIR_FORWARD ,
											RL_DIR_FORWARD , RR_DIR_FORWARD };
const short WHEEL_SPEED_SENSOR[] = { FL_wheel_sensor , FR_wheel_sensor ,
										RL_wheel_sensor , RR_wheel_sensor };


//LiquidCrystal lcd(__display_p4, __display_p6, __display_p11, 
//	__display_p12, __display_p13, __display_p14);

short OPERATION_MODE = 0; // 0: basic, 1: HC-05 ATMODE
char INPUT_MODE = 0; // 0: analog,  1: digital 
volatile bool enable_EMERGENCY_STOP = 0;
volatile bool enable_ultrasonic = 0;
volatile bool enable_lcd = 0;
char ch_movement;
volatile unsigned short MSG_RECEIVED = 0;

volatile bool en_TimerTick = false;
volatile bool en_halt = false;

int wheel_speed[] = { 0,0,0,0 };


void init_serial_comm();
void init_pins();
void init_pwm_freq();
//void init_lcd();

void hc_05_on();
void hc_05_off();
void hc_05_atmode();

int msg_extract(const char* _str_msg, int* _arr);

void func_movement_auto(const enum agv_movement val_movement, const unsigned short mag);
void func_movement_manual(const int* joystick_input);
void func_movement_joystick(const int* joystick_input);

void ISR_timer() {
	en_TimerTick = true;

	MSG_RECEIVED = 0;

}
 
void setup(){
	init_pins();
	init_pwm_freq();
	init_serial_comm();
	
	Wire.begin();
	hc_05_on();
	//func_movement_auto(STOP, 0);

	//MsTimer2::set(200, isr_tick_1); // 500ms period
	//MsTimer2::start();

	Timer3.initialize(TIMER_INTERVAL);         // initialize timer1, and set a 1/2 second period
	Timer3.pwm(6, 512);                // setup pwm on pin 9, 50% duty cycle
	Timer3.attachInterrupt(ISR_timer);  // attaches callback() as a timer overflow interrupt
}

void loop(){
	int i;
	static int iter = 0, total = 0;

	if (en_TimerTick) {
		Wire.requestFrom(11, 8);    // request 8 bytes from slave device #11

		if (Wire.available()) { // slave may send less than requested
			wheel_speed[0] = Wire.read(); // receive a byte as character
			//wheel_speed[0] |= Wire.read() << 8; // receive a byte as character
			wheel_speed[1] = Wire.read(); // receive a byte as character
			//wheel_speed[1] |= Wire.read() << 8; // receive a byte as character
			wheel_speed[2] = Wire.read(); // receive a byte as character
			//wheel_speed[2] |= Wire.read() << 8; // receive a byte as character
			wheel_speed[3] = Wire.read(); // receive a byte as character
			//wheel_speed[3] |= Wire.read() << 8; // receive a byte as character
			

			//for (i = 0; i < 4; i++) {
			//	Serial.print(wheel_speed[i] / 10);  // print the character
			//	Serial.print(' ');             // print the character
			//	
			//}
			//Serial.println();
		}
		en_TimerTick = false;
	}
}

void init_pins() {
	pinMode(FL_ONOFF, OUTPUT);
	pinMode(FR_ONOFF, OUTPUT);
	pinMode(RL_ONOFF, OUTPUT);
	pinMode(RR_ONOFF, OUTPUT);

	pinMode(FL_PWM, OUTPUT);  pinMode(FL_DIR, OUTPUT);
	pinMode(FR_PWM, OUTPUT);  pinMode(FR_DIR, OUTPUT);
	pinMode(RL_PWM, OUTPUT);  pinMode(RL_DIR, OUTPUT);
	pinMode(RR_PWM, OUTPUT);  pinMode(RR_DIR, OUTPUT);

	analogWrite(FL_PWM, 0x0);
	analogWrite(FR_PWM, 0x0);
	analogWrite(RL_PWM, 0x0);
	analogWrite(RR_PWM, 0x0);
	 
	digitalWrite(FL_ONOFF, LOW);
	digitalWrite(FR_ONOFF, LOW);
	digitalWrite(RL_ONOFF, LOW);
	digitalWrite(RR_ONOFF, LOW);
	
	pinMode(HC_05_V, OUTPUT);  pinMode(HC_05_E, OUTPUT); // pinMode(HC_05_S, INPUT);
	//pinMode(__display_p2_vcc, OUTPUT);	
	//digitalWrite(__display_p2_vcc, HIGH);

	pinMode(FL_wheel_sensor, INPUT);
	pinMode(FR_wheel_sensor, INPUT);
	pinMode(RL_wheel_sensor, INPUT);
	pinMode(RR_wheel_sensor, INPUT);
}

void init_serial_comm() {
	Serial.begin(9600);
	while (!Serial);
	//Serial.print("Serial0 OKAY!\r\n");
} 

void hc_05_on() {
	digitalWrite(HC_05_V, HIGH);
	digitalWrite(HC_05_E, HIGH);
	delay(100);
	Serial3.begin(9600);
	//Serial.println("HC_05 ON!");
}

void hc_05_off(){
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

void init_pwm_freq()
{
	// from https://arduino-info.wikispaces.com/Arduino-PWM-Frequency

	//For Arduino Mega1280, Mega2560, MegaADK, Spider or any other board using ATmega1280 or ATmega2560**

	//---------------------------------------------- Set PWM frequency for D4 & D13 ------------------------------

	//TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
	//TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
	//TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz (Default)
	//TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
	//TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz


	//---------------------------------------------- Set PWM frequency for D11 & D12 -----------------------------

	TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
												//TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
												//TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz
												//TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz
												//TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz

												//---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------

	TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz
												//TCCR2B = TCCR2B & B11111000 | B00000010;    // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
												//TCCR2B = TCCR2B & B11111000 | B00000011;    // set timer 2 divisor to    32 for PWM frequency of   980.39 Hz
												//TCCR2B = TCCR2B & B11111000 | B00000100;    // set timer 2 divisor to    64 for PWM frequency of   490.20 Hz
												//TCCR2B = TCCR2B & B11111000 | B00000101;    // set timer 2 divisor to   128 for PWM frequency of   245.10 Hz
												//TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to   256 for PWM frequency of   122.55 Hz
												//TCCR2B = TCCR2B & B11111000 | B00000111;    // set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz


												//---------------------------------------------- Set PWM frequency for D2, D3 & D5 ---------------------------

												//TCCR3B = TCCR3B & B11111000 | B00000001;    // set timer 3 divisor to     1 for PWM frequency of 31372.55 Hz
												//TCCR3B = TCCR3B & B11111000 | B00000010;    // set timer 3 divisor to     8 for PWM frequency of  3921.16 Hz
												//TCCR3B = TCCR3B & B11111000 | B00000011;    // set timer 3 divisor to    64 for PWM frequency of   490.20 Hz
												//TCCR3B = TCCR3B & B11111000 | B00000100;    // set timer 3 divisor to   256 for PWM frequency of   122.55 Hz
												//TCCR3B = TCCR3B & B11111000 | B00000101;    // set timer 3 divisor to  1024 for PWM frequency of    30.64 Hz


												//---------------------------------------------- Set PWM frequency for D6, D7 & D8 ---------------------------

												//TCCR4B = TCCR4B & B11111000 | B00000001;    // set timer 4 divisor to     1 for PWM frequency of 31372.55 Hz
												//TCCR4B = TCCR4B & B11111000 | B00000010;    // set timer 4 divisor to     8 for PWM frequency of  3921.16 Hz
												//TCCR4B = TCCR4B & B11111000 | B00000011;    // set timer 4 divisor to    64 for PWM frequency of   490.20 Hz
												//TCCR4B = TCCR4B & B11111000 | B00000100;    // set timer 4 divisor to   256 for PWM frequency of   122.55 Hz
												//TCCR4B = TCCR4B & B11111000 | B00000101;    // set timer 4 divisor to  1024 for PWM frequency of    30.64 Hz


												//---------------------------------------------- Set PWM frequency for D44, D45 & D46 ------------------------

												//TCCR5B = TCCR5B & B11111000 | B00000001;    // set timer 5 divisor to     1 for PWM frequency of 31372.55 Hz
												//TCCR5B = TCCR5B & B11111000 | B00000010;    // set timer 5 divisor to     8 for PWM frequency of  3921.16 Hz
												//TCCR5B = TCCR5B & B11111000 | B00000011;    // set timer 5 divisor to    64 for PWM frequency of   490.20 Hz
												//TCCR5B = TCCR5B & B11111000 | B00000100;    // set timer 5 divisor to   256 for PWM frequency of   122.55 Hz
												//TCCR5B = TCCR5B & B11111000 | B00000101;    // set timer 5 divisor to  1024 for PWM frequency of    30.64 Hz
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
	
	//Serial.print(_arr[0]);
	//Serial.print(',');
	//Serial.print(_arr[1]);
	//Serial.print(',');
	//Serial.print(',');

	//while (_str_msg[iter] != 'P') {
	//_arr[i++] = _str_msg[iter++] - 48;
	//}

	//for (i = 2; i < 9; i++) {
	//	_arr[i] = _str_msg[i + 9] - 48;
	//}

	//free(inv_x);
	//free(inv_y);


	//
	//	for (i = lx, j = 0; i < ly - 1; i++, j++) {
	//		inv_x[j] = _str_msg[i];
	//	}
	//	inv_x[j] = 0;
	//
	//	for (i = ly, j = 0; i < la - 1; i++, j++) {
	//		inv_y[j] = _str_msg[i];
	//	}
	//	inv_y[j] = 0;

	MSG_RECEIVED++;
	return 0;
}

void func_movement_auto(const enum agv_movement val_movement, const unsigned short mag)
{
	static short wheel_state[] = { 0,0,0,0 };
	switch (val_movement)
	{
	case EMERGENCY_STOP:
		//Serial.write('F');
		wheel_state[0] ? digitalWrite(FL_DIR, FL_DIR_BACKWARD) : digitalWrite(FL_DIR, FL_DIR_FORWARD);
		wheel_state[1] ? digitalWrite(FR_DIR, FR_DIR_BACKWARD) : digitalWrite(FR_DIR, FR_DIR_FORWARD);
		wheel_state[2] ? digitalWrite(RL_DIR, RL_DIR_BACKWARD) : digitalWrite(RL_DIR, RL_DIR_FORWARD);
		wheel_state[3] ? digitalWrite(RR_DIR, RR_DIR_BACKWARD) : digitalWrite(RR_DIR, RR_DIR_FORWARD);

		analogWrite(FL_PWM, 0);
		analogWrite(FR_PWM, 0);
		analogWrite(RL_PWM, 0);
		analogWrite(RR_PWM, 0);
		break;
	case FORWARD:
		//Serial.write('W');
		digitalWrite(FL_DIR, FL_DIR_FORWARD);
		digitalWrite(FR_DIR, FR_DIR_FORWARD);
		digitalWrite(RL_DIR, RL_DIR_FORWARD);
		digitalWrite(RR_DIR, RR_DIR_FORWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 1;
		wheel_state[1] = 1;
		wheel_state[2] = 1;
		wheel_state[3] = 1;
		break;

	case BACKWARD:
		//Serial.write('X');
		digitalWrite(FL_DIR, FL_DIR_BACKWARD);
		digitalWrite(FR_DIR, FR_DIR_BACKWARD);
		digitalWrite(RL_DIR, RL_DIR_BACKWARD);
		digitalWrite(RR_DIR, RR_DIR_BACKWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 0;
		wheel_state[1] = 0;
		wheel_state[2] = 0;
		wheel_state[3] = 0;
		break;

	case LEFT:
		//Serial.write('A');
		digitalWrite(FL_DIR, FL_DIR_BACKWARD);
		digitalWrite(FR_DIR, FR_DIR_FORWARD);
		digitalWrite(RL_DIR, RL_DIR_FORWARD);
		digitalWrite(RR_DIR, RR_DIR_BACKWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 0;
		wheel_state[1] = 1;
		wheel_state[2] = 1;
		wheel_state[3] = 0;
		break;

	case RIGHT:
		//Serial.write('D');
		digitalWrite(FL_DIR, FL_DIR_FORWARD);
		digitalWrite(FR_DIR, FR_DIR_BACKWARD);
		digitalWrite(RL_DIR, RL_DIR_BACKWARD);
		digitalWrite(RR_DIR, RR_DIR_FORWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 1;
		wheel_state[1] = 0;
		wheel_state[2] = 0;
		wheel_state[3] = 1;
		break;

	case LEFT_FORWARD:
		//Serial.write('Q');
		digitalWrite(FL_DIR, FL_DIR_FORWARD);
		digitalWrite(FR_DIR, FR_DIR_FORWARD);
		digitalWrite(RL_DIR, RL_DIR_FORWARD);
		digitalWrite(RR_DIR, RR_DIR_FORWARD);
		analogWrite(FL_PWM, 0);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, 0);
		wheel_state[0] = 1;
		wheel_state[1] = 1;
		wheel_state[2] = 1;
		wheel_state[3] = 1;
		break;

	case RIGHT_FORWARD:
		//Serial.write('E');
		digitalWrite(FL_DIR, FL_DIR_FORWARD);
		digitalWrite(FR_DIR, FR_DIR_FORWARD);
		digitalWrite(RL_DIR, RL_DIR_FORWARD);
		digitalWrite(RR_DIR, RR_DIR_FORWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, 0);
		analogWrite(RL_PWM, 0);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 1;
		wheel_state[1] = 1;
		wheel_state[2] = 1;
		wheel_state[3] = 1;
		break;

	case LEFT_BACKWARD:
		//Serial.write('Z');
		digitalWrite(FL_DIR, FL_DIR_BACKWARD);
		digitalWrite(FR_DIR, FR_DIR_BACKWARD);
		digitalWrite(RL_DIR, RL_DIR_BACKWARD);
		digitalWrite(RR_DIR, RR_DIR_BACKWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, 0);
		analogWrite(RL_PWM, 0);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 0;
		wheel_state[1] = 0;
		wheel_state[2] = 0;
		wheel_state[3] = 0;
		break;

	case RIGHT_BACKWARD:
		//Serial.write('C');
		digitalWrite(FL_DIR, FL_DIR_BACKWARD);
		digitalWrite(FR_DIR, FR_DIR_BACKWARD);
		digitalWrite(RL_DIR, RL_DIR_BACKWARD);
		digitalWrite(RR_DIR, RR_DIR_BACKWARD);
		analogWrite(FL_PWM, 0);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, 0);
		wheel_state[0] = 0;
		wheel_state[1] = 0;
		wheel_state[2] = 0;
		wheel_state[3] = 0;
		break;

	case CCW:
		//Serial.write('R');
		digitalWrite(FL_DIR, FL_DIR_BACKWARD);
		digitalWrite(FR_DIR, FR_DIR_FORWARD);
		digitalWrite(RL_DIR, RL_DIR_BACKWARD);
		digitalWrite(RR_DIR, RR_DIR_FORWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 0;
		wheel_state[1] = 1;
		wheel_state[2] = 0;
		wheel_state[3] = 1;
		break;

	case CW:
		//Serial.write('T');
		digitalWrite(FL_DIR, FL_DIR_FORWARD);
		digitalWrite(FR_DIR, FR_DIR_BACKWARD);
		digitalWrite(RL_DIR, RL_DIR_FORWARD);
		digitalWrite(RR_DIR, RR_DIR_BACKWARD);
		analogWrite(FL_PWM, mag);
		analogWrite(FR_PWM, mag);
		analogWrite(RL_PWM, mag);
		analogWrite(RR_PWM, mag);
		wheel_state[0] = 1;
		wheel_state[1] = 0;
		wheel_state[2] = 1;
		wheel_state[3] = 0;
		break;

	default: // or STOP
		//Serial.write('S');
		wheel_state[0] ? digitalWrite(FL_DIR, FL_DIR_BACKWARD) : digitalWrite(FL_DIR, FL_DIR_FORWARD);
		wheel_state[1] ? digitalWrite(FR_DIR, FR_DIR_BACKWARD) : digitalWrite(FR_DIR, FR_DIR_FORWARD);
		wheel_state[2] ? digitalWrite(RL_DIR, RL_DIR_BACKWARD) : digitalWrite(RL_DIR, RL_DIR_FORWARD);
		wheel_state[3] ? digitalWrite(RR_DIR, RR_DIR_BACKWARD) : digitalWrite(RR_DIR, RR_DIR_FORWARD);

		analogWrite(FL_PWM, 0);
		analogWrite(FR_PWM, 0);
		analogWrite(RL_PWM, 0);
		analogWrite(RR_PWM, 0);
		break;
	}
}
void func_movement_manual(const int* joystick_input)
{
	static short wheel_state[] = { 0,0,0,0 }; // 정지 0, 전진 1, 후진 2
	int wheel_speed[] = { 0,0,0,0 }; // FL, FR, RL, RR
	//int wheel_abs_speed[] = { 0,0,0,0 }; // FL, FR, RL, RR
	int omega = 0, i = 0;
	int vy = joystick_input[0];
	int vx = joystick_input[1];
	
	if (!joystick_input[2]) {
		omega = 100;
		//abs(vx) < 10  && abs(vy) < 10 ? omega = 250:  omega = 120;
	}
	else if (!joystick_input[3]) {
		omega = -100;
		//abs(vx) < 10 && abs(vy) < 10 ? omega = -250 : omega = -120;
	}
	//Serial.print(vx);
	//Serial.print(',');
	//Serial.print(vy);
	//Serial.print(',');
	//Serial.print(',');

	wheel_speed[0] = vx + vy - omega*(AGV_LENGTH + AGV_WIDTH);
	wheel_speed[1] = -vx + vy + omega*(AGV_LENGTH + AGV_WIDTH);
	wheel_speed[2] = -vx + vy - omega*(AGV_LENGTH + AGV_WIDTH);
	wheel_speed[3] = vx + vy + omega*(AGV_LENGTH + AGV_WIDTH);

	//for (i = 0; i < 4; i++) {
	//	wheel_abs_speed[i] = abs(wheel_speed[i]);
	//	if (wheel_abs_speed[i] < 10) {
	//		wheel_abs_speed[i] = 0;
	//		wheel_speed[i] = 0;
	//	}
	//}

	/////////////////////////////////////////////////////////////////////////

	for (i = 0; i < 4; i++) {
		if (wheel_speed[i] > 2) {
			//digitalWrite(WHEEL_PWR_PIN[i], LOW);
			//digitalWrite(WHEEL_PWR_PIN[i], HIGH);
			digitalWrite(WHEEL_DIR_PIN[i], WHEEL_DIRECTION_FORWARD[i]);
			//wheel_state[i] = 1;
		}
		else if(wheel_speed[i] < -2) {
			//digitalWrite(WHEEL_PWR_PIN[i], LOW);
			//digitalWrite(WHEEL_PWR_PIN[i], HIGH);
			digitalWrite(WHEEL_DIR_PIN[i], WHEEL_DIRECTION_BACKWARD[i]);
			//wheel_state[i] = 2;
		}
		else {

			//if (wheel_state[i] == 2) {
			//	wheel_state[i] = 1;
			//}
			//else if (wheel_state[i] == 1) {
			//	wheel_state[i] = 2;
			//}
		}
	}

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

	analogWrite(FL_PWM, abs(wheel_speed[0]));
	analogWrite(FR_PWM, abs(wheel_speed[1]));
	analogWrite(RL_PWM, abs(wheel_speed[2]));
	analogWrite(RR_PWM, abs(wheel_speed[3]));
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
//void init_lcd() {
//	lcd.begin(16, 2);
//	lcd.print(" *MECANUM AGV*");
//
//	lcd.setCursor(0, 1);
//}

//////////////////////////////////////////////////// Serial Event 0

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
			case 'n':
				hc_05_atmode();  break;
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
	char bluetooth_buffer[30] = { 0, };

	if (en_halt) { 
		digitalWrite(FL_DIR, FL_DIR_FORWARD);
		digitalWrite(FR_DIR, FR_DIR_FORWARD);
		digitalWrite(RL_DIR, RL_DIR_FORWARD);
		digitalWrite(RR_DIR, RR_DIR_FORWARD);
		analogWrite(FL_PWM, 0xFF);
		analogWrite(FR_PWM, 0xFF);
		analogWrite(RL_PWM, 0xFF);
		analogWrite(RR_PWM, 0xFF);

	} else if (Serial3.available()) {
		Serial3.readBytesUntil('>', bluetooth_buffer, 25);

		if (!msg_extract(bluetooth_buffer, joystick_input)) {
			//func_movement_joystick(joystick_input);
			func_movement_manual(joystick_input); 
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

//
//int ultrasonic_front() {
//	int us_micro, us_dist;
//
//	digitalWrite(__ultrasonic_front_trig, LOW);
//	delayMicroseconds(2);
//	digitalWrite(__ultrasonic_front_trig, HIGH);
//	delayMicroseconds(10);
//	digitalWrite(__ultrasonic_front_trig, LOW);
//
//	us_micro = pulseIn(__ultrasonic_front_echo, HIGH);
//	us_dist = us_micro / 27.6233 / 2;
//	if (us_dist > 1.80 && us_dist < 1000.00) {
//		delayMicroseconds(10);
//		return us_dist;
//	}
//	return -1;
//}

//
//void isr_tick_100ms() {
//	Serial.print("pwm : ");
//	Serial.print(pwm);
//	Serial.print(", pulse counter per sec : ");
//	Serial.println(pulse_counter);
//	pulse_counter = 0;
//
//}

