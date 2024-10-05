#include <Arduino.h>
#include "DYPlayerArduino.h"

// Initialise the player, it defaults to using Serial.
DY::Player player(&Serial1);

const byte interruptPin = 4;
const byte volUpPin = 2;
const byte volDownPin = 3;
const byte gameOverPin = 1;
volatile bool movement = false;
volatile bool no_move=false;
volatile bool in_game_over=false;
volatile bool game_over=false;
volatile uint8_t volume=25;

const uint16_t after_play_delay_s = 3;
const uint16_t game_over_delay_s = 30*60;

uint16_t delay_counter=1;
uint16_t file_number=1;
const uint16_t game_over_file_number=6;

void play_game_over_file()
{
  Serial.print("play file ");
    Serial.println(game_over_file_number);
    player.playSpecified(game_over_file_number); //Play specified file
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

void movement_interrupt()
{
  if (!no_move)
  {
    movement=true;
    no_move=true;
    
  }
  
}
void game_over_interrupt()
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
  if(!in_game_over)
  {
    Serial.print("Game over! (");
    Serial.print(game_over_delay_s);
    Serial.println("s)");
    game_over=true;
    in_game_over=true;

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
  in_game_over=false;
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
  in_game_over=false;
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
  attachInterrupt(digitalPinToInterrupt(interruptPin), movement_interrupt, RISING);

  pinMode(volUpPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(volUpPin), vol_up_interrupt, FALLING);
  pinMode(volDownPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(volDownPin), vol_down_interrupt, FALLING);
  pinMode(gameOverPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(gameOverPin), game_over_interrupt, FALLING);

}


void loop() {
  if (in_game_over)
  {
     if (game_over)
     {
      game_over=false;
      play_game_over_file();
     }
     delay_counter++;
      if (delay_counter>game_over_delay_s)
      {
        delay_counter=0;
        no_move=false;
        movement=false;
        in_game_over=false;
        Serial.println("Movement Detection Enabled");
      }
  }
  else 
  {
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
        if (delay_counter>after_play_delay_s)
        {
          delay_counter=0;
          no_move=false;
          Serial.println("Movement Detection Enabled");
        }
      }
    }
  }
  
  delay(1000); //wait for 5 second
}