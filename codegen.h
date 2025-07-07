#ifndef CODEGEN_H
#define CODEGEN_H

#include "codegen_aux.h"
#include "parser.h"
#include "semantico.h"
#include <map>
#include <vector>
#include <string>

int evalua(const expresion_termino* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   if (ex->termino.tipo == LITERAL_ENTERA) {
      return analisis.entero_referido.find(ex)->second;
   } else if (ex->termino.tipo == IDENTIFICADOR) {
      return estado[analisis.variable_referida.find(ex)->second];
   }
}

int evalua(const expresion_binaria* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   //evalua(ex->izq, analisis, pila);
   //evalua(ex->der, analisis, pila);
}

int evalua(const expresion_prefija* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   //evalua(ex->ex, analisis, pila);
}

int evalua(const expresion_llamada_funcion* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   std::vector<int> argumentos;
   for (const expresion* ex : ex->argumentos) {
      argumentos.push_back(evalua(ex, analisis, tabla, estado));
   }
   return evalua_llamada(analisis.funcion_referida.find(ex)->second->nombre.vista, argumentos, tabla, salida);
}

void evalua(const sentencia_expresion* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   evalua(s->ex, analisis, tabla, estado, salida);
}

void evalua(const sentencia_declaracion* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   for (int i = 0; i < s->nombres.size( ); ++i) {
      estado[&s->nombres[i]] = evalua(s->inicializadores[i], analisis, tabla, estado, salida);
   }
}

void evalua(const sentencia_if* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   auto parte = (evalua(s->condicion, analisis, tabla, estado) != 0 ? &s->parte_si : &s->parte_no);
   for (const auto& actual : *parte) {
      evalua(actual, analisis, tabla, estado, salida);
   }
}

void evalua(const sentencia_return* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   throw evalua(s->valor, analisis, tabla, estado, salida);
}

int evalua_llamada(const std::string_view& nombre, const std::vector<int>& argumentos, auto& tabla, auto& salida) {
   auto& analisis = tabla.funciones.find(nombre)->second;
   auto decl = analisis.declaracion;
   std::map<const token*, int> estado;
   for (int i = 0; i < decl->parametros.size( ); ++i) {
      estado[&decl->parametros[i]] = argumentos[i];
   }
   try {
      for (const sentencia* s : decl->sentencias) {
         evalua(s, analisis, tabla, estado, salida);
      }
   } catch (int r) {
      return r;
   }
   return 0xDEADBEEF;
}

std::vector<std::string> codegen(const arbol_sintactico& arbol, const tabla_simbolos& tabla) {
   std::vector<std::string> salida;
   evalua_llamada(tabla.funcion_inicial, tabla.argumentos_inicial, tabla, salida);
   return salida;
}

#endif
