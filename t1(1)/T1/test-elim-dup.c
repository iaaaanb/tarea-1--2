#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "elim-dup.h"
#include "test-util.h"

// ---------------------------------------------------------
// Funcion que verifica el resultado con divisiones y restos

static void verificar(uint64_t orig, uint64_t res) {
  int hexUlt= -1;
  uint64_t x= orig;
  uint64_t r= res;
  for (int i= 0; x!=0; i++) {
    uint64_t hexX = x % 16;
    x /= 16;
    if (hexX != hexUlt) {
      uint64_t hexR= r % 16;
      r /= 16;
      if (hexR != hexX) {
        fprintf(stderr, "Resultado incorrecto: "
                "con entrada %lx el resultado es %lx\n", orig, res);
        exit(1);
      }
    }
    hexUlt= hexX;
  }

  if (r != 0) {
    fprintf(stderr, "Resultado incorrecto: "
            "con entrada %lx el resultado es %lx\n", orig, res);
    exit(1);
  }

}

#define NUNIT 1000000
#define SEED 0x85ab128490fUL
static uint64_t seed= SEED;

void test_unitario(void) {
  // printf("-----------------------------------------------------\n\n");

  for (unsigned int i=0; i<16; i++) {
    printf("Verificando con x = 0x%x\n", i);
    uint64_t x= i;
    verificar(x, elimDup(x));
  }

  for (unsigned int i=0; i<16; i++) {
    printf("Verificando con duplicado x = 0x%x%x\n", i, i);
    uint64_t x= i*16+i;
    verificar(x, elimDup(x));
  }

  {
    printf("Verificando sin duplicado x = 0x123456789abcdef0\n");
    uint64_t x= 0x123456789abcdef0;
    verificar(x, elimDup(x));
  }

  {
    printf("Verificando ejemplo del enunciado x = 0x5ccc005c\n");
    uint64_t x= 0x5ccc005c;
    verificar(x, elimDup(x));
  }

  {
    printf("Verificando con duplicado x = 0x123456789abcdff0\n");
    uint64_t x= 0x123456789abcdff0;
    verificar(x, elimDup(x));
  }

  {
    printf("Verificando con duplicado x = 0x223456789abcdef0\n");
    uint64_t x= 0x223456789abcdef0;
    verificar(x, elimDup(x));
  }

  printf("Verificando con %d numeros aleatorios\n", 2*NUNIT);
  uint64_t x= seed;
  for (int k= 0; k<2; k++) {
    for (int i= 0; i<NUNIT; i++) {
      x ^= x >> 12;
      x ^= x << 25;
      x ^= x >> 27;
      x *= 2685821657736338717UL; // Esto es un generador de numeros aleatorios
      verificar(x, elimDup(x));
      if (x==seed) {
        fprintf(stderr, "El RNG es deficiente\n");
        exit(2);
      }
    }
    seed= x;
  }
}

#if 1

#ifdef OPT
#define N 100000000
#else
#define N 10000000
#endif

uint64_t crc;

static void bench_elimDup(void *ptr) {
  uint64_t x= seed, sum= 0;
  for (int k= 0; k<2; k++) {
    for (int i= 0; i<N; i++) {
      x ^= x >> 12;
      x ^= x << 25;
      x ^= x >> 27;
      x *= 2685821657736338717UL;
      sum += elimDup(x);
      if (x==seed) {
        fprintf(stderr, "El RNG es deficiente\n");
        exit(2);
      }
    }
    seed= x;
  }
  crc = sum;
}

static void dummy(void *ptr) {
}

#endif

void benchmark(int refs[], int n) {
#if 1
  printf("\n");
  printf("====================\n");
  printf("Benchmark de elimDup\n");
  printf("====================\n");
  //        prologo            benchmark           clean up
  run_bench(dummy, bench_elimDup, dummy,
  //        ptr,  tiempo_ref,    tolerancia, nintentos
            NULL,  refs[0]  ,        80    ,    5 );

#endif
}
