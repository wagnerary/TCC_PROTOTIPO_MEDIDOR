/*
 * Sensor.cpp
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 *
 *  Realiza o gerenciamento do sensor MAX30102.
 *
 */

#include "Sensores.h"

MAX30105 _sensor;

Sensores::Sensores()
{}

// Inicia o sensor.
bool Sensores::begin()
{
  if(_sensor.begin(Wire, I2C_SPEED_FAST) == false){
    return false;
  }

  // Inicializa o sensor e define metricas.
  _sensor.setup();
  _sensor.enableDIETEMPRDY();
  _sensor.setPulseAmplitudeRed(0x0A);
  _sensor.setPulseAmplitudeGreen(0);

  return true;
}

// Verifica se possui alguem proximo ao sensor.
bool Sensores::isLeaning()
{
  // Verifica o valor retornado pelo sensor de IR.
  return (_sensor.getIR() >= IR_APPROXIMATION_VALUE);
}

// Detecta os batimentos cardiacos.
void Sensores::beatDetection()
{
  // Utiliza a variacao do IR para detectar os batimentos.
  long __valueIR  = _sensor.getIR();
  this->_beatDetection = checkForBeat(__valueIR);
}

// Retorna se foi detectado o batimento.
bool Sensores::getBeatDetection()
{
  return this->_beatDetection;
}

// Realiza o calculo para oxigenacao.
void Sensores::readSaturationO2()
{
  if(this->getBeatDetection()) {
    // Recebendo valores de IR e Red do sensor.
    long __valueRed = _sensor.getRed();
    long __valueIR  = _sensor.getIR();

    // Calculando o valor da saturacao;
    float __divider  = __valueIR + __valueRed;
    float __satRatio = __valueIR / __divider;

    // Verifica se trouxe algum valor valido.
    if(!isnan(__satRatio)) {

      // Calculando a razao
      float __tmpValue = 100.0 * (__satRatio);

      if(this->_countSaturation >= SATURATION_ARR_SIZE) {
        float __sumValue = 0;

        // Faz a somatoria das medicoes.
        for(int i = 0; i <= SATURATION_ARR_SIZE; i++) {
          __sumValue += this->_arrSaturationO2[i];
        }

        // Realiza o calculo da media dos valores obtidos.
        float __newValue = float(__sumValue / SATURATION_ARR_SIZE);
        this->setSaturationO2(__newValue);

        this->_countSaturation = 0;
      }

      // Verifica se o valor obtido esta dentro da margem aceitavel.
      float __currentValue = Limiter::limiter(float(__tmpValue + SATURATION_OFFSET), float(SATURATION_MIN_VALUE), float(SATURATION_MAX_VALUE));

      // Controla a diferenca entre os valores, se a diferenca dos valores atual e anterior for muito grande ignora o valor atual medido.
      if((this->_saturationO2 > 0) && !(Limiter::range(float(this->_saturationO2), float(__currentValue), float(10)))) {
        __currentValue = this->_saturationO2;
      }

      this->_arrSaturationO2[this->_countSaturation] = __currentValue;
      this->_countSaturation++;
    }
  }
}

// Define o valor para a variavel (_saturationO2).
void Sensores::setSaturationO2(float __value)
{
  this->_saturationO2 = Limiter::limiter(float(__value), float(SATURATION_MIN_VALUE), float(SATURATION_MAX_VALUE));
}

// Retorna o valor da variavel (_saturationO2).
float Sensores::getSaturationO2()
{
  return this->_saturationO2;
}


// Realiza o calculo de batimento por minuto.
void Sensores::readBeatPerMinute(double __cycleTime)
{
  // Define a margem minima e maxima do bpm
  float __minValue = int(60 / BEATPERMINUTE_MIN_VALUE);
  float __maxValue = int(60 / BEATPERMINUTE_MAX_VALUE);

  // Soma o tempo entre os batimentos.
  _timeBetweenBeats += __cycleTime;

  // Verifica se houve batimento.
  if(this->getBeatDetection()) {
    // Calcula os batimentos por tempo.
    float __tmpValue = float(60 / _timeBetweenBeats);

    // Verifica se o batimento obtido esta dentro de uma margem valida.
    if(__tmpValue > BEATPERMINUTE_MIN_VALUE && __tmpValue < BEATPERMINUTE_MAX_VALUE) {
      // Calcula a media das ultimas N medicoes.
      if(this->_countBeatPerMinute >= BEATPERMINUTE_ARR_SIZE) {
        float __sumValue = 0;

        // Faz a somatoria das medicoes.
        for(int i = 0; i <= BEATPERMINUTE_ARR_SIZE; i++) {
          __sumValue += this->_arrBeatPerMinute[i];
        }

        // Realiza o calculo da media dos valores obtidos.
        float __newValue = __sumValue / BEATPERMINUTE_ARR_SIZE;
        this->setBeatPerMinute(__newValue);

        this->_countBeatPerMinute = 0;
      }

      // Verifica se o valor obtido esta dentro da margem aceitavel.
      float __currentValue = Limiter::limiter(float(__tmpValue + BEATPERMINUTE_OFFSET), float(BEATPERMINUTE_MIN_VALUE), float(BEATPERMINUTE_MAX_VALUE));

      // Controla a diferenca entre os valores, se a diferenca dos valores atual e anterior for muito grande ignora o valor atual medido.
      if((this->_beatPerMinute > 0) && !(Limiter::range(float(this->_beatPerMinute), float(__currentValue), float(50)))) {
        __currentValue = this->_beatPerMinute;
      }

      this->_arrBeatPerMinute[this->_countBeatPerMinute] = __currentValue;
      this->_countBeatPerMinute++;
    }

    // Zera o contador de tempo.
    _timeBetweenBeats = 0;
  }
}

// Define o valor para a variavel (_beatPerMinute).
void Sensores::setBeatPerMinute(float __value)
{
  this->_beatPerMinute = Limiter::limiter(float(__value), float(BEATPERMINUTE_MIN_VALUE), float(BEATPERMINUTE_MAX_VALUE));
}

// Retorna o valor da variavel (_beatPerMinute).
float Sensores::getBeatPerMinute()
{
  return this->_beatPerMinute;
}


// Realiza a leitura da temperatura no sensor.
void Sensores::readTemperature()
{
  // Recebe o valor capturado pelo sensor;
  float __temperature = _sensor.readTemperature();

  // Calcula a media das ultimas N medicoes.
  if(this->_countTemperature >= TEMPERATURE_ARR_SIZE){
    float __sumValue = 0;

    // Faz a somatoria dos valores obtidos.
    for(int i = 0; i <= TEMPERATURE_ARR_SIZE; i++){
      __sumValue += this->_arrTemperature[i];
    }

    // Realiza o calculo da media dos valores obtidos.
    float __newValue = __sumValue / TEMPERATURE_ARR_SIZE;
    this->setTemperature(__newValue);

    this->_countTemperature = 0;
  }

  // Verifica se o valor obtido esta dentro da margem aceitavel.
  float __currentValue = Limiter::limiter(float(__temperature + TEMPERATURE_OFFSET), float(TEMPERATURE_MIN_VALUE), float(TEMPERATURE_MAX_VALUE));

  // Controla a diferenca entre os valores, se a diferenca dos valores atual e anterior for muito grande ignora o valor atual medido.
  if((this->_temperature > 0) && !(Limiter::range(float(this->_temperature), float(__currentValue), float(7.5)))) {
    __currentValue = this->_temperature;
  }

  this->_arrTemperature[this->_countTemperature] = __currentValue;                                
  this->_countTemperature++;
}

// Define o valor para variavel (_temperature).
void Sensores::setTemperature(float __value)
{
  this->_temperature = Limiter::limiter(float(__value), float(TEMPERATURE_MIN_VALUE), float(TEMPERATURE_MAX_VALUE));
}

// Retorna o valor da variavel (_temperature).
float Sensores::getTemperature()
{
  return this->_temperature;
}
