#ifndef DEBUG_H
#define DEBUG_H

#include "lexer.h"
#include "parser.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const token& t) {
   return os << t.vista << " " << t.tipo;
}

std::ostream& operator<<(std::ostream& os, const expresion* ex);

std::ostream& operator<<(std::ostream& os, const std::vector<expresion*>& lista) {
   os << "\n\tArgumentos de funcion: \n";
   for (int i = 0; i < lista.size( ); ++i) {
      os << lista[i] << (i + 1 < lista.size( ) ? "," : "");
   }
   os << "\n";
   return os;
}

std::ostream& operator<<(std::ostream& os, const expresion* ex) {
   if (auto p = dynamic_cast<const expresion_termino*>(ex); p != nullptr) {
      return os << p->termino.vista;
   } else if (auto p = dynamic_cast<const expresion_binaria*>(ex); p != nullptr) {
      return os << "(" << p->izq << p->operador.vista << p->der << ")";
   } else if (auto p = dynamic_cast<const expresion_prefija*>(ex); p != nullptr) {
      return os << p->operador.vista << "(" << p->ex << ")";
   } else if (auto p = dynamic_cast<const expresion_llamada_funcion*>(ex); p != nullptr) {
      return os << p->nombre.vista << "(" << p->argumentos << ")";
   } else {
      return os;
   }
}

std::ostream& operator<<(std::ostream& os, const sentencia* ex);

std::ostream& operator<<(std::ostream& os, const std::vector<sentencia*> lista) {
   for (int i = 0; i < lista.size( ); ++i) {
      os << lista[i] << "\n";
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, const sentencia_declaracion*& s) {
   os << "\tVariables declaradas en una linea: \n";
   if (s != nullptr) {
      for(int i = 0; i<s->nombres.size(); ++i){
         os << s->nombres[i].vista 
            << (s->inicializadores[i] != nullptr ? " = " : " ") 
            << s->inicializadores[i]
            << "\n";
      }
   }
   return os;
};
std::ostream& operator<<(std::ostream& os, const sentencia* s) {
   if (auto p = dynamic_cast<const sentencia_expresion*>(s); p != nullptr) {
      return os << p->ex << ";";
   } else if (auto p = dynamic_cast<const sentencia_declaracion*>(s); p != nullptr) {
      return os << reinterpret_cast<const sentencia_declaracion*&>(s) << ";";
   } else if (auto p = dynamic_cast<const sentencia_if*>(s); p != nullptr) {
      return os << "if " << p->condicion << "{\n" << p->parte_si << "} else {\n" << p->parte_no << "}";
   } else if (auto p = dynamic_cast<const sentencia_return*>(s); p != nullptr) {
      return os << "return " << p->valor << ";";
   } else {
      return os;
   }
}

std::ostream& operator<<(std::ostream& os, const sentencia_funcion& f) {
   os << "int " << f.nombre.vista << "(";
   for (int i = 0; i < f.parametros.size( ); ++i) {
      os << f.parametros[i] << " " << (i + 1 < f.parametros.size( ) ? "," : "");
   }
   os << ") {\n";
   for (const auto& s : f.sentencias) {
      os << s << "\n";
   }
   os << "}";
   return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<token>& tokens) {
   for (const auto& t : tokens) {
      os << t << "\n";
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, const arbol_sintactico& arbol) {
   for (const auto& f : arbol.funciones) {
      os << f << "\n";
   }
   return os;
}

#endif
