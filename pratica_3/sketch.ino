#include <SPI.h>
#include <MFRC522.h>

#define PINO_SS 10
#define PINO_RST 9
#define PINO_BUZZER 13
#define LED_VERDE 5
#define LED_VERMELHO 6

MFRC522 mfrc522(PINO_SS, PINO_RST); 

int tentativasAcesso = 0;

void setup() {
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Sistema Inicializado: Aproxime o token");
  digitalWrite(LED_VERMELHO, HIGH);
}

void loop() {
  delay(500);
  digitalWrite(11, HIGH);
  
  if (!mfrc522.PICC_IsNewCardPresent()) return;

  if (!mfrc522.PICC_ReadCardSerial()) return;
  
  Serial.print("UID da tag:");

  String conteudo= "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      Serial.print(" 0");
    } else {
      Serial.print(" ");
    }
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    if (mfrc522.uid.uidByte[i] < 0x10) {
      conteudo.concat(String(" 0"));
    } else {
      conteudo.concat(String(" "));
    }
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();

  conteudo.toUpperCase();

  if (conteudo.substring(1) == "60 85 73 A5") {
    Serial.println("Acesso permitido.");
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(PINO_BUZZER, HIGH);
    delay(10000); 
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(PINO_BUZZER, LOW);
  } else {
    Serial.println("Cartão inválido.");
    digitalWrite(LED_VERMELHO, HIGH);
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_VERMELHO, LOW);
      delay(500);
      digitalWrite(LED_VERMELHO, HIGH);
      delay(500);
    }
    tentativasAcesso++;
  }

  if (tentativasAcesso == 5) {
    Serial.println("Sistema Bloqueado");
    digitalWrite(LED_VERMELHO, LOW);
    for(int i = 0; i < 15; i++) {
      digitalWrite(PINO_BUZZER, HIGH);
      delay(1000);
      digitalWrite(PINO_BUZZER, LOW);
      delay(1000);
    }
    tentativasAcesso = 0;
  }

  delay(1000);
}
