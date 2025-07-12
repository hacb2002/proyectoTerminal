#ifndef CODEGEN_AUX_H
#define CODEGEN_AUX_H

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include <sstream>

std::string concatena(const std::vector<std::string>& v, const char* s) {
   std::ostringstream oss;
   if (!v.empty( )) {
      oss << v.front( );
      for (int i = 1; i < v.size( ); ++i) {
         oss << " " << v[i];
      }
   }
   return oss.str( );
}

std::string concatena(const auto&... v) {
   std::ostringstream oss;
   (oss << ... << v);
   return oss.str( );
}

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
   return -1;
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

int evalua_operador(int izq, int der, auto op, auto& vista, auto& salida){
   if (op == MAS) {
      return izq + der;
   } else if (op == MENOS) {
      return izq - der;
   } else if (op == POR) {
      return izq * der;
   } else if (op == ENTRE || op == MODULO) {
      if (der == 0){
         throw "División entre cero";
      }
      return (op == ENTRE ? izq / der : izq % der);
   } else if (op == MENOR) {
      return izq < der;
   } else if (op == MENOR_IGUAL) {
      return izq <= der;
   } else if (op == MAYOR) {
      return izq > der;
   } else if (op == MAYOR_IGUAL) {
      return izq >= der;
   } else if (op == IGUAL) {
      return izq == der;
   } else if (op == DIFERENTE) {
      return izq != der;
   }
   return -1;
}

#endif
