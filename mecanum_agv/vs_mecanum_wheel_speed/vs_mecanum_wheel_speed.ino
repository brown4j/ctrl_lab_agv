#include <Wire.h>
#include "MsTimer2.h"
#define TIMER_INTERVAL 100


#define FL_wheel_sensor A1
#define FR_wheel_sensor A2
#define RL_wheel_sensor A3
#define RR_wheel_sensor A4

volatile bool en_TimerTick = false;
uint8_t wheel_speed[] = { 0,0,0,0 };
short rev_counter[] = { 0,0,0,0 };

void ISR_timer() {
	en_TimerTick = true;
} 

//
//void n_method() {
//	int i;
//
//	for (i = 0; i < 4; i++) {
//		//rev_counter[i] = 0;
//	}
//}


void requestEvent();
void wheel_encoder();
void setup()
{
	pinMode(FL_wheel_sensor, INPUT);
	pinMode(FR_wheel_sensor, INPUT);
	pinMode(RL_wheel_sensor, INPUT);
	pinMode(RR_wheel_sensor, INPUT);

	//pinMode(8, OUTPUT);
	//digitalWrite(8, HIGH);
	
	MsTimer2::set(TIMER_INTERVAL, ISR_timer); // ~ms period
	MsTimer2::start();

	Serial.begin(9600);
	Wire.begin(11); // address 11
	Wire.onRequest(requestEvent);
}

void loop()
{
	wheel_encoder();
	//wheel_encoder(wheel_speed);

}

void wheel_encoder() {
	const int sensitivity[] = {400, 400, 400, 400};
	static int pulse_up[] = { 0,0,0,0 }, pulse_down[] = { 0,0,0,0 };
	static int pulse_counter[] = { 0,0,0,0 }, pulse_iter[] = { 1,1,1,1 };

	//static int encoder_t0[] = { 0,0,0,0 };
	//static int encoder_t_sum[] = { 0,0,0,0 };
	//static short encoder_value_arr[500][4] = { 0, };
	static int encoder_high[4] = { 0,0,0,0 };
	static int encoder_low[4] = { 0,0,0,0 };
	//static short encoder_counter[4] = { 0,0,0,0 };

	int encoder_t1[] = { 0,0,0,0 };
	int i;

	encoder_t1[0] = analogRead(FL_wheel_sensor);
	encoder_t1[1] = analogRead(FR_wheel_sensor);
	encoder_t1[2] = analogRead(RL_wheel_sensor);
	encoder_t1[3] = analogRead(RR_wheel_sensor);
 
	//for (i = 0; i < 4; i++) {
	//	Serial.print(encoder_t1[i]);
	//	Serial.print(' ');
	//}

	//Serial.println();

	for (i = 0; i < 4; i++) {
		if (encoder_t1[i] > sensitivity[i]) { // // // high value
			pulse_up[i] = 1;

			if (pulse_down[i] == 1) {
				//pulse_counter[i] += (encoder_high[i] + encoder_low[i]);
				//pulse_iter[i]++;
				encoder_high[i] = 1;
				//encoder_low[i] = 0;
				pulse_down[i] = 0;
				rev_counter[i]++;
			}
			else {
				if (encoder_high[i] > 3600) {
					encoder_high[i] = 0;
					//rev_counter[i] = 0;

				}
				else {
					encoder_high[i]++;
				}

			}

		}
		else { // // // low value
			//if (__arr > 450) __arr = 0;
			//encoder_value_arr[__arr++][0] = 0;
			//encoder_low[i]++;
			
			if (pulse_up[i] == 1) {
				pulse_down[i] = 1;
			}
		}
	}


	if (en_TimerTick) {
		//n_method();
		for (i = 0; i < 4; i++) {
			//Serial.print(rev_counter[i]);
			wheel_speed[i] = ((60.0 * rev_counter[i]) / (360.0 * TIMER_INTERVAL / 1000.0)); // n method


			//Serial.print(encoder_high[i]);
			//wheel_speed[i] = pulse_counter[i] / pulse_iter[i];

			//Serial.print(wheel_speed[i]);
			Serial.print(wheel_speed[i]);
			Serial.print(' ');
			rev_counter[i] = 0;
			//pulse_counter[i] = 0;
			//pulse_iter[i] = 1;
		}
		   
		Serial.println(); 
		en_TimerTick = false;
	}
}

void requestEvent() {
	//uint8_t tt[8];
	//tt[0] = (uint8_t)rev_counter[0];
	//tt[1] = (uint8_t)(rev_counter[0] >> 8);
	//tt[2] = (uint8_t)rev_counter[1];
	//tt[3] = (uint8_t)(rev_counter[1] >> 8);
	//tt[4] = (uint8_t)rev_counter[2];
	//tt[5] = (uint8_t)(rev_counter[2] >> 8);
	//tt[6] = (uint8_t)rev_counter[3];
	//tt[7] = (uint8_t)(rev_counter[3] >> 8);


	//Wire.write(tt, 8); // respond with message of 8 bytes
	//Wire.write()
	Wire.write(wheel_speed,4); 
	//Wire.write(wheel_speed[2]); 
	//Wire.write(wheel_speed[3]); 
}