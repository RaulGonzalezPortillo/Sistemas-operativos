#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct argumentosOrdenar{
  int *arreglo;
  int tamanoArreglo;
} argumentosOrdenar;

typedef struct argumentosCriba{
  int inicio;
  int fin;
  int *primos;
  int *totalPrimos;
  int *numeros;
  int *totalNumeros;
} argumentosCriba;

sem_t s;
sem_t mutex;

int num_cpus();
int *crearArreglo(int tamanoArreglo);
void arregloAleatorio(int *arreglo, int tamanoArreglo);
void ordenarArreglo(int *arreglo, int tamanoArreglo);
void* bubbleSort(void *argumentos);
void mezclar(int arreglo[], int primero, int mitad, int ultimo);
void imprimirArreglo(int *arreglo, int tamanoArreglo);
void inicializadorCriba(int limite, int hilos);
void *criba(void* argumentos);

int main()
{
  int hilos, tamanoArreglo, operacion, maximoHilos, limite;
  long tiempoInicio, tiempoFin;
  double tiempoTotal;
  int *arreglo;

  system("clear");
  printf("Elíja una operación:\n1.- Ordenamiento\n2.- Criba\n");
  scanf(" %d", &operacion);
  printf("\n");

  switch(operacion)
  {
    case 1:
    printf("INFO: Número de hilos establecido en 2.\n\n");
    printf("\n");
    printf("Elíja el tamaño del arreglo:\n");
    scanf(" %d", &tamanoArreglo);
    printf("\n");
    arreglo = crearArreglo(tamanoArreglo);
    arregloAleatorio(arreglo, tamanoArreglo);
    tiempoInicio = clock();
    ordenarArreglo(arreglo, tamanoArreglo);
    tiempoFin = clock();
    printf("Arreglo ordenado:\n");
    imprimirArreglo(arreglo, tamanoArreglo);
    free(arreglo);
    break;

    case 2:
    maximoHilos = num_cpus();
    printf("INFO: Máximo de hilos = %d.\n\n", maximoHilos);
    printf("Elíja el número de hilos:\n");
    scanf(" %d", &hilos);
    printf("\n");
    if(hilos <= maximoHilos)
    {
      printf("\nElíja el número hasta el cuál buscar:\n");
      scanf(" %d", &limite);
      tiempoInicio = clock();
      inicializadorCriba(limite, hilos);
      tiempoFin = clock();
      printf("\n");
    }
    else
      printf("Número de hilos máximo superado.\n");
    break;

    default:
    break;
  }

  printf("\n");
  tiempoTotal = tiempoFin - tiempoInicio;
  tiempoTotal = tiempoTotal / ((double) CLOCKS_PER_SEC);
  printf("La operación tomó %f segundos\n", tiempoTotal);
}

int num_cpus(){
    unsigned int eax = 11, ebx = 0, ecx = 1, edx = 0;
    asm volatile(" cpuid "
                 : "=a"(eax),
                   "=b"(ebx),
                   "=c"(ecx),
                   "=d"(edx)
                 : "0"(eax), "2"(ecx)
                 :);
    return ebx;
}

int *crearArreglo(int tamanoArreglo)
{
  return (int*)malloc(sizeof(int) * tamanoArreglo);
}

void arregloAleatorio(int *arreglo, int tamanoArreglo)
{
  for(int i = 0; i < tamanoArreglo; i++){
      arreglo[i] = rand() % tamanoArreglo;
  }
}

void ordenarArreglo(int *arreglo, int tamanoArreglo)
{
  pthread_t hilo1, hilo2;
  pthread_attr_t attr;
  sem_init(&s, 1, 0);

  argumentosOrdenar argumentosHilo1;
  argumentosHilo1.arreglo = arreglo;
  argumentosHilo1.tamanoArreglo = tamanoArreglo / 2;

  argumentosOrdenar argumentosHilo2;
  argumentosHilo2.arreglo = &arreglo[tamanoArreglo / 2];
  argumentosHilo2.tamanoArreglo = tamanoArreglo / 2;

  pthread_attr_init(&attr);
  pthread_create(&hilo1, &attr, bubbleSort, &argumentosHilo1);
  pthread_create(&hilo2, &attr, bubbleSort, &argumentosHilo2);

  pthread_join(hilo1, NULL);
  pthread_join(hilo2, NULL);

  mezclar(arreglo, 0, (tamanoArreglo / 2) - 1, tamanoArreglo - 1);
}

void* bubbleSort(void *argumentos)
{
  argumentosOrdenar* argumentosTemp = (argumentosOrdenar*)argumentos;
  int *arreglo, tamanoArreglo, temp;

  arreglo = argumentosTemp->arreglo;
  tamanoArreglo = argumentosTemp->tamanoArreglo;

  for (int i = 0; i < tamanoArreglo; i++){
    for (int j = 0; j < tamanoArreglo - i - 1; j++){
      if (arreglo[j + 1] < arreglo[j]){
        temp = arreglo[j];
        arreglo[j] = arreglo[j + 1];
        arreglo[j + 1] = temp;
      }
    }
  }
}

void mezclar(int arreglo[], int primero, int mitad, int ultimo)
{
  int i = 0, primeroTemp = primero, mitadTemp = mitad + 1;
  int tamanoArreglo = ultimo - primero + 1;
  int *temp = malloc(sizeof(int) * (tamanoArreglo));

  while ((primeroTemp <= mitad) && (mitadTemp <= ultimo))
  {
    if (arreglo[primeroTemp] <= arreglo[mitadTemp])
    {
      temp[i] = arreglo[primeroTemp];
      primeroTemp++;
      i++;
    }
    else
    {
      temp[i] = arreglo[mitadTemp];
      mitadTemp++;
      i++;
    }
  }

  while (primeroTemp <= mitad)
    temp[i++] = arreglo[primeroTemp++];

  while (mitadTemp <= ultimo)
    temp[i++] = arreglo[mitadTemp++];

  for (int j = 0; j <= tamanoArreglo; j++)
    arreglo[primero + j] = temp[j];
}

void imprimirArreglo(int *arreglo, int tamanoArreglo)
{
  for(int i = 0; i < tamanoArreglo; i++)
  {
    printf("%d\n", arreglo[i]);
  }
}

void inicializadorCriba(int limite, int hilos)
{

  pthread_t *arregloHilos = (pthread_t*) malloc (sizeof (pthread_t) * hilos);
  pthread_attr_t attr;
  argumentosCriba *argumentos = (argumentosCriba*) malloc (sizeof (argumentosCriba) * hilos);
  int division = limite / hilos;
  int *primos = (int *) malloc (sizeof (int) * limite);
  int *numeros = (int *) malloc (sizeof (int) * limite);
  int totalPrimos = 0;
  for (int i = 0; i < hilos; i++)
  {
    if (i == 0)
    {
      argumentos[i].inicio = 2;
      argumentos[i].fin = division;
    }
    else
    {
      argumentos[i].inicio = argumentos[i - 1].fin;
      argumentos[i].fin = argumentos[i].inicio + division;
    }
  }

  sem_init(&mutex, 0, 1);

  pthread_attr_init(&attr);

  for(int i = 0; i < hilos; i++)
  {
    argumentos[i].primos = primos;
    argumentos[i].totalPrimos = &totalPrimos;
    argumentos[i].numeros = numeros;
    argumentos[i].totalNumeros = &limite;
  }

  for(int i = 0; i < hilos; i++)
  {
    pthread_create(&arregloHilos[i], &attr, criba, &argumentos[i]);
  }

  for(int i = 0; i < hilos; i++)
  {
    pthread_join(arregloHilos[i], NULL);
  }

  sem_destroy(&mutex);
  printf("Se encontraron %d números primos\n", totalPrimos);

}

void *criba(void* argumentos)
{
  argumentosCriba *argumentosTemp = (argumentosCriba*) argumentos;
  int finTemp = argumentosTemp -> fin;
  int i = argumentosTemp -> inicio;
  int j;

  sem_wait(&mutex);

  for (i; i <= finTemp; i++)
  {
    if (argumentosTemp -> numeros[i] != 1 || i == 2)
    {
      argumentosTemp -> primos[ *argumentosTemp -> totalPrimos] = i;
      printf ("%d es primo\n", i);
      for (j = 2; (j * i) <= *argumentosTemp -> totalNumeros; j++)
        argumentosTemp -> numeros[(j * i)] = 1;
      *argumentosTemp -> totalPrimos = *argumentosTemp-> totalPrimos + 1;
    }
  }

  sem_post(&mutex);
  pthread_exit(0);
}
