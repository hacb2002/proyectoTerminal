#ifndef PARSER_AUX_H
#define PARSER_AUX_H

#include "lexer.h"

struct control_vista {
   const token* ini;
   const token*& ref;

   control_vista(const token*& p)
   : ini(p), ref(p) {
   }

   explicit operator std::string_view( ) const {
      return { ini->vista.begin( ), size_t(ini == ref ? 0 : (ref - 1)->vista.end( ) - ini->vista.begin( )) };
   }
};

const token& espera(const token*& p, tipo_lexema esperado) {
   if (p->tipo != esperado) {
      throw error("Token inesperado", p->vista);
   }
   return *p++;
}

bool es_operador_prefijo(tipo_lexema t) {
   return t == MAS || t == MENOS || t == NOT;
}

bool es_operador_binario(tipo_lexema t) {
   return t == MAS || t == MENOS || t == POR || t == ENTRE || t == MODULO ||
          t == ASIGNACION ||
          t == MENOR || t == MENOR_IGUAL || t == MAYOR || t == MAYOR_IGUAL || t == IGUAL || t == DIFERENTE ||
          t == AND || t == OR;
}

bool es_tipo(tipo_lexema t) {
   return t == INT;
}

bool es_inicio_expr(tipo_lexema t) {
   return t == IDENTIFICADOR || t == LITERAL_ENTERA ||
          t == PARENTESIS_IZQ ||
         es_operador_prefijo(t);
}

int precedencia(tipo_lexema t) {
   switch (t) {
   case ASIGNACION:
      return 0;
   case OR:
      return 1;
   case AND:
      return 2;
   case MENOR:
   case MENOR_IGUAL:
   case MAYOR:
   case MAYOR_IGUAL:
   case IGUAL:
   case DIFERENTE:
      return 3;
   case MAS:
   case MENOS:
      return 4;
   case POR:
   case ENTRE:
   case MODULO:
      return 5;
   default:
      return -1;
   }
}

int asociatividad(tipo_lexema t) {
   return (t != ASIGNACION);
}

#endif
