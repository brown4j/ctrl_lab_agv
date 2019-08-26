#define __ultrasonic_front_trig A1
#define __ultrasonic_front_echo A2
#define __ultrasonic_back_vcc A5
#define __ultrasonic_back_trig A4
#define __ultrasonic_back_echo A3


int ultrasonic_front() {
	int us_micro, us_dist;

	digitalWrite(__ultrasonic_front_trig, LOW);
	delayMicroseconds(2);
	digitalWrite(__ultrasonic_front_trig, HIGH);
	delayMicroseconds(10);
	digitalWrite(__ultrasonic_front_trig, LOW);
	
	us_micro = pulseIn(__ultrasonic_front_echo, HIGH);
	us_dist = us_micro / 27.6233 / 2;
	if (us_dist > 1.80 && us_dist < 1000.00) {
		delayMicroseconds(10);
		return us_dist;
	}
	return -1;
}

void setup() {
	pinMode(__ultrasonic_front_trig, OUTPUT);
	pinMode(__ultrasonic_front_echo, INPUT);
}

void loop() {
	Serial.begin(9600);
	Serial.println(ultrasonic_front());
	Serial.end();
}
