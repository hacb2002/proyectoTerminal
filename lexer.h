#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <map>

enum tipo_lexema {
   INT,
   MAS,
   MENOS,
   POR,
   ENTRE,
   MODULO,
   ASIGNACION,
   MENOR,
   MENOR_IGUAL,
   MAYOR,
   MAYOR_IGUAL,
   IGUAL,
   DIFERENTE,
   AND,
   OR,
   NOT,
   PARENTESIS_IZQ,
   PARENTESIS_DER,
   LLAVE_IZQ,
   LLAVE_DER,
   COMA,
   PUNTO_COMA,
   IF,
   ELSE,
   RETURN,
   IDENTIFICADOR,
   LITERAL_ENTERA,
   FIN_ARCHIVO
};

const std::map<std::string, tipo_lexema> palabras = {
   { "int", INT },
   { "if", IF },
   { "else", ELSE },
   { "return", RETURN }
};

const std::map<std::string, tipo_lexema> simbolos = {
   { "+", MAS },
   { "-", MENOS },
   { "*", POR },
   { "/", ENTRE },
   { "%", MODULO },
   { "=", ASIGNACION },
   { "<", MENOR },
   { "<=", MENOR_IGUAL },
   { ">", MAYOR },
   { ">=", MAYOR_IGUAL },
   { "==", IGUAL },
   { "!=", DIFERENTE },
   { "&&", AND},
   { "||", OR},
   { "!", NOT },
   { "(", PARENTESIS_IZQ },
   { ")", PARENTESIS_DER },
   { "{", LLAVE_IZQ },
   { "}", LLAVE_DER },
   { ",", COMA },
   { ";", PUNTO_COMA }
};

struct token {
   tipo_lexema tipo;
   std::string_view vista;

   token( ) = default;
   token(tipo_lexema t, const char* ini, const char* fin)
   : tipo(t), vista(ini, fin - ini) {
   }
};

void esquiva_espacios(const char*& p) {
   while (std::isspace(*p)) {
      ++p;
   }
}

bool es_comentario_linea(const char*& p) {
   if (*p == '/' && *(p + 1) == '/') {
      p += 2;
      while (*p != '\n' && *p != '\0') {
         ++p;
      }
      return true;
   }
   return false;
}

bool es_comentario_bloque(const char*& p) {
   if(*p == '/' && *(p+1) == '*'){
      p += 2;
      while(*p != '\0' && *p != '*' && *(p+1) != '/'){
         ++p;
      }
      if(*p != '\0'){
         p += 2;
      }
      return true;
   }
   return false;
}

bool es_entero(const char*& p){
   if(std::isdigit(*p)){
      do{
         ++p;
      }while(std::isdigit(*p));
      return true;
   }
   return false;
}

bool es_id_o_palabra(const char*& p){
   if(std::isalnum(*p) || *p == '_'){
      do{
         ++p;
      }while(std::isalnum(*p) || *p == '_');
      return true;
   }
   return false;
}

tipo_lexema tipo_lexema_id_palabra(const std::string& palabra){
   auto it = palabras.find(palabra);
   return (it != palabras.end()) ? it->second : IDENTIFICADOR;
}

bool es_simbolo(const char*& p){
   for (int t : { 2, 1 }) {
      if (simbolos.contains(std::string(p, p + t))) {
         p += t;
         return true;
      }
   }
   return false;
}

std::vector<token> lexer(const std::string& entrada) {
   std::vector<token> res;
   const char* ini = &entrada[0];
   while (1) {
      esquiva_espacios(ini);
      const char* copia = ini;
      if (es_comentario_linea(ini) || es_comentario_bloque(ini)) {
         continue;
      }else if(es_entero(ini)){
         res.emplace_back(LITERAL_ENTERA, copia, ini);
      }else if(es_id_o_palabra(ini)){
         res.emplace_back(tipo_lexema_id_palabra(std::string(copia, ini)), copia, ini);
      }else if(es_simbolo(ini)){
         res.emplace_back(simbolos.find(std::string(copia, ini))->second, copia, ini);
      }else if(*ini == '\0'){
         res.push_back(token{FIN_ARCHIVO, ini, ini + 1 });
         break;
      }else{
         throw error("Token desconocido", copia, ini + 1);
      }
   }

   return res;
}

#endif
