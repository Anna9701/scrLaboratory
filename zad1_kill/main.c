#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char* argv[]) {
    if (argc < 4) {
        printf("Please, provide parameters: <pid> <sig> <data>\n");
        exit(EXIT_FAILURE);
    }
    int pid = strtol(argv[1], NULL, 0);
    int signal = strtol(argv[2], NULL, 0);
    int number = strtol(argv[3], NULL, 0);
    if (pid == 0L || signal == 0L || number == 0L ) {
        perror("Non numeric argument");
        exit(EXIT_FAILURE);
    }
    union sigval siVal;
    siVal.sival_int = number;
    if (sigqueue(pid, signal, siVal) < 0) {
        perror("Error during sending signal");
        exit(EXIT_FAILURE);
    }
    return 0;
}