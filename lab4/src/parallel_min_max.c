#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if(seed <= 0){
              printf("Seed must be a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if(array_size <= 0){
              printf("Array size must be a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if(pnum <= 0){
              printf("Number of processes must be a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            timeout = atoi(optarg);
            if(timeout < 0){
              printf("Timeout must be a non-negative number\n");
              return 1;
            }
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one non-option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipefd[2];
  if(!with_files){
    if(pipe(pipefd) == -1){
      printf("Pipe failed!\n");
      return 1;
    }
  }

  pid_t pids[pnum]; // массив для хранения идентификаторов дочерних процессов

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // успешное создание процесса
      active_child_processes += 1;
      pids[i] = child_pid;
      if (child_pid == 0) {
        // дочерний процесс
        int begin = i * (array_size / pnum);
        int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);
        struct MinMax min_max = GetMinMax(array, begin, end);

        if (with_files) {
          // используем файлы для записи
          char file_name[255];
          sprintf(file_name, "tmp_%d.txt", i);
          FILE *file = fopen(file_name, "w");
          if(file == NULL){
            printf("Error opening file!\n");
            return 1;
          }
          fprintf(file, "%d %d\n", min_max.min, min_max.max);
          fclose(file);
        } else {
          // используем канал для передачи данных
          close(pipefd[0]);
          write(pipefd[1], &min_max, sizeof(struct MinMax));
          close(pipefd[1]);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (timeout > 0) {
    // Таймер для завершения работы дочерних процессов по истечении таймаута
    sleep(timeout);
    for (int i = 0; i < pnum; i++) {
      kill(pids[i], SIGKILL); // отправка сигнала SIGKILL всем дочерним процессам
    }
  }

  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // чтение из файлов
      char file_name[255];
      sprintf(file_name, "tmp_%d.txt", i);
      FILE *file = fopen(file_name, "r");
      if(file == NULL){
        printf("Error opening file!\n");
        return 1;
      }
      fscanf(file, "%d %d", &min, &max);
      fclose(file);
    } 
    else {
      // чтение из канала
      close(pipefd[1]);
      struct MinMax local_min_max;
      read(pipefd[0], &local_min_max, sizeof(struct MinMax));
      min = local_min_max.min;
      max = local_min_max.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}