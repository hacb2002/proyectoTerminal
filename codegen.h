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

struct estado_interprete {
   std::map<const token*, int> variables;
   std::vector<std::vector<const sentencia_declaracion*>> pila{1};
};

struct salida_interprete {
   const std::string& entrada;
   std::vector<std::string> instrucciones;

   salida_interprete(const std::string& e)
   : entrada(e) {
   }

   void emite(const char* tipo_instruccion, const std::string_view& vista, const std::string& datos) {
      auto [linea, columna] = linea_columna(entrada, vista);
      instrucciones.push_back(concatena(tipo_instruccion, " ", linea + 1, ":", columna + 1, " ", datos));
   }

   void emite(const char* tipo_instruccion, std::nullptr_t llamada_inicial, const std::string& datos) {
      instrucciones.push_back(concatena(tipo_instruccion, " -:- ", datos));
   }
};

struct error_ejecucion {
   const char* mensaje;
   std::string_view vista;

   error_ejecucion(const char* m, const std::string_view& v)
   : mensaje(m), vista(v) {
   }
};

int evalua(const expresion_termino* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   if (ex->termino.tipo == LITERAL_ENTERA) {
      return analisis.entero_referido.find(ex)->second;
   } else if (ex->termino.tipo == IDENTIFICADOR) {
      return estado.variables[analisis.variable_referida.find(ex)->second];
   }
   return -1;
}

int evalua(const expresion_binaria* ex, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   int izq = evalua(ex->izq, analisis, tabla, estado, salida);
   if (ex->operador.tipo == OR) {
      return izq != 0 ? 1 : evalua(ex->der, analisis, tabla, estado, salida);
   } else if (ex->operador.tipo == AND) {
      return izq == 0 ? 0 : evalua(ex->der, analisis, tabla, estado, salida);
   }
   int der = evalua(ex->der, analisis, tabla, estado, salida);
   try {
      return evalua_operador(izq, der, ex->operador.tipo, ex->vista, salida);
   } catch (const char* mensaje_error) {
      throw error_ejecucion(mensaje_error, ex->vista);
   }
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
   for (const std::unique_ptr<expresion>& ex : ex->argumentos) {
      argumentos.push_back(evalua(ex, analisis, tabla, estado, salida));
   }
   return llama(ex, analisis.funcion_referida.find(ex)->second->nombre.vista, argumentos, tabla, salida);
}

void evalua(const sentencia_expresion* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   evalua(s->ex, analisis, tabla, estado, salida);
}

void evalua(const sentencia_declaracion* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   estado.pila.back( ).push_back(s);
   for (int i = 0; i < s->nombres.size( ); ++i) {
      int valor = evalua(s->inicializadores[i], analisis, tabla, estado, salida);
      salida.emite("DECLARA", s->vista, concatena(s->nombres[i].vista, "#", (std::size_t)&s->nombres[i], "=", valor));
      estado.variables[&s->nombres[i]] = valor;
   }
}

void evalua(const sentencia_if* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   auto parte = (evalua(s->condicion, analisis, tabla, estado, salida) != 0 ? &s->parte_si : &s->parte_no);
   estado.pila.emplace_back( );
   for (const auto& actual : *parte) {
      evalua(actual, analisis, tabla, estado, salida);
   }
   for (const sentencia_declaracion* s : estado.pila.back( )) {
      for (int i = 0; i < s->nombres.size( ); ++i) {
         salida.emite("DESTRUYE", s->vista, concatena(s->nombres[i].vista, "#", (std::size_t)&s->nombres[i]));
      }
   }
   estado.pila.pop_back( );
}

void evalua(const sentencia_return* s, auto& analisis, auto& tabla, auto& estado, auto& salida) {
   int r = evalua(s->valor, analisis, tabla, estado, salida);
   salida.emite("REGRESA", s->vista, std::to_string(r));
   throw r;
}

int llama(const expresion_llamada_funcion* ex, const std::string_view& nombre, auto& argumentos, const auto& tabla, auto& salida) {
   const auto& analisis = tabla.funciones.find(nombre)->second;
   auto decl = analisis.declaracion;
   estado_interprete estado;
   std::vector<std::string> datos_llamada = { std::string(nombre) };
   for (int i = 0; i < decl->parametros.size( ); ++i) {
      datos_llamada.push_back(concatena(decl->parametros[i].vista, "#", (std::size_t)&decl->parametros[i].vista, "=", argumentos[i]));
      estado.variables[&decl->parametros[i]] = argumentos[i];
   }
   if (ex == nullptr) {
      salida.emite("LLAMA", nullptr, concatena(datos_llamada, " "));
   } else {
      salida.emite("LLAMA", ex->vista, concatena(datos_llamada, " "));
   }

   try {
      for (const std::unique_ptr<sentencia>& s : decl->sentencias) {
         evalua(s, analisis, tabla, estado, salida);
      }
      throw error_ejecucion("Fin de función sin valor devuelto", decl->nombre.vista);
   } catch (int r) {
      return r;
   }
}

std::vector<std::string> codegen(const std::string& entrada, const arbol_sintactico& arbol, const tabla_simbolos& tabla) {
   salida_interprete salida(entrada);
   try {
      int res = llama(nullptr, tabla.funcion_inicial, tabla.argumentos_inicial, tabla, salida);
   } catch (const error_ejecucion& e) {
      salida.emite("ERROR_EJECUCION", e.vista, e.mensaje);
   }
   return salida.instrucciones;
}

#endif
