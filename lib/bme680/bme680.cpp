#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "bme680.h"

void BME680Wrapper::initBME680(){
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

bool BME680Wrapper::readBME680data()
{

  int32_t temp;
  int32_t humidity;
  int32_t gas;
  int32_t pressure;
  BME680.getSensorData(temp, humidity, pressure, gas);  // Get readings

//   char cstr[16];
//   sprintf(cstr, "%4d.%02d", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
//   // pubSubClient.publish("bme/gas_resistance", cstr); //milliomhsm

//   // the int32_t temperature value 3074 which needs to be formatted to 30.74 degrees 
//   sprintf(cstr, "%2d.%02d", (uint8_t)(temp/100), (uint8_t)(temp % 100));
//   pubSubClient.publish("bme/temperature", cstr);
//   Serial.print("Temperature =");
//   Serial.println(cstr);

//   // the int32_t humidity value 74371 which needs to be formatted to 74.371% 
//   sprintf(cstr, "%2d.%03d", (int8_t)(humidity / 1000),
//           (uint16_t)(humidity % 1000));  // Humidity milli-pct
//   pubSubClient.publish("bme/humidity", cstr);
//   Serial.print("Humidity =");
//   Serial.println(cstr);

  return true;
}
