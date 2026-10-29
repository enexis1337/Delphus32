@echo off
set "IDF_TOOLS_PATH=C:\Users\dfili\.espressif"
set "PATH=C:\Users\dfili\.espressif\python_env\idf5.5_py3.11_env\Scripts;%PATH%"
call C:\Espressif\frameworks\esp-idf-v5.5.5\export.bat
ninja -C build_esp32s3_st7567
