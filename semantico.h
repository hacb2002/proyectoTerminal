#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "lexer.h"
#include "parser.h"
#include <map>

struct analisis_funcion {
   std::map<const expresion_termino*, int> entero_referido;
   std::map<const expresion_termino*, const sentencia_declaracion*> variable_referida;
   std::map<const expresion_llamada*, const sentencia_funcion*> funcion_referida;
};

struct tabla_simbolos {
   std::map<std::string, analisis_funcion> funciones;
};

struct pila_simbolos {
   const tabla_simbolos& tabla;
   std::vector<std::map<std::string, const sentencia_declaracion*>> bloques(1);

   const analisis_funcion* busca_funcion(const std::string& s) const {
      auto iter = tabla.funciones.find(s);
      return (iter != tabla.funciones.end( ) ? &iter->second : nullptr);
   }

   const sentencia_declaracion* busca_variable(const std::string& s) const {
      for (int i = bloques.size( ) - 1; i >= 0; --i) {
         if (auto iter = bloques[i].find(s); iter != bloques[i].end( )) {
            return iter->second;
         }
      }
      return nullptr;
   }
};

void verifica(const expresion_termino* ex, auto& analisis, auto& pila) {

}

void verifica(const expresion_binaria* ex, auto& analisis, auto& pila) {

}

void verifica(const expresion_prefija* ex, auto& analisis, auto& pila) {

}

void verifica(const expresion_llamada* ex, auto& analisis, auto& pila) {

}

void verifica(const sentencia_expresion* s, auto& analisis, auto& pila) {
   verifica(s->ex, analisis, pila);
}

void verifica(const sentencia_declaracion* s, auto& analisis, auto& pila) {

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

tabla_simbolos semantico(const arbol_sintactico& arbol) {
   tabla_simbolos tabla;
   for (const auto& funcion : arbol.funciones) {
      if (!tabla.funciones.emplace(funcion.nombre.vista).second) {
         throw error("Nombre de funcion repetida", funcion.nombre.vista);
      }

      std::set<std::string> nombres_parametros;
      for (const auto& parametro : funcion.parametros) {
         if (!nombres_parametros.insert(parametro.vista).second) {
            throw error("Nombre de parametro repetido", parametro.vista);
         }
      }

      analisis_funcion& analisis = tabla.funciones[funcion.nombre.vista];
      pila_simbolos pila(tabla);
      for (const auto& parametro : funcion.parametros) {
         pila.bloques[0][parametro.vista] = ???;
      }
      for (const auto& sentencia : funcion.sentencias) {
         verifica(sentencia, analisis, pila);
      }

      if (!tabla.funciones.emplace(funcion.nombre.vista, datos_funcion(funcion, std::move(analisis))).second) {
         throw error("Nombre de funcion repetida", funcion.nombre.vista);
      }
   }

   return tabla;
}

#endif
