#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include <xCore.h> // https://github.com/xinabox/xCore
#include <xSW01.h> // https://github.com/xinabox/xSW01

//static xSW01 SW01;

#if SIMULATED_DATA

void initSensor(void)
{
  // use SIMULATED_DATA, no sensor need to be inited
}

uint16_t readTemperature(void)
{
  return random(20, 100);
}

bool readState(void)
{
  return random(0, 1);
}

#else

void initSensor(void)
{
  Wire.begin();
  SW01.begin();
}

float readTemperature(void)
{
  float var = SW01.getTemperature_C();
  return var;
}

#endif

bool readMessage(int messageId, char *payload)
{
  bool tempAlert;
  float tempC = readTemperature();
  StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  root["deviceId"] = DEVICE_ID;
  root["messageId"] = messageId;

  // NAN is not the valid json, change it to NULL
  if (std::isnan(tempC))
  {
    root["Temperature(C)"] = NULL;
  }
  else
  {
    root["Temperature(C)"] = tempC;
    if (tempC > TEMP_ALERT)
    {
      tempAlert = true;
    }
  }

  root.printTo(payload, MESSAGE_MAX_LEN);
  return tempAlert;
}

void parseTwinMessage(char *message)
{
  StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(message);
  if (!root.success())
  {
    Serial.printf("Parse %s failed.\r\n", message);
    return;
  }

  if (root["desired"]["interval"].success())
  {
    interval = root["desired"]["interval"];
  }
  else if (root.containsKey("interval"))
  {
    interval = root["interval"];
  }
}
