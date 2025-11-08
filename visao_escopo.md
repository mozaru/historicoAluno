# Sistema de Histórico Acadêmico – Visão e Escopo

## 1. Visão Geral

O Sistema de Histórico Acadêmico tem como objetivo gerenciar, de forma simples e confiável, as disciplinas cursadas por um estudante, permitindo o acompanhamento do desempenho ao longo do tempo.

O foco é fornecer uma experiência clara para consulta e manutenção do histórico, garantindo integridade das informações acadêmicas e cálculo consistente do coeficiente de rendimento (CR).

## 2. Objetivos de Negócio

- Registrar as disciplinas cursadas por um estudante com seus dados essenciais.
- Permitir a manutenção do histórico (inserir, editar e remover disciplinas).
- Calcular e exibir o coeficiente de rendimento (CR) de forma transparente.
- Apoiar decisões acadêmicas (acompanhamento de desempenho, identificação de problemas).
- Servir como base didática para ensinar organização, regras de negócio e validação de dados.

## 3. Escopo Funcional

### 3.1. Cadastro de Disciplinas

Cada disciplina registrada no histórico contém:

- Matrícula da disciplina
- Nome da disciplina
- Ano
- Semestre
- Créditos
- Nota 1
- Nota 2

### 3.2. Operações sobre o Histórico

Funcionalidades previstas:

- Listar todas as disciplinas do histórico do estudante.
- Exibir, junto com a listagem, o coeficiente de rendimento (CR) atualizado.
- Filtrar disciplinas (por ano, semestre, matrícula ou outros critérios definidos na interface).
- Inserir nova disciplina no histórico.
- Alterar uma disciplina existente.
- Remover uma disciplina existente.

### 3.3. Regras de Negócio Essenciais

- Não permitir disciplinas duplicadas para o mesmo estudante com a mesma combinação **(matrícula, ano, semestre)**.
- Ano deve estar entre **2000** e o ano corrente.
- Semestre deve ser **1** ou **2**.
- Créditos devem estar entre **1** e **20**.
- Nome da disciplina deve ter entre **3** e **30** caracteres.
- Notas devem estar no intervalo **0.0 a 10.0**.
- O CR deve ser calculado com base nas disciplinas válidas cadastradas (detalhamento técnico na documentação de arquitetura).

Mensagens de erro adequadas devem ser apresentadas em caso de violação das regras.

## 4. Público-Alvo

- Estudantes que desejam acompanhar seu histórico acadêmico.
- Professores e instrutores utilizando o sistema como apoio didático.
- Equipes acadêmicas interessadas em um modelo simples e estruturado de controle de histórico.

## 5. Itens Fora de Escopo

Não fazem parte deste projeto:

- Integração com sistemas acadêmicos oficiais.
- Autenticação, multiusuário e controle de permissões.
- Emissão de documentos oficiais (histórico escolar formal, declarações).
- Integrações externas (APIs de instituições, portais, etc.).
- Funcionalidades avançadas de análise acadêmica (projeções, alertas complexos, etc.).

Este sistema é intencionalmente focado em um escopo reduzido e bem definido, com ênfase em consistência das regras de negócio e clareza do modelo.
