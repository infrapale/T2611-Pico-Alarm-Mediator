#include <SPI.h>
#include "main.h"
#include "io.h"
#include "atask.h"
#include "super.h"

typedef struct
{
  uint8_t pin;
  uint32_t pattern;
  uint16_t tick_nbr;
} led_st;

typedef struct
{
  uint8_t pattern_bit;
  uint8_t switches;
  uint8_t tindx;
} io_ctrl_st;

io_ctrl_st io_ctrl;
led_st led[LED_INDX_NBR_OF] = 
{
    [LED_INDX_YELLOW]   = {PIN_LED_YELLOW, 0, 0},
    [LED_INDX_BLUE]     = {PIN_LED_BLUE, 0, 0},
};

const uint32_t led_pattern[BLINK_NBR_OF] = {
  [BLINK_OFF]       = 0b00000000000000000000000000000000,
  [BLINK_ON]        = 0b11111111111111111111111111111111,
  [BLINK_1_FLASH]   = 0b00000000000000000000000000000001,
  [BLINK_2_FLASH]   = 0b10000000000000001000000000000000,
  [BLINK_4_FLASH]   = 0b10000001000000001000000010000000,
  [BLINK_SLOW]      = 0b11111111111111110000000000000000,
  [BLINK_NORMAL]    = 0b11110000111100001111000011110000,
  [BLINK_FAST]      = 0b11001100110011001100110011001100,
  [BLINK_SOS]       = 0b10010010011100111001110010010010,
  [BLINK_JITTER_1]  = 0b10101010101010101010101010101010,
  [BLINK_JITTER_2]  = 0b10010010010010010010010010010010,
  [BLINK_JITTER_3]  = 0b10001000100010001000100010001000,
};

void io_task(void);
//                        123456789012345   ival  next  state  prev  cntr flag  call backup
atask_st io_th      =   {"I/O Task       ", 100,     0,     0,  255,    0,  1,  io_task };

void io_rfm69_spi0_initialize(void) {
  SPI.setRX(PIN_RFM_MISO);
  SPI.setTX(PIN_RFM_MOSI);
  SPI.setSCK(PIN_RFM_SCK);
  SPI.setCS(PIN_RFM_CS);
  pinMode(PIN_RFM_RESET, OUTPUT);
  digitalWrite(PIN_RFM_RESET, LOW);
  SPI.begin();

  SPI.beginTransaction(SPISettings(
    1000000,  // 8 MHz
    MSBFIRST,
    SPI_MODE0));
}



void io_initialize(void)
{
    Serial.println("io_initialize()");
    pinMode(PIN_PIR, INPUT);
    pinMode(PIN_LDR_AN, INPUT);
    pinMode(PIN_SW_1, INPUT_PULLUP);
    pinMode(PIN_SW_2, INPUT_PULLUP);
    pinMode(PIN_SW_3, INPUT_PULLUP);
    analogReadResolution(12);
    io_ctrl.pattern_bit = 0;
    for (uint8_t lindx = 0; lindx < LED_INDX_NBR_OF; lindx++ )
    {
       pinMode(led[lindx].pin, OUTPUT);
    }   
    Serial.println("---!");
} 

void io_task_initialize(void)
{
    Serial.println("io_task_initialize()");
    io_ctrl.tindx =  atask_add_new(&io_th);
    Serial.println("---!");
}

uint16_t io_read_ldr(void)
{
  return analogRead(PIN_LDR_AN);
}

bool io_wd_is_enabled(void)
{
    return (digitalRead(PIN_SW_WD_EN) == HIGH);
}

void io_led_flash(led_index_et lindx, blink_et bindx, uint16_t tick_nbr)
{
  led[lindx].pattern = led_pattern[bindx];
  led[lindx].tick_nbr = tick_nbr;
}

void io_task(void)
{

    uint32_t patt = 1UL << io_ctrl.pattern_bit;
    for (uint8_t i = 0; i < LED_INDX_NBR_OF; i++)
    {
        if (led[i].tick_nbr > 0) {
            if ((led[i].pattern & patt) != 0)
                digitalWrite(led[i].pin, HIGH);
            else  
                digitalWrite(led[i].pin, LOW);
            led[i].tick_nbr--;  
        }
    } 
    if (++io_ctrl.pattern_bit >= 16) io_ctrl.pattern_bit = 0;
    super_clear_cntr(SUPER_CNTR_IO);
}