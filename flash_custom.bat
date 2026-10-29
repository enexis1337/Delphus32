@echo off
set "IDF_TOOLS_PATH=C:\Users\dfili\.espressif"
set "PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env\Scripts;%PATH%"
set PORT=%1
if "%PORT%"=="" set PORT=COM8
echo Flashing to %PORT%...
python -m esptool --chip esp32s3 -p %PORT% -b 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size 16MB --flash_freq 80m 0x0 build_esp32s3_st7567\bootloader\bootloader.bin 0x8000 build_esp32s3_st7567\partition_table\partition-table.bin 0xf000 build_esp32s3_st7567\ota_data_initial.bin 0x20000 build_esp32s3_st7567\furi_esp32.bin
