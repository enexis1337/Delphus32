#include "furi_hal.h"
#include "boards/board.h"
#include <furi_hal_gpio.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_private/brownout.h>

static const char* TAG = "FuriHal";

void furi_hal_init_early(void) {
    esp_brownout_disable();
    furi_hal_cortex_init_early();

#if defined(BOARD_PIN_PWR_EN) && (BOARD_PIN_PWR_EN != UINT16_MAX)
    /* Power-enable must be set early — powers CC1101, BQ27220 fuel gauge, WS2812 */
    static const GpioPin pwr_en = {.port = NULL, .pin = BOARD_PIN_PWR_EN};
    furi_hal_gpio_init_simple(&pwr_en, GpioModeOutputPushPull);
    furi_hal_gpio_write(&pwr_en, true);
    ESP_LOGI(TAG, "PWR_EN GPIO%d set HIGH", BOARD_PIN_PWR_EN);
#endif

#if defined(BOARD_PIN_NRF24_CSN) && (BOARD_PIN_NRF24_CSN != UINT16_MAX)
    /* T-Embed Plus shares SPI2 between CC1101 and NRF24. Drive NRF24 CSN HIGH
     * (deselected) and CE LOW (standby) at boot, before any CC1101 SPI traffic.
     * Without this, NRF24 sees CC1101 traffic and corrupts the bus, manifesting
     * as a stuck ~312 MHz reading in the Frequency Analyzer and total RX failure. */
    static const GpioPin nrf24_csn = {.port = NULL, .pin = BOARD_PIN_NRF24_CSN};
    furi_hal_gpio_init_simple(&nrf24_csn, GpioModeOutputPushPull);
    furi_hal_gpio_write(&nrf24_csn, true);
    ESP_LOGI(TAG, "NRF24_CSN GPIO%d set HIGH (deselect)", BOARD_PIN_NRF24_CSN);
#endif

#if defined(BOARD_PIN_CC1101_CSN) && (BOARD_PIN_CC1101_CSN != UINT16_MAX)
    static const GpioPin cc1101_csn = {.port = NULL, .pin = BOARD_PIN_CC1101_CSN};
    furi_hal_gpio_init_simple(&cc1101_csn, GpioModeOutputPushPull);
    furi_hal_gpio_write(&cc1101_csn, true);
    ESP_LOGI(TAG, "CC1101_CSN GPIO%d set HIGH (deselect)", BOARD_PIN_CC1101_CSN);
#endif

#if defined(BOARD_PIN_SD_CS) && (BOARD_PIN_SD_CS != UINT16_MAX)
    static const GpioPin sd_cs = {.port = NULL, .pin = BOARD_PIN_SD_CS};
    furi_hal_gpio_init_simple(&sd_cs, GpioModeOutputPushPull);
    furi_hal_gpio_write(&sd_cs, true);
    ESP_LOGI(TAG, "SD_CS GPIO%d set HIGH (deselect)", BOARD_PIN_SD_CS);
#endif

#if defined(BOARD_PIN_LCD_CS) && (BOARD_PIN_LCD_CS != UINT16_MAX)
    static const GpioPin lcd_cs = {.port = NULL, .pin = BOARD_PIN_LCD_CS};
    furi_hal_gpio_init_simple(&lcd_cs, GpioModeOutputPushPull);
    furi_hal_gpio_write(&lcd_cs, true);
    ESP_LOGI(TAG, "LCD_CS GPIO%d set HIGH (deselect)", BOARD_PIN_LCD_CS);
#endif

#if defined(BOARD_PIN_SD_MISO) && (BOARD_PIN_SD_MISO != UINT16_MAX)
    /* Enable internal pull-up on shared MISO pin to prevent floating bus */
    gpio_set_pull_mode((gpio_num_t)BOARD_PIN_SD_MISO, GPIO_PULLUP_ONLY);
#endif

#if defined(BOARD_PIN_SD_CS) && defined(BOARD_PIN_LCD_SCLK) && \
    (BOARD_PIN_SD_CS != UINT16_MAX) && (BOARD_PIN_LCD_SCLK != UINT16_MAX)
    /* Clock 80 cycles with SD_CS HIGH to ensure SD card tristates DO/MISO */
    static const GpioPin sck_pin = {.port = NULL, .pin = BOARD_PIN_LCD_SCLK};
    furi_hal_gpio_init_simple(&sck_pin, GpioModeOutputPushPull);
    for(int i = 0; i < 80; i++) {
        furi_hal_gpio_write(&sck_pin, false);
        esp_rom_delay_us(1);
        furi_hal_gpio_write(&sck_pin, true);
        esp_rom_delay_us(1);
    }
    furi_hal_gpio_write(&sck_pin, false);
#endif

    ESP_LOGI(TAG, "Early init complete");
}

void furi_hal_deinit_early(void) {
}

void furi_hal_init(void) {
    /* NVS is required by WiFi and BLE — init once at boot */
    esp_err_t nvs_err = nvs_flash_init();
    if(nvs_err == ESP_ERR_NVS_NO_FREE_PAGES || nvs_err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    furi_hal_rtc_init();
    furi_hal_version_init();
    furi_hal_power_init();
    furi_hal_crypto_init();
    furi_hal_subghz_init();
    furi_hal_usb_init();
    furi_hal_light_init();
    furi_hal_display_init();
    furi_hal_speaker_init();
    furi_hal_nfc_init();
    ESP_LOGI(TAG, "Init complete");
}
