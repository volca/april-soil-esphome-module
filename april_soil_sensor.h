#include "esphome.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include <esp_adc_cal.h>
#include <Wire.h>

// I2C address for temperature sensor
const int TMP_ADDR  = 0x48;

const adc_channel_t      MOISTURE_CHANNEL    = ADC_CHANNEL_8;     // GPIO9
const adc_bits_width_t   WIDTH               = ADC_WIDTH_BIT_13;
const adc_atten_t        MOISTURE_ATTEN      = ADC_ATTEN_DB_2_5;

#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_DUTY_RES       LEDC_TIMER_3_BIT

#define LEDC_LS_CH0_GPIO       (17)
#define LEDC_LS_CH0_CHANNEL    LEDC_CHANNEL_0

// %50 duty: 2 ^ (LEDC_LS_DUTY_RES - 1) 
#define LEDC_TEST_DUTY          (4)

class AprilSoilSensor : public PollingComponent, public Sensor {

    private:

    float readTemp_() {
        float temp;

        Wire.beginTransmission(TMP_ADDR);
        // Select Data Registers
        Wire.write(0X00);
        Wire.endTransmission();

        // Request 2 bytes , Msb first
        Wire.requestFrom(TMP_ADDR, 2);

        // Read temperature as Celsius (the default)
        while(Wire.available()) {  
            int msb = Wire.read();
            int lsb = Wire.read();

            int rawtmp = msb << 8 |lsb;
            int value = rawtmp >> 4;
            temp = value * 0.0625;

            return temp;
        }

        return 0;
    }

    public:

    Sensor *temperature_sensor = new Sensor();
    Sensor *soil_sensor = new Sensor();

    AprilSoilSensor() : PollingComponent(3000) {}

    void setup() override {
        ledc_timer_config_t ledc_timer = {
            .speed_mode = LEDC_LS_MODE,           // timer mode
            .duty_resolution = LEDC_TIMER_3_BIT, // resolution of PWM duty
            .timer_num = LEDC_LS_TIMER,            // timer index
            .freq_hz = 8000000,                      // frequency of PWM signal
            .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
        };
        ledc_timer_config(&ledc_timer);

        ledc_channel_config_t ledc_channel = {
            .gpio_num   = LEDC_LS_CH0_GPIO,
            .speed_mode = LEDC_LS_MODE,
            .channel    = LEDC_LS_CH0_CHANNEL,
            .timer_sel  = LEDC_LS_TIMER,
            .duty       = LEDC_TEST_DUTY,
            .hpoint     = 0,
        };
        ledc_channel_config(&ledc_channel);

        //Configure ADC
        adc1_config_width(WIDTH);
        adc1_config_channel_atten((adc1_channel_t)MOISTURE_CHANNEL, MOISTURE_ATTEN);

        const int sdaPin = 8;
        const int sclPin = 10;
        Wire.begin(sdaPin, sclPin);

        delay(2000);
    }

    void update() override {
        float temp = readTemp_();
        temperature_sensor->publish_state(temp);
        uint32_t adc_reading = adc1_get_raw((adc1_channel_t)MOISTURE_CHANNEL);
        soil_sensor->publish_state(adc_reading);
    }

};
