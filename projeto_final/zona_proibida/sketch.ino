#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  public:
    String rssiString;

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (strcmp(advertisedDevice.getAddress().toString().c_str(), "74:b0:59:8d:3b:b9") == 0) {
        rssiString = String(advertisedDevice.getRSSI());

        Serial.print("Specific device found: ");
        Serial.print(advertisedDevice.getAddress().toString().c_str());
        Serial.print(" RSSI: ");
        Serial.println(rssiString);
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Bluetooth...");


  BLEDevice::init("");  // init BLE


  BLEScan* pBLEScan = BLEDevice::getScan();  // criar scanner
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

  pBLEScan->setInterval(1349); 
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true); 


  pBLEScan->start(1, false);  // iniciar scan
}

void loop() {
  // escanear a cada 1s
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->start(1, false);
  delay(1000);
}
