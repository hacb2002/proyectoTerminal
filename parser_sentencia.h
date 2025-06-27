#ifndef PARSER_SENTENCIA_H
#define PARSER_SENTENCIA_H

struct sentencia {
   virtual ~sentencia( ) = 0;
};

struct sentencia_if : sentencia {
   expresion* condicion;
   std::vector<sentencia> parte_si;
   std::vector<sentencia> parte_no;
};

struct sentencia_return : sentencia {
   expresion* valor;
};

struct sentencia_declaracion : sentencia {
   token tipo;
   std::vector<token> nombres;
   std::vector<expresion*> inicializadores;
};

struct sentencia_expresion : sentencia {
   expresion* ex;
};

void espera(const token*& p, lexema x) {
   if (p->tipo == x) {
      ++p;
   } else {
      throw "ERROR";
   }
}

sentencia* parser_sentencia(const token*& p) {
   if (p->tipo == IF) {
      auto res = new sentencia_if( );
      ++p;
      espera(p, PARENTESIS_IZQ);
      res->condicion = parsea_expresion(p);
      espera(p, PARENTESIS_DER);
      espera(p, LLAVE_IZQ);
      //...
      espera(p, LLAVE_DER);
      if (p->tipo == ELSE && (p + 1)->tipo == IF) {
         res->parte_no.push_back(parser_sentencia(++p));
      }
   } else if (p->tipo == RETURN) {

   } else if (p->tipo == INT) {

   } else {

   }
}

#endif

