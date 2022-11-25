/*
 * Limiter.h
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 * 
 *  Gerenciamento e Controle de valores numericos.
 *
 */

#pragma once

#ifndef LIMITER_H
#define LIMITER_H

#include "Arduino.h"

class Limiter
{
  public:
    Limiter(void);
    static  int   limiter(int, int, int);
    static  float limiter(float, float, float);
    static  bool  range(int, int, int);
    static  bool  range(float, float, float);

};

#endif