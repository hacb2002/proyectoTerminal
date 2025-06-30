#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

#include "lexer.h"
#include "parser_expresion.h"
#include <vector>

struct sentencia {
   virtual ~sentencia( ) = 0;
};

struct sentencia_if : sentencia {
   expresion* condicion;
   std::vector<sentencia*> parte_si;
   std::vector<sentencia*> parte_no;
};

struct sentencia_return : sentencia {
   expresion* valor;
};

struct sentencia_declaracion : sentencia {
   token tipo;
   std::vector<token> nombres;
   std::vector<expresion*> inicializadores;
};

struct sentencia_expresion : sentencia {
   expresion* ex;
};

sentencia* parser_sentencia(const token*& p) {
   if (p->tipo == IF) {
      ++p;
      auto res = new sentencia_if();
      espera(p, PARENTESIS_IZQ);
      res->condicion = parser_expresion(p);
      espera(p, PARENTESIS_DER);
      espera(p, LLAVE_IZQ);
      while(p->tipo != LLAVE_DER){
         res->parte_si.push_back(parser_sentencia(p));
      }
      espera(p, LLAVE_DER);
      if (p->tipo == ELSE) {
         ++p;
         if(p->tipo == IF){
            res->parte_no.push_back(parser_sentencia(p));
         }else{
            espera(p, LLAVE_IZQ);
            while(p->tipo != LLAVE_DER){
               res->parte_no.push_back(parser_sentencia(p));
            }
            espera(p, LLAVE_DER);
         }
      }
      return res;
   } else if (p->tipo == RETURN) {
      auto res = new sentencia_return();
      res->valor = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return res;
   } else if (p->tipo == INT) {
      auto res = new sentencia_declaracion();
      res->tipo = *(p++);
      do{
         res->nombres.emplace_back(espera(p, IDENTIFICADOR));
         espera(p, ASIGNACION);
         res->inicializadores.emplace_back(espera(p, LITERAL_ENTERA));
      }while(p->tipo == COMA);
      espera(p, PUNTO_COMA);
   } else {
      auto res = new sentencia_expresion();
      res->ex = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return res;
   }
}

#endif