#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <map>

enum lexema {
   INT,
   CHAR,
   VOID,
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
   AND_LOGICO,
   OR_LOGICO,
   PARENTESIS_IZQ,
   PARENTESIS_DER,
   CORCHETE_IZQ,
   CORCHETE_DER,
   LLAVE_IZQ,
   LLAVE_DER,
   COMA,
   PUNTO_COMA,
   IF,
   ELSE,
   RETURN,
   PRINT,
   SCAN,
   EXIT,
   IDENTIFICADOR,
   LITERAL_ENTERA,
   FIN_ARCHIVO
};

const std::map<std::string, lexema> palabras = {
   { "int", INT },
   { "char", CHAR },
   { "void", VOID },
   { "if", IF },
   { "else", ELSE },
   { "return", RETURN },
   { "print", PRINT },
   { "scan", SCAN },
   { "exit", EXIT }
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
   { "&&", AND_LOGICO},
   { "||", OR_LOGICO},
   { "&", AND },
   { "|", OR },
   { "!", NOT },
   { "(", PARENTESIS_IZQ },
   { ")", PARENTESIS_DER },
   { "[", CORCHETE_IZQ },
   { "]", CORCHETE_DER },
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

void esquiva_comentarios(const char*& p) {
   if (*p == '/' && *(p + 1) == '/') {
      p += 2;
      while (*p != '\n' && *p != '\0') {
         ++p;
      }
   }
   //Duda sobre si seria adecuado añadirlo desde este punto o momento. 
   if(*p == '/' && *(p+1) == '*'){
      p += 2;
      while(*p != '*' && *(p+1) != '/'){
         if(*(p) == '\0'){
            break;
         }
         ++p;
      }
      if(*(p) != '\0'){
         p += 2;
      }
   }
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
   if(std::isalpha(*ini) || *ini == '_'){
      do{
         ++ini;
      }while(std::isalpha(*ini) || *ini == '_');
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
      esquiva_comentarios(ini);
      const char* copia = ini;
      if(es_entero(ini)){
         res.emplace_back(LITERAL_ENTERA, copia, ini);
      }else if(es_id_o_palabra(ini)){
         res.emplace_back(lexema_id_palabra(std::string(copia, ini)), copia, ini);
      }else if(es_simbolo(ini)){
         res.emplace_back(simbolos.find(std::string(copia, ini))->second, copia, ini);
      }
   }
   res.push_back(token{FIN_ARCHIVO, ini, ini + 1 });
   return res;
}

#endif
