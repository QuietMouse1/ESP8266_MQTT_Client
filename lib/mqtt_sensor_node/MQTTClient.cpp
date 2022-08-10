#include "MQTTClient.h"

MQTTClient::MQTTClient(WiFiClient& _espClient, PubSubClient& _pubSubClient)
    :espClient(_espClient), pubSubClient(_pubSubClient)
    {

    }

void MQTTClient::initWifi(const char* ssid, const char* password)
{
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.println("Connecting to WiFi..");
	}
}

void MQTTClient::initMQTTClient(const char* mqttUser, const char* mqttPassword, const char* mqttServer, const int mqttPort)
{
    pubSubClient.setServer(mqttServer, mqttPort);
	while (!pubSubClient.connected())
	{
		Serial.println("Connecting to MQTT...");
		if (pubSubClient.connect("ESP8266Client", mqttUser, mqttPassword))
		{

			Serial.println("connected");
		}
		else
		{
			Serial.print("failed with state ");
			Serial.print(pubSubClient.state());
			delay(2000);
		}
	}
}

void MQTTClient::initSensor()
{
    #ifdef SENSOR_SCD30
    iniitSCD30();
    #endif
    #ifdef SENSOR_BME680
    initBME680();
    #endif
    #ifdef SENSOR_SCD41
    initSCD41();
    #endif

}

bool MQTTClient::publish(const char * topic, const char * data)
{
    return pubSubClient.publish(topic, data);
}

void MQTTClient::reconnect()
{
	while (!pubSubClient.connected())
	{
		Serial.print("Attempting MQTT connection...");
		if (pubSubClient.connect("ESP8266Client"))
		{
			Serial.println("connected");
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.print(pubSubClient.state());
			Serial.println(" try again in 5 seconds");
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}

bool MQTTClient::checkConnection()
{
    return pubSubClient.connected();
}

void MQTTClient::pubSubClientLoop()
{
	pubSubClient.loop();
}

bool MQTTClient::readData()
{
    #ifdef SENSOR_SCD30
    readSCD30data();
    #endif
    #ifdef SENSOR_BME680
    readBME680data();
    #endif
    #ifdef SENSOR_SCD41
    readSCD41data();
    #endif

    return true;
}

#ifdef SENSOR_SCD30
void MQTTClient::iniitSCD30()
{
    uint16_t settingVal; // The settings will be returned in settingVal

	// Init The sensor
    //Start sensor using the Wire port, but disable the auto-calibration
    if (airSensor.begin(Wire, false) == false)
    {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        while (1)
            ;
    }
    
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
}

bool MQTTClient::readSCD30data()
{
    bool status = false;
    if (airSensor.dataAvailable())
    {
        uint16_t co2Reading = airSensor.getCO2(); 
        float airTemperatureReading = airSensor.getTemperature();
        float humidityReading = airSensor.getHumidity();

        char cstr[16];
        sprintf(cstr, "%i", co2Reading);
        pubSubClient.publish("esp/co2", cstr);
        sprintf(cstr, "%f", airTemperatureReading);
        pubSubClient.publish("esp/temperature", cstr);
        sprintf(cstr, "%f", humidityReading);
        pubSubClient.publish("esp/humidity", cstr);

        Serial.print("co2(ppm):");
        Serial.print(co2Reading);

        Serial.print(" temp(C):");
        Serial.print(airTemperatureReading);

        Serial.print(" humidity(%):");
        Serial.print(humidityReading);
        Serial.println();

        return true;
    }
    else
        return false;
}

#endif

#ifdef SENSOR_BME680
void MQTTClient::initBME680(){
  while (!BME680.begin(I2C_STANDARD_MODE)) {  // Start using I2C, use first device found
    Serial.print("-  Unable to find BME680. Trying again in 5 seconds.\n");
    delay(2000);
  }  // of loop until device is located
  Serial.print("- Setting 16x oversampling for all sensors\n");
  Serial.print("- Setting IIR filter to a value of 4 samples\n");
  Serial.print("- Turning off gas measurements\n");
  BME680.setOversampling(TemperatureSensor, Oversample16);
  BME680.setOversampling(HumiditySensor, Oversample16);
  BME680.setOversampling(PressureSensor, Oversample16);
  BME680.setIIRFilter(IIR4);
  BME680.setGas(0, 0);  // Setting either to 0 turns off gas

}

bool MQTTClient::readBME680data()
{

  int32_t temp;
  int32_t humidity;
  int32_t gas;
  int32_t pressure;
  BME680.getSensorData(temp, humidity, pressure, gas);  // Get readings

  char cstr[16];
  sprintf(cstr, "%4d.%02d", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
  // pubSubClient.publish("bme/gas_resistance", cstr); //milliomhsm

  // the int32_t temperature value 3074 which needs to be formatted to 30.74 degrees 
  sprintf(cstr, "%2d.%02d", (uint8_t)(temp/100), (uint8_t)(temp % 100));
  pubSubClient.publish("bme/temperature", cstr);
  Serial.print("Temperature =");
  Serial.println(cstr);

  // the int32_t humidity value 74371 which needs to be formatted to 74.371% 
  sprintf(cstr, "%2d.%03d", (int8_t)(humidity / 1000),
          (uint16_t)(humidity % 1000));  // Humidity milli-pct
  pubSubClient.publish("bme/humidity", cstr);
  Serial.print("Humidity =");
  Serial.println(cstr);

  return true;
}
#endif


#ifdef SENSOR_SCD41

void MQTTClient::printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void MQTTClient::printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

void MQTTClient::initSCD41()
{
    uint16_t error;
    char errorMessage[256];

    scd4x.begin(Wire);

    // stop potentially previously started measurement
    error = scd4x.stopPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x.getSerialNumber(serial0, serial1, serial2);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        printSerialNumber(serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x.startPeriodicMeasurement();
    if (error) {
        Serial.print("Error trying to execute startPeriodicMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }

    Serial.println("Waiting for first measurement... (5 sec)");
}

bool MQTTClient::readSCD41data()
{
    // Read Measurement
    bool status = false;
    uint16_t error;
    char errorMessage[256];

    uint16_t co2;
    float temperature;
    float humidity;
    error = scd4x.readMeasurement(co2, temperature, humidity);
    if (error) {
        Serial.print("Error trying to execute readMeasurement(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
        status = false;
    } else if (co2 == 0) {
        Serial.println("Invalid sample detected, skipping.");
        status = false;
    } else {

        char cstr[16];
        sprintf(cstr, "%i", co2);
        pubSubClient.publish("scd41/co2", cstr);
        sprintf(cstr, "%f", temperature);
        pubSubClient.publish("scd41/temperature", cstr);
        sprintf(cstr, "%f", humidity);
        pubSubClient.publish("scd41/humidity", cstr);
        Serial.print("Co2:");
        Serial.print(co2);
        Serial.print("\t");
        Serial.print("Temperature:");
        Serial.print(temperature);
        Serial.print("\t");
        Serial.print("Humidity:");
        Serial.println(humidity);
        status = true;
    }
    return status;
}
#endif
