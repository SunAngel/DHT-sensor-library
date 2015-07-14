/* DHT library

MIT license
written by Adafruit Industries
*/

#include "DHT.h"

DHT::DHT(uint8_t pin, uint8_t type) {
	_pin = pin;
	_type = type;
	firstreading = true;
}

void DHT::begin(void) {
	// set up the pins!
	pinMode(_pin, INPUT);
	digitalWrite(_pin, HIGH);
	_lastreadtime = 0;
}

//boolean S == Scale.  True == Farenheit; False == Celcius
int16_t DHT::readTemperature(void) {
	int16_t tempx10;

	if (read()) {
		switch (_type) {
			case DHT11:
				return data[2];
			case DHT22:
			case DHT21:
				tempx10 = ((data[2] & 0x7F) << 8) + data[3];
				if (data[2] & 0x80) tempx10 = -tempx10;

				return tempx10;
		}
	}
	return DHT::badTemperature;
}

int16_t DHT::readHumidity(void) {
	int16_t humx10;
	if (read()) {
		switch (_type) {
			case DHT11:
				 return data[0];
			case DHT22:
			case DHT21:
				humx10 = (data[0] << 8) + data[1];
				return humx10;
		}
	}
	return DHT::badHumidity;
}

boolean DHT::read(void) {
	uint8_t j = 0, i;
	int32_t delayT;
	long int currenttime = 0;
	// Check if sensor was read less than two seconds ago and return early
	// to use last reading.
	currenttime = millis();
	if (currenttime < _lastreadtime) {
		// ie there was a rollover
		_lastreadtime = 0;
	}
	if (!firstreading && ((currenttime - _lastreadtime) < 2000)) {
		return true; // return last correct measurement
		//delay(2000 - (currenttime - _lastreadtime));
	}
	firstreading = false;
	/*
	   Serial.print("Currtime: "); Serial.print(currenttime);
	   Serial.print(" Lasttime: "); Serial.print(_lastreadtime);
	   */
	_lastreadtime = millis();

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	// pull the pin high and wait 10 milliseconds
	digitalWrite(_pin, HIGH);
	delay(10);

	// now pull it low for ~1 milliseconds
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	delay(1);
	noInterrupts();
	digitalWrite(_pin, HIGH);
	delayMicroseconds(40);
	pinMode(_pin, INPUT);
	delayMicroseconds(10);

	// read in timings
	//  pulseIn(_pin,LOW);
	for ( i=0; i< MAXTIMINGS; i++) {
		delayT = pulseIn(_pin,HIGH, 100000);
		if (delayT == 0) { 
			break; 
		}

		// ignore first 3 transitions
		if (i >= 1) {
			// shove each bit into the storage bytes
			data[j/8] <<= 1;
			if (delayT > 50)
				data[j/8] |= 1;
			j++;
		}

	}

	interrupts();

	/* 
	   Serial.println(j, DEC);
	   Serial.print(data[0], HEX); Serial.print(", ");
	   Serial.print(data[1], HEX); Serial.print(", ");
	   Serial.print(data[2], HEX); Serial.print(", ");
	   Serial.print(data[3], HEX); Serial.print(", ");
	   Serial.print(data[4], HEX); Serial.print(" =? ");
	   Serial.println(data[0] + data[1] + data[2] + data[3], HEX);
	   */

	// check we read 40 bits and that the checksum matches
	if ((j >= 40) && 
			(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) ) {
		return true;
	}


	return false;

}
