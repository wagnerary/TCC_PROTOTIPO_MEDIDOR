/*
 * Timer.h
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 *  
 *  Realiza o controle do fluxo de tempo.
 *
 */

#pragma once

#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

#define ADJUST_TIME_VALUE 1000000.0

class Timer
{
  private:
    static  double  _delta_time;
    static  double  _current_time;

  public:
    Timer(void);
    static  double  time();
    static  void    update(void);
    static  double  deltaTime(void);
    static  double  elapsedTime(void);

};

#endif
