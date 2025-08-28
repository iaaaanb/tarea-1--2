#define _DEFAULT_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "test-util.h"

// ----------------------------------------------------
// Funcion que entrega el tiempo de CPU usado desde
// que se invoco resetTime()

static int time0= 0;

static int getTime0() {
    struct timeval Timeval;
    gettimeofday(&Timeval, NULL);
    return Timeval.tv_sec*1000+Timeval.tv_usec/1000;
}

void resetTime() {
  time0= getTime0();
}

int getTime() {
  return getTime0()-time0;
}

// ----------------------------------------------------
// Funcion que genera numeros pseudo aleatorios no muy buenos pero muy rapida
// (inspirada de la funcion rand)

static uint64_t next = 1;

uint64_t randNum() {
  return (next = next * 1103515245 + 12345);
}

void srandNum(uint64_t seed) {
  next = seed;
  randNum();
  randNum();
  randNum();
}

// ----------------------------------------------------

void check_i32(int32_t x, int32_t ref, char *str) {
  if ( x != ref) {
    fprintf( stderr,
             "Error: el resultado de %s debio ser %d, "
             "no %d\n", str, ref, x);
    exit(1);
  }
}

void check_u64(uint64_t x, uint64_t ref, char *str) {
  if ( x != ref) {
    fprintf( stderr,
             "Error: el resultado de %s debio ser 0x%lx, "
             "no 0x%lx\n", str, ref, x);
    exit(1);
  }
}

static FILE *ref_file= NULL;

void run_bench( BenchFun init, BenchFun bench, BenchFun clean, void *ptr,
                int tiempo_ref, int tolerancia, int nintentos ) {
  if (ref_file!=NULL) {
    int tiempos[3];
    for (int k= 0; k<3; k++) {
      (*init)(ptr);
      resetTime();
      (*bench)(ptr);
      tiempos[k]= getTime();
      (*clean)(ptr);
    }
    int med;
    // Calcular la mediana de los 3 tiempos
    if (tiempos[0]>tiempos[1]) {
      int temp= tiempos[0];
      tiempos[0]= tiempos[1];
      tiempos[1]= temp;
    }
    if (tiempos[1]<=tiempos[2])
      med= tiempos[1];
    else if (tiempos[2]<=tiempos[0])
      med= tiempos[0];
    else
      med= tiempos[2];
    fprintf(ref_file, "%d ", med);
    return;
  }

  int k= 0;
  for (;;) {
    (*init)(ptr);
    resetTime();
    (*bench)(ptr);
    int tiempo= getTime();
    (*clean)(ptr);
    printf("Tiempo de cpu usado por su version: %d milisegundos\n", tiempo);
    if (tiempo_ref<1)
      return;
    printf("Tiempo de la version del profesor : %d milisegundos\n", tiempo_ref);
    double q= (double)tiempo/(double)tiempo_ref;
    int porciento= (q-1.)*100;
    printf("Porcentaje de sobrecosto: %d %%\n", porciento);
    if (porciento<=tolerancia)
      return;
    fprintf(stderr, "*** El sobrecosto excede lo requerido ***\n");
    if (k>=nintentos)
      break;
    fprintf(stderr, "Volveremos a intentar\n");
    k++;
  }
  fprintf(stderr, "Lo siento: despues de %d intentos supera "
                  "el sobrecosto tolerado de %d %%\n",
                  nintentos, tolerancia);
  fprintf(stderr, "Coloque su computador en modo alto rendimiento, porque el\n"
                  "economizador de bateria puede alterar los resultados.\n"
                  "No ejecute este programa junto a otros programas que hagan\n"
                  "un uso intensivo de la CPU.  En windows puede lanzar el\n"
                  "administrador de tareas para verificar que el uso de CPU\n"
                  "sea bajo.\n");
  exit(1);
}

#define MAXB 100
static int refs[MAXB];

int main(int argc, char **argv) {
  if (argc>=2 && strcmp(argv[1], "ref")==0) {
    // Calcular tiempos de referencia con la version del profesor
    ref_file= fopen("tiempo-ref.txt", "w");
    benchmark(refs, 0);
    fclose(ref_file);
  }
  else {
    printf("Tests unitarios\n");
    test_unitario(); // Verificar correctitud de la solucion
    printf("Aprobado\n");
    srandNum(0x12345678);
    if (argc==1) {
      printf("Tests masivos\n");
      benchmark(refs, 0);
      printf("Aprobado\n");
    }
    else if (argc>MAXB) {
      fprintf(stderr, "No se puede realizar mas de 100 benchmarks\n");
    }
    else {
      int refs[argc-1];
      for (int k= 1; k<argc; k++)
        refs[k-1]= atoi(argv[k]);
      benchmark(refs, argc-1); // Verificar tiempos de ejecucion
      printf("Aprobado\n");
    }
  }
  printf("Felicitaciones: aprobo los tests de este modo de ejecucion\n");
  return 0;
}
