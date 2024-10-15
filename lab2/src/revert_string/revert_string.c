#include <stdio.h>
#include <string.h>

void RevertString(char* str) {
    if (str == NULL) return;  // Проверка на NULL
    int length = strlen(str);  // Получаем длину строки
    for (int i = 0; i < length / 2; i++) {
        // Меняем местами символы с начала и конца
        char temp = str[i];
        str[i] = str[length - i - 1];
        str[length - i - 1] = temp;
	printf("Hello");
    }
}

