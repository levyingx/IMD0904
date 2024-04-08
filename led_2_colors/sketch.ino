const int redPin = 9;    // Pino para o LED vermelho
const int greenPin = 10; // Pino para o LED verde

int count = 0;
int leitura = 0;
int btPin = 6;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  Serial.begin(9600);
  pinMode(btPin, INPUT);
}

void loop() {

  leitura = digitalRead(btPin);
  //Serial.println(leitura);
  //delay(500);
 

  if(leitura == HIGH){
    delay(100);
    leitura = 0;
    count = count + 1;
    Serial.println("count:");
    Serial.println(count);

    if(count%2 == 0){
      setColor(0, 255);//verde
      delay(1000);
    }else{
      setColor(255, 0);//vermelho
      delay(1000);
    }

    delay(1000); // Espera 1 segundo

  }
}

// Função para definir a cor do LED RGB
void setColor(int redValue, int greenValue) {
  analogWrite(redPin, redValue);     // Define a intensidade da cor vermelha
  analogWrite(greenPin, greenValue); // Define a intensidade da cor verde
}
