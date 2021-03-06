#include <Arduino.h>
#include "SPIFFS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

#define CALIBRATION_FILE "/calibrationData"

void setup(void) {
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;

  Serial.begin(115200);
  Serial.println("starting");

  tft.init();

  tft.setRotation(2);
  tft.fillScreen((0xFFFF));

  tft.setCursor(20, 0, 2);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.setTextSize(1);
  tft.println("calibration run");

  // check file system
  if (SPIFFS.begin(true)) {
    Serial.println("formating file system");

    SPIFFS.format();
    SPIFFS.begin();
  }

  Serial.println("OK");

  // check if calibration file exists
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f) {
      if (f.readBytes((char *)calibrationData, 14) == 14)
        calDataOK = 1;
      f.close();
    }
  }
  if (calDataOK) {
    // calibration data valid
    tft.setTouch(calibrationData);
  } else {
    // data not valid. recalibrate
    tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calibrationData, 14);
      f.close();
    }

    for(int i =0;i<5;i++) {
      Serial.printf("%d, %d\n", i, calibrationData[i]);
    }
  }

  tft.fillScreen((0xFFFF));

}
// 0, 415
// 1, 3392
// 2, 345
// 3, 3526
// 4, 2

void loop() {
  uint16_t x, y;
  static uint16_t color;

  if (tft.getTouch(&x, &y)) {

    tft.setCursor(5, 5, 2);
    tft.printf("x: %i     ", x);
    tft.setCursor(5, 20, 2);
    tft.printf("y: %i    ", y);

    // tft.drawPixel(x, y, color);
    tft.drawCircle(x, y, 3, color);
    color += 155;
  }
}



