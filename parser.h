#ifndef PARSER_H
#define PARSER_H

#include "parser_sentencia.h"
#include <vector>

struct parametro {
   token tipo;
   token nombre;
};

struct funcion {
   token tipo_retorno;
   token nombre;
   std::vector<parametro> parametros;
   std::vector<sentencia*> sentencias;
};

struct arbol_sintactico {
   std::vector<funcion> funciones;
};

arbol_sintactico parser(const std::vector<token>& tokens) {
   //...
   //
   //
   //
   //
}

#endif
