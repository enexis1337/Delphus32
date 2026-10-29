/**
 * @file board_esp32s3_st7567.h
 * Board definition: ESP32-S3 with ST7567 128x64 Mono LCD
 *
 * MCU:      ESP32-S3
 * Display:  ST7567 128x64 Mono LCD via SPI
 * Input:    6 GPIO buttons (OK, Back, Left, Up, Down, Right)
 * SubGHz:   CC1101 via SPI (shared bus with LCD + SD)
 * SD Card:  SPI (shared bus with LCD + SubGHz)
 * iButton:  1-Wire (IO2)
 * Vibro:    GPIO 7
 * IR:       TX (IO17) + RX (IO16)
 * I2C:      SCL (IO18) + SDA (IO8)
 */

#pragma once

/* ---- Board metadata ---- */
#define BOARD_NAME        "ESP32-S3 ST7567"
#define BOARD_TARGET      "esp32s3"

/* ---- Hardware Button Pins ---- */
#define BOARD_PIN_BUTTON_BOOT   0   /* OK button / BOOT button */
#define BOARD_PIN_BUTTON_KEY    46  /* Back button */
#define BOARD_PIN_BATTERY_ADC   UINT16_MAX

/* ---- Direct 6 GPIO Buttons ---- */
#define BOARD_PIN_BTN_OK        0
#define BOARD_PIN_BTN_BACK      46
#define BOARD_PIN_BTN_LEFT      42
#define BOARD_PIN_BTN_UP        41
#define BOARD_PIN_BTN_DOWN      40
#define BOARD_PIN_BTN_RIGHT     39

/* ---- LCD Pins (ST7567 128x64 Mono LCD via SPI) ---- */
#define BOARD_LCD_IS_ST7567     1
#define BOARD_PIN_LCD_MOSI      9
#define BOARD_PIN_LCD_SCLK      11
#define BOARD_PIN_LCD_DC        6   /* RES / DC (Data/Command) */
#define BOARD_PIN_LCD_CS        4
#define BOARD_PIN_LCD_RST       5
#define BOARD_PIN_LCD_BL        45

/* ---- LCD Display Configuration ---- */
#define BOARD_LCD_H_RES         128
#define BOARD_LCD_V_RES         64
#define BOARD_LCD_SPI_HOST      SPI2_HOST
#define BOARD_LCD_SPI_FREQ_HZ   (10 * 1000 * 1000)
#define BOARD_LCD_CMD_BITS      8
#define BOARD_LCD_PARAM_BITS    8
#define BOARD_LCD_SWAP_XY       false
#define BOARD_LCD_MIRROR_X      false
#define BOARD_LCD_MIRROR_Y      false
#define BOARD_LCD_INVERT_COLOR  false
#define BOARD_LCD_GAP_X         0
#define BOARD_LCD_GAP_Y         0
#define BOARD_LCD_BL_ACTIVE_LOW false

#define BOARD_LCD_FG_COLOR      0x0000
#define BOARD_LCD_BG_COLOR      0xFFFF
#define BOARD_LCD_CONTRAST      26  /* Default ST7567 contrast (0..63) */

/* ---- SD Card Pins (shared SPI bus with LCD and CC1101) ---- */
#define BOARD_PIN_SD_CS         13
#define BOARD_PIN_SD_MISO       10

/* ---- Touch Controller — NOT PRESENT ---- */
#define BOARD_PIN_TOUCH_SCL     UINT16_MAX
#define BOARD_PIN_TOUCH_SDA     UINT16_MAX
#define BOARD_PIN_TOUCH_RST     UINT16_MAX
#define BOARD_PIN_TOUCH_INT     UINT16_MAX
#define BOARD_TOUCH_I2C_ADDR    0x00
#define BOARD_TOUCH_I2C_PORT    I2C_NUM_0
#define BOARD_TOUCH_I2C_FREQ_HZ 0
#define BOARD_TOUCH_I2C_TIMEOUT 0

/* ---- SubGHz / CC1101 (shared SPI bus with LCD + SD) ---- */
#define BOARD_PIN_CC1101_SCK    11
#define BOARD_PIN_CC1101_CSN    12
#define BOARD_PIN_CC1101_MISO   10
#define BOARD_PIN_CC1101_MOSI   9
#define BOARD_PIN_CC1101_GDO0   3
#define BOARD_PIN_CC1101_GDO2   UINT16_MAX
#define BOARD_PIN_CC1101_SW1    UINT16_MAX
#define BOARD_PIN_CC1101_SW0    UINT16_MAX
#define BOARD_CC1101_SPI_SHARED 1

/* ---- NRF24L01 — NOT PRESENT (GPIO 43/44 are UART Console) ---- */
#define BOARD_PIN_NRF24_SCK     UINT16_MAX
#define BOARD_PIN_NRF24_MISO    UINT16_MAX
#define BOARD_PIN_NRF24_MOSI    UINT16_MAX
#define BOARD_PIN_NRF24_CSN     UINT16_MAX
#define BOARD_PIN_NRF24_CE      UINT16_MAX
#define BOARD_HAS_NRF24         0

/* ---- Power Enable ---- */
#define BOARD_PIN_PWR_EN        UINT16_MAX

/* ---- IR ---- */
#define BOARD_PIN_IR_TX         17
#define BOARD_PIN_IR_RX         16

/* ---- NFC / PN532 (disabled) ---- */
#define BOARD_PIN_NFC_SCL       18
#define BOARD_PIN_NFC_SDA       8
#define BOARD_PIN_NFC_IRQ       UINT16_MAX
#define BOARD_PIN_NFC_RST       UINT16_MAX
#define BOARD_NFC_I2C_PORT      I2C_NUM_0

/* ---- iButton ---- */
#define BOARD_PIN_IBUTTON       2

/* ---- Speaker / Buzzer ---- */
#define BOARD_PIN_SPEAKER_DOUT  15
#define BOARD_PIN_SPEAKER_BCLK  UINT16_MAX
#define BOARD_PIN_SPEAKER_WCLK  UINT16_MAX

/* ---- WS2812 RGB LED (DISABLED — GPIO 38 used by CC1101_GDO2) ---- */
#define BOARD_PIN_WS2812_DATA   UINT16_MAX
#define BOARD_WS2812_LED_COUNT  0

/* ---- Vibro Motor ---- */
#define BOARD_PIN_VIBRO         7

/* ---- Features ---- */
#define BOARD_HAS_TOUCH         0
#define BOARD_HAS_ENCODER       0
#define BOARD_HAS_SD_CARD       1
#define BOARD_HAS_BLE           1
#define BOARD_HAS_RGB_LED       0
#define BOARD_HAS_VIBRO         1
#define BOARD_HAS_SPEAKER       0
#define BOARD_HAS_IR            1
#define BOARD_HAS_IBUTTON       1
#define BOARD_HAS_RFID          0
#define BOARD_HAS_NFC           0
#define BOARD_HAS_SUBGHZ        1
#define BOARD_HAS_MIC           0

#define BQ27220_ADDR                    0x55
#define BQ25896_CHARGE_LIMIT            1280
#define FURI_HAL_POWER_VIRTUAL_CAPACITY_MAH (1000U)
