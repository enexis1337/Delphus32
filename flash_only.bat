@echo off
set "PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env\Scripts;%PATH%"
set IDF_PATH=C:\Espressif\frameworks\esp-idf-v5.5.5
set IDF_TOOLS_PATH=C:\Users\dfili\.espressif
set IDF_PYTHON_ENV_PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env
set FLIPPER_BOARD=esp32s3_st7567

echo === Setting up ESP-IDF environment ===
call "%IDF_PATH%\export.bat"

echo === Flashing already built firmware ===
echo Trying COM8 first...
idf.py -B build_esp32s3_st7567 -DFLIPPER_BOARD=esp32s3_st7567 -p COM8 flash
if not errorlevel 1 goto :success

echo.
echo COM8 failed or not found. Trying auto-detecting port...
idf.py -B build_esp32s3_st7567 -DFLIPPER_BOARD=esp32s3_st7567 flash
if not errorlevel 1 goto :success

goto :fail

:success
echo.
echo === DONE! Firmware flashed successfully ===
goto :end

:fail
echo.
echo === FLASH FAILED ===
echo 1. Check that ESP32-S3 is connected via USB data cable.
echo 2. Check Device Manager for the COM port number.
echo 3. Hold BOOT button while plugging in USB cable if needed.

:end
pause
