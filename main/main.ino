#define TCAADDR 0x70

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <math.h>

#include "IMU.h"

IMU g_imu(0x68);
IMU g_imu2(0x68);

static const char g_SSID[] = "MoCap";
static const char g_pass[] = "";
ESP8266WebServer g_webServer(80);
WebSocketsServer g_webSocket(81);

bool i2cReadData(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *data, uint8_t length)
{
    uint8_t bytesRead = 0;
    
    Wire.beginTransmission(deviceAddress); 
    Wire.write(registerAddress);
    Wire.endTransmission();
      
    Wire.beginTransmission(deviceAddress);
    Wire.requestFrom(deviceAddress, length);
    while (Wire.available())
        data[bytesRead++] = Wire.read();
    Wire.endTransmission();

    return bytesRead == length;
}

void i2cSendCmd(uint8_t deviceAddress, uint8_t registerAddress, uint8_t opCode)
{
    Wire.beginTransmission(deviceAddress); 
    Wire.write(registerAddress);
    Wire.write(opCode);
    Wire.endTransmission();
}

void webServerRoot()
{
    g_webServer.sendHeader("Location", "/index.html", true);
    g_webServer.send(302, "text/plane", "");
}

void webServerNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += g_webServer.uri();
    message += "\nMethod: ";
    message += (g_webServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += g_webServer.args();
    message += "\n";
    for (uint8_t i = 0; i < g_webServer.args(); ++i)
        message += " " + g_webServer.argName(i) + ": " + g_webServer.arg(i) + "\n";
    g_webServer.send(404, "text/plain", message);
}

void tcaselect(uint8_t i) {
 //if (i > 7) return;
 
 Wire.beginTransmission(TCAADDR);
 Wire.write(1 << i);
 Wire.endTransmission();
}

void setup() {
  // put your setup code here, to run once:
  delay(1000);

    Serial.begin(115200);
    Serial.println();
    Serial.println("=== SciVi ES v0.1 @ ESP8266 ===");
    Serial.println();

    WiFi.softAP(g_SSID, g_pass);

    Serial.println("[SETUP] access point is up");
    Serial.print("[SETUP] SSID: ");
    Serial.println(g_SSID);
    Serial.print("[SETUP] IP: ");
    Serial.println(WiFi.softAPIP());

    //g_webSocket.onEvent(webSocketEvent);
    g_webSocket.begin();   
    Serial.println("[SETUP] WebSocket server started");

    g_webServer.onNotFound(webServerNotFound);
    g_webServer.on("/", webServerRoot);
    g_webServer.begin();

    Serial.println("[SETUP] Web server started");

    Wire.begin(4, 5);

    delay(100);

    tcaselect(2);
    bool imuReady = g_imu.init();
    if (imuReady)
        Serial.println("IMU status: ok");
    else
        Serial.println("IMU status: failed");

    delay(1000);

    tcaselect(7);
    bool imu2Ready = g_imu2.init();
    Serial.println(imu2Ready);
}


int iterationCounter = 0;
int x, y, z;
void loop() {
  // put your main code here, to run repeatedly:
  tcaselect(2);
  Quat cur = g_imu.read();
  tcaselect(7);
  Quat cur2 = g_imu2.read();
  String q = String("\"q1\": [") +
               String(cur.coords[3]) + String(", ") +
               String(cur.coords[0]) + String(", ") +
               String(cur.coords[1]) + String(", ") +
               String(cur.coords[2]) +
               String("],")
               + 
               String("\"q2\": [") +
               String(cur2.coords[3]) + String(", ") +
               String(cur2.coords[0]) + String(", ") +
               String(cur2.coords[1]) + String(", ") +
               String(cur2.coords[2]) +
               String("]");
  delay(1);

  if(iterationCounter == 0)
  {
    x = cur.coords[0];
    y=cur.coords[1];
    z=cur.coords[2];
    iterationCounter++;
  }
  else{
    float angle = acos(x*cur.coords[0]+y*cur.coords[1]+z*cur.coords[2]);
    Serial.println(180/3.14159265358979 * angle * 0.5); //код для вычисления ошибки отклонения
  }
               

    String msg = String("{ ") + q + String(" }");
    g_webSocket.broadcastTXT(msg);

    g_webSocket.loop();
    g_webServer.handleClient();

    delay(16);
}
