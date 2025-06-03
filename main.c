#include "brute_force.h"
#include "semaforo.h"
#include <openssl/evp.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  brute_force(argc, argv);
  // semaforo(argc, argv);
}
