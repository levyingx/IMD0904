/*
  LED bar graph

  https://wokwi.com/arduino/projects/309829489359061570

  Turns on a series of LEDs based on the value of an analog sensor.
  This is a simple way to make a bar graph display. Though this graph uses 10
  LEDs, you can use any number by changing the LED count and the pins in the
  array.

  This method can be used to control any series of digital outputs that depends
  on an analog input.

  The circuit:
  - LEDs from pins 2 through 11 to ground

  created 4 Sep 2010
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/BarGraph
*/

// these constants won't change:

int leitura = 0;
int potPin = A0;
int total_pins = 10;
int pin_list[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
void setup() {
  // loop over the pin array and set them all to output:
    Serial.begin(9600);

    // Loop for para iterar sobre a lista de pinos
    for (int i = 0; i <total_pins; ++i) {
      pinMode(pin_list[i], OUTPUT);
      Serial.println(pin_list[i]);
    }
   
}

void loop() {
// put your main code here, to run repeatedly:
  leitura = analogRead(potPin);

  if(leitura < 500 ){

    for (int i = 0; i <total_pins; ++i) {
      ofPin(pin_list[i]);
    }

  }else if(leitura >= 500 && leitura <= 600){


    for (int i = 0; i <total_pins/2; ++i) {
      onPin(pin_list[i]);
    }

    for (int i = total_pins/2; i <total_pins; ++i) {
      ofPin(pin_list[i]);
    }
    
  }else if(leitura == 1023){
    for (int i = 0; i <total_pins; ++i) {
      onPin(pin_list[i]);
    }
  }

  
}

void onPin(int pin){
  int readPin = digitalRead(pin);
  if(readPin == LOW){
      digitalWrite(pin, HIGH);
      Serial.println("ligando pino");
    }

}

  void ofPin(int pin){
    int readPin = digitalRead(pin);
    if(readPin == HIGH){
      digitalWrite(pin, LOW);
      Serial.println("deSligando pino");
    }
    
}