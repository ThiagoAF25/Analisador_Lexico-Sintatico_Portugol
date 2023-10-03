/*
Compilador fase 1: Análise léxica e sintática

Nome Completo               -       TIA
Leonardo Pinheiro de Souza  -       32127391
Thiago Aidar Figueiredo     -       32144547
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "main.h"


char *buffer; //ponteiro que recebe o arquivo de texto
int contaLinha = 1; //indica a linha que o buffer esta
//Funcao de leitura do arquivo.txt

void leitura(char* arq){
  FILE *fptr = fopen(arq , "r");
  if (fptr == NULL)  {
        printf("Erro no arquivo");
        exit(1);
  }
  fseek(fptr, 0, SEEK_END);
  long size = ftell(fptr); //retorna tamanho do arquivo de texto
  rewind(fptr); //volta para o inicio do arquivo de texto
  char *inputs = (char*) calloc (size+1, sizeof(char));
  fread(inputs,1,size,fptr);
  buffer = inputs;
  fclose(fptr);
}

char strAtomo[][30] = {"COMENTARIO","Erro Lexico", "IDENTIFICADOR" , "VARIAVEL", "NUMERO", "INTEIRO", "LOGICO","INICIO", "FIM","SE","ENTAO","SENAO",
                       "ENQUANTO","FACA","LEIA","ESCREVA", "OP_SOMA", "OP_MULT", "OP_SUB", "DIV", "E", "OU", "ALGORITMO", ";", ".", ",", ":", ":=",
                       "<", "<=", "=", "#", ">", ">=", "VERDADEIRO", "FALSO", "(", ")", "Fim Analise Sintatica"};

TInfoAtomo InfoAtomo;
TAtomo lookahead;

int main() {
  leitura("portugol.txt");
  printf("Analise do arquivo:");
  InfoAtomo = obter_atomo();
  lookahead = InfoAtomo.atomo;
  programa();
  consome(EOS); //consome '\x0' para verificar se tudo foi lido
  printf("\n%d linhas analisadas, programa sintaticamente correto", contaLinha);

  return 0;
}

//####################
// ANALISADOR LEXICO
//####################

TInfoAtomo obter_atomo(){
    TInfoAtomo infoAtomo;
    infoAtomo.linha = -1;
    while (*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r') {
        if (*buffer == '\n')contaLinha++;
        buffer++;
    }

    if (isdigit(*buffer)){
        infoAtomo = reconhece_numero();
    }
    else if (isalpha(*buffer)){
        infoAtomo = reconhece_id();
    }
    else if (*buffer == '+') {
        infoAtomo.atomo = OP_SOMA;
        buffer++;
    }
    else if (*buffer == '*') {
        infoAtomo.atomo = OP_MULT;
        buffer++;
    }
    else if (*buffer == '-') {
        infoAtomo.atomo = OP_SUB;
        buffer++;
    }
    else if (*buffer == ';') {
        infoAtomo.atomo = PONTO_VIRGULA;
        buffer++;
    }
    else if (*buffer == '.') {
        infoAtomo.atomo = PONTO;
        buffer++;
    }
    else if (*buffer == ',') {
        infoAtomo.atomo = VIRGULA;
        buffer++;
    }
    else if (*buffer == ':') {
        buffer++;
        if (*buffer == '=') {
            infoAtomo.atomo = ATRIBUICAO;
            buffer++;
        }
        else infoAtomo.atomo = DOIS_PONTOS;
    }
    else if (*buffer == '<') {
        buffer++;
        if (*buffer == '=') {
            infoAtomo.atomo = MENOR_IGUAL;
            buffer++;
        }
        else infoAtomo.atomo = MENOR;
    }
    else if (*buffer == '>') {
        buffer++;
        if (*buffer == '=') {
            infoAtomo.atomo = MAIOR_IGUAL;
        buffer++;
        }
        else infoAtomo.atomo = MAIOR;
        }
    else if (*buffer == '=') {
        infoAtomo.atomo = IGUAL;
        buffer++;
    }
    else if (*buffer == '#') {
        infoAtomo.atomo = DIFERENTE;
        buffer++;
    }
    else if (*buffer == '(') {
        infoAtomo.atomo = ABRE_PARENTESES;
        buffer++;
    }
    else if (*buffer == ')') {
        infoAtomo.atomo = FECHA_PARENTESES;
        buffer++;
    }
    else if(*buffer == '/'){
        infoAtomo = reconhece_comentario();
    }
    else if (*buffer == '\x0')
        infoAtomo.atomo = EOS;
    else
        infoAtomo.atomo = ERRO;
    if (infoAtomo.linha == -1) infoAtomo.linha = contaLinha;
    return infoAtomo;
}

TInfoAtomo reconhece_numero(){
    char *pIniNum = buffer;
    TInfoAtomo infoAtomo;
    infoAtomo.linha = -1;
    infoAtomo.atomo = ERRO;

    if(isdigit(*buffer)){
        buffer++;
        while(isdigit(*buffer)){
            buffer++;
        }
        if(*buffer=='E'||*buffer == 'e'){ //se identificar exponencial
            buffer++;
            if (*buffer=='+'||*buffer == '-') buffer++;
            if(isdigit(*buffer)){ //leitura obrigatoria de ao menos 1 digito
                while(isdigit(*buffer)){
                    buffer++;
                }
            }
            else return infoAtomo;
        }
    }else return infoAtomo;
    if (buffer-pIniNum > 15) return infoAtomo;
    strncpy(infoAtomo.atributo_ID, pIniNum, buffer-pIniNum);
    infoAtomo.atributo_ID[buffer-pIniNum] = '\x0';

    infoAtomo.atomo = NUMERO;
    return infoAtomo;
}

TInfoAtomo reconhece_id() {
    char *pIniID = buffer;
    TInfoAtomo infoAtomo;
    infoAtomo.linha = -1;
    infoAtomo.atomo = ERRO;

    if(isalpha(*buffer)) { //leitura obrigatoria da primeira letra
        buffer++;
        while(isalpha(*buffer) || isdigit(*buffer) || *buffer == '_'){
            buffer++;
        }
    }
    else return infoAtomo;
    //verifica se o tamanho do identificador [e menor ou igual a 15
    if (buffer-pIniID > 15) return infoAtomo;
    strncpy(infoAtomo.atributo_ID, pIniID, buffer-pIniID);
    infoAtomo.atributo_ID[buffer-pIniID] = '\x0';
    /* if-else if-else para identificar tipo de enum de acordo
     com o que foi armazenado em infoAtomo.atributo_ID*/
    if(strcmp(infoAtomo.atributo_ID, "algoritmo") == 0)
        infoAtomo.atomo = ALGORITMO;
    else if (strcmp(infoAtomo.atributo_ID, "variavel") == 0)
        infoAtomo.atomo = VARIAVEL;
    else if (strcmp(infoAtomo.atributo_ID, "verdadeiro") == 0)
        infoAtomo.atomo = VERDADEIRO;
    else if (strcmp(infoAtomo.atributo_ID, "falso") == 0)
        infoAtomo.atomo = FALSO;
    else if (strcmp(infoAtomo.atributo_ID, "inteiro") == 0)
        infoAtomo.atomo = INTEIRO;
    else if (strcmp(infoAtomo.atributo_ID, "logico") == 0)
        infoAtomo.atomo = LOGICO;
    else if (strcmp(infoAtomo.atributo_ID, "inicio") == 0)
        infoAtomo.atomo = INICIO;
    else if (strcmp(infoAtomo.atributo_ID, "fim") == 0)
        infoAtomo.atomo = FIM;
    else if (strcmp(infoAtomo.atributo_ID, "se") == 0)
        infoAtomo.atomo = SE;
    else if (strcmp(infoAtomo.atributo_ID, "senao") == 0)
        infoAtomo.atomo = SENAO;
    else if (strcmp(infoAtomo.atributo_ID, "entao") == 0)
        infoAtomo.atomo = ENTAO;
    else if (strcmp(infoAtomo.atributo_ID, "enquanto") == 0)
        infoAtomo.atomo = ENQUANTO;
    else if (strcmp(infoAtomo.atributo_ID, "faca") == 0)
        infoAtomo.atomo = FACA;
    else if (strcmp(infoAtomo.atributo_ID, "leia") == 0)
        infoAtomo.atomo = LEIA;
    else if (strcmp(infoAtomo.atributo_ID, "escreva") == 0)
        infoAtomo.atomo = ESCREVA;
    else if (strcmp(infoAtomo.atributo_ID, "div") == 0)
        infoAtomo.atomo = DIV;
    else if (strcmp(infoAtomo.atributo_ID, "e") == 0)
        infoAtomo.atomo = E;
    else if (strcmp(infoAtomo.atributo_ID, "ou") == 0)
        infoAtomo.atomo = OU;
    else
        infoAtomo.atomo = IDENTIFICADOR;

    return infoAtomo;
}

TInfoAtomo reconhece_comentario(){
    TInfoAtomo infoAtomo;
    infoAtomo.linha=contaLinha;
    infoAtomo.atomo = ERRO;

    if(*buffer == '/' && *(buffer+1)=='/'){
        buffer=buffer+2;
        //quebra de linha ou fim de arquivo pode encerrar um comentario iniciado por '//'
        while (*buffer != '\n' && *buffer != '\x0'){
            buffer++;
        }
    }else if(*buffer == '/' && *(buffer+1)=='*'){
        buffer=buffer+2;
        //comentario em bloco e finalizado ao reconhecer '*/'
        while(*buffer != '*' || *(buffer+1)!='/'){
            if(*buffer == '\n') contaLinha++;
            //a leitura de '\x0' determina o fim de um comentario em bloco
            if(*buffer == '\x0'){
                infoAtomo.atomo = COMENTARIO;
                return infoAtomo;
            }
            buffer++;
        }
        buffer=buffer+2;
    }else return infoAtomo;

    infoAtomo.atomo = COMENTARIO;
    return infoAtomo;
}

//#######################
// ANALISADOR SINTATICO
//#######################

void consome(TAtomo atomo) {
      if (lookahead == atomo) {
            printf("\n#  %d:%s", InfoAtomo.linha, strAtomo[InfoAtomo.atomo]);
            if(InfoAtomo.atomo == IDENTIFICADOR){
                printf(" - atributo:%s", InfoAtomo.atributo_ID);
            }
            else if (InfoAtomo.atomo == NUMERO){
                printf(" - numero:%s", InfoAtomo.atributo_ID);
            }
            InfoAtomo = obter_atomo();
            lookahead = InfoAtomo.atomo;
            while(lookahead == COMENTARIO){ //Consome comentario enquanto lookahead for um comentario
                consome(COMENTARIO);
      }
      }
      else {
            printf("\n#  %d:Erro Sintatico: esperado [%s] encontrado [%s]\n", InfoAtomo.linha, strAtomo[atomo], strAtomo[lookahead]);
            exit(1);
      }
}

// <programa>::= algoritmo identificador “;” <bloco> “.”
void programa() {
      //Consome comentarios antes de algoritmo
      while(lookahead == COMENTARIO){
            consome(COMENTARIO);
      }
      consome(ALGORITMO);
      consome(IDENTIFICADOR);
      consome(PONTO_VIRGULA);
      bloco();
      consome(PONTO);
}

// <bloco>::= [ <declaracao_de_variaveis> ] <comando_composto>
void bloco() {
      if (lookahead == VARIAVEL){
        declaracao_variavel();
      }
      comando_composto();
}

// <declaracao_de_variaveis> ::= variavel {<lista_variavel> “:” <tipo> “;”}+
void declaracao_variavel() {
      consome(VARIAVEL);
      while(lookahead == IDENTIFICADOR){
            lista_variavel();
            consome(DOIS_PONTOS);
            tipo();
            consome(PONTO_VIRGULA);
      }
}

// <lista_variavel> ::= identificador { “,” identificador }
void lista_variavel() {
      consome(IDENTIFICADOR);
      while(lookahead == VIRGULA){
            consome(VIRGULA);
            consome(IDENTIFICADOR);
      }
}

// // <tipo> ::= inteiro | logico
void tipo() {
    if(lookahead == INTEIRO) consome(INTEIRO);
    else consome(LOGICO);
}

// <comando_composto> ::= inicio <comando> { “;” <comando>} fim
void comando_composto() {
      consome(INICIO);
      comando();
      while(lookahead == PONTO_VIRGULA){
            consome(PONTO_VIRGULA);
            comando();
      }
      consome(FIM);
}

// <comando> ::= <comando_atribuicao> | <comando_se> | <comando_enquanto> | <comando_entrada> | <comando_saida> | <comando_composto>
void comando() {
      if(lookahead == IDENTIFICADOR) comando_atribuicao();
      else if(lookahead == SE) comando_se();
      else if(lookahead == ENQUANTO) comando_enquanto();
      else if(lookahead == LEIA) comando_entrada();
      else if(lookahead == ESCREVA) comando_saida();
      else comando_composto();
}

// <comando_atribuicao> ::= identificador “:=” <expressao>
void comando_atribuicao() {
      consome(IDENTIFICADOR);
      consome(ATRIBUICAO);
      expressao();
}

// <comando_se> ::= se “(” <expressao> “)” entao <comando> [senao <comando>]
void comando_se() {
      consome(SE);
      consome(ABRE_PARENTESES);
      expressao();
      consome(FECHA_PARENTESES);
      consome(ENTAO);
      comando();
      if(lookahead == SENAO){
            consome(SENAO);
            comando();
      }
}

// <comando_enquanto> ::= enquanto “(” <expressao> “)” faca <comando>
void comando_enquanto() {
      consome(ENQUANTO);
      consome(ABRE_PARENTESES);
      expressao();
      consome(FECHA_PARENTESES);
      consome(FACA);
      comando();
}

// <comando_entrada> ::= leia “(“ <lista_variavel> “)”
void comando_entrada() {
      consome(LEIA);
      consome(ABRE_PARENTESES);
      lista_variavel();
      consome(FECHA_PARENTESES);
}

// <comando_saida> ::= escreva “(“ <expressao> { “,” <expressao> } “)”
void comando_saida() {
      consome(ESCREVA);
      consome(ABRE_PARENTESES);
      expressao();
      while(lookahead == VIRGULA){
            consome(VIRGULA);
            expressao();
      }
      consome(FECHA_PARENTESES);
}

// <expressao> ::= <expressao_simples> [<relacional> <expressao_simples>]
void expressao() {
      expressao_simples();
      if(lookahead == MENOR || lookahead == MENOR_IGUAL || lookahead == IGUAL || lookahead == DIFERENTE || lookahead == MAIOR || lookahead == MAIOR_IGUAL) {
            relacional();
            expressao_simples();
      }
}

// <relacional> ::= “<” | “<=” | “=” | “#” | “>” | “>=”
void relacional() {
      if(lookahead == MENOR) consome(MENOR);
      else if(lookahead == MENOR_IGUAL) consome(MENOR_IGUAL);
      else if(lookahead == IGUAL) consome(IGUAL);
      else if(lookahead == DIFERENTE) consome(DIFERENTE);
      else if(lookahead == MAIOR) consome(MAIOR);
      else consome(MAIOR_IGUAL);
}

// <expressao_simples> ::= [“+” | “−”] <termo> { (“+” | “−” | ou ) <termo> }
void expressao_simples() {
      if(lookahead == OP_SOMA) consome(OP_SOMA);
      else if(lookahead == OP_SUB) consome(OP_SUB);
      termo();
      while(lookahead == OP_SOMA || lookahead == OP_SUB || lookahead == OU){
            if(lookahead == OP_SOMA) consome(OP_SOMA);
            else if(lookahead == OP_SUB) consome(OP_SUB);
            else consome(OU);
            termo();
      }
}

// <termo> ::= <fator> { ( “*” | div | e )<fator> }
void termo() {
      fator();
      while(lookahead == OP_MULT || lookahead == DIV || lookahead == E){
            if(lookahead == OP_MULT) consome(OP_MULT);
            else if(lookahead == DIV) consome(DIV);
            else consome(E);
            fator();
      }
}

// <fator> ::= identificador | numero | verdadeiro | falso | “(” <expressao> “)”
void fator() {
      if(lookahead == IDENTIFICADOR) consome(IDENTIFICADOR);
      else if(lookahead == NUMERO) consome(NUMERO);
      else if(lookahead == VERDADEIRO) consome(VERDADEIRO);
      else if(lookahead == FALSO) consome(FALSO);
      else {
            consome(ABRE_PARENTESES);
            expressao();
            consome(FECHA_PARENTESES);
      }
}
