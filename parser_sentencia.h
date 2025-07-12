#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include <utility>
#include <vector>

struct sentencia {
   std::string_view vista;
   sentencia(const control_vista& cv)
   : vista(cv){
   }

   std::pair<int, int> obtener_linea_columna(const std::string& s) {
   int linea = 0, columna = 0;
   for (auto p = &s[0]; p != vista.begin( ); ++p) {
      if (*p == '\n') {
         linea += 1, columna = 0;
      } else {
         columna += 1;
      }
   }
   return { linea, columna };
}
   virtual ~sentencia( ) = 0;
};
sentencia::~sentencia( ) = default;

struct sentencia_expresion : sentencia {
   expresion* ex;

   sentencia_expresion(const control_vista& cv, expresion* e)
   : sentencia(cv), ex(e) {
   }
};

struct sentencia_declaracion : sentencia {
   std::vector<token> nombres;
   std::vector<expresion*> inicializadores;

   sentencia_declaracion(const control_vista& cv, std::vector<token>&& n, std::vector<expresion*>&& i)
   : sentencia(cv), nombres(std::move(n)), inicializadores(std::move(i)) {
   }
};

struct sentencia_if : sentencia {
   expresion* condicion;
   std::vector<sentencia*> parte_si;
   std::vector<sentencia*> parte_no;

   sentencia_if(const control_vista& cv, expresion* c, std::vector<sentencia*>&& s, std::vector<sentencia*>&& n)
   : sentencia(cv), condicion(c), parte_si(std::move(s)), parte_no(std::move(n)) {
   }
};

struct sentencia_return : sentencia {
   expresion* valor;

   sentencia_return(const control_vista& cv, expresion* v)
   : sentencia(cv), valor(v) {
   }
};

sentencia* parser_sentencia(const token*& p) {
   control_vista cv(p);
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
      return new sentencia_if(cv, condicion, std::move(parte_si), std::move(parte_no));
   } else if (p->tipo == RETURN) {
      ++p;
      expresion* valor = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return new sentencia_return(cv, valor);
   } else if (p->tipo == INT) {
      ++p;
      std::vector<token> nombres;
      std::vector<expresion*> inicializadores;
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
      return new sentencia_declaracion(cv, std::move(nombres), std::move(inicializadores));
   }else {
      expresion* ex = parser_expresion(p);
      espera(p, PUNTO_COMA);
      return new sentencia_expresion(cv, ex);
   }
}

#endif
