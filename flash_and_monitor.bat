@echo off
set "PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env\Scripts;%PATH%"
set IDF_PATH=C:\Espressif\frameworks\esp-idf-v5.5.5
set IDF_TOOLS_PATH=C:\Users\dfili\.espressif
set IDF_PYTHON_ENV_PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env
set FLIPPER_BOARD=esp32s3_st7567

echo === Setting up ESP-IDF environment ===
call "%IDF_PATH%\export.bat"

echo === Building, flashing and starting monitor on COM8 ===
idf.py -B build_esp32s3_st7567 -DFLIPPER_BOARD=esp32s3_st7567 build flash monitor
if errorlevel 1 goto :fail
goto :end

:fail
echo.
echo === FLASH OR MONITOR FAILED ===
echo Check that ESP32-S3 is connected. Hold BOOT while plugging in if needed.

:end
pause
