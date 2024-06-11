// Zona proibida:
// Atua como servidor BLE;
// Detecta o Tracker;
// Envia a distância entre o tracker e a zona para a central (aq ta mandando a direto pro broker, tem q mudar);

#include <WiFi.h>
#include <PubSubClient.h>
#include <math.h>

#define IO_USERNAME  "levyfernandes14@gmail.com"
#define IO_KEY       "teste123" 
const char* ssid = "Wokwi-GUEST";
const char* password ="";

const char* mqttserver = "maqiatto.com";
const int mqttport = 1883;
const char* mqttUser = IO_USERNAME;
const char* mqttPassword = IO_KEY;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// Saídas digitais
#define LED 26
#define BUZZER 25

void setup_wifi() {
  delay(10);

  // Conexão WiFi
  Serial.println();
  Serial.print("Conectando a rede ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    //messageTemp += (char)payload[i]; <----------Usar quando tiver uma mensagem na resposta do bloco
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(LED, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("LED ligado");    
  } else if ((char)payload[0] == '0') {
    digitalWrite(LED, LOW);  // Turn the LED off by making the voltage HIGH
    Serial.println("LED desligado");    
  }

  // Switch on the buzzer if an i was received as first character
  if ((char)payload[0] == 'i') {
    digitalWrite(BUZZER, HIGH);   // Ligar buzzer
    Serial.println("Buzzer ligado");    
  } else if ((char)payload[0] == 'o') {
    digitalWrite(BUZZER, LOW);  // Desligar buzzer
    Serial.println("Buzzer desligado");    
  }
}

void reconnect() {
  // Repetir até conectar novamente
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Client ID
    String clientId = "ESP32 - Sensores";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("Conectado");
      // Publish
      client.publish("levyfernandes14@gmail.com/distancia", "Iniciando Comunicação");
      // Subscribe
      client.subscribe("levyfernandes14@gmail.com/led"); // <<<<----- mudar aqui
      client.subscribe("levyfernandes14@gmail.com/buzzer"); // <<<<----- mudar aqui
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
      
  pinMode(LED, OUTPUT); 
  pinMode(BUZZER, OUTPUT); 

  setup_wifi();
  client.setServer(mqttserver, 1883); // Publicar
  client.setCallback(callback); // Receber mensagem
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(2000); //Intervalo de 2s entre leituras
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    /*
    pot = analogRead(POT)/100; // Mudar essa parte pra ler a distância 
    char s_pot[8];
    dtostrf(pot,1,2,s_pot);
    Serial.print("Potênciometro: ");
    Serial.println(s_pot);
    Serial.println(pot);
    client.publish("levyfernandes14@gmail.com/pot", s_pot);
    */
  }
}
