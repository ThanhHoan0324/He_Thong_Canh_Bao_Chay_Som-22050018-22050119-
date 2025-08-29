#include <WiFi.h>
#include "DHT.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// ====== WiFi ======
const char* ssid     = "BUN CA RO";
const char* password = "14082012";

// ====== Adafruit IO ======
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883   // dùng 8883 nếu SSL
#define AIO_USERNAME    "ThanhHoan"
#define AIO_KEY         "aio_owtK41HjgevOUcRFl4NG5pqhe98t"

// MQTT client
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// MQTT feeds
Adafruit_MQTT_Publish feedTemp   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish feedHumi   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish feedGas    = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gas");
Adafruit_MQTT_Publish feedFlame  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/flame");

// ====== Cảm biến ======
#define DHTPIN   14
#define DHTTYPE  DHT22
DHT dht(DHTPIN, DHTTYPE);

#define MQ2_AO   34
#define FLAME_DO 33
#define BUZZER   25   // Chân Buzzer

int threshold = 3000; // ngưỡng gas (chỉ tham khảo, không dùng cảnh báo)

// ====== Biến dùng chung ======
float g_temp = 0, g_humi = 0;
int g_gas = 0, g_flame = 0;

// ====== MQTT connect ======
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Kết nối MQTT... ");
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Thử lại trong 5s...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("OK");
}

// ====== TASK: đọc cảm biến ======
void TaskReadSensors(void *pvParameters) {
  for (;;) {
    g_temp = dht.readTemperature();
    g_humi = dht.readHumidity();
    g_gas  = analogRead(MQ2_AO);

    int flameRaw = digitalRead(FLAME_DO);
    g_flame = (flameRaw == LOW) ? 1 : 0;

    // In có đơn vị °C và %
    Serial.printf("[Sensors] T=%.1f°C  H=%.1f%%  Gas=%d  Flame=%d\n", 
                  g_temp, g_humi, g_gas, g_flame);

    // Điều khiển buzzer: kêu nếu có lửa hoặc gas vượt ngưỡng
    if (g_flame == 1 || g_gas > threshold) {
      digitalWrite(BUZZER, HIGH);
    } else {
      digitalWrite(BUZZER, LOW);
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS); // đọc mỗi 5s
  }
}

// ====== TASK: gửi dữ liệu lên Adafruit IO ======
void TaskSendCloud(void *pvParameters) {
  static unsigned long lastTH = 0;
  for (;;) {
    MQTT_connect();

    // gửi gas + flame mỗi 10s
    if (!feedGas.publish((int32_t)g_gas)) Serial.println("Gas FAIL");
    if (!feedFlame.publish((int32_t)g_flame)) Serial.println("Flame FAIL");

    // nhiệt độ + độ ẩm mỗi 30s
    unsigned long now = millis();
    if (now - lastTH > 30000) {
      if (!isnan(g_temp)) feedTemp.publish(g_temp);
      if (!isnan(g_humi)) feedHumi.publish(g_humi);
      lastTH = now;
    }

    vTaskDelay(10000 / portTICK_PERIOD_MS); // 10s/lần
  }
}

// ====== TASK: duy trì kết nối MQTT ======
void TaskKeepAlive(void *pvParameters) {
  for (;;) {
    mqtt.processPackets(100);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(MQ2_AO,   INPUT);
  pinMode(FLAME_DO, INPUT);
  pinMode(BUZZER,   OUTPUT); // cấu hình buzzer

  dht.begin();

  // WiFi connect
  Serial.println("Kết nối WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());

  // Tạo task
  xTaskCreate(TaskReadSensors, "ReadSensors", 4096, NULL, 1, NULL);
  xTaskCreate(TaskSendCloud, "SendCloud", 4096, NULL, 1, NULL);
  xTaskCreate(TaskKeepAlive, "KeepAlive", 2048, NULL, 1, NULL);
}

void loop() {
  // loop trống vì đã dùng FreeRTOS
}
