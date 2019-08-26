#include <Wire.h>
#include "MsTimer2.h"

#define FL_wheel_sensor A1
#define FR_wheel_sensor A2
#define RL_wheel_sensor A3
#define RR_wheel_sensor A4

volatile bool en_TimerTick = false;

void ISR_timer() {
	en_TimerTick = true;
} 

void wheel_encoder();
void setup()
{
	pinMode(FL_wheel_sensor, INPUT);
	pinMode(FR_wheel_sensor, INPUT);
	pinMode(RL_wheel_sensor, INPUT);
	pinMode(RR_wheel_sensor, INPUT);

	//pinMode(8, OUTPUT);
	//digitalWrite(8, HIGH);
	
	MsTimer2::set(100, ISR_timer); // ~ms period
	MsTimer2::start();

	Serial.begin(9600);

}

void loop()
{
	//static int wheel_speed[] = { 0,0,0,0 };
	int i, sum = 0;


	wheel_encoder();
	//wheel_encoder(wheel_speed);

}


void wheel_encoder() {
	static int encoder_t0[] = { 0,0,0,0 };
	static int encoder_t_sum[] = { 0,0,0,0 };
	static short encoder_value_arr[250][4] = { 0, };
	static short encoder_high[4] = { 0,0,0,0 };
	static short encoder_low[4] = { 0,0,0,0 };
	static short encoder_counter[4] = { 0,0,0,0 };

	int encoder_t1[] = { 0,0,0,0 };
	int i;
	static int __counter = 0;
	static int iter = 0;

	int sensitivity = 500;

	encoder_t1[0] = analogRead(FL_wheel_sensor);
	encoder_t1[1] = analogRead(FR_wheel_sensor);
	encoder_t1[2] = analogRead(RL_wheel_sensor);
	encoder_t1[3] = analogRead(RR_wheel_sensor);


	for (i = 0; i < 4; i++) {
		//encoder_value_arr[iter][i] = encoder_t1[i];
		if (encoder_t1[i] > sensitivity) {
			encoder_high[i]++;
			encoder_value_arr[iter][i] = 1;
		}
		else {
			encoder_low[i]++;
			encoder_value_arr[iter][i] = 0;
		}
	}

	if (++iter > 248) {
		iter = 0;
	}
	//Serial.print(iter);

	
	//for (i = 0; i < 4; i++) {
	//	Serial.print(encoder_t1[i]);
	//	Serial.print(' ');
	//}
	//Serial.println();

	//for (i = 0; i < 4; i++) {
	//	if (encoder_t1[i] < sensitivity && encoder_t0[i] > sensitivity) {
	//		//        if(++pulse_counter > 239){
	//		//             Serial.println("1rev");
	//		//             pulse_counter=0;
	//		//        }
	//		++encoder_counter[i];
	//	}
	//	encoder_t0[i] = encoder_t1[i];
	//}

	if (en_TimerTick) {
		//}
		//	//Serial.print(iter);
		//Serial.println();

		for (i = 0; i < 4; i++) {
      Serial.print(iter-encoder_high[i]);
      Serial.print(' ');
//      Serial.print(encoder_low[i]);
//      Serial.print(' ');
//      Serial.print(encoder_high[3]+encoder_low[3]);
//      Serial.print(' ');
			encoder_high[i] = 0;
			encoder_low[i] = 0;
		}


//		Serial.print(iter);
//		Serial.print(' ');
		iter = 0;

		//for (i = 0; i < 4; i++) {
		//	Serial.print(' ');
		//	Serial.print(encoder_counter[i]);

		//	//encoder_t_sum[i] += wheel_cnt[i];
		//	encoder_counter[i] = 0;
		//}

		for (i = 0; i < iter; i++) {
			//Serial.print(encoder_value_arr[i][4]);

		}
		Serial.println();
		iter = 0;
		//++__counter;
		//if (__counter > 0) {
		//	for (i = 0; i < 4; i++) {
		//		//	Serial.print((int)(encoder_t_sum[i] / __counter));
		//		//	Serial.print(' ');
		//		//	encoder_t_sum[i] = 0;

		//		}
		//		//Serial.println();
		//		__counter = 0;
		//	}

		//	//Serial.println();



		en_TimerTick = false;


	}



}
