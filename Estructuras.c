#include <dirent.h>
#include <stdio.h>
#include <time.h>

int main()
{
  clock_t tiempoInicial, tiempoFinal;
  long double tiempoTotal;

  DIR *directorio;
  struct dirent *estructura;
  int archivoActual = 0;
  directorio = opendir("."); //Abre el directorio actual

  printf("Leyendo directorio actual.\n\n");
  tiempoInicial = clock();

  while((estructura=readdir(directorio)))
    {
        archivoActual++;
        printf("Archivo %3d: %s\n", archivoActual, estructura->d_name);
    }

  tiempoFinal = clock();
  tiempoTotal = (long double)(tiempoFinal - tiempoInicial) / CLOCKS_PER_SEC;

  printf("\nTiempo inicial: %ld \n", tiempoInicial);
  printf("Tiempo final: %ld\n", tiempoFinal);
  printf("Velocidad del reloj: %ld\n", CLOCKS_PER_SEC);
  printf("Tomó %Lf segundos leer el directorio actual\n\n", tiempoTotal);

}
