#### Itens:
- 2 ESP32 com conexão BLE (tracker e central);
- 1 ou mais dispositivos com conexâo BLE (zonas proibidas)
- 1 buzzer ou item para afastar o gato da zona (trigger);
- Broker MQTT (MaQiaTTo);
- Aplicativo pra dashboard (IoT MQTT Panel);
---
#### Central:
- Recebe a distância da zona proibida e envia continuamente para o broker;
- 
#### Tracker:
- Representa a posição do gato;
#### Zona proibida:
- Detecta o Tracker;
- Envia a distância entre o tracker e a zona para a central (a cada _x_ segundos);
#### App:
- Recebe a distância do broker entre o tracker e a zona proibida e mostra no dashboard:
```
Distância entre (nome_do_gato) e (nome_da_zona): 260m  
```
- Switch para o trigger;
