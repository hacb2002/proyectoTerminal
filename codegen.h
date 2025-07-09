#ifndef CODEGEN_H
#define CODEGEN_H

#include "error.h"
#include "codegen_aux.h"
#include "parser.h"
#include "semantico.h"
#include "debug.h"
#include <map>
#include <vector>
#include <string>

int evalua(const expresion_termino* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   if (ex->termino.tipo == LITERAL_ENTERA) {
      return analisis.entero_referido.find(ex)->second;
   } else if (ex->termino.tipo == IDENTIFICADOR) {
      return estado[analisis.variable_referida.find(ex)->second];
   }
   throw error("Error en el dato", ex->termino.vista);
}

int evalua(const expresion_binaria* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   int izq = evalua(ex->izq, analisis, tabla, estado, salida);
   if (ex->operador.tipo == OR) {
   return izq != 0 ? 1 : evalua(ex->der, analisis, tabla, estado, salida);
   } else if (ex->operador.tipo == AND) {
   return izq == 0 ? 0 : evalua(ex->der, analisis, tabla, estado, salida);
   }
   int der = evalua(ex->der, analisis, tabla, estado, salida);
   return evaluar_operador(izq, der, ex, analisis, estado);
}


int evalua(const expresion_prefija* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   int val = evalua(ex->ex, analisis, tabla, estado, salida);
   if(ex->operador.tipo == NOT){
      return val == 0 ? 1 : 0;
   }else if(ex->operador.tipo == MENOS){
      return -val;
   }
   return val;
}

int evalua(const expresion_llamada_funcion* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   std::vector<int> argumentos;
   for (const expresion* ex : ex->argumentos) {
      argumentos.push_back(evalua(ex, analisis, tabla, estado, salida));
   }
   return llama(analisis.funcion_referida.find(ex)->second->nombre.vista, argumentos, tabla, salida);
}

void evalua(const sentencia_expresion* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   evalua(s->ex, analisis, tabla, estado, salida);
}

void evalua(const sentencia_declaracion* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   for (int i = 0; i < s->nombres.size( ); ++i) {
      int valor = evalua(s->inicializadores[i], analisis, tabla, estado, salida);
      salida.push_back("DECLARA " + std::string(s->nombres[i].vista) + "#" + std:to_string((std::size_t)&s->nombres[i]) + ":" + std::to_string(valor));
      estado[&s->nombres[i]] = valor;
   }
}

void evalua(const sentencia_if* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   auto parte = (evalua(s->condicion, analisis, tabla, estado, salida) != 0 ? &s->parte_si : &s->parte_no);
   for (const auto& actual : *parte) {
      evalua(actual, analisis, tabla, estado, salida);
   }
}

void evalua(const sentencia_return* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   int r = evalua(s->valor, analisis, tabla, estado, salida);
   salida.push_back("REGRESA " + std::to_string(r));
   throw r;
}

int llama(const std::string_view& nombre, auto& argumentos, const auto& tabla, auto& salida) {
   const auto& analisis = tabla.funciones.find(nombre)->second;
   auto decl = analisis.declaracion;
   std::map<const token*, int> estado;
   std::string comando_llamada = "LLAMA " + nombre;
   for (int i = 0; i < decl->parametros.size( ); ++i) {
      comando_llamada += " ";
      comando_llamada += decl->parametros[i].vista;
      comando_llamada += ":";
      comando_llamada += std::to_string(argumentos[i]);
      estado[&decl->parametros[i]] = argumentos[i];
   }
   salida.push_back(comando_llamada);

   try {
      for (const sentencia* s : decl->sentencias) {
         evalua(s, analisis, tabla, estado, salida);
      }
   } catch (int r) {
      return r;
   }
   salida.push_back("ERROR NO_RETORNO");
   return 0xDEADBEEF;
}

std::vector<std::string> codegen(const arbol_sintactico& arbol, const tabla_simbolos& tabla) {
   std::vector<std::string> salida;
   int res = llama(tabla.funcion_inicial, tabla.argumentos_inicial, tabla, salida);
   salida.push_back("Resultado: " + std::to_string(res));
   return salida;
}

#endif
