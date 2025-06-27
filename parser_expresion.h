#ifndef PARSER_EXPRESION_H
#define PARSER_EXPRESION_H

#include "error.h"
#include "lexer.h"
#include "parser_aux.h"
#include <memory>
#include <utility>
#include <vector>

struct expresion {
   std::string_view vista;

   expresion(const control_vista& cv)
   : vista(cv) {
   }

   virtual ~expresion( ) = default;
};

struct expresion_termino : expresion {
   const token& token_expresion;

   expresion_termino(const control_vista& cv, const token& t)
   : expresion(cv), token_expresion(t) {
   }
};

struct expresion_binaria : expresion {
   std::unique_ptr<expresion> izq;
   const token& op;
   std::unique_ptr<expresion> der;

   expresion_binaria(const control_vista& cv, std::unique_ptr<expresion> i, const token& p, std::unique_ptr<expresion> d)
   : expresion(cv), izq(std::move(i)), op(p), der(std::move(d)) {
   }
};

struct expresion_prefija : expresion {
   const token& op;
   std::unique_ptr<expresion> ex;

   expresion_prefija(const control_vista& cv, const token& p, std::unique_ptr<expresion> e)
   : expresion(cv), op(p), ex(std::move(e)) {
   }
};

struct expresion_llamada : expresion {
   const token& funcion;
   std::vector<std::unique_ptr<expresion>> parametros;

   expresion_llamada(const control_vista& cv, const token& f, std::vector<std::unique_ptr<expresion>> p)
   : expresion(cv), funcion(f), parametros(std::move(p)) {
   }
};

struct expresion_secuencia : expresion {
   std::vector<std::unique_ptr<expresion>> valores;
   expresion_secuencia(const control_vista& cv, std::vector<std::unique_ptr<expresion>> v)
   : expresion(cv), valores(std::move(v)) {
   }
};

std::unique_ptr<expresion> expr(const token*&);
std::unique_ptr<expresion> expr_primaria(const token*&);
std::unique_ptr<expresion> expr_unaria(const token*&);
std::unique_ptr<expresion> expr_n_aria(const token*&, int);

std::vector<std::unique_ptr<expresion>> lista_expr(const token*& p) {
   // Pendiente de explicar. 
}

std::unique_ptr<expresion> expr_primaria(const token*& p) {
   control_vista cv(p);
   if ((p->tipo == IDENTIFICADOR || es_tipo(p->tipo)) && (p + 1)->tipo == PARENTESIS_IZQ) {
      auto funcion = p;
      auto params = lista_expr(p += 2);
      espera(p, PARENTESIS_DER);
      return std::make_unique<expresion_llamada>(cv, *funcion, std::move(params));
   } else if (p->tipo == IDENTIFICADOR || p->tipo == LITERAL_ENTERA || es_tipo(p->tipo)) {
      return std::make_unique<expresion_termino>(cv, *p++);
   } else if (p->tipo == PARENTESIS_IZQ) {
      auto ex = expr(++p);
      espera(p, PARENTESIS_DER);
      return ex;
   } else {
      throw error("Expresion esperada", p->vista);
   }
}

std::unique_ptr<expresion> expr_unaria(const token*& p) {
   control_vista cv(p);
   if (es_operador_prefijo(p->tipo)) {
      const token& op = *p++;
      auto operando = expr_unaria(p);
      return std::make_unique<expresion_prefija>(cv, op, std::move(operando));
   }
   return expr_primaria(p);
}


std::unique_ptr<expresion> expr_n_aria(const token*& p, int prec) {
   control_vista cv(p);
   auto ex = expr_unaria(p);
   while (es_operador_binario(p->tipo) && precedencia(p->tipo) >= prec) {
      auto operador = p;
      ex = std::make_unique<expresion_binaria>(cv, std::move(ex), *operador , expr_n_aria(++p, precedencia(operador->tipo) + asociatividad(operador->tipo)));
   }
   return ex;
}

std::unique_ptr<expresion> expr(const token*& p) {
   return expr_n_aria(p, 0);
}

#endif
