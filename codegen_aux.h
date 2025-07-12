#ifndef CODEGEN_AUX_H
#define CODEGEN_AUX_H

#include "lexer.h"
#include "parser.h"
#include "error.h"

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

int evaluar_operador(int izq, int der, auto op, auto& vista, auto& salida){
   if (op == MAS) {
      return izq + der;
   } else if (op == MENOS) {
      return izq - der;
   } else if (op == POR) {
      return izq * der;
   } else if (op == ENTRE) {
      if (der == 0){
         //Error y se regresa -1 para que continue. 
         salida.push_back("ERROR DIV0");
         return -1;
      }
      return izq / der;
   } else if (op == MODULO) {
      if (der == 0){
         //Error en ejecucion. 
         salida.push_back("ERROR MOD0");
         return -1;
      }
      return izq % der;
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
