#ifndef _CONVERSION_HPP_
#define _CONVERSION_HPP_

#include <string>

// Funções utilitárias de conversão.
//
// Regras:
// - Em caso de erro de parsing/conversão -> lança ConversionError (core/Errors.hpp).
// - Conversão de double para string SEMPRE com 1 casa decimal.
// - Usa ponto como separador decimal (estilo C/locale neutro).

int toInt(const std::string& text);
double toDouble(const std::string& text);

// Converte int para string (sem firula, só std::to_string simplificado se quiser).
std::string toString(int value);

// Converte double para string, sempre com 1 casa decimal, ex:
//  7      -> "7.0"
//  7.01   -> "7.0"
//  7.05   -> "7.1"
//  10.0   -> "10.0"
std::string toString(double value);

#endif
