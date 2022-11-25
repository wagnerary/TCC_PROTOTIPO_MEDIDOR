/*
 * Timer.cpp
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 *  
 *  Realiza o controle do fluxo de tempo.
 *
 */

#include "Timer.h"

double Timer::_delta_time;
double Timer::_current_time;

Timer::Timer()
{}

// Exibe o tempo corrente em microssegundos.
double Timer::time()
{
  return micros();
}

// Exibe o tempo corrente em segundos.
double Timer::elapsedTime()
{
  return (time() / ADJUST_TIME_VALUE);
}

// Atualiza o contador do deltaTime entre os ciclos.
void Timer::update()
{
  double __old_time = Timer::_current_time;
  Timer::_current_time = time();
  Timer::_delta_time = (Timer::_current_time - __old_time) / ADJUST_TIME_VALUE;
}

// Exibe a diferenca de tempo entre os ciclos.
double Timer::deltaTime()
{
  return Timer::_delta_time;
}
