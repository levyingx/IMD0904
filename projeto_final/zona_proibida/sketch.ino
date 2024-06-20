#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include<WiFi.h>
#include <PubSubClient.h>
#include <math.h>

// Defina os UUIDs do serviço e da característica
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"


//broker connect
#define IO_USERNAME  "michelleteixeiramartins@gmail.com" //<----------Mudar para o seu
#define IO_KEY       "123" //<----------Mudar para o seu


//wifi connect
const char* ssid = "TEIXEIRA";
const char* password ="20011005";


const char* mqttserver = "maqiatto.com";
const int mqttport = 1883;
const char* mqttUser = IO_USERNAME;
const char* mqttPassword = IO_KEY;

WiFiClient espClient;
PubSubClient client(espClient);


// Crie uma característica BLE
BLECharacteristic *characteristic;

struct Location {
    int index;
    std::string latitude;
    std::string longitude;
};


// Vetor para armazenar as localizações dos clientes
std::vector<Location> zones;
bool deviceConnected = false;


// Função para imprimir todos os dados no vetor clientLocations
void printClientLocations() {
    Serial.println("Client Locations:");
    for (const auto& location : zones) {
        Serial.print("Index: ");
        Serial.println(location.index);
        Serial.print("Latitude: ");
        Serial.println(location.latitude.c_str());
        Serial.print("Longitude: ");
        Serial.println(location.longitude.c_str());
    }
}

// Callback para eventos do servidor BLE
class MyServerCallbacks: public BLEServerCallbacks {
   
   void onConnect(BLEServer* pServer) {
      Serial.println("cliente conectado!!!!");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


// Callback para eventos da característica BLE
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue().c_str();
      Serial.print("Received data: ");
      Serial.println(value.c_str());

      // Encontra a posição da vírgula
       size_t separator = value.find(',');
       if (separator != std::string::npos) {
            // Extrai a latitude e longitude
            std::string latitude = value.substr(0, separator);
            std::string longitude = value.substr(separator + 2); // Ignora a vírgula e o espaço após ela

            // Armazena a latitude e longitude na estrutura de dados
            Location location = {zones.size(), latitude, longitude };
            zones.push_back(location);

            // Envia o index da zona
            std::string zone_index = "Zone " + location.index;
            pCharacteristic->setValue(zone_index.c_str());
            pCharacteristic->notify();
            printClientLocations(); 
            
        } else {
          
            //Serial.println("Dado inválido!");
        }
      
    }
};


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin("TEIXEIRA", "20011005");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("WiFi connected");
 //  Serial.print("IP address: ");
 //  Serial.print(WiFi.localIP());
}


void connect_broker() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // Create a random client ID
    String clientId = "Cat - Zone";
    clientId += String(random(0xffff), HEX);
    // Se conectado
    if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      Serial.println("conectado");
      // Depois de conectado, publique um anúncio ...
      client.publish("michelleteixeiramartins@gmail.com/connect", "Gato conectado!");
      //... e subscribe.
      client.subscribe("michelleteixeiramartins@gmail.com/buzzer"); // <<<<----- mudar aqui
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5s");
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Messagem recebida [");
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
    Serial.println("buzzzer Ligado");    
  } else {
    Serial.println("buzzer Desligado");    
  }

}



void setup(){
  
  Serial.begin(115200);
  setup_wifi();
  
  
  BLEDevice::init("ESP32");
  
  // Cria o servidor BLE
  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks());

  // Cria o serviço BLE
  BLEService *service = server->createService(SERVICE_UUID);

  // Cria a característica BLE para leitura
  characteristic = service->createCharacteristic(
                       CHARACTERISTIC_UUID,
                       BLECharacteristic::PROPERTY_READ |
                       BLECharacteristic::PROPERTY_WRITE |
                       BLECharacteristic::PROPERTY_NOTIFY
                    );

  characteristic->setCallbacks(new MyCallbacks());
  // Adiciona descritor para notificações
  characteristic->addDescriptor(new BLE2902());
  // Inicia o serviço
  service->start();

  // Inicia a publicação do serviço
  BLEAdvertising *pAdvertising = server->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("Servidor BLE iniciado");  
  Serial.println("Waiting a client connection to notify...");

  client.setServer(mqttserver, 1883); // Publicar
  client.setCallback(callback); // Receber mensagem
}

void loop() {

  if (!client.connected()) {
    connect_broker();
  }

  client.loop();
    
   if (deviceConnected) {
    // Código para quando o dispositivo está conectado
    //characteristic->setValue("Bem-vindo! Conexão estabelecida.");
    //characteristic->notify();
  }

  delay(1000); 
}
 