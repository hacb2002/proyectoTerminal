#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "parser_aux.h"
#include "parser_expresion.h"
#include "parser_sentencia.h"
#include <vector>

struct declaracion_funcion {
   std::unique_ptr<expresion> retorno;
   const token& nombre;
   std::vector<std::unique_ptr<sentencia_declaracion>> parametros;
   std::vector<std::unique_ptr<sentencia>> cuerpo;
};

struct arbol_sintactico {
   std::vector<declaracion_funcion> funciones;
};

declaracion_funcion parser_funcion(const token*& p) {
   //Pendiente de explicar o entender. 
}

arbol_sintactico parser(const std::vector<token>& tokens) {
   auto p = &tokens[0];
   arbol_sintactico arbol;
   while (p->tipo != FIN_ARCHIVO) {
      arbol.funciones.push_back(parser_funcion(p));
   }
   return arbol;
}

#endif
