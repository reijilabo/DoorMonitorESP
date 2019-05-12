#include <Arduino.h>
#include <ESP8266WiFi.h>

#ifndef DataToMaker_h
class DataToMaker
{
  public:
    DataToMaker(const char* key); // constructor
    bool connect();
    bool setValue(int num, String val);
    bool setValue(int num, int val);
    bool setValue(int num, float val);

    bool post(String ev);

  protected: // it is protected because the subclass needs access
    //to max distance!

  private:
    void compileData();
    WiFiClient client;
    const char* privateKey;
    String event;
    String value1, value2, value3 = "";
    bool dataAvailable;
    String postData;
};

DataToMaker::DataToMaker(const char* _privateKey)
{
  privateKey = _privateKey;
}

bool DataToMaker::connect()
{
  if (client.connect("maker.ifttt.com", 80))
    return true;
  else return false;
}

bool DataToMaker::post(String _event)
{
  event = _event;
  compileData();
  client.print("POST /trigger/");
  client.print(event);
  client.print("/with/key/");
  client.print(privateKey);
  client.println(" HTTP/1.1");

  client.println("Host: maker.ifttt.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  if (dataAvailable)
  { // append json values if available
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  }
  else
    client.println();

  while (client.available()) {
      char inChar = client.read();
      Serial.write(inChar);
  }
  Serial.println();

  client.stop();
  return true;
}

bool DataToMaker::setValue(int valueToSet, String value)
{
  switch (valueToSet)
  {
    case 1:
      value1 = value;
      break;
    case 2:
      value2 = value;
      break;
    case 3:
      value3 = value;
      break;
    default:
      return false;
      break;
  }
  return true;
}

bool DataToMaker::setValue(int valueToSet, int value)
{
  switch (valueToSet)
  {
    case 1:
      value1 = String(value);
      break;
    case 2:
      value2 = String(value);
      break;
    case 3:
      value3 = String(value);
      break;
    default:
      return false;
      break;
  }
  return true;
}

bool DataToMaker::setValue(int valueToSet, float value)
{
  switch (valueToSet)
  {
    case 1:
      value1 = String(value);
      break;
    case 2:
      value2 = String(value);
      break;
    case 3:
      value3 = String(value);
      break;
    default:
      return false;
      break;
  }
  return true;
}

void DataToMaker::compileData()
{
  if (value1 != "" || value2 != "" || value3 != "")
  {
    dataAvailable = true;
    bool valueEntered = false;
    postData = "{";
    if (value1 != "")
    {
      postData.concat("\"value1\":\"");
      postData.concat(value1);
      valueEntered = true;
    }
    if (value2 != "")
    {
      if (valueEntered)postData.concat("\",");
      postData.concat("\"value2\":\"");
      postData.concat(value2);
      valueEntered = true;
    }
    if (value3 != "")
    {
      if (valueEntered)postData.concat("\",");
      postData.concat("\"value3\":\"");
      postData.concat(value3);
    }
    postData.concat("\"}");
  }
  else dataAvailable = false;
}
#endif
