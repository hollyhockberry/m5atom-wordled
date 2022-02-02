// Copyright (c) 2022 Inaba
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

#include <string>
#include <M5Atom.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
#include "grove_two_rgb_led_matrix.h"

const char SSID[] = "**********";
const char PSK[] = "+*********";

AsyncWebServer server(80);
GroveTwoRGBLedMatrixClass matrix;

// ret 0xff:blank, 0x55:green, 0x25:yellow
int find(const std::string& str, int* index) {
  const std::string chip[] = { "⬜", "⬛", "🟨", "🟩" };
  const int code[] = { 0xff, 0xff, 0x25, 0x55 };
  for (auto i = 0; i < 4; ++i) {
    if (str.compare(*index, chip[i].length(), chip[i]) == 0) {
      *index += chip[i].length();
      return code[i];
    }
  }
  return -1;
}

uint8_t score[5*6] = {0};

bool find(const std::string& str, int line) {
  if (str.empty()) {
    return false;
  }
  int x = 0;
  for (auto i = 0; i < str.length(); ++x) {
    auto n = find(str, &i);
    if (n < 0) {
      return false;
    }
    score[5 * line + x] = n;
  }
  return true;
}

void showMatrix(int line) {
  if (line <= 0) return;
  static uint8_t buffer[8*8] = {0};
  ::memset(buffer, 0xff, 64 * sizeof(uint8_t));
  for (auto l = 0; l < line; ++l) {
    auto y = 6 - line + l + 1;
    for (auto x = 0; x < 5; ++x) {
      buffer[y * 8 + x + 1] = score[5 * l + x];
    }
  }
  matrix.displayFrames(buffer, 0, true, 1);
}

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);
  Wire.begin(G26, G32);

  ::delay(1000);
  if (matrix.getDeviceVID() != 0x2886) {
    Serial.println("Can not detect led matrix!!!");
    while (1);
  }
  Serial.println("Matrix init success!!!");
  matrix.displayColorBlock(0x000000, 0, true);

  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("connecting");
  }
  if (!MDNS.begin("M5Wordled")) {
    ESP_LOGE("", "MDNS: Failed");
  }
  Serial.println(WiFi.localIP());

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/poi", HTTP_ANY, [](AsyncWebServerRequest *request) {
    if (request->hasArg("text")) {
      std::string str(request->arg("text").c_str());

      auto offset = std::string::size_type(0);
      auto line = 0;
      while (true) {
        auto pos = str.find("\r\n", offset);
        if (pos == std::string::npos) {
          if (find(str.substr(offset), line)) {
            line++;
          }
          break;
        }
        if (find(str.substr(offset, pos - offset), line)) {
          line++;
        }
        offset = pos + 2;
      }
      showMatrix(line);
    }
    request->send(200, String(), "Enjoy!");
  });
  server.begin();
}

void loop() {
}
