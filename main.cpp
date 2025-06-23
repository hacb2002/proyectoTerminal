#include "lexer.h"
#include <iostream>
#include <string>
#include <vector>

int main( ) {
   std::string entrada;
   std::getline(std::cin, entrada, '\0');
   entrada += '\0';

   std::vector<token> tokens = lexer(entrada);
   // arbol_sintaxis arbol = parser(tokens);
   // tabla_simbolos tabla = semantico(arbol);
   // std::vector<std::string> instrucciones = codegen(arbol, tabla);
   //
   // for (const auto& s : instrucciones) {
   //    std::cout << s << "\n";
   // }
}
