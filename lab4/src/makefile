# Установка компилятора и флаго
CC = gcc
CFLAGS = -Wall

# Определение имен исполняемых файлов
TASK1 = task1.out
TASK3 = task3.out
TASK5 = task5.out

# Определение объектных файлов
OBJ1 = utils.o find_min_max.o
SRC1 = parallel_min_max.c utils.h find_min_max.h
SRC3 = process_memory.c
OBJ5 = utils.o threads_sum.o
SRC5 = parallel_sum.c utils.c utils.h threads_sum.c threads_sum.h

# Цель по умолчанию: сборка всех программ
all: $(TASK1) $(TASK3) $(TASK5)

# Сборка task1.out, зависимость от utils.o и find_min_max.o
$(TASK1): parallel_min_max.c $(OBJ1)
	$(CC) parallel_min_max.c $(OBJ1) -o $(TASK1)

# Сборка task3.out
$(TASK3): $(SRC3)
	$(CC) $(SRC3) -o $(TASK3)

# Сборка task5.out, зависимость от utils.o и threads_sum.o
$(TASK5): parallel_sum.c $(OBJ5)
	$(CC) parallel_sum.c $(OBJ5) -o $(TASK5)

# Компиляция utils.o
utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c -o utils.o

# Компиляция find_min_max.o
find_min_max.o: find_min_max.c find_min_max.h
	$(CC) $(CFLAGS) -c find_min_max.c -o find_min_max.o

# Компиляция threads_sum.o
threads_sum.o: threads_sum.c threads_sum.h
	$(CC) $(CFLAGS) -c threads_sum.c -o threads_sum.o

# Очистка скомпилированных файлов
clean:
	rm -f $(TASK1) $(TASK3) $(TASK5) *.o
