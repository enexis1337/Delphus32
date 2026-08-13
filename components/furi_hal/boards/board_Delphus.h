/**
 * @file board_Delphus.h
 * Board definition: Delphus ESP32-S3 with 2.8" TFT + ST7567 monochrome LCD
 *
 * MCU:      ESP32-S3 (dual-core Xtensa LX7)
 * Display:  ST7567 132x64 monochrome SPI (shared with SubGHz/SD)
 * Input:    6 GPIO buttons (OK/Back/Left/Up/Down/Right)
 * SubGHz:   SPI + GDO0 (shared bus with LCD + SD)
 * SD Card:  SPI (shared bus with LCD + SubGHz)
 * IR:       TX (IO17) + RX (IO16)
 * NFC:      ST25R3916 SPI (PN532 disabled)
 * RGB LED:  IO38 (WS2812)
 * Vibro:    IO7
 * Buzzer:   IO15
 * iButton:  IO2
 * Buttons:  OK-IO0, Back-IO46, Left-IO42, Up-IO41, Down-IO40, Right-IO39
 */

#pragma once

/* ---- Board metadata ---- */
#define BOARD_NAME        "Delphus"
#define BOARD_TARGET      "esp32s3"

/* ---- Hardware Button Pins ---- */
#define BOARD_PIN_BTN_OK       0
#define BOARD_PIN_BTN_BACK     46
#define BOARD_PIN_BTN_LEFT     42
#define BOARD_PIN_BTN_UP       41
#define BOARD_PIN_BTN_DOWN     40
#define BOARD_PIN_BTN_RIGHT    39

/* ---- Rotary Encoder (not present, using buttons only) ---- */
#define BOARD_PIN_ENCODER_A    UINT16_MAX
#define BOARD_PIN_ENCODER_B    UINT16_MAX
#define BOARD_PIN_ENCODER_BTN  UINT16_MAX

/* ---- LCD ST7567 Pins (shared SPI bus with SubGHz/SD) ---- */
#define BOARD_PIN_LCD_MOSI     9
#define BOARD_PIN_LCD_SCLK     11
#define BOARD_PIN_LCD_CS       4      /* ST7567 CS — shared SPI, muxed with LCD_CS/CC1101_CSN */
#define BOARD_PIN_LCD_RST      5      /* ST7567 RES */
#define BOARD_PIN_LCD_RES      6      /* ST7567 RST — alternate name for RST */

/* ---- LCD ST7567 Configuration ---- */
#define BOARD_LCD_H_RES        128     /* ST7567 width - Flipper Zero compatible */
#define BOARD_LCD_V_RES        64      /* ST7567 height - Flipper Zero compatible */
#define BOARD_LCD_SPI_HOST     SPI2_HOST
#define BOARD_LCD_SPI_FREQ_HZ  (8 * 1000 * 1000)
#define BOARD_LCD_CMD_BITS     8
#define BOARD_LCD_PARAM_BITS   8
/* ST7567 does not use swap_xy/mirror like ST7789 */
#define BOARD_LCD_SWAP_XY      false
#define BOARD_LCD_MIRROR_X     false
#define BOARD_LCD_MIRROR_Y     false
#define BOARD_LCD_INVERT_COLOR false

/* ST7567 framebuffer → display mapping (1-bit monochrome) */
#define BOARD_LCD_FG_COLOR     0xFDA0  /* Flipper Orange (dithered for 1-bit) */
#define BOARD_LCD_BG_COLOR     0x0000  /* Black */

/* ---- SD Card Pins (shared SPI bus with LCD + SubGHz) ---- */
#define BOARD_PIN_SD_CS        13
#define BOARD_PIN_SD_MISO      10
#define BOARD_PIN_SD_MOSI      9

/* ---- SubGHz / CC1101 (shared SPI bus with LCD + SD) ---- */
#define BOARD_PIN_CC1101_SCK    11      /* Shared with LCD_CLK and SD_SCLK */
#define BOARD_PIN_CC1101_CSN    12      /* LORA_CS — note: LCD_CS also 12, muxed */
#define BOARD_PIN_CC1101_MISO   10      /* Shared with SD_MISO */
#define BOARD_PIN_CC1101_MOSI   9       /* Shared with LCD_MOSI and SD_MOSI */
#define BOARD_PIN_CC1101_GDO0   3       /* LORA_IO0 — CC1101 GDO0 interrupt */
#define BOARD_PIN_CC1101_GDO2   38      /* LORA_IO2 — CC1101 GDO2 */
/* Band selection: not wired on this board */
#define BOARD_CC1101_SPI_SHARED 1       /* CC1101 shares SPI2_HOST with LCD+SD */

/* ---- NRF24L01 (optional, parallel on shared SPI) ---- */
#define BOARD_PIN_NRF24_SCK     BOARD_PIN_CC1101_SCK    /* IO11 -- shared */
#define BOARD_PIN_NRF24_MISO    BOARD_PIN_CC1101_MISO   /* IO10 -- shared */
#define BOARD_PIN_NRF24_MOSI    BOARD_PIN_CC1101_MOSI   /* IO9  -- shared */
#define BOARD_PIN_NRF24_CSN     44                      /* IO44 -- NRF24-only */
#define BOARD_PIN_NRF24_CE      43                      /* IO43 -- NRF24-only */
#define BOARD_HAS_NRF24         0

/* ---- Power Enable ---- */
#define BOARD_PIN_PWR_EN        UINT16_MAX

/* ---- IR ---- */
#define BOARD_PIN_IR_TX         17      /* IR_EN */
#define BOARD_PIN_IR_RX         16      /* IR_RX */

/* ---- NFC / PN532 (via I2C) ---- */
#define BOARD_PIN_NFC_SCL       18
#define BOARD_PIN_NFC_SDA       8
#define BOARD_PIN_NFC_IRQ       UINT16_MAX
#define BOARD_PIN_NFC_RST       UINT16_MAX
#define BOARD_NFC_I2C_PORT      I2C_NUM_0

/* ---- Vibro ---- */
#define BOARD_PIN_VIBRO         7

/* ---- Buzzer ---- */
#define BOARD_PIN_BUZZER        15

/* ---- RGB LED (WS2812) ---- */
#define BOARD_PIN_WS2812_DATA   38
#define BOARD_WS2812_LED_COUNT  1

/* ---- iButton ---- */
#define BOARD_PIN_IBUTTON       2

/* ---- Features ---- */
#define BOARD_HAS_TOUCH         0
#define BOARD_HAS_ENCODER       0
#define BOARD_HAS_ST7567        1
#define BOARD_HAS_SD_CARD       1
#define BOARD_HAS_BLE           1
#define BOARD_HAS_RGB_LED       1
#define BOARD_HAS_VIBRO         1
#define BOARD_HAS_SPEAKER       0
#define BOARD_HAS_IR            1
#define BOARD_HAS_IBUTTON       1
#define BOARD_HAS_RFID          0
#define BOARD_HAS_SUBGHZ        1       /* Built-in CC1101 via shared SPI */
#define BOARD_HAS_ST25R3916     1       /* ST25R3916 SPI NFC */
#define BOARD_HAS_NFC           0       /* PN532 disabled, using ST25R3916 instead */

/* ST25R3916 SPI pins */
#define BOARD_PIN_ST25R_SCK     48      /* SCK */
#define BOARD_PIN_ST25R_MISO    47      /* MISO */
#define BOARD_PIN_ST25R_MOSI    46      /* MOSI */
#define BOARD_PIN_ST25R_CS      47      /* CS — shared with GDO0, muxed */
#define BOARD_PIN_ST25R_IRQ     48      /* IRQ — shared with SCK, muxed */

/* Battery Charger (none) */
#define BQ27220_ADDR            0x00
#define BQ_I2C_PORT     I2C_NUM_0
#define BQ_I2C_SDA      GPIO_NUM_8
#define BQ_I2C_SCL      GPIO_NUM_18
#define FURI_HAL_POWER_VIRTUAL_CAPACITY_MAH     (0U)