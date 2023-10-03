#ifndef MAIN_H
#define MAIN_H

typedef enum{
  COMENTARIO,
  ERRO,
  IDENTIFICADOR,
  VARIAVEL,
  NUMERO,
  INTEIRO,
  LOGICO,
  INICIO,
  FIM,
  SE,
  ENTAO,
  SENAO,
  ENQUANTO,
  FACA,
  LEIA,
  ESCREVA,
  OP_SOMA,
  OP_MULT,
  OP_SUB,
  DIV,
  E,
  OU,
  ALGORITMO,
  PONTO_VIRGULA,
  PONTO,
  VIRGULA,
  DOIS_PONTOS,
  ATRIBUICAO,
  MENOR,
  MENOR_IGUAL,
  IGUAL,
  DIFERENTE,
  MAIOR,
  MAIOR_IGUAL,
  VERDADEIRO,
  FALSO,
  ABRE_PARENTESES,
  FECHA_PARENTESES,
  EOS
} TAtomo;

typedef struct {
  TAtomo atomo;
  int linha;
  //float atributo_numero;
  char atributo_ID[100];
} TInfoAtomo;

//###########################
// FUNCOES ANALISADOR LEXICO
//###########################

TInfoAtomo obter_atomo();
TInfoAtomo reconhece_numero();
TInfoAtomo reconhece_id();
TInfoAtomo reconhece_comentario();

//##############################
// FUNCOES ANALISADOR SINTATICO
//##############################

void programa(); // <programa>::= algoritmo identificador “;” <bloco> “.”
void bloco(); // <bloco>::= [ <declaracao_de_variaveis> ] <comando_composto>
void declaracao_variavel(); // <declaracao_de_variaveis> ::= variavel {<lista_variavel> “:” <tipo> “;”}+
void lista_variavel(); // <lista_variavel> ::= identificador { “,” identificador }
void tipo(); // <tipo> ::= inteiro | logico
void comando_composto(); // <comando_composto> ::= inicio <comando> { “;” <comando>} fim
void comando(); // <comando> ::= <comando_atribuicao> | <comando_se> | <comando_enquanto> | <comando_entrada> | <comando_saida> | <comando_composto>
void comando_atribuicao(); // <comando_atribuicao> ::= identificador “:=” <expressao>
void comando_se(); // <comando_se> ::= se “(” <expressao> “)” entao <comando> [senao <comando>]
void comando_enquanto(); // <comando_enquanto> ::= enquanto “(” <expressao> “)” faca <comando>
void comando_entrada(); // <comando_entrada> ::= leia “(“ <lista_variavel> “)”
void comando_saida(); // <comando_saida> ::= escreva “(“ <expressao> { “,” <expressao> } “)”
void expressao(); // <expressao> ::= <expressao_simples> [<relacional> <expressao_simples>]
void relacional(); // <relacional> ::= “<” | “<=” | “=” | “#” | “>” | “>=”
void expressao_simples(); // <expressao_simples> ::= [“+” | “−”] <termo> { (“+” | “−” | ou ) <termo> }
void termo(); // <termo> ::= <fator> { ( “*” | div | e )<fator> }
void fator(); // <fator> ::= identificador | numero | verdadeiro | falso | “(” <expressao> “)”
void consome(TAtomo atomo);

#endif
