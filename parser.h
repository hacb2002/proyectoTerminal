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

/* 
¿Bastará con solo hacer eso?

¿Se tiene que hacer una distinción entre int, 
identificador y la coma o eso se debe tomar 
en otro modulo del programa?

*/
std::vector<token> parser_lista_parametros(const token* p){
   std::vector<token> parametros;
   while(p->tipo != PARENTESIS_DER){
      parametros.emplace_back(*(p++));
   }
   return parametros;
}

arbol_sintactico parser(const std::vector<token>& tokens) {
   const token* p = &tokens[0];
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
      arbol.funciones.emplace_back(nombre, std::move(parametros), std::move(sentencias));
   }
   return arbol;
}

#endif