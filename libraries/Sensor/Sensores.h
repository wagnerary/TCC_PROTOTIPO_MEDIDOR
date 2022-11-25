/*
 * Sensor.h
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 *
 *  Gerencia o sensor.
 *
 */

#pragma once

#ifndef SENSORES_H
#define SENSORES_H

#include "Arduino.h"
#include "math.h"
#include "MAX30105.h"
#include "heartRate.h"
#include "Limiter.h"

#define IR_APPROXIMATION_VALUE  100000  // Valor minimo para o sensor detectar presenca.

#define SATURATION_OFFSET       13      // Valor de ajuste para a saturacao
#define SATURATION_MIN_VALUE    93      // Valor minimo para saturacao
#define SATURATION_MAX_VALUE    99      // Valor maximo para saturacao
#define SATURATION_ARR_SIZE     10      // Quantidade de elementos do array

#define BEATPERMINUTE_OFFSET    0       // Valor de ajuste para batimentos
#define BEATPERMINUTE_MIN_VALUE 40      // Valor minimo para batimentos
#define BEATPERMINUTE_MAX_VALUE 250     // Valor maximo para batimentos
#define BEATPERMINUTE_ARR_SIZE  10      // Quantidade de elementos do array

#define TEMPERATURE_OFFSET      2       // Valor de ajuste para temperatura
#define TEMPERATURE_MIN_VALUE   30      // Valor minimo para temperatura
#define TEMPERATURE_MAX_VALUE   45      // Valor maximo para temperatura
#define TEMPERATURE_ARR_SIZE    10      // Quantidade de elementos do array

class Sensores
{
  private:
    static const int spO2LUT[43];
    bool  _beatDetection = false;

    int   _saturationO2;
    int   _arrSaturationO2[SATURATION_ARR_SIZE];
    int   _countSaturation;

    int   _beatPerMinute;
    int   _arrBeatPerMinute[BEATPERMINUTE_ARR_SIZE];
    int   _countBeatPerMinute;
    float _timeBetweenBeats;

    float _temperature;
    float _arrTemperature[TEMPERATURE_ARR_SIZE];
    int   _countTemperature;

    void  setSaturationO2(int);
    void  setBeatPerMinute(int);
    void  setTemperature(float);

  public:
    Sensores(void);
    bool  begin(void);
    bool  isLeaning(void);
    void  beatDetection(void);
    bool  getBeatDetection(void);
    void  readSaturationO2(void);
    int   getSaturationO2(void);
    void  readBeatPerMinute(double);
    int   getBeatPerMinute(void);
    void  readTemperature(void);
    float getTemperature(void);

};

#endif
