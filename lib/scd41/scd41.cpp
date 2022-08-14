#include "scd41.h"
void SCD41Wrapper::printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void SCD41Wrapper::printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}

void SCD41Wrapper::initSCD41()
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

bool SCD41Wrapper::readSCD41data()
{
    // Read Measurement
    bool status = false;
    uint16_t error;
    char errorMessage[256];

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
        // char cstr[16];
        // sprintf(cstr, "%i", co2);
        // sprintf(cstr, "%.2f", temperature);
        // sprintf(cstr, "%.2f", humidity);
        // Serial.print("Co2:");
        // Serial.print(co2);
        // Serial.print("\t");
        // Serial.print("Temperature:");
        // Serial.print(temperature);
        // Serial.print("\t");
        // Serial.print("Humidity:");
        // Serial.println(humidity);
        status = true;
    }
    return status;
}
