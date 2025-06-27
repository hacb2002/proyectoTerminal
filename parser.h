#ifndef PARSER_H
#define PARSER_H

#include "parser_aux.h"
#include "parser_sentencia.h"
#include <vector>

struct funcion {
   token nombre;
   std::vector<token> parametros;
   std::vector<sentencia*> sentencias;
};

struct arbol_sintactico {
   std::vector<funcion> funciones;
};

arbol_sintactico parser(const std::vector<token>& tokens) {
   const token* p = tokens[0];
   arbol_sintactico arbol;

   while (p->tipo != FIN_ARCHIVO) {
      espera(p, INT);
      token nombre = *espera(p, IDENTIFICADOR);
      espera(p, PARENTESIS_IZQ);
      std::vector<token> parametros = parser_lista_parametros(p);
      espera(p, PARENTESIS_DER);
      espera(p, LLAVE_IZQ);
      std::vector<sentencia*> sentencias;
      while (p->tipo != LLAVE_DER) {
         sentencias.push_back(parser_sentencia(p));
      }
      espera(p, LLAVE_DER);
      arbol.emplace_back(nombre, std::move(parametros), std::move(sentencias));
   }

   return arbol;
}

#endif
