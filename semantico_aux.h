#ifndef SEMANTICO_AUX_H
#define SEMANTICO_AUX_H

#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <charconv>
#include <vector>
#include <stddef.h>

enum contexto_tipo {
   RETORNO, PARAMETRO, LOCAL
};

//Este es posible ahorrarselo? Más que nada porque lo identicamos fácil con 0. 
enum modificador_tipo {
   VALOR
};

enum tiempo_vida {
   VARIABLE, TEMPORAL
};

struct tipo_evaluado {
   token tipo_base;
   modificador_tipo modificador = VALOR;
   tiempo_vida vida = TEMPORAL;

   bool operator==(const tipo_evaluado& t) const {
      if (tipo_base.vista != t.tipo_base.vista || modificador != t.modificador) {
        return false;
      }
      return true;
   }
};


/*Lo desglosé para que se entendiera mejor paso a paso, pero igual podemos regresarlo a una sola línea*/
int evalua_entero(const std::string_view& s) {
   int res;
   std::from_chars_result conversion = std::from_chars(s.begin(), s.end(), res);
   if (conversion.ec == std::errc( )) {
    return res;
   }
   throw error("Entero no representable", s);
}

tipo_evaluado evalua_como_tipo(const expresion* ex, contexto_tipo contexto) {
    if (auto b = dynamic_cast<const expresion_termino*>(ex); b != nullptr) {
        if (b->termino.tipo == INT) {
            auto vida = (contexto == RETORNO ? TEMPORAL : VARIABLE);
            return { b->termino, VALOR, vida };
        }
    }
    throw error("Tipo incorrecto", ex->vista);
}


tipo_evaluado evalua(const expresion*& ex, auto&... params) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
    } else if (auto p = dynamic_cast<const expresion_binaria*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
    } else if (auto p = dynamic_cast<const expresion_prefija*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
    } else if (auto p = dynamic_cast<const expresion_llamada_funcion*>(ex.get( )); p != nullptr) {
      return evalua(p, params...);
    } else {
      return { };
    }
}

void evalua(const expresion*& s, auto&... params) {
   if (auto p = dynamic_cast<const sentencia_expresion*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_declaracion*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_if*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_return*>(s.get( )); p != nullptr) {
      return evalua(p, params...);
   }
}

#endif
