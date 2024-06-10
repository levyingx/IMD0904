####Central:
- Recebe a distância da zona proibida e envia continuamente para o broker;
####Tracker:
- Representa a posição do gato. 
####Zona proibida:
- Detecta o Tracker;
- Envia a distância entre o tracker para a central (atualizado a cada _x_ segundos);
####Dashboard:
- Mostra a distância entre o tracker e a zona proibida (em metros);
- Botão para realizar um trigger pra tirar o gato da zona; 

