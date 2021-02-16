#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>

void bubbleSort(int a[], int length);
void imp (int arr[], int len);
void mezcla(int arr[], int ini, int med, int fin);

int *arr;

int main() {

    arr = mmap(NULL, 10 * sizeof(int), PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (int i = 0; i < 100; i++) {
        arr[i] = rand() % 100; // Entre 0 y 99
    }

    int pid = fork();

    if (pid > 0) {
        imp(arr, 10);
        sleep(1);
        printf("hello\nfather here %d\n", arr[0]);
        wait(&pid);
        mezcla(arr, 0, 50, 99);
        imp(arr, 100);

    } else {
        bubbleSort(arr, 10);
        printf("hello\nchild here %d\n", arr[0]);
        imp(arr, 10);
    }

    return 0;
}

void bubbleSort(int a[], int length)
{
    int i, j, temp;
    for (i = 0; i < length; i++)
    {
        for (j = 0; j < length - i - 1; j++)
        {
            if (a[j + 1] < a[j])
            {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
        }
    }
}

void imp (int arr[], int len)
{
    int i;
    for (i=0; i<len; i++) {
        printf("%d ",arr[i]);
    }
    printf("\n");
}

void mezcla(int arr[], int ini, int med, int fin) {
    int *aux = malloc(sizeof(int) * (fin - ini + 1));
    int finIni = med - 1;
    int s = ini;
    int tam = fin-ini+1;
    int t = 0;
    while ((ini <= finIni) && (med <= fin)) {
        if (arr[ini] <= arr[med]) {
            aux[t] = arr[ini];
            ini++;
            t++;
        } else {
            aux[t] = arr[med];
            med++;
            t++;
        }
    }
    while (ini <= finIni) {
        aux[t++] = arr[ini++];
    }
    while (med <= fin) {
        aux[t++] = arr[med++];
    }
    for (int i = 0; i <= (tam); i++) {
        arr[s+i] = aux[i];
    }
}
