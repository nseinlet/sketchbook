/*
  FrSky sensor data polling class for Teensy 3.x and 328P based boards (e.g. Pro Mini, Nano, Uno)
  (c) Pawelsky 20160818
  Not for commercial use
*/

#include "FrSkySportPolling.h"

FrSkySportPolling::FrSkySportPolling()
{
  nextPollIdIdx = 0;
  nextPollTime = 0;
  rssiPollTime = 0;
}

FrSkySportSensor::SensorId FrSkySportPolling::getNextId()
{
  if(nextPollIdIdx >= FRSKY_POLLIED_ID_COUNT) nextPollIdIdx = 0;
  return POLLED_ID_TABLE[nextPollIdIdx++];
}

FrSkySportSensor::SensorId FrSkySportPolling::pollData(FrSkySportSingleWireSerial& serial, uint32_t now)
{
  FrSkySportSensor::SensorId id = FrSkySportSensor::ID_IGNORE;

  if(serial.port != NULL)
  {
    // Send RSSI every 444ms independent of other IDs
    if(now >= rssiPollTime)
    {
      serial.sendHeader(FrSkySportSensor::ID25);
      serial.sendData(POLLING_RSSI_DATA_ID, 100); // Always report full strength
      rssiPollTime = now + POLLING_RSSI_POLL_TIME;
      nextPollTime = now + POLLING_ID_POLL_TIME;
    }
    // Poll next ID every 12ms
    else if(now >= nextPollTime)
    {
      id = getNextId();
      serial.sendHeader(id);
      nextPollTime = now + POLLING_ID_POLL_TIME;
    }
  }

  return id;
}
