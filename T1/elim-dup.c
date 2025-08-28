#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "elim-dup.h"

uint64_t elimDup(uint64_t x) {
    if (x == 0) {
        return 0;
    }
    
    uint64_t result = 0;
    int shift = 0;
    int prev_digit = -1;
    
    // Procesar desde el dígito menos significativo hacia el más significativo
    while (x > 0) {
        int current_digit = x & 0xF; // Extraer los 4 bits menos significativos
        x >>= 4; // Desplazar 4 bits a la derecha
        
        // Si el dígito actual es diferente al anterior, lo agregamos al resultado
        if (current_digit != prev_digit) {
            result |= ((uint64_t)current_digit << shift);
            shift += 4;
        }
        
        prev_digit = current_digit;
    }
    
    return result;
}