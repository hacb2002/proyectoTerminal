#ifndef SEMANTICO_AUX_H
#define SEMANTICO_AUX_H

#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <charconv>
#include <vector>
#include <stddef.h>

/*Lo desglosé para que se entendiera mejor paso a paso, pero igual podemos regresarlo a una sola línea*/
int evalua_entero(const std::string_view& s) {
   int res;
   std::from_chars_result conversion = std::from_chars(s.data(), s.data() + s.size(), res);
   if (conversion.ec == std::errc( )) {
    return res;
   }
   throw error("Entero no representable", s);
}

void verifica(const std::unique_ptr<expresion>& ex, auto&... params) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex.get( )); p != nullptr) {
      return verifica(p, params...);
    } else if (auto p = dynamic_cast<const expresion_binaria*>(ex.get( )); p != nullptr) {
      return verifica(p, params...);
    } else if (auto p = dynamic_cast<const expresion_prefija*>(ex.get( )); p != nullptr) {
      return verifica(p, params...);
    } else if (auto p = dynamic_cast<const expresion_llamada_funcion*>(ex.get( )); p != nullptr) {
      return verifica(p, params...);
    }
}

void verifica(const std::unique_ptr<sentencia>& s, auto&... params) {
   if (auto p = dynamic_cast<const sentencia_expresion*>(s.get( )); p != nullptr) {
      return verifica(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_declaracion*>(s.get( )); p != nullptr) {
      return verifica(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_if*>(s.get( )); p != nullptr) {
      return verifica(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_return*>(s.get( )); p != nullptr) {
      return verifica(p, params...);
   }
}

#endif
