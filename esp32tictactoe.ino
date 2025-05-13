#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SDA 14
#define SCL 13
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "google wifi";
const char* password = "yu825828";
const char* broker = "34.145.124.95";

WiFiClient net;
PubSubClient mqtt(net);

char board[9];
char current = 'X';
int xWins = 0, oWins = 0, ties = 0, games = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  if (!testI2C(0x27)) lcd = LiquidCrystal_I2C(0x3F, 16, 2);
  lcd.init(); lcd.backlight();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi OK: " + WiFi.localIP().toString());

  mqtt.setServer(broker, 1883);
  mqtt.setCallback(handleMsg);
  connectMQTT();

  reset();
  sendTurn();
}

void loop() {
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
}

void handleMsg(char* topic, byte* payload, unsigned int len) {
  if (games >= 100) return;

  String msg((char*)payload, len);
  Serial.println("Msg: " + msg);
  if (msg.length() != 2 || msg[0] != current) return;

  int pos = msg[1] - '0';
  if (pos < 0 || pos > 8 || board[pos] != ' ') {
    Serial.printf("Invalid %c move: %d\n", current, pos);
    sendTurn(); return;
  }

  board[pos] = current;
  sendBoard();
  Serial.printf("%c -> %d\n", current, pos);

  if (won(current)) {
    record(current);
    Serial.printf("%c wins\n", current);
  } else if (full()) {
    record('T');
    Serial.println("Tie");
  } else {
    current = (current == 'X') ? 'O' : 'X';
    sendTurn(); return;
  }

  showStats();
  delay(2000);
  reset();
  mqtt.loop();
  current = 'X';
  games++;
  mqtt.publish("game/complete", String(games).c_str(), true);
  mqtt.loop();
  if (games < 100) sendTurn();
}

void reset() {
  memset(board, ' ', 9);
  sendBoard();
  mqtt.loop();
  Serial.println("Board cleared");
}

void sendBoard() {
  mqtt.publish("game/board", String(board).c_str(), true);
}

void sendTurn() {
  String msg(current);
  mqtt.publish("game/turn", msg.c_str(), true);
  mqtt.publish("test/debug", ("Turn: " + msg).c_str());
  Serial.println("Turn: " + msg);
}

bool won(char p) {
  const int win[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},
    {0,3,6}, {1,4,7}, {2,5,8},
    {0,4,8}, {2,4,6}
  };
  for (auto& line : win)
    if (board[line[0]] == p && board[line[1]] == p && board[line[2]] == p)
      return true;
  return false;
}

bool full() {
  for (char c : board) if (c == ' ') return false;
  return true;
}

void record(char who) {
  if (who == 'X') xWins++;
  else if (who == 'O') oWins++;
  else ties++;
  Serial.printf("Score: X:%d O:%d T:%d G:%d\n", xWins, oWins, ties, games + 1);
}

void showStats() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.printf("X:%d O:%d", xWins, oWins);
  lcd.setCursor(0, 1); lcd.printf("T:%d G:%d", ties, games + 1);
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.println("MQTT connecting...");
    if (mqtt.connect("ESP32Client")) {
      Serial.println("MQTT connected");
      mqtt.subscribe("game/move");
    } else {
      Serial.print("MQTT fail, rc="); Serial.println(mqtt.state());
      delay(2000);
    }
  }
}

bool testI2C(uint8_t addr) {
  Wire.beginTransmission(addr);
  return Wire.endTransmission() == 0;
}
