@echo off
gcc main.c brute_force.c semaforo.c -o decrypt.exe -lssl -lcrypto
