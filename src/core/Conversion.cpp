#include "Conversion.hpp"
#include "Errors.hpp"
#include "Uteis.hpp"

#include <sstream>
#include <locale>
#include <algorithm>
#include <cctype>

int toInt(const std::string& text) {
    const std::string s = trim(text);
    if (s.empty()) {
        throw ConversionError("Erro de conversao para inteiro: valor vazio");
    }

    std::istringstream iss(s);
    iss.imbue(std::locale::classic());

    int value;
    if (!(iss >> value)) {
        throw ConversionError("Erro de conversao para inteiro: '" + text + "'");
    }

    // Verifica lixo após o número
    iss >> std::ws;
    if (!iss.eof()) {
        throw ConversionError("Erro de conversao para inteiro (caracteres extras): '" + text + "'");
    }

    return value;
}

double toDouble(const std::string& text) {
    const std::string s = trim(text);
    if (s.empty()) {
        throw ConversionError("Erro de conversao para real: valor vazio");
    }

    std::istringstream iss(s);
    iss.imbue(std::locale::classic());

    double value;
    if (!(iss >> value)) {
        throw ConversionError("Erro de conversao para real: '" + text + "'");
    }

    // Verifica lixo após o número
    iss >> std::ws;
    if (!iss.eof()) {
        throw ConversionError("Erro de conversao para real (caracteres extras): '" + text + "'");
    }

    return value;
}

std::string toString(int value) {
    return std::to_string(value);
}

std::string toString(double value) {
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss.setf(std::ios::fixed);
    oss.precision(1);
    oss << value;
    return oss.str();
}
