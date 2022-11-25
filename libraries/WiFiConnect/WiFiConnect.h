/*
 * WiFiConnect.h
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 * 
 *  
 *
 */

#pragma once

#ifndef WIFICONNECT_H
#define WIFICONNECT_H

#include "Arduino.h"
#include "WiFi.h"

class WiFiConnect : public WiFiClass {
  private:
    char* _SSID;
    char* _PSWD;

  public:
    WiFiConnect(char*, char*);
    void  connect(void);
    void  disconnect(void);
    bool  connected(void);

};

#endif