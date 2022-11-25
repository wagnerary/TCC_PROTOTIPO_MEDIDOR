/*
 * MySQLConnect.h
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 * 
 *  Gerencia as conexoes com o SGBD.
 *
 */

#pragma once

#ifndef MYSQLCONNECT_H
#define MYSQLCONNECT_H

#include "Arduino.h"
#include "MySQL_Connection.h"
#include "MySQL_Cursor.h"
#include "WiFiClient.h"

class MySQLConnect
{
  private:
    char*     _userName;
    char*     _userPswd;
    IPAddress _serverAddress;

  public:
    MySQLConnect(IPAddress, char*, char*);
    bool      connect(void);
    void      disconnect(void);
    int       receiveData(int);
    bool      sendData(int, int, int, float);
    bool      disableDevice(int);

};

#endif