# Wstep 

## Mikroukład teensy 3.1
https://www.pjrc.com/store/teensy31.html

![alt text](https://www.pjrc.com/teensy/teensy31_front_pinout.png)
![alt text](https://www.pjrc.com/teensy/teensy31_back_pinout.png)

## Lista bibliotek do Teensy
https://www.pjrc.com/teensy/td_libs.html

## Teensy jako arduino:
https://www.pjrc.com/teensy/td_download.html wtyczka do arduino, najpierw wgrywasz arduino potem wtyczke

## Biblioteka audio
* https://hackaday.com/2014/09/30/the-teensy-audio-library/
* https://www.pjrc.com/teensy/td_libs_Audio.html
* https://github.com/PaulStoffregen/Audio
* https://www.pjrc.com/store/teensy3_audio.html

## Biblioteka pliki sd
* https://forum.pjrc.com/threads/16758-Teensy-3-MicroSD-guide
* może się przydać też https://mb-raw.blogspot.com/2017/07/teensy-tutorial-sd-card.html?m=1

### Modul SD (narazie jest microsd bo cos wiring nie dzialal- sprawdzic po sformatowaniu karty bo to moze nie wiring tylko formatowanie i jednak juz dziala)
https://fluxworkshop.com/products/bfaa100012-sd-module-lc-blue 

## Instrukcja analogicznego do arduino
https://majsterkowo.pl/arduino-jako-odtwarzacz-audio/?fbclid=IwAR0HchEp5gZIIBgqg8ME3ZCD8FFU8VF3pODgldetK8HswZvixeNZp2EnDjs

## Przygotowanie plików
*Na kartę sd wrzucamy pliki WAV w formacie 44,1khz (najzwyklejszy red book CD audio)*
* Po każdej zmianie na karcie sd robimy procedurę:
* Zaznacz wszystkie pliki na karcie SD, kliknij na którymś prawym przyciskiem myszy i "zmien nazwę"
* napisz wielką literę S i naciśnij enter (nazwa przed enterem to np. "S.wav")
* Wciśnij SHIFT i kliknij prawym przyciskiem myszy na bieżącym folderze a z menu wybierz "Otwórz tutaj okno programu Power Shell"
* Wklej taką komendę:
```
Get-ChildItem -File | Rename-Item -NewName { $_.Name -replace ' ','' }
Get-ChildItem -File | Rename-Item -NewName { $_.Name -replace '\(','' }
Get-ChildItem -File | Rename-Item -NewName { $_.Name -replace '\)','' }

```
* Potwierdź enterem (karta sd to nie demon szybkości, zajmie pewnie z minutę)
* Pliki będą miały nazwy takie: "S107.wav" i podobne to znaczy, że jest ok
* Sprawdź jaka największa liczba występuje w nazwie pliku i wstaw ją do zmiennej SD_FILES_COUNTER
* Verify-> Upload i powinno działać
* jak coś tu źródło https://stackoverflow.com/questions/36894586/how-do-i-remove-blank-space-from-file-names 

Dlaczego nie listuję plików na karcie z poziomu kodu? Zapytaj ziomków od arduino co tam naodpieprzali, nie robię tego, powołuję się na klauzulę sumienia, może kiedyś znajdę czas //todo w kodzie jakis sampel dodałem sprawdz i usun ten komentarz

## Wzór kodu źródłowego (punkt wyjściowy)
https://github.com/PaulStoffregen/Audio/blob/master/examples/WavFilePlayer/WavFilePlayer.ino 

### Jakiś pomysł na wzmacniacz
https://www.reddit.com/r/arduino/comments/3qygp2/how_to_drive_a_35_mm_jack/cwjt6xz?utm_source=share&utm_medium=web2x&context=3

### Dźwięki w folderach np. cel1, cel2 ...
https://forum.arduino.cc/index.php?topic=367844.0

