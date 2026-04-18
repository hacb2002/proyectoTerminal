#ifndef PARSER_H
#define PARSER_H

#include "error.h"
#include "parser_aux.h"
#include "parser_sentencia.h"
#include <vector>

struct sentencia_funcion {
   token nombre;
   std::vector<token> parametros;
   std::vector<std::unique_ptr<sentencia>> sentencias;
};

struct arbol_sintactico {
   std::vector<sentencia_funcion> funciones;
};

std::vector<token> parser_lista_parametros(const token*& p){
   std::vector<token> parametros;
   for (;;) {
      if(p->tipo == INT){
         espera(p, INT);
         parametros.emplace_back(espera(p, IDENTIFICADOR));
         if(p->tipo != COMA || (p->tipo == COMA && (p+1)->tipo == PARENTESIS_DER)){
            break;
         }
         espera(p, COMA);
      }else{
         break;
      }
   }
   return parametros;
}

arbol_sintactico parser(const std::vector<token>& tokens) {
   const token* p = &tokens[0];
   arbol_sintactico arbol;

   while (p->tipo != FIN_ARCHIVO) {
      espera(p, INT);
      token nombre = espera(p, IDENTIFICADOR);
      espera(p, PARENTESIS_IZQ);
      std::vector<token> parametros = parser_lista_parametros(p);
      espera(p, PARENTESIS_DER);
      espera(p, LLAVE_IZQ);
      std::vector<std::unique_ptr<sentencia>> sentencias;
      while (p->tipo != LLAVE_DER) {
         sentencias.push_back(parser_sentencia(p));
      }
      espera(p, LLAVE_DER);
      arbol.funciones.emplace_back(nombre, std::move(parametros), std::move(sentencias));
   }
   return arbol;
}

#endif
