@echo off
g++ tx.c -o output.exe -mwindows -lgdi32 -luser32 -O2
pyinstaller --add-data "music.wav;." --add-data "output.exe;." --onefile --noconsole --uac-admin 1.txt
