#ifndef BWT_H_INCLUDED
#define BWT_H_INCLUDED

#include "utils.h"

char* bwtEncode(const char* word);
char* bwtDecode(const char* bwt);
char* bwts(const char* bwt);
char* ibwts(const char* bwt);
char* bwtz (const char* word);
char* ibwtz (const char* word);


node_t* getSplittedFactors (const char* word, size_t len);
size_t getBorderSize(const char* w);
char** conjTableICFL(const char* word);
void sortConjTableICFL(char** conjTable, int n);
#endif // BWT_H_INCLUDED
