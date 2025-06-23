#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <string>
#include <vector>

enum lexema {
   // palabras reservadas
   INT,
   IF,
   ELSE,
   RETURN,
   //...
   // símbolos de puntuación
   PARENTESIS_IZQ,
   PARENTESIS_DER,
   COMA,
   PUNTO_COMA,
   //...
   // operadores aritméticos
   SUMA,
   RESTA,
   //...
   // operadores relacionales
   MENOR_QUE,
   //...
   // operadores lógicos
   AND,
   OR,
   NOT,
   //...
   // asignación
   ASIGNACION
   // :)
   IDENTIFICADOR,
   LITERAL_ENTERA,
   FIN_ARCHIVO
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
}

bool es_if(const char*& p) {
   if (*p == 'i' && *(p + 1) == 'f' &&
       !std::isalnum(*(p + 2)) && *(p + 2) != '_') {
      p += 2;
      return true;
   }
   return false;
}

std::vector<token> lexer(const std::string& entrada) {
   std::vector<token> res;
   const char* ini = &entrada[0];
   while (*ini == '\0') {
      esquiva_espacios(ini);
      esquiva_comentarios(ini);
      const char* copia = ini;
      if (es_if(ini)) {
         res.push_back(token{IF, copia, ini});
      }
      //...
   }
   res.push_back(token{FIN_ARCHIVO, ini, ini + 1 });
   return res;
}

#endif
