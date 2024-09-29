#include <Arduino.h>
#include "DYPlayerArduino.h"

// Initialise the player, it defaults to using Serial.
DY::Player player(&Serial1);

const byte interruptPin = 4;
const byte volUpPin = 2;
const byte volDownPin = 3;
volatile bool movement = false;
volatile bool no_move=false;
volatile uint8_t volume=20;

int delay_counter=1;
int file_number=1;
void play_next_file();

void movement_interrupt()
{
  if (!no_move)
  {
    
    movement=true;
    no_move=true;
    
  }

}

void vol_up_interrupt()
{
  static uint32_t last_time = 0;
  if (millis()-last_time > 500)
  {
    last_time = millis();
  }
  else 
  {
    return ;
  }
  if (volume < 30)
  {
    volume++;
    player.setVolume(volume);
  }
    Serial.print("vol: ");
    Serial.println(static_cast<uint16_t>(volume));
}

void vol_down_interrupt()
{
    static uint32_t last_time = 0;
  if (millis()-last_time > 500)
  {
    last_time = millis();
  }
  else 
  {
    return ;
  }
  if (volume > 10)
  {
    volume--;
    player.setVolume(volume);
  }
    Serial.print("vol: ");
    Serial.println(static_cast<uint16_t>(volume));
}

void setup() {
  Serial.begin(115200);
  player.begin(); //initialize the module
  player.setVolume(volume); // Set Volume to 100%

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), movement_interrupt, CHANGE);

  pinMode(volUpPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(volUpPin), vol_up_interrupt, FALLING);
  pinMode(volDownPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(volDownPin), vol_down_interrupt, FALLING);
  

}

void play_next_file()
{
    Serial.print("play file ");
    Serial.println(file_number);
    player.playSpecified(file_number); //Play specified file
    file_number++;
    if (file_number>5)
    {
      file_number=1;
    }
}

void loop() {
  if (movement)
  {
    Serial.println("move detected");
    play_next_file();
    delay(2);
    movement=false;
    
  }
  if (player.checkPlayState() != DY::PlayState::Playing)
  {
    if (no_move)
    {
      delay_counter++;
      if (delay_counter>3)
      {
        delay_counter=0;
        no_move=false;
        Serial.println("Movement Detection Enabled");
      }
    }
  }
  // if (player.checkPlayState() != DY::PlayState::Playing)
  // {


 
  // }
  delay(1000); //wait for 5 second
}