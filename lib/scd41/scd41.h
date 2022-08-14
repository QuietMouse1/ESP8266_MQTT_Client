#pragma once
#include <stdint.h>
#include <SensirionI2CScd4x.h> // << SCD41 sensor libary

class SCD41Wrapper{
    private:
        SensirionI2CScd4x scd4x; // I2C
        uint16_t co2;
        float temperature;
        float humidity;
    public:
        void printUint16Hex(uint16_t value);
        void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2);
        void initSCD41();
        bool readSCD41data();
        uint16_t getCO2Data(){return co2;};
        float getTemperaturData(){return temperature;};
        float getHumidityData(){return humidity;};
        
        SCD41Wrapper ();
};