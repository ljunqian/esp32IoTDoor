#include <AWS_IOT.h>
#include <WiFi.h>

#include <ESP32_Servo.h>

#include <ArduinoJson.h>

AWS_IOT esp32;

Servo myservo;

char WIFI_SSID[] = "SP_EEE";
char WIFI_PASSWORD[] = "matlabt903";
char HOST_ADDRESS[] = "a1je8kwt0ntrsy.iot.ap-southeast-1.amazonaws.com";
char CLIENT_ID[] = "IoTDoor1";
char TOPIC_PUBLISH[] = "$aws/things/IoTDoor/shadow/update";
char TOPIC_SUBSCRIBE[] = "$aws/things/IoTDoor/shadow/get";

int status = WL_IDLE_STATUS;
int msgReceived = 0;
int servoPin = 18;
char payload[512];
char rcvdPayload[512];

const size_t bufferSize = 3 * JSON_OBJECT_SIZE(1) + 30;

StaticJsonDocument<bufferSize> doc;

void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.print("Attempting to connect to SSID: ");

  while (status != WL_CONNECTED)
  {
    Serial.print(".");
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // wait 5 seconds for connection:
    delay(5000);
  }
  Serial.println("Connected to wifi");

  if (esp32.connect(HOST_ADDRESS, CLIENT_ID) == 0)
  {
    Serial.println("Connected to AWS");
    delay(1000);
    if (0 == esp32.subscribe(TOPIC_SUBSCRIBE, mySubCallBackHandler))
    {
      Serial.println("Subscribe Successfull");
    }
    else
    {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while (1);
    }
  }
  else
  {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1);
  }
  delay(1000);
  sprintf(payload, "{\"state\":{\"reported\":{\"connected\":\"true\",\"status\":\"close\"}}}");
  if (esp32.publish(TOPIC_PUBLISH, payload) == 0)
  {
    Serial.print("Publish Message:");
    Serial.println(payload);
  }
  else
  {
    Serial.println("Publish failed");
  }
  delay(2000);
  myservo.attach(servoPin, 500, 2400);
  myservo.write(40);
}

void loop() {
  if (msgReceived == 1)
  {
    msgReceived = 0;
    Serial.print("Received Message:");
    Serial.print(rcvdPayload);
    DeserializationError error = deserializeJson(doc, rcvdPayload);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    JsonObject& root = doc.as<JsonObject>();
    if (strcmp(root["state"]["desired"]["status"], "open") == 0) {
      myservo.write(180);
      delay(1000);
      sprintf(payload, "{\"state\":{\"reported\":{\"status\":\"open\"}}}");
      if (esp32.publish(TOPIC_PUBLISH, payload) == 0)
      {
        Serial.print("Publish Message:");
        Serial.println(payload);
      }
      else
      {
        Serial.println("Publish failed");
      }
      delay(2000);
      myservo.write(40);
    }
  }
}
/*
  if (msgReceived == 1)
  {
  msgReceived = 0;
  Serial.print("Received Message:");
  DeserializationError error = deserializeJson(doc, rcvdPayload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  JsonObject& root = doc.as<JsonObject>();
  const char* sensor = root["state"]["desired"]["status"];
  Serial.println(sensor);
  if (strcmp(root["state"]["desired"]["status"], "open") == 0) {
    myservo.write(180);
    sprintf(payload, "{\"state\":{\"desired\":{\"status\":\"close\"}}}");
    if (esp32.publish(TOPIC_NAME, payload) == 0)
    {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else
    {
      Serial.println("Publish failed");
    }
    delay(1000);
    myservo.write(20);
  }
  }
*/
//}
