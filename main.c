#include "brute_force.h"
#include "semaforo.h"
#include <openssl/evp.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <md5_hash>\n", argv[0]);
  }
  semaforo(argv);
  // semaforo(argc, argv);
}
