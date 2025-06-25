#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <map>

enum lexema {
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

const std::map<std::string, lexema> palabras = {
   { "int", INT },
   { "if", IF },
   { "else", ELSE },
   { "return", RETURN }
};

const std::map<std::string, lexema> simbolos = {
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
   lexema tipo;
   const char* ini;
   const char* fin;
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

bool es_entero(const char*& ini){
   if(std::isdigit(*ini)){
      do{
         ++ini;
      }while(std::isdigit(*ini));
      return true;
   }
   return false;
}

bool es_id_o_palabra(const char*& ini){
   if(std::isalnum(*ini) || *ini == '_'){
      do{
         ++ini;
      }while(std::isalnum(*ini) || *ini == '_');
      return true;
   }
   return false;
}

lexema lexema_id_palabra(const std::string& palabra){
   auto it = palabras.find(palabra);
   return (it != palabras.end()) ? it->second : IDENTIFICADOR;
}

bool es_simbolo(const char*& ini){
   if(simbolos.contains(std::string(ini, ini+1))){
      ini += 1 + simbolos.contains(std::string(ini, ini+2));
      return true;
   }
   return false;
}

std::vector<token> lexer(const std::string& entrada) {
   std::vector<token> res;
   const char* ini = &entrada[0];
   while (*ini != '\0') {
      esquiva_espacios(ini);
      if (es_comentario_linea(ini) || es_comentario_bloque(ini)) {
         continue;
      }
      const char* copia = ini;
      if(es_entero(ini)){
         res.emplace_back(LITERAL_ENTERA, copia, ini);
      }else if(es_caracter(ini)){
         res.emplace_back(LITERAL_CHAR, copia, ini);
      }else if(es_id_o_palabra(ini)){
         res.emplace_back(lexema_id_palabra(std::string(copia, ini)), copia, ini);
      }else if(es_simbolo(ini)){
         res.emplace_back(simbolos.find(std::string(copia, ini))->second, copia, ini);
      }else{
         ++ini;
      }
   }
   res.push_back(token{FIN_ARCHIVO, ini, ini + 1 });
   return res;
}

#endif
