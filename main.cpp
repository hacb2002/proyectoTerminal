#include "codegen.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "debug.h"
#include "semantico.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

int main(int argc, const char* argv[]) {
   if (argc < 2) {
      std::cout << "Uso: " << argv[0] << " funcion v1 v2 ...\n";
      return 0;
   }
   std::ifstream ifs("codigo.txt");
   if (!ifs) {
      std::cerr << "No se pudo abrir el codigo\n";
      return 1;
   }
   std::string entrada((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
   entrada.push_back('\0');
   try {                                                             // agregué la captura del error para poder ver dónde muere la cosa
      std::vector<token> tokens = lexer(entrada);
      arbol_sintactico arbol = parser(tokens);
      tabla_simbolos tabla = semantico(arbol, argv);
      std::vector<std::string> instrucciones = codegen(entrada, arbol, tabla);
      //std::cout << tokens;
      //std::cout << arbol;
      //std::cout << tabla;
      //Imprimir el valor de retorno de la funcion.
      std::cout << instrucciones;
   } catch (const error& e) {
      if(entrada.data( ) <= e.vista.begin( ) && e.vista.end( ) <= entrada.data( ) + entrada.size( )) {
         auto [linea, columna] = linea_columna(entrada, e.vista);
         std::cout << "ERROR " << linea + 1 << ":" << columna + 1 << "\n";
         std::cout << e.mensaje << "\n";
         std::cout << e.vista << "\n";
      }else{
         std::cout << "ERROR argv\n";
         std::cout << e.mensaje << "\n";
         std::cout << e.vista << "\n";
      }
   }

}
