#ifndef _ERRORS_HPP_
#define _ERRORS_HPP_

#include <stdexcept>
#include <string>

// Erro de negócio (regra violada, dado inválido, etc.)
// Pode ser mostrado para o usuário de forma direta.
class BusinessError : public std::runtime_error {
    public:
        explicit BusinessError(const std::string& message)
            : std::runtime_error(message) {}
};

// Erro de conversão (string <-> número, parsing de campos, etc.)
// Geralmente indica problema de entrada ou de arquivo mal formatado.
class ConversionError : public std::runtime_error {
    public:
        explicit ConversionError(const std::string& message)
            : std::runtime_error(message) {}
};

// Erro de infraestrutura (arquivo, banco, IO, libs externas, etc.)
// Não deve expor detalhes crus diretamente ao usuário final.
class InfraError : public std::runtime_error {
    public:
        explicit InfraError(const std::string& message)
            : std::runtime_error(message) {}
};

#endif