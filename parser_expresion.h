#ifndef PARSER_EXPRESION_H
#define PARSER_EXPRESION_H

#include "lexer.h"
#include "parser_aux.h"
#include <utility>
#include <vector>

struct expresion {
   virtual ~expresion( ) = 0;
};
expresion::~expresion( ) = default;

struct expresion_termino : expresion {
   token termino;

   expresion_termino(token t)
   : termino(t) {
   }
};

struct expresion_binaria : expresion {
   expresion* izq;
   token operador;
   expresion* der;

   expresion_binaria(expresion* i, token op, expresion* d)
   : izq(i), operador(op), der(d) {
   }
};

struct expresion_prefija : expresion {
   token operador;
   expresion* ex;

   expresion_prefija(token op, expresion* e)
   : operador(op), ex(e) {
   }
};

struct expresion_llamada_funcion : expresion {
   token nombre;
   std::vector<expresion*> argumentos;

   expresion_llamada_funcion(token t, std::vector<expresion*>&& a)
   : nombre(t), argumentos(std::move(a)) {
   }
};

expresion* parser_expresion(const token*&);

std::vector<expresion*> parser_lista_expresion(const token*& p) {
   std::vector<expresion*> res;
   while (es_inicio_expr(p->tipo)) {
      res.push_back(parser_expresion(p));
      if (p->tipo == COMA) {
         ++p;
      } else {
         break;
      }
   }
   return res;
}

expresion* parser_expresion_primaria(const token*& p) {
   if (p->tipo == IDENTIFICADOR || p->tipo == LITERAL_ENTERA) {
      return new expresion_termino(*p++);
   } else {
      espera(p, PARENTESIS_IZQ);
      expresion* ex = parser_expresion(p);
      espera(p, PARENTESIS_DER);
      return ex;
   }
}

expresion* parser_expresion_unaria(const token*& p) {
   if (es_operador_prefijo(p->tipo)) {
      token operador = *p++;
      return new expresion_prefija(operador, parser_expresion_unaria(p));
   } else if (p->tipo == IDENTIFICADOR && (p + 1)->tipo == PARENTESIS_IZQ) {
      token nombre = *p++;
      espera(p, PARENTESIS_IZQ);
      std::vector<expresion*> argumentos = parser_lista_expresion(p);
      espera(p, PARENTESIS_DER);
      return new expresion_llamada_funcion(nombre, std::move(argumentos));
   } else {
      return parser_expresion_primaria(p);
   }
}

expresion* parser_expresion_n_aria(const token*& p, int prec) {   // precedence climbing
   expresion* ex = parser_expresion_unaria(p);
   while (es_operador_binario(p->tipo) && precedencia(p->tipo) >= prec) {
      token operador = *p++;
      ex = new expresion_binaria(ex, operador, parser_expresion_n_aria(p, precedencia(operador.tipo) + asociatividad(operador.tipo)));
   }
   return ex;
}

expresion* parser_expresion(const token*& p) {
   return parser_expresion_n_aria(p, 0);
}

#endif
