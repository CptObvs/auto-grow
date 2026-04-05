@echo off
:: Kompiliert ESPHome ausserhalb von MSYS/MinGW (noetig fuer ESP-IDF)
set MSYSTEM=
set MSYS=
set MINGW_PREFIX=
set MINGW_CHOST=
cd /d %~dp0
.venv\Scripts\esphome.exe compile auto-grow.yaml
