#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* host = "script.google.com";
const int httpsPort = 443;

String GAS_ID = "AKfycbwDdKcORKrRIa11NfU-9wdLySyz5h3GHsRRQ758zyUB6ui6PPQYyOgtLNCc2IexolBg";

const char* ssid = "Pham Tuan 2";
const char* password = "0944246472";


WiFiClientSecure client;

void setup() {
  Serial.begin(9600);
  delay(500);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  client.setInsecure();
}

void loop() {
  // Đọc dữ liệu từ cổng Serial
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); // Đọc dữ liệu cho đến khi gặp ký tự xuống dòng ('\n')
    
    // Kiểm tra xem dữ liệu bắt đầu bằng ký tự đặc biệt 'S' không
    if (data.startsWith("SE")) {
      String studentID = data;
      Serial.println(studentID);

      // Gửi dữ liệu lên Google Sheets
      sendata(studentID); // Chuyển đổi dữ liệu từ String sang int và gửi lên Google Sheets
      delay(4000);
    }
  }
  
}

void sendata(String id){
  Serial.print("connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/macros/s/" + GAS_ID + "/exec?id=" + id;
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
 Serial.println("OK");
}
