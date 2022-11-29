// ===== Bibliotecas =================================================== //
#include "stdio.h"
#include "SPI.h"
#include "Wire.h"

#include "Timer.h"
#include "Display.h"
#include "Sensores.h"
#include "WiFiConnect.h"
#include "MySQLConnect.h"

// ===== Variáveis ===================================================== //

// Identificador do dispositivo.
int   _devideID         = 99;             // ID do dispositivo
int   _patientID        = 0;              // ID do paciente
bool  _activeDevice     = true;           // Estado do dispositivo

// Credenciais de conexao com Wi-Fi.
char* _wifiSSID         = "Mastodonte";   // SSID de acesso Wi-Fi
char* _wifiPSWD         = "1234567890";   // Senha de acesso

// Credenciais de conexao com sgbd.
char* _sgbdUser         = "root";         // Usuario de conexao SGBD
char* _sgbdPswd         = "root";         // Senha de conexao SGBD
int   _maxErrorValue    = 5;              // Quantidade maxima de erros
int   _sendErrorCount   = 0;              // Contador de erros de envio

// Metricas de tempo
float _timeToWait       = 60;             // Tempo de espera para calibragem do sensor (em segundos)
float _curTimeWait      = 0;              // Contador de espera para calibragem
float _timeToResetWait  = 0;              // Contador para resetar o contador de _curTimeWait
float _timeToRefresh    = 2;              // Tempo de atualizacao da tela (em segundos)
float _curTimeRefresh   = 0;              // Contador de espera para atualizacao de tela
float _timeToSendData   = 15;             // Tempo de envio de dados para SGBD (em segundos)
float _curTimeSendData  = 0;              // Contador de espera para envio de dados
float _timeToInactive   = 600;            // Tempo para desativar o paciente (em segundos)
float _curTimeInactive  = 0;              // Contador de espera para desativar

// Dados de medicao.
int   _saturationO2     = 0;              // Valor da saturacao
int   _beatPerMinute    = 0;              // Valor do batimento cardiaco
float _temperature      = 0;              // Valor da temperatura

// Mensagens de erro do display.
String msgErrorActive   = String("Aviso: O Dispositivo foi desativado");
String msgErrorSensor   = String("Falha: O dispositivo do Sensor nao foi    inicializado");
String msgErrorWiFi     = String("Falha: A rede Wi-Fi  nao esta disponivel");
String msgErrorSGBD     = String("Falha: Nao conectado ao banco de dados");
String msgErrorSend     = String("Falha: Dados não enviados");
String msgErrorPatient  = String("Falha: Paciente nao  localizado no        sistema");

// Endereço do servidor SGBD.
IPAddress serverAddr(192, 168, 137, 1);   // IP do servidor SGBD

// ===== Instancias ==================================================== //

Display screen(_devideID);
Sensores sensor;

WiFiConnect wfConn(_wifiSSID, _wifiPSWD);
MySQLConnect sqlConn(serverAddr, _sgbdUser, _sgbdPswd);

// ===================================================================== //

void setup()
{
  Serial.begin(115200);
  
  // Portas de uso dos shields.
  Wire.begin(SDA, SCL);
  
  // Led(onboard) do arduino.
  pinMode(LED_BUILTIN, OUTPUT);

  // Inicializa a tela.
  startScreen();

  // Inicializa o sensor.
  startSensor();

  // Conectando ao WiFi.
  connectWiFi();

  // ID do paciente.
  getPatientID();
}

// ===================================================================== //

void loop()
{
  // Verifica se o dispositivo esta ativo.
  if(_activeDevice) {

    // Controle do fluxo de tempo entre ciclos.
    Timer::update();
    //=========================================

    // Verifica se a pulseira esta em contato.
    if(sensor.isLeaning()) {
      // Detecta os batimentos cardiacos.
      sensor.beatDetection();
      
      // Processa os dados recebidos do sensor.
      sensor.readSaturationO2();
      sensor.readBeatPerMinute(Timer::deltaTime());
      sensor.readTemperature();
  
      // Pega os valores obtidos.
      _saturationO2  = int(sensor.getSaturationO2());
      _beatPerMinute = int(sensor.getBeatPerMinute());
      _temperature   = float(sensor.getTemperature());
  
      // Aguarda o sensor calibrar para enviar os dados.
      if(_curTimeWait >= _timeToWait) {
        // Envia os dados para o sgbd.
        if(_curTimeSendData >= _timeToSendData) {
          // Verifica se está conectado ao wifi.
          if(wfConn.connected()) {
            // Tenta enviar os dados.
            if(!sqlConn.sendData(_patientID, _saturationO2, _beatPerMinute, _temperature)) {
              _sendErrorCount++;
              // Se ocorrer erro n vezes consecutivas exibe uma mensagem.
              if(_sendErrorCount == _maxErrorValue) {
                Serial.println(msgErrorSend);
                screen.showErrorMessage(msgErrorSend);
                deviceError();
              }
            }
            else {
              _sendErrorCount = 0;
            }
          }
          else {
            connectWiFi();
          }
          _curTimeSendData = 0;
        }
        // Contado para envio dos dados.
        _curTimeSendData += Timer::deltaTime();
      }
      else {
        // Contador para iniciar os envios.
        _curTimeWait += Timer::deltaTime();
      }
      
      _curTimeInactive = 0;
    }
    else {
      // Exibe o valor 0 caso não esteja em contato.
      _saturationO2  = 0;
      _beatPerMinute = 0;
      _temperature   = 0;
  
      // Reinicia o contador de calibragem, se ficar mais de 10 segundos sem contato.
      if(_timeToResetWait >= 10) {
        _curTimeWait = 0;
      }
      else {
        _timeToResetWait += Timer::deltaTime();
      }

      // Desativa o dispositivo.
      if(_curTimeInactive >= _timeToInactive) {
        deviceDisable();
      }
      else {
        _curTimeInactive += Timer::deltaTime();
      }
    }

    // Atualiza os dados do display.
    if(_curTimeRefresh >= _timeToRefresh) {
      screen.showDataSensor(_saturationO2, _beatPerMinute, _temperature);
      _curTimeRefresh = 0;
    }
    _curTimeRefresh += Timer::deltaTime();
 
  }
}

// ===================================================================== //

// Inicializa a tela.
void startScreen()
{
  if(!screen.begin()) {
    Serial.println("Não foi possivel iniciar a tela.");
    deviceError();
  }

  // Mensagem de carregamento.
  screen.showLoading();
  delay(1000);
}

// Inicializa o sensor.
void startSensor()
{
  if(!sensor.begin()) {
    Serial.println(msgErrorSensor);
    screen.showErrorMessage(msgErrorSensor);
    deviceError();
  }
}

// Conecta no WiFi.
void connectWiFi()
{
  // Tenta conectar ao WiFi
  wfConn.connect();
  delay(1000);
  
  // Verifica se esta conectado.
  for(int i = 0; (!wfConn.connected()); i++) {
    // Realiza 10 tentativas de conexao.
    if(i >= 10) {
      // Exibe uma mensagem de erro de conexao.
      Serial.println(msgErrorWiFi);
      screen.showErrorMessage(msgErrorWiFi);
      deviceError();
    }
    delay(300);
  }
}

// Obtendo o ID do paciente.
void getPatientID()
{
  if(wfConn.connected()) {
    _patientID = sqlConn.receiveData(_devideID);
    delay(1000);

    // Verifica se pegou o ID do paciente.
    if(_patientID <= 0) {
      Serial.println(msgErrorPatient);
      screen.showErrorMessage(msgErrorPatient);
      deviceError();
    }
  }
}

// Desativa o dispositivo.
void deviceDisable()
{
  _activeDevice = false;
  Serial.println(msgErrorActive);
  screen.showErrorMessage(msgErrorActive);
  
  for(int i = 0; (!sqlConn.disableDevice(_devideID)); i++) {
    if(i >= 5) {
      break;
    }
    delay(500);
  }

  wfConn.disconnect();
  deviceError();
}


// Gera um alerta visual em caso de falhas.
void deviceError()
{
  while(true) {
    digitalWrite(LED_BUILTIN, HIGH);  
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
  }
}
