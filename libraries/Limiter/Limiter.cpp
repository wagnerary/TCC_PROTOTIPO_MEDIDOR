/*
 * Limiter.cpp
 * 
 *  Autor: Wagner Ary Mota
 *  Setembro de 2022
 * 
 *  Define limites minimos e maximos para os valores informados.
 *
 */

#include "Limiter.h"

Limiter::Limiter()
{
}

// Define o limite minimo e maximo para valores (inteiros).
int Limiter::limiter(int _value, int _min_value, int _max_value)
{
    int __new_value = _value;

    if(_value < _min_value){
        __new_value = _min_value;
    }
    else if(_value > _max_value){
        __new_value = _max_value;
    }

    return __new_value;
}

// Define o limite minimo e maximo para valores (flutuantes).
float Limiter::limiter(float _value, float _min_value, float _max_value)
{
    float __new_value = _value;

    if(_value < _min_value){
        __new_value = _min_value;
    }
    else if(_value > _max_value){
        __new_value = _max_value;
    }

    return __new_value;
}

// Define um limite de diferenca entre dois valores (inteiros).
bool Limiter::range(int _previous_value, int _current_value, int _range_value)
{
    if(int(abs(_previous_value - _current_value)) <= _range_value) {
        return true;
    }

    return false;
}

// Define um limite de diferenca entre dois valores (flutuantes).
bool Limiter::range(float _previous_value, float _current_value, float _range_value)
{
    if(float(abs(_previous_value - _current_value)) <= _range_value) {
        return true;
    }
    
    return false;
}