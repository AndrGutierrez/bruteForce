#include "brute_force.h"
#include <pthread.h> //<-- liberia de POSIX para crear Hilos
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

  // Prepare thread arguments
  ThreadArgs batch1;
  batch1.argv = argv; // Pass the entire argv (or just args.numero = numero;)
  batch1.breakpoint = breakpoints[0];
  ThreadArgs batch2;

  batch2.argv = argv; // Pass the entire argv (or just args.numero = numero;)
  batch2.breakpoint = breakpoints[1];

  // Obtener los atributos predeterminados
  pthread_attr_init(&attr);
  // Inicializa el semaforo
  pthread_mutex_init(&mutex_acceso, NULL);
  // Crear la hebra

  pthread_create(&tid1, &attr, brute_force, (void *)&batch1);
  pthread_create(&tid2, &attr, brute_force, (void *)&batch2);
  // Espera a que las hebras termine
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
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
