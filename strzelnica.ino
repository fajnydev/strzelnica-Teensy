// Teensyduino Shooting Range

#include <Audio.h>
#include <Wire.h>

#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <Bounce.h> 

AudioPlaySdWav           playWav1;
AudioOutputAnalog        audioOutput; // Audio as DAC here
AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);

#define BUTTON_1 0 //Guzika 1 nasłuchujemy na PIN 0
#define BUTTON_2 1 //Guzika 2 nasłuchujemy na PIN 1
#define BUTTON_3 2 //Guzika 3 nasłuchujemy na PIN 2
#define BUTTON_4 3 //Guzika 4 nasłuchujemy na PIN 3
#define BUTTON_5 4 //Guzika 5 nasłuchujemy na PIN 4
#define BUTTON_6 5 //Guzika 6 nasłuchujemy na PIN 5

#define BUTTON_RESET 14 //Guzika RESET nasłuchujemy na PIN 14
#define BUTTON_HARDER 15 //Guzika TRUDNIEJ nasłuchujemy na PIN 15


// Instantiate a Bounce object with a 5 millisecond debounce time
// This code turns a target hit through a debounced button
// like on the circuit indicated here: http://arduino.cc/en/Tutorial/Button
Bounce bouncer_1 = Bounce( BUTTON_1, 50 ); 
Bounce bouncer_2 = Bounce( BUTTON_2, 50 ); 
Bounce bouncer_3 = Bounce( BUTTON_3, 50 ); 
Bounce bouncer_4 = Bounce( BUTTON_4, 50 ); 
Bounce bouncer_5 = Bounce( BUTTON_5, 50 ); 
Bounce bouncer_6 = Bounce( BUTTON_6, 50 ); 
Bounce bouncer_BUTTON_RESET  = Bounce(BUTTON_RESET , 300);
Bounce bouncer_BUTTON_HARDER = Bounce(BUTTON_HARDER, 300);

#define LED_1 23 //Napięcie do Led1 podajemy na PIN 23
#define LED_2 22 //Napięcie do Led2 podajemy na PIN 22
#define LED_3 21 //Napięcie do Led3 podajemy na PIN 21
#define LED_4 20 //Napięcie do Led4 podajemy na PIN 20
#define LED_5 19 //Napięcie do Led5 podajemy na PIN 19
#define LED_6 18 //Napięcie do Led6 podajemy na PIN 18

// The SD card may connect on those specific pins only without additional configuration
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13


#define SD_FOLDER_SIZE_COUNTER 6
int randNumber;

uint8_t ledLightMask = 0;

bool defaultMode=true;
int timeToChangeTarget = 5000;// co 3 sekundy zmiana celu
int lastResetTime = millis();

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
      Serial.println("Unable to access the SD card! is card inserted? check wiring? chipset pin usage set? is CPU frequency 24MHz(Tools->CPU speed)?");
      delay(1000);
    }
  }
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  // Na pinach BUTTON będziemy sprawdzać podawanie lub nie napięcia
  // Guziki mają być "normalnie wysokie" czyli dają napięcie dopóki ich nie wdusimy (wtedy zwiera do GND i nie dają napięcia). Można je zwierać do masy i będzie OK, rezystor podciągający jest w czipie
  // The pullup resistors are useful when connecting pushbuttons that can connect the pin to ground (low), but when the button is not pressed there is no connection at all. The pullup resistor causes the voltage to be high when nothing is connected.
  // A brief delay may be needed between pinMode() configuring INPUT_PULLUP mode and digitalRead() reporting the unconnected pin as HIGH. The pullup resistor raises the voltage slowly, depending on capacitance of any circuitry attached, plus the capacitance of the pin and breadboard or wires. Usually delayMicroseconds(10) is plenty.
  pinMode(BUTTON_1,INPUT_PULLUP); 
  pinMode(BUTTON_2,INPUT_PULLUP); 
  pinMode(BUTTON_3,INPUT_PULLUP); 
  pinMode(BUTTON_4,INPUT_PULLUP); 
  pinMode(BUTTON_5,INPUT_PULLUP); 
  pinMode(BUTTON_6,INPUT_PULLUP); 
  pinMode(BUTTON_RESET , INPUT_PULLUP);
  pinMode(BUTTON_HARDER, INPUT_PULLUP);
  delayMicroseconds(10);

  //Na pinach LED będziemy sterować podawaniem lub nie napięcia
  pinMode(LED_1,OUTPUT);
  pinMode(LED_2,OUTPUT);
  pinMode(LED_3,OUTPUT);
  pinMode(LED_4,OUTPUT);
  pinMode(LED_5,OUTPUT);
  pinMode(LED_6,OUTPUT);
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  playWav1.play(filename);  // Start playing the file.  This sketch continues to run while the file plays.
  delay(5);  // A brief delay for the library read WAV info

  
}

void playRandomSound(int folderNumber)
{
  randNumber = random(1, SD_FOLDER_SIZE_COUNTER);
  char str[128];

  //sprintf(str, "hello %s", "world");
  snprintf(str, 128, "/T%d/S%d.WAV", folderNumber, randNumber);
  playFile(str);  // filenames are always uppercase 8.3 format
  //  Analog DAC - Connect the DAC pin to an amplified speaker
  //  http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
// Simply wait for the file to finish playing. jakbys chcial zeby kolejny strzal(dzwiek) nie zatrzymywal poprzedniego dzwieku. To zamula gre, narazie wylaczam  //

 while (playWav1.isPlaying()) { };
  //   // uncomment these lines if you audio shield  //   // has the optional volume pot soldered  //   //float vol = analogRead(15);  //   //vol = vol / 1024;  //   // sgtl5000_1.volume(vol);  // }
ledLightMask |= 0b00000000;
updateLedsAsInMask();

if(!defaultMode){delay(random(700, timeToChangeTarget));}
}
void playWelcomeSound(int folderNumber)
{
  char str[128];

  //sprintf(str, "hello %s", "world");
  snprintf(str, 128, "W%d.WAV", folderNumber);
  playFile(str);  // filenames are always uppercase 8.3 format
  //  Analog DAC - Connect the DAC pin to an amplified speaker
  //  http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
}

void updateLedsAsInMask()
{ 
  if (__builtin_ctz(ledLightMask)+1 == 1) {
    digitalWrite(LED_1, HIGH );
  }else {digitalWrite(LED_1, LOW );}

  if (__builtin_ctz(ledLightMask)+1 == 2) {

    digitalWrite(LED_2, HIGH );
  }else {digitalWrite(LED_2, LOW );}

  if (__builtin_ctz(ledLightMask)+1 == 3) {

    digitalWrite(LED_3, HIGH );
  }else {digitalWrite(LED_3, LOW );}

  if (__builtin_ctz(ledLightMask)+1 == 4) {

    digitalWrite(LED_4, HIGH );
  }else{digitalWrite(LED_4, LOW );}

  if (__builtin_ctz(ledLightMask)+1 == 5) {
    digitalWrite(LED_5, HIGH );
  } else {digitalWrite(LED_5, LOW );}
  if (__builtin_ctz(ledLightMask)+1 == 6) {
    digitalWrite(LED_6, HIGH );
  }else {digitalWrite(LED_6, LOW );}
  
  if(ledLightMask == 0b00000000){
    digitalWrite(LED_6, LOW );
    digitalWrite(LED_5, LOW );
    digitalWrite(LED_4, LOW );
    digitalWrite(LED_3, LOW );
    digitalWrite(LED_2, LOW );
    digitalWrite(LED_1, LOW );
  }

  if(ledLightMask == 0b00111111){
    digitalWrite(LED_1, HIGH );
    digitalWrite(LED_2, HIGH );
    digitalWrite(LED_3, HIGH );
    digitalWrite(LED_4, HIGH );
    digitalWrite(LED_5, HIGH );
    digitalWrite(LED_6, HIGH );

  }
}

void reset()
{
    ledLightMask |= 0b00111111;
    Serial.println(__builtin_ctz(ledLightMask)+1);
    updateLedsAsInMask();
    timeToChangeTarget = 5000;
    lastResetTime = millis();
}

void hitDone(){
if (defaultMode){//all works
ledLightMask |= 0b00111111;
updateLedsAsInMask();
} else { //reload
lastResetTime = millis();
ledLightMask |= 0b00000000;
ledLightMask = (uint8_t) (1u << (uint8_t)random(0, 3));//tu zmien na 6 ! do testow starczy 3
updateLedsAsInMask();
playWelcomeSound(__builtin_ctz(ledLightMask)+1);
}
}

//ta funkcja wywołuje się w kółko, jak skończy raz leci drugi i tak zawsze
void loop() {


  if(millis()-lastResetTime>(unsigned long)timeToChangeTarget){
    hitDone();
  }

 // Update the debouncer
  bouncer_1.update();
  bouncer_2.update();
  bouncer_3.update();
  bouncer_4.update();
  bouncer_5.update();
  bouncer_6.update();
  bouncer_BUTTON_RESET.update();
  bouncer_BUTTON_HARDER.update();
 
 // Get the update value
 int value_1 = bouncer_1.read();
 int value_2 = bouncer_2.read();
 int value_3 = bouncer_3.read();
 int value_4 = bouncer_4.read();
 int value_5 = bouncer_5.read();
 int value_6 = bouncer_6.read();
 int value_RESET = bouncer_BUTTON_RESET.read();
 int value_HARDER = bouncer_BUTTON_HARDER.read();


 if ( value_1 == LOW && (ledLightMask == 0b00111111 ||  (__builtin_ctz(ledLightMask)+1 == 1))) {
  Serial.println("jeden");
    playRandomSound(1); 

    hitDone();
 }
  if ( value_2 == LOW &&  (ledLightMask == 0b00111111 ||  (__builtin_ctz(ledLightMask)+1 == 2))) {
    Serial.println("dwa");
    playRandomSound(2); 
    hitDone();
 }
  if ( value_3 == LOW && (ledLightMask == 0b00111111 ||  (__builtin_ctz(ledLightMask)+1 == 3))) {
    Serial.println("trzy");
    playRandomSound(3); 
    hitDone();
 }
  if ( value_4 == LOW && (ledLightMask == 0b00111111 ||  (__builtin_ctz(ledLightMask)+1 == 4))) {
    playRandomSound(4); 
    hitDone();
 }
  if ( value_5 == LOW && (ledLightMask == 0b00111111 ||  (__builtin_ctz(ledLightMask)+1 == 5))) {
    playRandomSound(5); 
    hitDone();
 }
  if ( value_6 == LOW && (ledLightMask == 0b00111111 || (__builtin_ctz(ledLightMask)+1 == 6))) {
     playRandomSound(6); 
    hitDone();
 }
  if ( value_RESET == LOW) {
      defaultMode = true;
      timeToChangeTarget = 5000;
      reset();
      hitDone();
 }
  if ( value_HARDER == LOW) {
      defaultMode = false;
      timeToChangeTarget = timeToChangeTarget*0.99;
      Serial.println(timeToChangeTarget);
      hitDone();
 }
}
