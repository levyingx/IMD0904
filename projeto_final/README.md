#### Itens:
- 2 ou mais ESP32 com conexão BLE (trackers e área proibidas);
- 1 trigger para afastar o animal da área (trigger);
- Broker MQTT (MaQiaTTo);
- Aplicativo pra dashboard (IoT MQTT Panel);
---
#### Central:
- Tem comunicação direta com o broker;
- Recebe a distância da área proibida e envia para o broker;
- Recebe comandos do aplicativo do dashboard para executar ações; 
#### Tracker:
- Representa a posição do gato;
- Se comunica diretamente com o ESP32 da área proibida;
- Envia a distância entre o tracker e a área para a central;
#### Área proibida:
- Recebe a informação da alerta;
- Atua como servidor BLE;
- Detecta o Tracker;
#### Dashboard:
- Recebe dados dos dispositivos e mostra no aplicativo;
- Tem botões para executar ações e envia esses dados para a central;
---