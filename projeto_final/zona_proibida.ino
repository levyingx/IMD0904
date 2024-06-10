#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; // Tempo de scan em segundos
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      int rssi = advertisedDevice.getRSSI();
      std::string name = advertisedDevice.getName();
      if (name == "Gato Tracker") {
        Serial.print("Dispositivo encontrado: ");
        Serial.print(name.c_str());
        Serial.print(" com RSSI: ");
        Serial.println(rssi);

        // Calcule a distância usando o valor de RSSI
        double distance = calculateDistance(rssi);
        Serial.print("Distância estimada: ");
        Serial.print(distance);
        Serial.println(" metros");
      }
    }
};

double calculateDistance(int rssi) {
  int txPower = -69; // Potência de transmissão em dBm (pode variar)
  if (rssi == 0) {
    return -1.0; // Não foi possível calcular a distância
  }
  double ratio = rssi*1.0/txPower;
  if (ratio < 1.0) {
    return pow(ratio,10);
  } else {
    double distance =  (0.89976)*pow(ratio,7.7095) + 0.111;
    return distance;
  }
}

void setup() {
  Serial.begin(115200);
  
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Cria o objeto scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Ative o scan ativo
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void loop() {
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Dispositivos encontrados: ");
  Serial.println(foundDevices.getCount());
  delay(2000);
}