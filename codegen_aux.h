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
   return -1; //Se agrega para evitar los warning en la compilación. 
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

int evaluar_operador(int& izq, int& der, const expresion_binaria* ex, auto& analisis, auto& estado){
   auto op = ex->operador.tipo;
   if (op == MAS) {
   return izq + der;
   } else if (op == MENOS) {
   return izq - der;
   } else if (op == POR) {
   return izq * der;
   } else if (op == ENTRE) {
   if (der == 0) throw error("División por cero en expresión binaria", ex->operador.vista);
   return izq / der;
   } else if (op == MODULO) {
   if (der == 0) throw error("Módulo por cero en expresión binaria", ex->operador.vista);
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
   } else if (op == ASIGNACION) {
      auto pt = dynamic_cast<const expresion_termino*>(ex->izq);
      if (!pt || pt->termino.tipo != IDENTIFICADOR) {
         throw error("Lado izquierdo de asignación no es variable", ex->operador.vista);
      }
      const token* varTok = analisis.variable_referida.find(pt)->second;
      estado[varTok] = der;
      return der;
   }
   throw error("Operador binario desconocido en evalua", ex->operador.vista);
}

#endif
