#### Itens:
- 2 ESP32 com conexão BLE (tracker e central);
- 1 ou mais dispositivos com conexâo BLE (zonas proibidas)
- 1 buzzer ou item para afastar o gato da zona (trigger);
- Broker MQTT (MaQiaTTo);
- Aplicativo pra dashboard (IoT MQTT Panel);
---
#### Central:
- Tem comunicação direta com o broker;
- Recebe a distância da zona proibida e envia para o broker;
- Recebe comandos do aplicativo do dashboard para executar ações; 
#### Tracker:
- Representa a posição do gato;
- Se comunica diretamente com o ESP32 da zona proibida;
#### Zona proibida:
- Detecta o Tracker;
- Envia a distância entre o tracker e a zona para a central;
#### Dashboard:
- Recebe dados dos dispositivos e mostra no aplicativo;
- Tem botões para executar ações e envia esses dados para a central;