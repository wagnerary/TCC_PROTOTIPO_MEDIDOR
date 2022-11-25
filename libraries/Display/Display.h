/*
 * Display.h
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 *
 */

#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Arduino.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

class Display : public Adafruit_SSD1306
{
  private:
    int   _deviceID;
    void  showDeviceID(void);

  public:
    Display(int);
    bool  begin(void);
    void  showLoading(void);
    void  showErrorMessage(String);
    void  showDataSensor(int, int, float);

};

#endif
