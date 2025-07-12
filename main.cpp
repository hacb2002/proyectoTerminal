#include "codegen.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "debug.h"
#include "semantico.h"
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <vector>
#include <ctype.h>

#ifdef __EMSCRIPTEN__
   #include <emscripten.h>
#endif

namespace impl {
   const char* traduce(std::string&& entrada, const std::span<std::string_view>& args) {
      if (entrada.empty( ) || entrada.back( ) != '\0') {
         entrada += '\0';
      }

      std::ostringstream oss;
      try {                                                             // agregué la captura del error para poder ver dónde muere la cosa
         std::vector<token> tokens = lexer(entrada);
         arbol_sintactico arbol = parser(tokens);
         tabla_simbolos tabla = semantico(arbol, args);
         std::vector<std::string> instrucciones = codegen(entrada, arbol, tabla);
         //std::cout << tokens;
         //std::cout << arbol;
         //std::cout << tabla;
         //Imprimir el valor de retorno de la funcion.
         oss << instrucciones;
      } catch (const error& e) {
         if(entrada.data( ) <= e.vista.data( ) && e.vista.data( ) + e.vista.size( ) <= entrada.data( ) + entrada.size( )) {
            auto [linea, columna] = linea_columna(entrada, e.vista);
            oss << "ERROR " << linea + 1 << ":" << columna + 1 << "\n";
            oss << e.mensaje << "\n";
            oss << e.vista << "\n";
         }else{
            oss << "ERROR argv\n";
            oss << e.mensaje << "\n";
            oss << e.vista << "\n";
         }
      }

      static std::string salida;
      salida = std::move(oss).str( );
      return salida.c_str( );
   }
}

#ifdef __EMSCRIPTEN__
   extern "C" EMSCRIPTEN_KEEPALIVE
#endif
const char* traduce(const char* entrada_cstr, const char* argv_cstr) {
   std::vector<std::string_view> args;
   std::string_view argv_v(argv_cstr);
   auto ini = argv_v.begin( ), fin = argv_v.end( );
   for (;;) {
      ini = std::find_if(ini, fin, std::not_fn(::isspace));
      if (ini == fin) {
         break;
      }
      auto t = std::find_if(ini, fin, ::isspace);
      args.emplace_back(ini, t);
      ini = t;
   }
   return impl::traduce(entrada_cstr, args);
}

#ifndef __EMSCRIPTEN__
   int main(int argc, const char* argv[]) {
      if (argc < 3) {
         std::cout << "Uso: " << argv[0] << " archivo funcion v1 v2 ...\n";
         return 0;
      }
      std::ifstream ifs(argv[1]);
      if (!ifs) {
         std::cerr << "No se pudo abrir el codigo\n";
         return 1;
      }
      std::vector<std::string_view> args(&argv[2], &argv[2] + argc - 2);
      std::cout << impl::traduce(std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()), std::span(args.begin( ), args.end( )));
   }
#endif
