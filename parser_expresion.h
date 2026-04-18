#ifndef PARSER_EXPRESION_H
#define PARSER_EXPRESION_H

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

   virtual ~expresion( ) = 0;
};
expresion::~expresion( ) = default;

struct expresion_termino : expresion {
   token termino;

   expresion_termino(const control_vista& cv, token t)
   : expresion(cv), termino(t) {
   }
};

struct expresion_binaria : expresion {
   std::unique_ptr<expresion> izq;
   token operador;
   std::unique_ptr<expresion> der;

   expresion_binaria(const control_vista& cv, std::unique_ptr<expresion> i, const token& p, std::unique_ptr<expresion> d)
   : expresion(cv), izq(std::move(i)), operador(p), der(std::move(d)) {
   }
};

struct expresion_prefija : expresion {
   token operador;
   std::unique_ptr<expresion> ex;

   expresion_prefija(const control_vista& cv, token op, std::unique_ptr<expresion> e)
   : expresion(cv), operador(op), ex(std::move(e)) {
   }
};

struct expresion_llamada_funcion : expresion {
   token nombre;
   std::vector<std::unique_ptr<expresion>> argumentos;

   expresion_llamada_funcion(const control_vista& cv, token t, std::vector<std::unique_ptr<expresion>>&& a)
   : expresion(cv), nombre(t), argumentos(std::move(a)) {
   }
};

std::unique_ptr<expresion> parser_expresion(const token*&);

std::vector<std::unique_ptr<expresion>> parser_lista_expresion(const token*& p) {
   std::vector<std::unique_ptr<expresion>> res;
   while (es_inicio_expr(p->tipo)) {
      res.push_back(parser_expresion(p));
      if (p->tipo == COMA && es_inicio_expr((p+1)->tipo)){
         ++p;
      } else {
         break;
      }
   }
   return res;
}

std::unique_ptr<expresion> parser_expresion_primaria(const token*& p) {
   control_vista cv(p);
   if (p->tipo == IDENTIFICADOR || p->tipo == LITERAL_ENTERA) {
      return std::make_unique<expresion_termino>(cv, *p++);
   } else {
      espera(p, PARENTESIS_IZQ);
      std::unique_ptr<expresion> ex = parser_expresion(p);
      espera(p, PARENTESIS_DER);
      return ex;
   }
}

std::unique_ptr<expresion> parser_expresion_unaria(const token*& p) {
   control_vista cv(p);
   if (es_operador_prefijo(p->tipo)) {
      token operador = *p++;
      return std::make_unique<expresion_prefija>(cv, operador, parser_expresion_unaria(p));
   } else if (p->tipo == IDENTIFICADOR && (p + 1)->tipo == PARENTESIS_IZQ) {
      token nombre = *p++;
      espera(p, PARENTESIS_IZQ);
      std::vector<std::unique_ptr<expresion>> argumentos = parser_lista_expresion(p);
      espera(p, PARENTESIS_DER);
      return std::make_unique<expresion_llamada_funcion>(cv, nombre, std::move(argumentos));
   } else {
      return parser_expresion_primaria(p);
   }
}

std::unique_ptr<expresion> parser_expresion_n_aria(const token*& p, int prec) {
   control_vista cv(p);
   std::unique_ptr<expresion> ex = parser_expresion_unaria(p);
   while (es_operador_binario(p->tipo) && precedencia(p->tipo) >= prec) {
      token operador = *p++;
      if(operador.tipo == ASIGNACION){
         throw error("En este subconjunto de C no se pueden asignar valores excepto al inicializar variables", operador.vista);
      }
      ex = std::make_unique<expresion_binaria>(cv, std::move(ex), operador, parser_expresion_n_aria(p, precedencia(operador.tipo) + asociatividad(operador.tipo)));
   }
   return ex;
}

std::unique_ptr<expresion> parser_expresion(const token*& p) {
   return parser_expresion_n_aria(p, 0);
}

#endif
