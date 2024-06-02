#define BTN_START 12
#define BTN_1 14
#define BTN_2 27

#define LED_START 16
#define LED_1 2
#define LED_2 4

int state_game = 0;

unsigned long time_game = 0;

int score_player1 = 0;
int score_player2 = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Touch Test");
  pinMode(LED_START, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(BTN_START, INPUT);
}

void start_game() {
  int winner = 0;
  bool game_end = false;

  long startMills = millis();
  
  while(!game_end){

    int touch_1 = touchRead(BTN_1);
    int touch_2 = touchRead(BTN_2);

    if(touch_1 > 10 && 15 >= touch_1){
      Serial.println("Jogador 1 tocou primeiro.");
      winner = 1;
      game_end = true;
      break;

    }else if(touch_2 > 15 && 20 >= touch_2){
      Serial.println("Jogador 2 tocou primeiro.");
      winner = 2;
      game_end = true;
      break;
    }

  }

  long endMills = millis();
  time_game = endMills - startMills;
  if(winner == 1) {
    score_player1++;
    digitalWrite(LED_1, HIGH);
    delay(1000);
    digitalWrite(LED_1, LOW);
    Serial.println("Tempo jogador 1: ");
    Serial.println(time_game);
  } else if (winner == 2) {
    score_player2++;
    digitalWrite(LED_2, HIGH);
    delay(1000);
    digitalWrite(LED_2, LOW);
    Serial.println("Tempo jogador 2: ");
    Serial.println(time_game);
  }
  Serial.println("Jogador 1: ");
  Serial.println(score_player1);
  Serial.println("Jogador 2: ");
  Serial.println(score_player2);
}

void loop() {
  if(state_game == 0) {
    digitalWrite(BTN_START, 0);
    int touch_start = touchRead(BTN_START); 
    if(touch_start >=14 && touch_start <= 24) {
      Serial.println("INICIAR PARTIDA");
      int led_on = 0;

      while(led_on != 3) {
        digitalWrite(LED_START, HIGH);
        delay(500); 
        digitalWrite(LED_START, LOW);
        delay(500); 
        led_on++;
      }
      state_game = 1;
    }
  }

  if(state_game == 1) {
    Serial.println("PARTIDA INICIADA");
    start_game();
    state_game = 0; // Update state after game ends
    delay(1000);
    Serial.println("PARTIDA FINALIZADA");
  }


}