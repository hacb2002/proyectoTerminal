#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "semantico.h"
#include <iostream>
#include <string>
#include <vector>

int main( ) {
   std::string entrada;
   std::getline(std::cin, entrada, '\0');
   entrada += '\0';
   try {                                                             // agregué la captura del error para poder ver dónde muere la cosa
      std::vector<token> tokens = lexer(entrada);
      arbol_sintactico arbol = parser(tokens);
      tabla_simbolos tabla = semantico(arbol);
      // std::vector<std::string> instrucciones = codegen(arbol, tabla);
      //
      // for (const auto& s : instrucciones) {
      //    std::cout << s << "\n";
      // }
      for(auto const& token: tokens){
         std::cout << token.tipo << " " << token.vista << "\n";      // metí el .ini y .fin de token en un std::string_view (que guarda lo mismo) pero así ya se puede imprimir directo
      }
   } catch (const error& e) {
      auto [linea, columna] = linea_columna(entrada, e.vista);
      std::cout << "ERROR " << linea + 1 << ":" << columna + 1 << "\n";
      std::cout << e.mensaje << "\n";
      std::cout << e.vista << "\n";
   }
}
