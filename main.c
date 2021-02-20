#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

void swap (long long *a, long long *b) {
    long long tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Для библиотечной qsort ф-ия compare должна возвращать (если по неубыванию сорт):
Если arg1 меньше, чем arg2, то возвращается значение меньше 0.
Если arg1 равно arg2, то возвращается 0.
Если arg1 больше, чем arg2, то возвращается величина больше 0.*/
int compareIncrease (const void *x1, const void *x2) {
// функция сравнения элементов массива для qsort по неубыванию
    long long diff = *(long long*)x1 - *(long long *)x2;
    if (diff > 0)
        return 1;
    if (diff < 0)
        return -1;
    return 0;
}

int compareDecrease (const void *x1, const void *x2) {
// функция сравнения элементов массива для qsort по невозрастанию
    return compareIncrease(x2, x1);
}

void arrayGeneration (long long *a, int n, int param) { // генерация массива
    srand(time(NULL)); // запускаем псевдорандом
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 2; // от 0 до 32767 == RAND_MAX как раз поровну чёт/нечет 
        for (int j = 1; j <= 62; j++) { // формируем младшие 63 бита
            a[i] <<= 1;
            a[i] += rand() % 2;
        }
        if (rand() % 2 == 1)// в старшем бите ставим единицу
            a[i] += LLONG_MIN;
        /* то есть в допкоде имеем не x (например, 111 == 7), 
        а x - 2^63 (1111 == -1 == 7 - 2^3)
        иначе в старшем бите 0 и это само число x (0111 = 7) */
    }
    if (param == 1) // нужен отсортированный по неубыванию
        qsort(a, n, sizeof(long long), compareIncrease);
    else if (param == 2) // нужен отсортированный по невозрастанию
        qsort(a, n, sizeof(long long), compareDecrease);
}

unsigned long long cntcompares = 0, cntmoves = 0;
// было сказано, что можно глобальные переменные (удобно для рекурсивного qsort)

int absCompare (long long x1, long long x2, unsigned test) {
    cntcompares += test;
    if (x1 == LLONG_MIN) // самый большой модуль
        if (x2 == LLONG_MIN)
            return 0; // равны, а не >
        else
            return 1; // x1 > x2
    else
        if (x2 == LLONG_MIN)
            return 0; // x1 < x2, так как у x2 самый большой модуль
    long long absdiff = ((x1 >= 0)? x1: -x1) - ((x2 >= 0)? x2: -x2);
    return (absdiff > 0)? 1: 0;
}

void bubbleSort (long long *a, int n) {
    for (int i = 1; i < n; i++) {
        int was_swap = 0;
        for (int j = n - 1; j >= i; --j) //самый маленький элемент "спускается вниз"
            if (absCompare(a[j - 1], a[j], 1)) {
                swap(a + j - 1, a + j);
                cntmoves++;
                was_swap = 1;
            }
        if (!was_swap) // если не было перемещений, то массив отсортирован и можно выйти
            return;
    }
}

void qSort (long long *a, int left, int right) {
    int i = left, j = right; // была бага с unsigned из-за строчки 43 j > left при left = 0
    long long comp = a[(left + right) / 2];
    /* comp -- компаранд, от его хорошего выбора зависит скорость сортировки
    можно брать a[left + rand() % (j - i + 1)], но для надёжности оставим так */
    do {
        while (absCompare(comp, a[i], 1) && i < right)
            i++; // теперь левая часть меньше comp
        while (absCompare(a[j], comp, 1) && j > left)
            j--; // теперь правая часть больше comp
        if (i <= j) { // есть элементы, на которых порядок нарушился
            swap(a + i, a + j);
            cntmoves++;
            i++, j--;
        }
    } while (i <= j);
    if (left < j)
        qSort(a, left, j); // сортируем левую часть
    if (i < right)
        qSort(a, i, right); // сортируем правую часть
}

void quickSort (long long *a, int n) {
    // Функции не имеют возвращаемых значений и принимают по два параметра: число n и массив a
    qSort(a, 0, n - 1);
}

int checkArrayAbsAscending (long long *a, int n) {
    for (int i = 1; i < n; i++)
        if (absCompare(a[i - 1], a[i], 0)) // не упорядочено по неубыванию модулей :(
            return 0;
    return 1;
}

int main (void) {
    int n, param;
    printf("Please enter a size of array...\n");
    scanf("%u", &n);
    printf("If you want data ordered ascending - press 1, "
           "reverse (descending) - press 2, random - press 3 or 4\n");
    scanf("%u", &param);
    long long *a = malloc(n * sizeof(long long));
    arrayGeneration(a, n, param);
    quickSort(a, n);
    if (checkArrayAbsAscending(a, n))
        printf("You're god damn right! QuickSort done %llu compares"
               " and %llu moves\n", cntcompares, cntmoves);
    else
        printf("Your sort is failed! Please check QuickSort's realisation!\n");
    cntcompares = 0;
    cntmoves = 0;
    arrayGeneration(a, n, param);
    bubbleSort(a, n);
    if (checkArrayAbsAscending(a, n))
        printf("You're god damn right! BubbleSort done %llu compares"
               " and %llu moves\n", cntcompares, cntmoves);
    else
        printf("Your sort is failed! Please check BubbleSort's realisation!\n");
    free(a); // не забываем почистить за собой
    return 0;
}
