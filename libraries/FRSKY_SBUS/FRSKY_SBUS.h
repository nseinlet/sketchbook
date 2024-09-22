
#ifndef FRSKY_SBUS_h
#define FRSKY_SBUS_h

#include <Arduino.h>


#define SBUS_SIGNAL_OK          0x00
#define SBUS_SIGNAL_LOST        0x01
#define SBUS_SIGNAL_FAILSAFE    0x03
#define BAUDRATE 98000

#if defined(ARDUINO_AVR_NANO_EVERY)
  #define port Serial1
#else
  #define port Serial
#endif

#define ACCESS_24
#if defined(ACCESS_24)
  #define CHANNEL_SIZE 26
  #define SBUS_DATA_STATUS 34
  #define SBUS_DATA_SIZE 35
#else
  #define CHANNEL_SIZE 18
  #define SBUS_DATA_STATUS 23
  #define SBUS_DATA_SIZE 24
#endif

#define ALL_CHANNELS

class FRSKY_SBUS
{
	public:
		uint8_t sbusData[SBUS_DATA_SIZE+1];
		int16_t channels[CHANNEL_SIZE];
		int16_t servos[CHANNEL_SIZE];
		uint8_t  failsafe_status;
		int sbus_passthrough;
		int toChannels;
		void begin(void);
		int16_t Channel(uint8_t ch);
		uint8_t DigiChannel(uint8_t ch);
		void Servo(uint8_t ch, int16_t position);
		void DigiServo(uint8_t ch, uint8_t position);
		uint8_t Failsafe(void);
		void PassthroughSet(int mode);
		int PassthroughRet(void);
		void UpdateServos(void);
		void UpdateChannels(void);
		void FeedLine(void);
	private:
		uint8_t byte_in_sbus;
		uint8_t bit_in_sbus;
		uint8_t ch;
		uint8_t bit_in_channel;
		uint8_t bit_in_servo;
		uint8_t inBuffer[SBUS_DATA_SIZE+1];
		int bufferIndex;
		uint8_t inData;
		int feedState;
};

#endif
