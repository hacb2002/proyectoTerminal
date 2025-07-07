#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "lexer.h"
#include "parser.h"
#include "semantico_aux.h"
#include <set>
#include <map>

struct analisis_funcion {
   const sentencia_funcion* declaracion;
   std::map<const expresion_termino*, int> entero_referido;
   std::map<const expresion_termino*, const token*> variable_referida;
   std::map<const expresion_llamada_funcion*, const sentencia_funcion*> funcion_referida;
};

struct tabla_simbolos {
   std::map<std::string_view, analisis_funcion> funciones;
   std::string_view funcion_inicial;
   std::vector<int> argumentos_inicial;
};

struct pila_simbolos {
   const tabla_simbolos& tabla;
   std::vector<std::map<std::string_view, const token*>> bloques{1};

   const analisis_funcion* busca_funcion(const std::string_view& s) const {
      auto iter = tabla.funciones.find(s);
      return (iter != tabla.funciones.end( ) ? &iter->second : nullptr);
   }

   const token* busca_variable(const std::string_view& s) const {
      for (int i = bloques.size( ) - 1; i >= 0; --i) {
         if (auto iter = bloques[i].find(s); iter != bloques[i].end( )) {
            return iter->second;
         }
      }
      return nullptr;
   }
};

void verifica(const expresion_termino* ex, auto& analisis, auto& pila) {
   if (ex->termino.tipo == LITERAL_ENTERA) {
      analisis.entero_referido[ex] = evalua_entero(ex->termino.vista);
   } else if (ex->termino.tipo == IDENTIFICADOR) {
      const token* t = pila.busca_variable(ex->termino.vista);
      if (t != nullptr) {
         analisis.variable_referida[ex] = t;
      } else if (pila.busca_funcion(ex->termino.vista) != nullptr) {
         throw error("Función usada como variable en expresión", ex->termino.vista);
      } else {
         throw error("Variable no declarada", ex->termino.vista);
      }
   }
}

void verifica(const expresion_binaria* ex, auto& analisis, auto& pila) {
   verifica(ex->izq, analisis, pila);
   verifica(ex->der, analisis, pila);
}

void verifica(const expresion_prefija* ex, auto& analisis, auto& pila) {
   verifica(ex->ex, analisis, pila);
}

void verifica(const expresion_llamada_funcion* ex, auto& analisis, auto& pila) {
   if (pila.busca_variable(ex->nombre.vista) != nullptr) {
      throw error("Variable usada como función", ex->nombre.vista);
   }

   const analisis_funcion* p = pila.busca_funcion(ex->nombre.vista);
   if (p == nullptr) {
      throw error("Función no declarada", ex->nombre.vista);
   } else if (ex->argumentos.size( ) != p->declaracion->parametros.size( )) {
      throw error("Número incorrecto de argumentos", ex->nombre.vista);
   }

   analisis.funcion_referida[ex] = p->declaracion;
   for (const expresion* ex : ex->argumentos) {
      verifica(ex, analisis, pila);
   }
}

void verifica(const sentencia_expresion* s, auto& analisis, auto& pila) {
   verifica(s->ex, analisis, pila);
}

void verifica(const sentencia_declaracion* s, auto& analisis, auto& pila) {
   for (int i = 0; i < s->nombres.size( ); ++i) {
      if (!pila.bloques.back( ).emplace(s->nombres[i].vista, &s->nombres[i]).second) {
         throw error("Redeclaracion de variable", s->nombres[i].vista);
      }
      verifica(s->inicializadores[i], analisis, pila);
   }
}

void verifica(const sentencia_if* s, auto& analisis, auto& pila) {
   verifica(s->condicion, analisis, pila);
   for (auto parte : { &s->parte_si, &s->parte_no }) {
      pila.bloques.emplace_back( );
      for (const auto& actual : *parte) {
         verifica(actual, analisis, pila);
      }
      pila.bloques.pop_back( );
   }
}

void verifica(const sentencia_return* s, auto& analisis, auto& pila) {
   verifica(s->valor, analisis, pila);
}

tabla_simbolos semantico(const arbol_sintactico& arbol, const char* argv[]) {
   tabla_simbolos tabla;
   for (const auto& funcion : arbol.funciones) {
      if (tabla.funciones.contains(funcion.nombre.vista)) {
         throw error("Nombre de funcion repetida", funcion.nombre.vista);
      }

      std::set<std::string_view> nombres_parametros;
      for (const auto& parametro : funcion.parametros) {
         if (!nombres_parametros.emplace(parametro.vista).second) {
            throw error("Nombre de parametro repetido", parametro.vista);
         }
      }

      analisis_funcion& analisis = tabla.funciones[funcion.nombre.vista];
      analisis.declaracion = &funcion;
      pila_simbolos pila(tabla);
      for (const token& parametro : funcion.parametros) {
         pila.bloques[0][parametro.vista] = &parametro;
      }
      for (const sentencia* s : funcion.sentencias) {
         verifica(s, analisis, pila);
      }
   }

   tabla.funcion_inicial = argv[1];
   if (!tabla.funciones.contains(argv[1])) {
      throw error("Funcion de inicio no declarada", argv[1]);
   }
   for (int i = 2; argv[i] != nullptr; ++i) {
      tabla.argumentos_inicial.push_back(evalua_entero(argv[i]));
   }
   if (tabla.argumentos_inicial.size( ) != tabla.funciones[argv[1]].declaracion->parametros.size( )) {
      throw error("Numero incorrecto de argumentos", argv[1]);
   }

   return tabla;
}

#endif
