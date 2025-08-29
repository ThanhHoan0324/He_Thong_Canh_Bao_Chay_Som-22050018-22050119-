🔥 Hệ Thống Cảnh Báo Cháy & Khí Gas Dựa Trên ESP32
📌 Giới thiệu

Dự án sử dụng ESP32 kết hợp với các cảm biến:

DHT22: đo nhiệt độ & độ ẩm

MQ-2: phát hiện khí gas & khói

Flame sensor: phát hiện lửa

Buzzer: cảnh báo tại chỗ

Kết quả được gửi real-time lên Adafruit IO để theo dõi qua dashboard.

⚡ Sơ đồ kết nối phần cứng
Thiết bị	ESP32s 	
   ----------------
   3.3V  -------- DHT22 VCC
   GND   -------- DHT22 GND
   GPIO14-------- DHT22 DATA (10kΩ lên 3.3V)

   5V    -------- MQ-2 VCC
   GND   -------- MQ-2 GND
   GPIO34-------- MQ-2 AO

   5V    -------- Flame VCC
   GND   -------- Flame GND
   GPIO33-------- Flame DO

   GPIO25  -----> Buzzer (+)
   GND     -----> Buzzer (-)
📍 Sơ đồ khối:

Buzzer và DHT22 bên trái ESP32

MQ-2 và Flame sensor bên phải ESP32

🖥️ Flow hoạt động (FreeRTOS)

ESP32 Boot → kết nối WiFi

Khởi tạo cảm biến DHT22, MQ2, Flame

Chạy 3 Task song song:

TaskReadSensors: đọc cảm biến mỗi 5s (nhiệt độ, độ ẩm, gas, lửa)

TaskSendCloud: gửi dữ liệu gas & flame mỗi 10s, temp & humi mỗi 30s lên Adafruit IO

TaskKeepAlive: duy trì MQTT connection

📊 Dashboard trên Adafruit IO

Biểu đồ: hiển thị realtime nhiệt độ, độ ẩm, khí gas

Gauge: đo nồng độ khí gas

Cảnh báo: buzzer kêu khi phát hiện lửa hoặc khí gas vượt ngưỡng

🛠️ Cài đặt & Chạy chương trình
1. Yêu cầu

Arduino IDE (>=1.8.19) hoặc PlatformIO

ESP32 board library (esp32 by Espressif Systems)

Thư viện:

WiFi.h (ESP32 built-in)

DHT sensor library by Adafruit

Adafruit MQTT Library

2. Cấu hình WiFi & Adafruit IO

Trong code, sửa lại WiFi SSID/Password và Adafruit IO Key:

const char* ssid     = "Tên_WiFi";
const char* password = "Mật_khẩu_WiFi";

#define AIO_USERNAME    "Tên_tài_khoản_AIO"
#define AIO_KEY         "Key_AIO"

3. Nạp code

Kết nối ESP32 qua cáp USB

Chọn board NodeMCU-32S trong Arduino IDE

Chọn đúng cổng COM

Upload code

4. Theo dõi Serial Monitor

Baudrate: 115200
Kết quả hiển thị:

[Sensors] T=27.4°C  H=65.0%  Gas=827  Flame=0

5. Dashboard

Mở Adafruit IO → Dashboards → Quan sát dữ liệu realtime.

🚨 Chức năng cảnh báo

Gas > 3000 (ngưỡng cài đặt) → buzzer kêu

Phát hiện Flame (DO = LOW) → buzzer kêu ngay lập tức
📷 Hình ảnh thực tế
🔹 Sơ đồ mạch
<img width="675" height="302" alt="{2081089F-8B27-45D0-B1F9-0351A2F91485}" src="https://github.com/user-attachments/assets/a2400fab-024c-4238-9d9d-3fcb4382c2b9" />

🔹 Flowchart
<img width="938" height="692" alt="image" src="https://github.com/user-attachments/assets/f03aceef-e754-48bf-b67c-6b4fa0537345" />

🔹 Dashboard
<img width="1695" height="694" alt="image" src="https://github.com/user-attachments/assets/1929e2a2-646f-43cc-a615-b47880d23fbf" />

<img width="1712" height="710" alt="image" src="https://github.com/user-attachments/assets/71ee887c-8c48-4d41-a32b-b6dfe9b194ed" />

🔹 Serial Monitor
<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/c4945e76-6270-43ed-8bc6-96f3ba0a86df" />
