#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <string_view>
#include <utility>

struct error {
   const char* mensaje;
   std::string_view vista;
   bool fuera_archivo;
   error(const char* m, const char* p)
   : mensaje(m), vista(p) {
   }

   error(const char* m, const char* ini, const char* fin)
   : mensaje(m), vista(ini, fin - ini) {
   }

   error(const char* m, const std::string_view& v)
   : mensaje(m), vista(v) {
   }

   error(const char* m, const char* p, bool a)
   : mensaje(m), vista(p), fuera_archivo(a) {
   }

};

std::pair<int, int> linea_columna(const std::string& s, const std::string_view& v) {
   int linea = 0, columna = 0;
   for (auto p = &s[0]; p != v.begin( ); ++p) {
      if (*p == '\n') {
         linea += 1, columna = 0;
      } else {
         columna += 1;
      }
   }
   return { linea, columna };
}

#endif
