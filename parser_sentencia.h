#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include <memory>
#include <utility>
#include <vector>

struct sentencia {
   std::string_view vista;
   sentencia(const control_vista& cv)
   : vista(cv){
   }

   virtual ~sentencia( ) = 0;
};
sentencia::~sentencia( ) = default;

struct sentencia_expresion : sentencia {
   std::unique_ptr<expresion> ex;

   sentencia_expresion(const control_vista& cv, std::unique_ptr<expresion> e)
   : sentencia(cv), ex(std::move(e)) {
   }
};

struct sentencia_declaracion : sentencia {
   std::vector<token> nombres;
   std::vector<std::unique_ptr<expresion>> inicializadores;

   sentencia_declaracion(const control_vista& cv, std::vector<token>&& n, std::vector<std::unique_ptr<expresion>>&& i)
   : sentencia(cv), nombres(std::move(n)), inicializadores(std::move(i)) {
   }
};

struct sentencia_if : sentencia {
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> parte_si;
   std::vector<std::unique_ptr<sentencia>> parte_no;

   sentencia_if(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>>&& s, std::vector<std::unique_ptr<sentencia>>&& n)
   : sentencia(cv), condicion(std::move(c)), parte_si(std::move(s)), parte_no(std::move(n)) {
   }
};

struct sentencia_return : sentencia {
   std::unique_ptr<expresion> valor;

   sentencia_return(const control_vista& cv, std::unique_ptr<expresion> v)
   : sentencia(cv), valor(std::move(v)) {
   }
};

std::unique_ptr<sentencia> parser_sentencia(const token*& p) {
   control_vista cv(p);
   if (p->tipo == IF) {
      ++p;
      espera(p, PARENTESIS_IZQ);
      std::unique_ptr<expresion> condicion = parser_expresion(p);
      espera(p, PARENTESIS_DER);
      espera(p, LLAVE_IZQ);
      std::vector<std::unique_ptr<sentencia>> parte_si, parte_no;
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
      return std::make_unique<sentencia_if>(cv, std::move(condicion), std::move(parte_si), std::move(parte_no));
   } else if (p->tipo == RETURN) {
      ++p;
      std::unique_ptr<expresion> valor = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return std::make_unique<sentencia_return>(cv, std::move(valor));
   } else if (p->tipo == INT) {
      ++p;
      std::vector<token> nombres;
      std::vector<std::unique_ptr<expresion>> inicializadores;
      for (;;) {
         nombres.emplace_back(espera(p, IDENTIFICADOR));
         espera(p, ASIGNACION);
         inicializadores.emplace_back(parser_expresion(p));
         if(p->tipo != COMA){
            break;
         }
         espera(p, COMA);
      }
      espera(p, PUNTO_COMA);
      return std::make_unique<sentencia_declaracion>(cv, std::move(nombres), std::move(inicializadores));
   }else {
      std::unique_ptr<expresion> ex = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return std::make_unique<sentencia_expresion>(cv, std::move(ex));
   }
}

#endif
