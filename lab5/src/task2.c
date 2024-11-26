#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>

typedef struct {
    int start;
    int end;
    int mod;
    long long *result;
    pthread_mutex_t *mutex;
} ThreadData;

void *factorial_mod(void *arg) 
{
    ThreadData *data = (ThreadData *)arg;
    long long local_result = 1;

    for (int i = data->start; i <= data->end; i++)
    {
        local_result = (local_result * i) % data->mod;
    }

    pthread_mutex_lock(data->mutex);
    *data->result = (*data->result * local_result) % data->mod;
    pthread_mutex_unlock(data->mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    int k = 0, pnum = 1, mod = 1;

    int opt;
    while ((opt = getopt(argc, argv, "k:p:m:")) != -1) {
        switch (opt) {
            case 'k':
                k = atoi(optarg);
                break;
            case 'p':
                pnum = atoi(optarg);
                break;
            case 'm':
                mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> -p <threads> -m <mod>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Invalid input parameters.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[pnum];
    ThreadData thread_data[pnum];
    long long result = 1;
    pthread_mutex_t mutex;

    pthread_mutex_init(&mutex, NULL);

    int range = k / pnum;
    for (int i = 0; i < pnum; i++) {
        thread_data[i].start = i * range + 1;
        thread_data[i].end = (i == pnum - 1) ? k : (i + 1) * range;
        thread_data[i].mod = mod;
        thread_data[i].result = &result;
        thread_data[i].mutex = &mutex;

        if (pthread_create(&threads[i], NULL, factorial_mod, &thread_data[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Factorial of %d mod %d is: %lld\n", k, mod, result);

    pthread_mutex_destroy(&mutex);
    return 0;
}