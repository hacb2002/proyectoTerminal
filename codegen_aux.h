#ifndef CODEGEN_AUX_H
#define CODEGEN_AUX_H

#include "parser.h"

int evalua(const expresion* ex, auto&... params) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex); p != nullptr) {
      return evalua(p, params...);
    } else if (auto p = dynamic_cast<const expresion_binaria*>(ex); p != nullptr) {
      return evalua(p, params...);
    } else if (auto p = dynamic_cast<const expresion_prefija*>(ex); p != nullptr) {
      return evalua(p, params...);
    } else if (auto p = dynamic_cast<const expresion_llamada_funcion*>(ex); p != nullptr) {
      return evalua(p, params...);
    }
}

void evalua(const sentencia* s, auto&... params) {
   if (auto p = dynamic_cast<const sentencia_expresion*>(s); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_declaracion*>(s); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_if*>(s); p != nullptr) {
      return evalua(p, params...);
   } else if (auto p = dynamic_cast<const sentencia_return*>(s); p != nullptr) {
      return evalua(p, params...);
   }
}

#endif
