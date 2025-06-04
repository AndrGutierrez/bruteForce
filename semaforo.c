#include "brute_force.h"
#include <pthread.h> //<-- liberia de POSIX para crear Hilos
#include <stdio.h>
#include <stdlib.h>

// Librerías globales que comparten los hilos
int sum = 0, numero;

// Semoforo para controlar la sincronización de hilos
pthread_mutex_t mutex_acceso;
void *runner(void *param); // La hebra
                           //
int iteracion = 0;
int breakpoints[] = {0, 3727490};
// int breakpoints[] = {
//     0,       1242496, 2484993, 3727490,
//     4969987, 6212484}; // en el ultimo sobra uno por el redondeo cuidado

int semaforo(char *argv[]) {
  pthread_t tid1, tid2; // identificador de las hebras
  pthread_attr_t attr;  // conjunto de atributos de la hebra

  // numero = atoi(argv[1]);
  // if (numero < 0) {
  //   fprintf(stderr, "%d debe ser >=0\n", numero);
  //   return -1;
  // }

  int selected_breakpoint = breakpoints[1]; // Choose the appropriate breakpoint

  // Prepare thread arguments
  ThreadArgs args;
  args.argv = argv; // Pass the entire argv (or just args.numero = numero;)
  args.breakpoint = selected_breakpoint;
  // Obtener los atributos predeterminados
  pthread_attr_init(&attr);
  // Inicializa el semaforo
  pthread_mutex_init(&mutex_acceso, NULL);
  // Crear la hebra

  pthread_create(&tid1, &attr, brute_force, (void *)&args);
  // pthread_create(&tid2, &attr, test, &numero);
  // Espera a que las hebras termine
  pthread_join(tid1, NULL);
  // pthread_join(tid2, NULL);
  // sum /=2 ; //Arreglar para que cada hilo funcione correctamente y evitar
  // esta linea
  // printf("sum= %d\n", sum);
}

/*La hebra inicia su ejecución en esta función*/
// void *runner(void *param) {
//   int i, numero;
//   // numero = int param; en 32 bits
//   numero = *((int *)param); // 64 bits
//   pthread_mutex_lock(&mutex_acceso);
//   for (i = 1; i <= numero; i++)
//     sum += i;
//   pthread_mutex_unlock(&mutex_acceso);
//   pthread_exit(0);
// }
