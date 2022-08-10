#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30

// To do
// Get the adafruit library. Compare between sensirion, Seed studio and Adafruit
// get mqtt up and running
// debug lights - Connecting problems, fail to connect to wifi or fail to connecto to mqtt server.
// low power mode

String dataOut = "";

// Change the credentials below, so your ESP8266 connects to your router
const char *ssid = "SINGTEL-61B8";
const char *password = "0001663416";

const char *mqttServer = "192.168.1.87";
const int mqttPort = 1884;
const char *mqttUser = "user";
const char *mqttPassword = "user";
unsigned long timeStart = 0;
unsigned long timeNow = 0;

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running
WiFiClient espClient;
PubSubClient client(mqttServer, 1883, espClient);

SCD30 airSensor;

// This functions reconnects your ESP8266 to your MQTT broker
void reconnect()
{
	while (!client.connected())
	{
		Serial.print("Attempting MQTT connection...");
		if (client.connect("ESP8266Client"))
		{
			Serial.println("connected");
			// Subscribe or resubscribe to a topic
			// You can subscribe to more topics (to control more LEDs in this example)
			client.subscribe("test/test1");
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

/* This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
 Change the function below to add logic to your program, so when a device publishes a message to a topic that
 your ESP8266 is subscribed you can actually do something
*/

void callback(String topic, byte *message, unsigned int length)
{
	Serial.println("Message arrived on topic: ");
	Serial.println(topic);
	String command;

	for (int i = 0; i < length; i++)
	{
		command += (char)message[i];
	}
	if (topic == "esp/test")
	{
		Serial.print("Topic == esp/test recieved ");
		Serial.println(command);
		if (command == "Sweep")
		{
			Serial.println("Sweep");
		}
		if (command == "Calibrate")
		{
			Serial.println("Calibrate");
		}
	}
}

void setup()
{
	// Init
	Serial.begin(9600);
	WiFi.begin(ssid, password);
	Wire.begin();

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.println("Connecting to WiFi..");
	}
	Serial.println("Connected to the WiFi network");

	client.setServer(mqttServer, mqttPort);
	client.setCallback(callback);

	while (!client.connected())
	{
		Serial.println("Connecting to MQTT...");

		if (client.connect("ESP8266Client", mqttUser, mqttPassword))
		{

			Serial.println("connected");
		}
		else
		{
			Serial.print("failed with state ");
			Serial.print(client.state());
			delay(2000);
		}
	}

	client.publish("esp/test", "Hello from ESP8266");
	client.subscribe("esp/test");

	Serial.println("Wifi + MQTT set up completed");
	delay(100);

	// Init The sensor
    //Start sensor using the Wire port, but disable the auto-calibration
    if (airSensor.begin(Wire, false) == false)
    {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        while (1)
            ;
    }

    uint16_t settingVal; // The settings will be returned in settingVal
    
    if (airSensor.getForcedRecalibration(&settingVal) == true) // Get the setting
    {
      Serial.print("Forced recalibration factor (ppm) is ");
      Serial.println(settingVal);
    }
    else
    {
      Serial.print("getForcedRecalibration failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    if (airSensor.getMeasurementInterval(&settingVal) == true) // Get the setting
    {
      Serial.print("Measurement interval (s) is ");
      Serial.println(settingVal);
    }
    else
    {
      Serial.print("getMeasurementInterval failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    if (airSensor.getTemperatureOffset(&settingVal) == true) // Get the setting
    {
      Serial.print("Temperature offset (C) is ");
      Serial.println(((float)settingVal) / 100.0, 2);
    }
    else
    {
      Serial.print("getTemperatureOffset failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    if (airSensor.getAltitudeCompensation(&settingVal) == true) // Get the setting
    {
      Serial.print("Altitude offset (m) is ");
      Serial.println(settingVal);
    }
    else
    {
      Serial.print("getAltitudeCompensation failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    if (airSensor.getFirmwareVersion(&settingVal) == true) // Get the setting
    {
      Serial.print("Firmware version is 0x");
      Serial.println(settingVal, HEX);
    }
    else
    {
      Serial.print("getFirmwareVersion! Freezing...");
      while (1)
        ; // Do nothing more
    }

    Serial.print("Auto calibration set to ");
    if (airSensor.getAutoSelfCalibration() == true)
        Serial.println("true");
    else
        Serial.println("false");

	// Update Time now 
	timeStart = millis();
}

void loop()
{
	// every 2 second collect data
	timeNow = millis();
	if (timeNow - timeStart > 2000) 
	{
		// Only need to get data once
		if (airSensor.dataAvailable())
		{
			uint16_t co2Reading = airSensor.getCO2(); 
			float airTemperatureReading = airSensor.getTemperature();
			float humidityReading = airSensor.getHumidity();

			Serial.print("co2(ppm):");
			uint16_t co2reading = airSensor.getCO2(); 
			Serial.print(co2reading);

			Serial.print(" temp(C):");
			Serial.print(airTemperatureReading);

			Serial.print(" humidity(%):");
			Serial.print(humidityReading);
			Serial.println();
			char cstr[16];
			sprintf(cstr, "%i", co2Reading);
			client.publish("esp/co2", cstr);
			sprintf(cstr, "%f", airTemperatureReading);
			client.publish("esp/temperature", cstr);
			sprintf(cstr, "%f", humidityReading);
			client.publish("esp/humidity", cstr);

		}
		else
			Serial.println("Waiting for new data");
		timeStart = millis();
	}

	if (!client.connected())
	{
		reconnect();
	}
	client.loop();
}
