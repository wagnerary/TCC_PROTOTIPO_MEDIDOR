/*
 * WiFiConnect.cpp
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 * 
 */

#include "WiFiConnect.h"

// Contrutor, recebe as credencias de acesso.
WiFiConnect::WiFiConnect(char* __ssid, char* __pswd)
{
    this->_SSID = __ssid;
    this->_PSWD = __pswd;
}

// Realiza a conexao com o WiFi.
void WiFiConnect::connect()
{
    if(!this->connected()){
        begin(this->_SSID, this->_PSWD);
    }
}

// Realiza a desconexao com o WiFi.
void WiFiConnect::disconnect()
{
    if(this->connected()){
        disconnect();
    }
}

// Verifica se esta conectado.
bool WiFiConnect::connected()
{
    return (status() == WL_CONNECTED);
}