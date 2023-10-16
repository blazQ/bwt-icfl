#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "factorizations.h"
#include "utils.h"
#include "cylist.h"

/**
        TRADITIONAL BWT RELATED FUNCTIONS
**/

char** conjTable(const char* word){
  size_t len = strlen(word);
  char** table = malloc(len*sizeof(*table));
  for(size_t i = 0; i<len; i++) {
    table[i] = malloc(len + 1);
    memcpy(table[i], word + len -i, i);
    memcpy(table[i] + i, word, len - i);
    table[i][len] = '\0';
  }
  return table;
}

static int lexiCompare(const void* a, const void* b){
    return strcmp(*(const char**)a, *(const char**)b);
}

void sortConjTable(char** conjTable, int n){
    qsort(conjTable, n, sizeof(const char*), lexiCompare);
}

/**
    Assuming \0 terminated
**/
char* last(char* word){
    return &word[strlen(word)-1];
}

/**
    Adds a '$' character to denote EOS. It assumes '$' is not present in the word. If you need to transform a word that contains '$', use a different EOS symbol.
**/
char* bwtEncode(const char* word){
    size_t length = strlen(word);
    char* wordMarker = malloc(length+2);
    memcpy(wordMarker, word, length);
    wordMarker[length] = '$';
    wordMarker[length+1] = '\0';

    char** table = conjTable(wordMarker);

    sortConjTable(table, length+1);

    char* bwtWord = malloc(length+2);
    bwtWord[0] = '\0';

    for(size_t i=0; i<length+1; i++){
        strncat(bwtWord, last(table[i]), 1);
    };
    for(size_t i=0; i<length; i++){
        free(table[i]);
    }
    free(table);
    free(wordMarker);
    return bwtWord;
}

char* bwtDecode(const char* bwt){
     //allocating tabella
     size_t len = strlen(bwt);
     char** table = malloc(len*sizeof(*table));
     for(size_t t = 0; t < len; t++) {
        table[t] = calloc(len + 1, sizeof(char));
        /* calloc must be called to correctly initialize*/
     }

     //initializing table
     //inner for: moving column right and filling first column
     //outer for: sorting table
     for(size_t i=0; i<len; i++){
            for(size_t j=0; j<len; j++){
                memmove(table[j] +1, table[j], len);
                table[j][0] = bwt[j];
            }
           sortConjTable(table, len);
     }

     int returnIndex = 0;
     for(size_t j = 0; j<len; j++)
        if((table[j][len-1] - '$') == 0) {
            returnIndex = j;
            break;
        }

    //copy string to return
    char* decodedString = malloc(len);
    strncpy(decodedString, table[returnIndex], len-1);
    decodedString[len-1] = '\0';

    //clean memory
    for(size_t i=0; i<len; i++){
        free(table[i]);
    }
    free(table);

    return decodedString;
}



/**
        BIJECTIVE BWTS (Scott) RELATED FUNCTIONS
**/


/**
        getinfCmpKey returns the context of order "key" of a word s.
        For example, let strlen(s) be 3 (oldSize), and key = 5. A string "sKey" 5 characters long will be built,
        containing s + s[1] + s[2], that is s + the repeating 5-3 = 2 characters.
**/
char* getInfCmpKey(const char *s, size_t key, size_t oldSize){
        //per azzerare il blocco uso calloc
    char* sKey = calloc(key+1, sizeof(char));
    strcat(sKey, s);
    //in sKey ora c'� solo la stringa piccola
    int addedChar = 0;
    int charToAdd = key-oldSize;
    const char * z = s;

    /* finch� il numero di caratteri aggiunti non pareggia quello dei caratteri da aggiungere,
        alla stringa sKey aggiungi di volta in volta un carattere a partire dalla fine, preso dalla stringa
        di partenza. Se dopo l'aggiunta il puntatore alla stringa di partenza si trova sul carattere di terminazione,
        ricomincia da capo. */
    while(addedChar < charToAdd){
        sKey[oldSize+addedChar] = *z;
        z++;
        addedChar++;
        if(!*z){
            z = s;
        }
    }
    return sKey;
}

/**
    Defines the Infinite Lexicographic Order, according to specification.
    Two words s1 and s2 get checked for their length:
    If the length is the same, the ordering corresponds to the classical lexicographic order and so we delegate the computation to strcmp.
    If one is longer than the other, we get the corresponding key (prolonging the shortest word until they are the same length) and then we compare them with strcmp.
    If they are the same according to strcmp after keying them, the shortest one is the smallest.
**/

int strInfcmp (const char *s1, const char *s2){
    size_t s1len = strlen(s1);
    size_t s2len = strlen(s2);
        if(s1len == s2len)
            return strcmp(s1, s2);
        else if(s1len < s2len){
            char *s1key = getInfCmpKey(s1, s2len, s1len);
            int result = strcmp(s1key, s2);
            free(s1key);
            /*if they are the same when compared with respect to the infinite lexicographic order, we set the
              shortest one as the smallest
            */
            if(result == 0)
                return -1;
            return result;
        }
        else {
            char *s2key = getInfCmpKey(s2, s1len, s2len);
            int result = strcmp(s1, s2key);
            free(s2key);
            if(result==0)
                return 1;
            return result;
        }
}

static int InfiniteLexiCompare(const void* a, const void* b){
    return strInfcmp(*(const char**)a, *(const char**)b);
}

/**
    Constructs the conjugation table for all factors of a word w.
    What it does:
    1) Allocates a table of strlen(w) rows, since we already know there will be that much entries. (the sum of the length of all factors returns the length of the word,
    every factor has as many conjugates as its length)
    2) Uses an utils function to find Lyndon Factorization of the word w, stored in a linked list, as specified in the header.
    3) Cycles for strlen(w) times:
        We start from the first row and the first factor, allocating precise space corresponding to the factor's length
        Then we compute its conjugates and fill the next i+j rows. Then, we increase i by the number of words added to the table (which wil be the current word's length)
    At the end the table will be filled according to specification.
**/
char** conjTableCFL(const char* word){
  size_t len = strlen(word);
  char** table = malloc(len*sizeof(*table));
  node_t* factorizationsMock = CFL(word);
  node_t* factorizations = factorizationsMock;
  for(size_t i = 0; i<len && factorizations != NULL;) {
    char* current_word = factorizations->factor;
    size_t current_len = strlen(current_word);
    for(size_t j = 0; j<current_len; j++) {
        table[i+j] = malloc(current_len + 1);
        memcpy(table[i+j], current_word + current_len -j, j);
        memcpy(table[i+j] + j, current_word, current_len - j);
        table[i+j][current_len] = '\0';
    }
    factorizations = factorizations->next;
    i += current_len;
  }
  free_list(factorizationsMock);
  return table;
}

/**
    Uses C's default qsort function to sort the conj table in place, according to InfiniteLexiCompare, an helper function built around the notion of the Infinite
    Lexicographic Ordering.
**/

void sortConjTableCFL(char** conjTable, int n){
    qsort(conjTable, n, sizeof(const char*), InfiniteLexiCompare);
}

/**
    Computes Bijective BWT (BWTS) according to specification.
    First we obtain the conjugacy table for all lyndon factors of a word w, then we sort it, then we construct the BWTS (w) = last(u1) + last (u2) + .... + last (uN)
**/
char* bwts(const char* bwt){
    size_t len_bwt = strlen(bwt);
    char** table = conjTableCFL(bwt);
    sortConjTableCFL(table, len_bwt);
    char* bwtEncoded = calloc(len_bwt+1, sizeof(char));

    for(size_t i = 0; i < len_bwt; i++){
        strncat(bwtEncoded, last(table[i]), 1);
    }
    bwtEncoded[len_bwt] = '\0';

    //freeing memory allocated to the table
    for(size_t i=0; i<len_bwt; i++){
        free(table[i]);
    }
    free(table);

    return bwtEncoded;
}

/**
    To compute the standard permutation, I use a struct "permutationElement" containing a pair of values:
    The character that I'm considering when commuting, and its index in the BWT.
    To obtain the standard permutation I will simply sort an array of permutationElements according to the characters, preserving the original indexes.
**/
struct permutationElement {
    unsigned char value;
    int index;
};

/**
    How comparing works:
    Let (x,i) and (y,j) be 2 permutation elements.
    We say that (x,i) <= (y,j) if x<y or, if x==y and i<=j.
    For example, if we have (a, 3) and (a, 5), since a==a and 3<5, (a,3) < (a,5).
**/

static int permutationCompare(const void* a, const void* b){
    struct permutationElement *a1 = (struct permutationElement*)a;
    struct permutationElement *a2 = (struct permutationElement*)b;
    if((*a1).value > (*a2).value)
        return 1;
    else if((*a1).value < (*a2).value)
        return -1;
    else {
        if((*a1).index < (*a2).index)
            return -1;
        else if((*a1).index > (*a2).index)
            return 1;
        else return 0;
    }
}
/**
    Simply computes the permutation using everything from the previous functions and then copies to a plain
    int array.
**/
int* standardPermutation(const char* bwt, size_t bwt_length){
    struct permutationElement permutation[bwt_length];
    for(size_t i=0; i<bwt_length; i++){
        permutation[i].index = i;
        permutation[i].value = bwt[i];
    }
    qsort(permutation, bwt_length, sizeof(permutation[0]), permutationCompare);
    int* standardPerm = malloc(sizeof(int)*bwt_length);
    for(size_t i=0; i<bwt_length; i++){
        standardPerm[i] = permutation[i].index;
    }
    return standardPerm;
}
/**
        Uses custom list of lists implementation to findCycles.
        The list of lists is built so that when adding a new list to the list, it is added
        on top (to avoid reversing the list at the end as done in the original Python code), but every list
        that is inside the list is built to add elements to the bottom (so to start from the second element as
        planned and avoid reversing those instead of the "main" list)
**/

cyListNode findCycles(int* standardPermutation, size_t permutationLength){
    cyListNode allCycles = NULL;
    for(size_t i=0; i<permutationLength; i++){
        int currentIndex = i;
        cycleNode currentCycle = NULL;
        while(standardPermutation[currentIndex] != -1){
            int newIndex = standardPermutation[currentIndex];
            currentCycle = addcycleNode(currentCycle, newIndex);
            standardPermutation[currentIndex] = -1;
            currentIndex = newIndex;
        }
        if(currentCycle != NULL)
            allCycles = addcyListNode(allCycles, currentCycle);
    }
    return allCycles;
}

char* ibwts(char* bwt){
    size_t bwt_len = strlen(bwt);
    int * permutation = standardPermutation(bwt, bwt_len);
    cyListNode listaCicliMock = findCycles(permutation, bwt_len);
    cyListNode listaCicli = listaCicliMock;
    char* bwtDecoded = calloc(bwt_len+1, sizeof(char));

    size_t i = 0;
    while(listaCicli != NULL && i < bwt_len){
        while(listaCicli->current != NULL){
            bwtDecoded[i] = bwt[listaCicli->current->index];
            listaCicli->current = listaCicli->current->next;
            i++;
        }
        listaCicli = listaCicli->next;
    }
    bwtDecoded[bwt_len] = '\0';
    free(permutation);
    return bwtDecoded;
}





/***
        BIJECTIVE BTWZ (Zack) related functions
**/

/**
    Implementation of the Knuth-Morris-Pratt algorithm to find the longest border size.
    I commented out a naive way to find the border. If you encounter any problem, uncomment it and comment before the declaration
    of the prefix function to use the naive algorithm
**/

size_t getBorderSize(const char* w){
    size_t border = 0;
    size_t wlen = strlen(w);

    size_t* prefixFunction = calloc(wlen, sizeof(size_t));
    for(size_t i = 1; i < wlen; i++){
        while(border>0 && w[border] != w[i])
            border = prefixFunction[border-1];
        if(w[border] == w[i])
            border++;

        prefixFunction[i] = border;
    }
    int borderSize = prefixFunction[wlen-1];
    free(prefixFunction);
    return borderSize;

    /*for(int i=0; i<wlen+1; i++){
        char* a1 = substring(w, 0, i);
        char* a2 = substring(w, wlen-i, wlen);
        if(strcmp(a1, a2) == 0){
            if(strcmp(a1, w) != 0)
                border = i;
        }
    }*/
    return border;

}

/**

**/

node_t* getSplittedFactors (const char* word, size_t len){

  node_t* factorizationsMock = ICFL_recursive(word);
  node_t* factorizations = factorizationsMock;

  node_t* splitted_factors = NULL;

  while(factorizations != NULL){
    while(1){
        size_t border = getBorderSize(factorizations->factor);
        size_t factorLen = strlen(factorizations->factor);
        if(border > 0 && factorLen > 1){
                if(border == factorLen){
                    //a^k
                    for(size_t i=0; i < factorLen; i++){
                        char* factor;
                        factor = calloc(1+1, sizeof(*factor));
                        factor[0] = factorizations->factor[i];
                        factor[1] = '\0';
                        node_t *node =  malloc(sizeof(*node));
                        node->factor = factor;
                        node->next = splitted_factors;
                        splitted_factors = node;
                        //splitted_factors = insertBottom(splitted_factors, factor);
                    }
                }
                else {
                    char* factor1;
                    factor1 = calloc(factorLen-border+1, sizeof(*factor1));
                    //factor2 = calloc(border+1, sizeof(*factor2));

                    memcpy(factor1, factorizations->factor, factorLen-border);
                    factor1[factorLen-border] = '\0';

                    node_t *node1 =  malloc(sizeof(*node1));
                    node1->factor = factor1;
                    node1->next = splitted_factors;
                    splitted_factors = node1;
                    //splitted_factors = insertBottom(splitted_factors, factor1);

                    /*node_t *node2 =  malloc(sizeof(*node2));
                    node2->factor = factor2;
                    node2->next = splitted_factors;
                    splitted_factors = node2;*/
                    char* factorNew = substring(factorizations->factor, factorLen-border, factorLen);
                    char* temp = factorizations->factor;
                    factorizations->factor = factorNew;
                    free(temp);

                }
        }
        else {
            char* factor = malloc(factorLen+1 * sizeof(*factor));
            memcpy(factor, factorizations->factor, factorLen);
            factor[factorLen] = '\0';

            node_t *node3 =  malloc(sizeof(*node3));
            node3->factor = factor;
            node3->next = splitted_factors;
            splitted_factors = node3;
            //splitted_factors = insertBottom(splitted_factors, factor);
            break;
        }
    }
     factorizations = factorizations->next;
  }
  free_list(factorizationsMock);
  return splitted_factors;
}

/**
    Applies the same algorithm we already know for BWTS, but to the splitted_factors of the ICFL.
**/
char** conjTableICFL(const char* word, size_t len){
    char** table = malloc(len*sizeof(*table));
    node_t* splitted_factorsMock = getSplittedFactors(word, len);
    node_t* splitted_factors = splitted_factorsMock;

  for(size_t i = 0; i<len && splitted_factors != NULL;) {

    char* current_word = splitted_factors->factor;

    size_t current_len = strlen(current_word);

    for(size_t j = 0; j<current_len; j++) {
        table[i+j] = malloc(current_len + 1);
        memcpy(table[i+j], current_word + current_len -j, j);
        memcpy(table[i+j] + j, current_word, current_len - j);
        table[i+j][current_len] = '\0';
    }
    splitted_factors = splitted_factors->next;
    i += current_len;
  }
  free_list(splitted_factorsMock);

  return table;
}

/**
    The next 3 functions do the exact same thing their similarly named versions do for the BWTS, but in descending order.
    DEPRECATED: I'll comment them, since we do not use them any longer. They still work, but they are very slow.
**/

int strInfcmpDescending (const char *s1, const char *s2){
    size_t s1len = strlen(s1);
    size_t s2len = strlen(s2);
        if(s1len == s2len)
            return -strcmp(s1, s2);
        else if(s1len < s2len){
            char *s1key = getInfCmpKey(s1, s2len, s1len);
            int result = -strcmp(s1key, s2);
            free(s1key);
            //if they are the same when compared with respect to the infinite lexicographic order, we set the shortest one as the biggest

            if(result == 0)
                return 1;
            return result;
        }
        else {
            char *s2key = getInfCmpKey(s2, s1len, s2len);
            int result = -strcmp(s1, s2key);
            free(s2key);
            if(result==0)
                return -1;
            return result;
        }
        //Anche nella BWTZ, nonostante io debba ordinare al contrario
        //(e quindi dal pi� grande al pi� piccolo) se due stringhe sono uguali secondo l'ordine infinito, la pi� piccola rimane davanti
}

static int InfiniteLexiCompareDescending(const void* a, const void* b){
    return strInfcmpDescending(*(const char**)a, *(const char**)b);
}

void sortConjTableICFL(char** conjTable, int n){
    qsort(conjTable, n, sizeof(const char*), InfiniteLexiCompareDescending);
}

/**
    END OF DEPRECATED FUNCTIONS
**/

/**
    This struct is introduced for optimizing the sorting of the conjugation table.
    If we delegate the extension of the strings to the getInfCmpKey function, the BWT is very slow: Everytime there's a comparison,
    we have to extend the strings. This means allocating space for the extended string, comparing, deallocating space etc.
    This also means that a string gets extended not once, but every single time there's a comparison involving that string.
    This makes for HUGE time loss when the block to transform is very big.
    If we could somehow extend all strings beforehand and sort, we could save a lot of time by extending every factor just once.
    Thing is, if we only just extend the strings and then sort, reading the last character of the resulting table doesn't return us the
    BWT anymore! So we need to:
    1) extend the strings beforehand
    2) sort the table containing the extended strings
    3) somehow, find a way to link the extended strings with their original counterpart, so that we can read the last character of the original factor
    by iterating over the table containing the extended factors...
    To obtain this, we need a pair:
**/
struct bwtElement {
    char* factor;
    int index;
};

/**
    The next function is just a comparator function for the pair
**/
static int bwtElementCompare(const void* a, const void* b){
    struct bwtElement *a1 = (struct bwtElement*)a;
    struct bwtElement *a2 = (struct bwtElement*)b;
    int compare = -strcmp(a1->factor, a2->factor);
    if(compare == 0){
        if(a1->index > a2->index)
            return -1;
        else if(a1->index < a2->index)
            return 1;
        else return 0;
    }
    else{
        return compare;
    }
}

/**
        Bijective BWT (Zack):
        1) Build conjTable(w) from ICFL Factorization of w
        2) Order said table according to the infinite descending lexicographic order
        3) Return BWTZ as BWTZ(w) = last(u1) + last(u2) + .... last(uN)
**/

/**
        The trick is in place:
**/

char* bwtz (char* word){
    size_t len_bwt = strlen(word);
    if(len_bwt <= 1)
        return word;

    char** table = conjTableICFL(word, len_bwt);
    //ulteriore miglioramento: costruire direttamente la tabella di bwtElement invece di fare prima la tabella dei fattori e poi la tableSort
    struct bwtElement tableSort[len_bwt];
    for(size_t i = 0; i < len_bwt; i++){
        tableSort[i].factor = getInfCmpKey(table[i], len_bwt, strlen(table[i]));
        tableSort[i].index = i;
    }
    qsort(tableSort, len_bwt, sizeof(tableSort[0]), bwtElementCompare);

    /*This instruction refers to the old way of sorting the table without extending just once. It uses the 3 deprecated functions listed earlier*/
    //sortConjTableICFL(table, len_bwt);

    char* bwtEncoded = calloc(len_bwt+1, sizeof(char));

    for(size_t i = 0; i < len_bwt; i++){
        bwtEncoded[i] = last(table[tableSort[i].index])[0];
    }
    bwtEncoded[len_bwt] = '\0';

    //freeing memory allocated to the table
    for(size_t i=0; i<len_bwt; i++){
        free(table[i]);
    }
    free(table);

    //freeing memory allocated to the trick table
    for(size_t i = 0; i < len_bwt; i++){
        free(tableSort[i].factor);
    }

    return bwtEncoded;
}


/**
    IBWTZ: Symmetric of the IBWTS, but as with the BWTZ, I have to rewrite similar code but this time in reversed order
**/

static int permutationCompareDescending(const void* a, const void* b){
    struct permutationElement *a1 = (struct permutationElement*)a;
    struct permutationElement *a2 = (struct permutationElement*)b;
    if((*a1).value > (*a2).value)
        return -1;
    else if((*a1).value < (*a2).value)
        return 1;
    else {
        if((*a1).index < (*a2).index)
            return -1;
        else if((*a1).index > (*a2).index)
            return 1;
        else return 0;
    }
}

int* standardPermutationDescending(const char* bwt, size_t bwt_length){
    struct permutationElement permutation[bwt_length];
    for(size_t i=0; i<bwt_length; i++){
        permutation[i].index = i;
        permutation[i].value = bwt[i];
    }
    qsort(permutation, bwt_length, sizeof(permutation[0]), permutationCompareDescending);
    int* standardPerm = malloc(sizeof(int)*bwt_length);
    for(size_t i=0; i<bwt_length; i++){
        standardPerm[i] = permutation[i].index;
    }
    return standardPerm;
}

char* ibwtz(char* bwt){
    size_t bwt_len = strlen(bwt);
    if(bwt_len <= 1)
        return bwt;
    int * permutation = standardPermutationDescending(bwt, bwt_len);
    cyListNode listaCicliMock = findCycles(permutation, bwt_len);
    cyListNode listaCicli = listaCicliMock;
    char* bwtDecoded = calloc(bwt_len+1, sizeof(char));

    size_t i = 0;
    while(listaCicli != NULL && i < bwt_len){
        while(listaCicli->current != NULL){
            bwtDecoded[i] = bwt[listaCicli->current->index];
            listaCicli->current = listaCicli->current->next;
            i++;
        }
        listaCicli = listaCicli->next;
    }
    bwtDecoded[bwt_len] = '\0';
    free(permutation);
    return bwtDecoded;
}
