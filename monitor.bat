@echo off
set "PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env\Scripts;%PATH%"
set IDF_PATH=C:\Espressif\frameworks\esp-idf-v5.5.5
set IDF_TOOLS_PATH=C:\Users\dfili\.espressif
set IDF_PYTHON_ENV_PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env
set FLIPPER_BOARD=esp32s3_st7567

echo === Starting Serial Monitor on COM8 ===
echo (Press Ctrl + ] to exit monitor)
call "%IDF_PATH%\export.bat"
idf.py -B build_esp32s3_st7567 -p COM8 monitor
pause
