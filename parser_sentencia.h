#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

#include "parser_aux.h"
#include "parser_expresion.h"
#include <memory>
#include <string>
#include <vector>

struct sentencia {
   std::string_view vista;

   sentencia(const control_vista& cv)
   : vista(cv) {
   }

   virtual ~sentencia( ) = default;
};

struct sentencia_expresion : sentencia {
   std::unique_ptr<expresion> ex;

   sentencia_expresion(const control_vista& cv, std::unique_ptr<expresion> e)
   : sentencia(cv), ex(std::move(e)) {
   }
};

struct sentencia_declaracion : sentencia {
   std::unique_ptr<expresion> tipo;
   const token& nombre;
   std::unique_ptr<expresion> inicializador;

   sentencia_declaracion(const control_vista& cv, std::unique_ptr<expresion> t, const token& n, std::unique_ptr<expresion> i)
   : sentencia(cv), tipo(std::move(t)), nombre(n), inicializador(std::move(i)) {
   }
};

struct sentencia_if : sentencia {
   std::unique_ptr<expresion> condicion;
   std::vector<std::unique_ptr<sentencia>> parte_si;
   std::vector<std::unique_ptr<sentencia>> parte_no;

   sentencia_if(const control_vista& cv, std::unique_ptr<expresion> c, std::vector<std::unique_ptr<sentencia>> s, std::vector<std::unique_ptr<sentencia>> n)
   : sentencia(cv), condicion(std::move(c)), parte_si(std::move(s)), parte_no(std::move(n)) {
   }
};


struct sentencia_return : sentencia {
   std::unique_ptr<expresion> ex;

   sentencia_return(const control_vista& cv, std::unique_ptr<expresion> e)
   : sentencia(cv), ex(std::move(e)) {
   }
};

std::unique_ptr<sentencia> stmt(const token*&);

std::vector<std::unique_ptr<sentencia>> lista_stmt(const token*& p) {
   std::vector<std::unique_ptr<sentencia>> res;
   while (p->tipo != LLAVE_DER) {
      res.push_back(stmt(p));
   }
   return res;
}

std::unique_ptr<sentencia_declaracion> stmt_declaracion(const token*& p, control_vista& cv, std::unique_ptr<expresion> tipo, bool permitir_init) {
   auto nombre = espera(p, IDENTIFICADOR);
   auto ex = (p->tipo == ASIGNACION && permitir_init ? expr(++p) : nullptr);
   return std::make_unique<sentencia_declaracion>(cv, std::move(tipo), *nombre, std::move(ex));
}

std::unique_ptr<sentencia_declaracion> stmt_declaracion(const token*& p, bool permitir_init) {
   control_vista cv(p);
   return stmt_declaracion(p, cv, expr(p), permitir_init);
}

std::unique_ptr<sentencia> stmt(const token*& p) {
   control_vista cv(p);
   if (p->tipo == IF) {
      auto ex = expr(++p);
      espera(p, LLAVE_IZQ);
      auto parte_si = lista_stmt(p), parte_no = decltype(parte_si)( );
      espera(p, LLAVE_DER);
      if (p->tipo == ELSE) {
         if ((++p)->tipo == IF) {
            parte_no.push_back(stmt(p));
         } else {
            espera(p, LLAVE_IZQ);
            parte_no = lista_stmt(p);
            espera(p, LLAVE_DER);
         }
      }
      return std::make_unique<sentencia_if>(cv, std::move(ex), std::move(parte_si), std::move(parte_no));
   } else if (p->tipo == RETURN) {
      auto ex = (es_inicio_expr((++p)->tipo) ? expr(p) : nullptr);
      espera(p, PUNTO_COMA);
      return std::make_unique<sentencia_return>(cv, std::move(ex));
   }  else {
      auto ex = expr(p);
      if (p->tipo == IDENTIFICADOR) {
         auto sen = stmt_declaracion(p, cv, std::move(ex), true);
         espera(p, PUNTO_COMA);
         return sen;
      } else {
         espera(p, PUNTO_COMA);
         return std::make_unique<sentencia_expresion>(cv, std::move(ex));
      }
   }
}

#endif
