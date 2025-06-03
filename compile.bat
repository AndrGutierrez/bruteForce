@echo off
set OPENSSL_PATH="C:\OpenSSL-Win64"  REM Change this to your OpenSSL install path

gcc main.c brute_force.c semaforo.c -o decrypt.exe ^
    -I%OPENSSL_PATH%\include ^
    -L%OPENSSL_PATH%\lib ^
    -lssl -lcrypto -lws2_32 -lgdi32
