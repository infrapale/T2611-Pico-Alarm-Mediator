#ifndef __IO_H__ 
#define __IO_H__
#include "Arduino.h"
#include "main.h"
//#define BOARD_PICO_TFT_4KEYS

#define PIN_TX0         (0u)
#define PIN_RX0         (1u)
#define PIN_I2C1_SDA    (2u)
#define PIN_I2C1_SCL    (3u)
#define PIN_I2C0_SDA    (4u)
#define PIN_I2C0_SCL    (5u)
// LED
#define PIN_LED_YELLOW  (6u)
#define PIN_LED_BLUE    (7u)

#define PIN_TX1         (8u)
#define PIN_RX1         (9u)

// I2S Audio Out
#define PIN_I2S_BCLK    (10u)
#define PIN_I2S_LRCLK   (11u)
#define PIN_I2S_DOUT    (12u)

#define PIN_SW_1        (13u)
#define PIN_SW_2        (14u)
#define PIN_SW_3        (15u)

#define PIN_SW_WD_EN    PIN_SW_1


#define PIN_RFM_MISO    (16u)
#define PIN_RFM_CS      (17u)
#define PIN_RFM_SCK     (18u)
#define PIN_RFM_MOSI    (19u)
#define PIN_RFM_RESET   (20u)
#define PIN_RFM_IRQ     (21u)

#define PIN_PIR         (22u)
#define PIN_LDR_AN      (A0)

typedef enum
{
    LED_INDX_YELLOW = 0,
    LED_INDX_BLUE,
    LED_INDX_NBR_OF
} led_index_et;


typedef enum
{
  BLINK_OFF = 0,
  BLINK_ON,
  BLINK_1_FLASH,
  BLINK_2_FLASH,
  BLINK_4_FLASH,
  BLINK_SLOW,
  BLINK_NORMAL,
  BLINK_FAST,
  BLINK_SOS,
  BLINK_JITTER_1,
  BLINK_JITTER_2,
  BLINK_JITTER_3,
  BLINK_NBR_OF
} blink_et;

// MISO 16 MOSI 19 SCLK 18 CS 13 DC 15 RST 9 BL 13 T_CS 21
//#define PIN_TFT_CS            (17u)


void io_initialize(void);

void io_rfm69_spi0_initialize(void);

void io_task_initialize(void);

bool io_wd_is_enabled(void);

uint16_t io_read_ldr(void);

uint16_t io_read_akbd(void);

void io_led_flash(led_index_et lindx, blink_et bindx, uint16_t tick_nbr);

void io_task(void);



#endif
