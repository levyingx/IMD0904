
int potPin_1 = A0;
int leitura_1 = 0;

int potPin_2 = A1;
int leitura_2 = 0;

const int redPin = 11;    // Pino para o LED vermelho
const int greenPin = 10; // Pino para o LED verde

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
}

void loop() {
  leitura_1 = analogRead(potPin_1);

  leitura_2 = analogRead(potPin_2);
 
  analogWrite(redPin,convertToRead(leitura_1));
  analogWrite(greenPin,convertToRead(leitura_2));
  delay(100);
}

int convertToRead(int x){
  int value = 255 * x;
  return int(value / 1023);
}
