// Teensyduino Shooting Range
//
//

// Tu podłączam do projektu biblioteki z potrzebnymi funkcjami 
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Bounce.h> // To jest biblioteka do DeBauncingu - kiedy drganie mechaniczne styków przycisku wyprzedza taktowanie procesora bywa, że jedno kliknięcie jest rozpoznawane jako kilka (prawdopodobnie nas jeszcze nie dotyczy, ale zawsze to się robi, zakładając, że zaraz zacznie)

//Tu inicjalizuję składniki dźwięku, mówię czego będę używał i niech już będzie gotowe później
AudioPlaySdWav           playWav1;
AudioOutputAnalog        audioOutput; // Audio as DAC here
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);

#define BUTTON_1 2 //Guzika 1 nasłuchujemy na PIN 2
#define BUTTON_2 3 //Guzika 2 nasłuchujemy na PIN 3
#define BUTTON_3 4 //Guzika 3 nasłuchujemy na PIN 4
// Instantiate a Bounce object with a 5 millisecond debounce time
// This code turns a led on/off through a debounced button
// like on the circuit indicated here: http://arduino.cc/en/Tutorial/Button
Bounce bouncer_1 = Bounce( BUTTON_1, 5 ); 
Bounce bouncer_2 = Bounce( BUTTON_2, 5 ); 
Bounce bouncer_3 = Bounce( BUTTON_3, 5 ); 

#define LED_1_B 23 //Kolor Niebieski Guzika 1 podajemy na PIN 23
#define LED_1_R 22 //Kolor Czerwony Guzika 1 podajemy na PIN 22
#define LED_1_G 21 //Kolor Zielony Guzika 1 podajemy na PIN 21

#define LED_2_B 20 //Kolor Niebieski Guzika 2 podajemy na PIN 20
#define LED_2_R 19 //Kolor Czerwony Guzika 2 podajemy na PIN 19
#define LED_2_G 18 //Kolor Zielony Guzika 2 podajemy na PIN 18

#define LED_3_B 17 //Kolor Niebieski Guzika 3 podajemy na PIN 17
#define LED_3_R 16 //Kolor Czerwony Guzika 3 podajemy na PIN 16
#define LED_3_G 15 //Kolor Zielony Guzika 3 podajemy na PIN 15  


// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

// Mówię, ile plików jest na karcie SD - docelowo można o tą liczbę odpytywać kartę SD, https://arduino.stackexchange.com/questions/54339/arduino-sd-count-files-and-open-last-how-to tu gość to robi przy okazji
#define SD_FILES_COUNTER 1077

//Tu uprzedzam, że będę używał losowego numeru do .. losowania plików
int randNumber;

//Ten blok uruchamia się 1 raz - podczas włączenia urządzenia
void setup() {
  Serial.begin(9600); //to jest dla karty SD - łączymy się do niej SPI

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(8);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(1500);
    }
  }
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // Mówię tylko, że na pinach BUTTON będziemy sprawdzać podawanie lub nie napięcia
  // Mówię, że guziki mają być "normalnie wysokie" czyli dają napięcie dopóki ich nie wdusimy (wtedy zwiera do GND i nie dają napięcia)
  // The pullup resistors are useful when connecting pushbuttons that can connect the pin to ground (low), but when the button is not pressed there is no connection at all. The pullup resistor causes the voltage to be high when nothing is connected.
  // A brief delay may be needed between pinMode() configuring INPUT_PULLUP mode and digitalRead() reporting the unconnected pin as HIGH. The pullup resistor raises the voltage slowly, depending on capacitance of any circuitry attached, plus the capacitance of the pin and breadboard or wires. Usually delayMicroseconds(10) is plenty.
  pinMode(BUTTON_1,INPUT_PULLUP); 
  pinMode(BUTTON_2,INPUT_PULLUP); 
  pinMode(BUTTON_3,INPUT_PULLUP); 

  //Mówię tylko, że na pinach LED będziemy sterować podawaniem lub nie napięcia
  pinMode(LED_1_G,OUTPUT);
  pinMode(LED_1_R,OUTPUT);
  pinMode(LED_1_B,OUTPUT);
  
  pinMode(LED_2_G,OUTPUT);
  pinMode(LED_2_R,OUTPUT);
  pinMode(LED_2_B,OUTPUT);
  
  pinMode(LED_3_G,OUTPUT);
  pinMode(LED_3_R,OUTPUT);
  pinMode(LED_3_B,OUTPUT);
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  // Simply wait for the file to finish playing.
  while (playWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
  }
}

void playRandomSound()
{
  //assuming there could be system files like desktop.ini - decreasing by 1
  randNumber = random(1, SD_FILES_COUNTER-1);
  char str[128];

  //sprintf(str, "hello %s", "world");
  snprintf(str, 128, "S%d.WAV",  randNumber);
  playFile(str);  // filenames are always uppercase 8.3 format
  //  Analog DAC - Connect the DAC pin to an amplified speaker
  //  http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
  delay(200); // teoretycznie odegranie dźwięku i ten delay już są debounncingiem, ale zostawmy go, dojdą guziki np. tryb gry to sie przyda
}

//ta funkcja wywołuje się w kółko, jak skończy raz leci drugi i tak zawsze
void loop() {

 // Update the debouncer
  bouncer_1.update ( );
  bouncer_2.update ( );
  bouncer_3.update ( );
 
 // Get the update value
 int value_1 = bouncer_1.read();
 int value_2 = bouncer_2.read();
 int value_3 = bouncer_3.read();

 //jeśli trzebaby było tez odtwarzać z innego folderu dźwięki (per guzik) można albo a) skopiować funkcję playRandomSound() i zrobić playRandomSound_1() playRandomSound_2() itd i w każdej z nich obsługiwać tylko jeden folder(uwaga zmienna SD_FILES_COUNTER też do powielenia) albo można przekazywać w parametrze funkcji nazwę folderu playRandomSound("3") co jest bardziej eleganckie ale nieco trudniejsze
 if ( value_1 == HIGH) {
  //jeśli przycisk 1 jest niewduszony
 } else {
  //jeśli przycisk 1 jest wduszony zapalam odpowiednią diodę czerwona
    digitalWrite(LED_1_G, LOW );
    digitalWrite(LED_1_R, HIGH );
    playRandomSound(); 
    //zapalam odpowiednie diody w kolorze zielonym (reset)
  digitalWrite(LED_1_R, LOW );
  digitalWrite(LED_1_G, HIGH );
 }

 //jeśli trzebaby było tez odtwarzać z innego folderu dźwięki (per guzik) można albo a) skopiować funkcję playRandomSound() i zrobić playRandomSound_1() playRandomSound_2() itd i w każdej z nich obsługiwać tylko jeden folder(uwaga zmienna SD_FILES_COUNTER też do powielenia) albo można przekazywać w parametrze funkcji nazwę folderu playRandomSound("3") co jest bardziej eleganckie ale nieco trudniejsze
 if ( value_2 == HIGH) {
  //jeśli przycisk 2 jest niewduszony
 } else {
  //jeśli przycisk 2 jest wduszony zapalam odpowiednią diodę czerwona
    digitalWrite(LED_2_G, LOW );
    digitalWrite(LED_2_R, HIGH );
    playRandomSound(); 
    //zapalam odpowiednie diody w kolorze zielonym (reset)
  digitalWrite(LED_2_R, LOW );
  digitalWrite(LED_2_G, HIGH );
 }

//jeśli trzebaby było tez odtwarzać z innego folderu dźwięki (per guzik) można albo a) skopiować funkcję playRandomSound() i zrobić playRandomSound_1() playRandomSound_2() itd i w każdej z nich obsługiwać tylko jeden folder(uwaga zmienna SD_FILES_COUNTER też do powielenia) albo można przekazywać w parametrze funkcji nazwę folderu playRandomSound("3") co jest bardziej eleganckie ale nieco trudniejsze
 if ( value_3 == HIGH) {
  //jeśli przycisk 3 jest niewduszony
 } else {
  //jeśli przycisk 3 jest wduszony zapalam odpowiednią diodę czerwona
    digitalWrite(LED_3_G, LOW );
    digitalWrite(LED_3_R, HIGH );
    playRandomSound(); 
    //zapalam odpowiednie diody w kolorze zielonym (reset)
  digitalWrite(LED_3_R, LOW );
  digitalWrite(LED_3_G, HIGH );
 }
 
}
