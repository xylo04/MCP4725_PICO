/*!
  @file     main.cpp
  @author   Chris Keller
  @brief Code Practice Oscillator.
  @note
  When the Morse Code key is pressed, output a sine wave from the MCP4725 to be
  amplified into a speaker.
*/

#include "mcp4725/mcp4725.hpp"
#include "mcp4725/mcp4725_Sinewave_Data.hpp" // Sine wave data structure's
#include "pico/stdlib.h"

#define DAC_I2C_ADDRESS myDAC.MCP4725A0_Addr_A00 // MCP4725 I2C address
#define DAC_REF_VOLTAGE 3.3   // Volts DAC supply-reference voltage
#define MORSE_KEY_GPIO_PIN 20 // GPIO pin for the Morse Code key
#define LED_GPIO_PIN 25       // GPIO pin for the LED
#define I2C_BLOCK i2c0        // I2C block
#define I2C_SDA_PIN 16        // I2C GPIO for data line
#define I2C_SCL_PIN 17        // I2C GPIO for Clock line
#define I2C_SPEED 1000        // I2C speed in Khz
#define I2C_TIMEOUT 50000     // I2C timeout delay in uS.

uint32_t sineWaveFrequency = 600;                // Frequency in Hz
uint32_t periodUs = 1000000 / sineWaveFrequency; // Period in microseconds

MCP4725_PICO myDAC(DAC_REF_VOLTAGE);

int main() {

  if (!myDAC.begin(DAC_I2C_ADDRESS, I2C_BLOCK, I2C_SPEED, I2C_SDA_PIN,
                   I2C_SCL_PIN, I2C_TIMEOUT)) {
    while (1) {
    };
  }

  // Initialize the morse code key
  gpio_init(MORSE_KEY_GPIO_PIN);
  gpio_set_dir(MORSE_KEY_GPIO_PIN, GPIO_IN);
  gpio_pull_up(MORSE_KEY_GPIO_PIN);

  // Initialize the LED
  gpio_init(LED_GPIO_PIN);
  gpio_set_dir(LED_GPIO_PIN, GPIO_OUT);
  gpio_put(LED_GPIO_PIN, 1);

  printf("MCP4725_PICO : Init success\r\n");
  while (1) {
    // Check if the morse code key is closed
    if (gpio_get(MORSE_KEY_GPIO_PIN) == 0) {
      // Generate a sine wave
      uint64_t currentTime = time_us_64();
      uint32_t elapsedTime = currentTime % periodUs;
      uint16_t index = (elapsedTime * SINEWAVE_RES) / periodUs;
      myDAC.setInputCode(pDacLookupSineWave[index], myDAC.MCP4725_FastMode,
                         myDAC.MCP4725_PowerDown_Off);
    } else {
      // No tone
      // TODO: Edge shaping
      myDAC.setInputCode(2048, myDAC.MCP4725_FastMode,
                         myDAC.MCP4725_PowerDown_Off);
    }
  }
  return 0;
}
