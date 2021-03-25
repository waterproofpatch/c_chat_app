#include <pthread.h>

int          g_sock_fd;          // socket descriptor for the client connection
pthread_t    g_receive_thread;   // thread to handle receiving data
volatile int g_is_alive =
    1;   // global to keep listen thread alive, volatile to ensure cache flush
