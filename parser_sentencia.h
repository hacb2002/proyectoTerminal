#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include <utility>
#include <vector>

struct sentencia {
   virtual ~sentencia( ) = 0;
};
sentencia::~sentencia( ) = default;

struct sentencia_if : sentencia {
   expresion* condicion;
   std::vector<sentencia*> parte_si;
   std::vector<sentencia*> parte_no;

   sentencia_if(expresion* c, std::vector<sentencia*>&& s, std::vector<sentencia*>&& n)
   : condicion(c), parte_si(std::move(s)), parte_no(std::move(n)) {
   }
};

struct sentencia_return : sentencia {
   expresion* valor;

   sentencia_return(expresion* v)
   : valor(v) {
   }
};

struct sentencia_declaracion : sentencia {
   std::vector<token> nombres;
   std::vector<expresion*> inicializadores;

   sentencia_declaracion(std::vector<token>&& n, std::vector<expresion*>&& i)
   : nombres(std::move(n)), inicializadores(std::move(i)) {
   }
};

struct sentencia_expresion : sentencia {
   expresion* ex;

   sentencia_expresion(expresion* e)
   : ex(e) {
   }
};

sentencia* parser_sentencia(const token*& p) {
   if (p->tipo == IF) {
      ++p;
      espera(p, PARENTESIS_IZQ);
      expresion* condicion = parser_expresion(p);
      espera(p, PARENTESIS_DER);
      espera(p, LLAVE_IZQ);
      std::vector<sentencia*> parte_si, parte_no;
      while(p->tipo != LLAVE_DER){
         parte_si.push_back(parser_sentencia(p));
      }
      espera(p, LLAVE_DER);
      if (p->tipo == ELSE) {
         ++p;
         if(p->tipo == IF){
            parte_no.push_back(parser_sentencia(p));
         }else{
            espera(p, LLAVE_IZQ);
            while(p->tipo != LLAVE_DER){
               parte_no.push_back(parser_sentencia(p));
            }
            espera(p, LLAVE_DER);
         }
      }
      return new sentencia_if(condicion, std::move(parte_si), std::move(parte_no));
   } else if (p->tipo == RETURN) {
      ++p;
      expresion* valor = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return new sentencia_return(valor);
   } else if (p->tipo == INT) {
      ++p;
      std::vector<token> nombres;
      std::vector<expresion*> inicializadores;
      do{
         nombres.emplace_back(espera(p, IDENTIFICADOR));
         espera(p, ASIGNACION);
         inicializadores.emplace_back(parser_expresion(p));
         if(p->tipo != COMA){
            break;
         }else{
            espera(p, COMA);
         }
      }while(1);
      espera(p, PUNTO_COMA);
      return new sentencia_declaracion(std::move(nombres), std::move(inicializadores));
   } else {
      expresion* ex = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return new sentencia_expresion(ex);
   }
}

#endif
