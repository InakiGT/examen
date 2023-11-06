#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


/*** Declaraciones para el buffer circular ***/

#define UBERBLACK 1
#define UBERX 2
#define NC 6   // Número de hilos consumidores.

// Declaración del tipo Producto para los viajes.
typedef struct {
   char destino;  // Destino del viaje en UBER
   int tiempo;    // Duración del viaje en UBER
   int tipoUber;  // Tipo de Uber: UBERBLACK o UBERX
} Producto;

// Declaración del nuevo tipo de datos Buffer
typedef struct {
   Producto* elementos;  // almacenamiento interno del buffer.
   int capacidad;   // capacidad máxima del buffer.
   int frente;      // índice para apuntar al elemento que está al frente.
   int final;       // índice para apuntar al elemento que está al final.
} Buffer;


/** Funciones para modificar el buffer. **/

// buf: dirección de variable tipo Buffer ya declarada.
// tam: tamaño máximo fijo del buffer que se creará.
void creaBuffer(Buffer* buf, int tam);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
// p: el producto que se va insertar.
static inline void meterProducto(Buffer *b, Producto p);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
// p: DIRECCIÓN de una variable de tipo producto donde se DEJARÁ el resultado.
static inline void sacarProducto(Buffer *b, Producto* p);

// buf: dirección de un buffer ya declarado e INICIALIZADO.
void destruyeBuffer(Buffer* buf);

// Estas funciones son internas al buffer, NO tienen que usarlas ustedes.
static inline bool bufLleno(Buffer* buf);  
static inline bool bufVacio(Buffer* buf);

int aleatorioEn(int minVal, int maxVal);

/*** FIN Declaraciones para el buffer circular ***/



// Número máximo de peticiones y tamaño del buffer.
#define  MAX_PETICIONES 3
Buffer bufPeticiones;    // Buffer de peticiones de viajes.

#define MAX_CONSUMIDORES 5

// NO olvidar IMPLEMENTAR abajo del main esta función.
void observador(void* ptr);
void consumidor(void* ptr);
void productorUberBlack(void* ptr);
void productorUberX(void* ptr);

/*** TERMINAR ****/ 
sem_t semLugares;
sem_t semSolicitudes;
sem_t semX;
sem_t semBlack;
sem_t clientesX;
sem_t clientesBlack;
sem_t hayViaje;
pthread_mutex_t mutexBuffer;
pthread_mutex_t mutexGanancia;
/* Declaración de semáforos y/o mutex */

/*** Fin TERMINAR */

/*** TERMINAR ****/
int gananciaTotal = 0;
int cX = 0;
int cBlack = 0;
// Declarar las variables compartidas que sean necesarias.

/*** Fin TERMINAR */



int main(int argc, char const *argv[]) 
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufPeticiones, MAX_PETICIONES);
   sem_init(&semLugares, 0, MAX_PETICIONES);
   sem_init(&semSolicitudes, 0, 0);
   sem_init(&semX, 0, 3);
   sem_init(&semBlack, 0, 3);
   sem_init(&clientesBlack, 0, 0);
   sem_init(&clientesX, 0, 0);
   pthread_mutex_init(&mutexBuffer, NULL);
   pthread_mutex_init(&mutexGanancia, NULL);
   
   sem_init(&hayViaje, 0, 0);
   /*** TERMINAR ****/ 
   // Incialización de semáforos y mutex
   /*** Fin TERMINAR */


   /*** TERMINAR ****/ 
   // Declaración de los hilos consumidores.
    pthread_t hilosCons[MAX_CONSUMIDORES];

   // Creación de los hilos NC consumidores para que ejecuten 
   //   la función "consumidor", que está abajo.
    for(int i=0; i < MAX_CONSUMIDORES; i++)
      pthread_create(&hilosCons[i], NULL, (void*) consumidor, NULL);
   /*** Fin TERMINAR */

   // Creación e inicio de los DOS hilos productores
    pthread_t hiloBlack;
    pthread_create(&hiloBlack, NULL, (void*) productorUberBlack,  NULL);

    pthread_t hiloX;
    pthread_create(&hiloX, NULL, (void*) productorUberX,  NULL);
   /*** Fin TERMINAR */

   pthread_t hiloObservador;
   pthread_create(&hiloObservador, NULL, (void *) observador, NULL);


   /** Lo que sigue será el hilo GENERADOR DE CLIENTES **/
   /*** TERMINAR ****/ 
   // Poner el ciclo que genera en cada iteración un cliente ya sea UBERX o UBERBLACK
    bool salir = false;
    while (!salir) {
        pthread_mutex_lock(&mutexGanancia);
        if (gananciaTotal >= 5000) {
            pthread_mutex_unlock(&mutexGanancia);
            salir = true;
            break;
        }
        pthread_mutex_unlock(&mutexGanancia);
        if (rand() % 2 == 0 && cX < 3) {
            cX++;
            sem_wait(&semX);
            sem_post(&clientesX);
        } else if (cBlack < 3) {
            cBlack++;
            sem_wait(&semBlack);
            sem_post(&clientesBlack);
        }
    }
   /*** Fin TERMINAR ***/


   /*** TERMINAR ****/ 
   // Esperar a TODOS los hilos productores y consumidores
    for ( int i = 0; i < MAX_CONSUMIDORES; i++ ) 
        pthread_join (hilosCons[i], NULL);
    pthread_join (hiloBlack, NULL);
    pthread_join (hiloX, NULL);
   /*** Fin TERMINAR ***/

   destruyeBuffer(&bufPeticiones);

   printf("\n\nFIN.\n\n");

   return 0;
}

/* Definición de la tarea que hará un conductor */
void observador(void* ptr) {
   /*** TERMINAR ****/ 

    bool salir = false;
    while(!salir){
        pthread_mutex_lock(&mutexGanancia);
        if (gananciaTotal >= 5000) {
            pthread_mutex_unlock(&mutexGanancia);
            salir = true;
            return;
        }
        pthread_mutex_unlock(&mutexGanancia);
        sem_wait(&hayViaje);
        printf("\nLa ganancia total actual: %d \n", gananciaTotal);
    }

   /*** Fin TERMINAR ***/
}


/* Definición de la tarea que hará un conductor */
void consumidor(void* ptr) {
   /*** TERMINAR ****/ 
    Producto viaje;
    bool salir = false;

    while(!salir) {
        sem_wait(&semSolicitudes);

        pthread_mutex_lock(&mutexBuffer);
        sacarProducto(&bufPeticiones, &viaje);
        pthread_mutex_unlock(&mutexBuffer);

        if (viaje.destino == 'x'){
            salir = true;

            sem_wait(&semLugares);
            pthread_mutex_lock(&mutexBuffer);
            meterProducto(&bufPeticiones, viaje);
            pthread_mutex_unlock(&mutexBuffer); 
            sem_post(&semSolicitudes);
        } else {
            printf("Viaje a %c\n", viaje.destino );
            sleep(viaje.tiempo);
            int ganancia = (viaje.tipoUber == UBERBLACK) ? 40 : 20;
            pthread_mutex_lock(&mutexGanancia);
            gananciaTotal += ganancia*viaje.tiempo;
            pthread_mutex_unlock(&mutexGanancia); 
            sem_post(&hayViaje);
        }

        sem_post(&semLugares);
    }

   /*** Fin TERMINAR ***/
}

void productorUberX(void* ptr) {
   Producto viaje;

   /*** TERMINAR ****/ 
   // Todo lo que falta para el ciclo principal.
   // No olvidar que al final, este productor mete al búfer NC
   // viajes especiales con destino= 'x'. I.e., uno para cada consumidor.
    bool salir = false;
    while (!salir) {
        sem_wait(&clientesX);
        viaje.destino=aleatorioEn('a', 'e');
        pthread_mutex_lock(&mutexGanancia);
        if(gananciaTotal >= 5000){
            salir = true;
            viaje.destino = 'x';
        }
        pthread_mutex_unlock(&mutexGanancia);
        viaje.tiempo=aleatorioEn(2, 4);
        
        viaje.tipoUber = UBERX;

        pthread_mutex_lock(&mutexBuffer);
        meterProducto(&bufPeticiones, viaje);
        pthread_mutex_unlock(&mutexBuffer);
        
        sem_post(&semSolicitudes);
        sem_post(&semX);
        cX--;
    }
}

void productorUberBlack(void* ptr) {
   Producto viaje;

   /*** TERMINAR ****/ 
   // Usar la variable de Ganancia Total para la condición
   // para terminar el ciclo de abajo.
   /*** Fin TERMINAR ***/
   bool salir = false;

   while (!salir) {
        sem_wait(&clientesBlack);
        viaje.destino=aleatorioEn('a', 'e');
        pthread_mutex_lock(&mutexGanancia);
        if(gananciaTotal >= 5000){
            salir = true;
            pthread_mutex_unlock(&mutexGanancia);
            return;
        }
            
        pthread_mutex_unlock(&mutexGanancia);

        viaje.tiempo=aleatorioEn(2, 4);
        
        viaje.tipoUber = UBERBLACK;
        sem_wait(&semLugares);

        pthread_mutex_lock(&mutexBuffer);
        meterProducto(&bufPeticiones, viaje);
        pthread_mutex_unlock(&mutexBuffer);
        
        sem_post(&semSolicitudes);
        sem_post(&semBlack);
        cBlack--;
   }
}





// NO ES NECESARIO VER ESTE CÓDIGO, SIMPLEMENTE HACER LAS LLAMADAS PARA USAR EL BUFFER.

int aleatorioEn(int minVal, int maxVal) {
   return (minVal + rand() % (maxVal-minVal + 1) );
}

void creaBuffer(Buffer *buf, int tam)
{
   buf->capacidad = tam;
   buf->elementos = (Producto *)malloc(tam * sizeof(Producto));
   buf->frente = -1; // Para indicar que está vacía la cola
   buf->final = -1;  // Para indicar que está vacía la cola
}

void destruyeBuffer(Buffer *buf)
{
   free(buf->elementos);
}

static inline bool bufLleno(Buffer *b)
{
   int next = (b->final + 1) % b->capacidad;

   if (b->frente == next)
      return true;
   else
      return false;
}

static inline bool bufVacio(Buffer *b)
{
   if (b->frente == -1)
      return true;
   else
      return false;
}

static inline void meterProducto(Buffer *b, Producto p)
{
   if (!bufLleno(b))
   {
      b->final = (b->final + 1) % b->capacidad;
      b->elementos[b->final] = p;
      if (b->frente == -1) // Cuando estaba vacia, el nuevo elemento esta en pos 0.
         b->frente = 0;
   }
}

static inline void sacarProducto(Buffer *b, Producto *p)
{
   if (!bufVacio(b))
   {
      *p = b->elementos[b->frente];

      if (b->frente == b->final)
      {  // Era el unico elemento.
         b->frente = -1;
         b->final = -1;
      }
      else
         b->frente = (b->frente + 1) % b->capacidad;
   }
}