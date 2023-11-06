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
void consumidor(void* ptr);
void productorUberBlack(void* ptr);
void productorUberX(void* ptr);

/*** TERMINAR ****/ 
/* Declaración de semáforos y/o mutex */
sem_t semLugares;
sem_t semSolicitudes;
pthread_mutex_t mutexBuffer;
pthread_mutex_t mutexGanancia;

// Falta semaforo del observador
sem_t hayViaje;

/*** Fin TERMINAR */

/*** TERMINAR ****/
// Declarar las variables compartidas que sean necesarias.
int gananciaTotal = 0;
/*** Fin TERMINAR */

int main(int argc, char const *argv[]) 
{
   // Inicializar el buffer con una capacidad máxima FIJA.
   creaBuffer(&bufPeticiones, MAX_PETICIONES);

   /*** TERMINAR ****/ 
   // Incialización de semáforos y mutex
   sem_init(&semLugares, 0, MAX_PETICIONES);
   sem_init(&semSolicitudes, 0, 0);
   pthread_mutex_init(&mutexBuffer, NULL);
   pthread_mutex_init(&mutexGanancia, NULL);
   
   sem_init(&hayViaje, 0, 0);
   /*** Fin TERMINAR */


   /*** TERMINAR ****/ 
   // Declaración de los hilos consumidores.
   pthread_t hilosCons[MAX_CONSUMIDORES];

   // Creación de los hilos consumidores para que ejecuten 
   //   la función "consumidor", que está abajo.
   for(int i=0; i < MAX_CONSUMIDORES; i++){
      pthread_create(&hilosCons[i], NULL, (void*) consumidor, NULL);
   }

   /*** Fin TERMINAR */

   // Creación e inicio de los DOS hilos productores
   pthread_t hiloBlack;
   pthread_create(&hiloBlack, NULL, (void*) productorUberBlack,  NULL);

   pthread_t hiloX;
   pthread_create(&hiloX, NULL, (void*) productorUberX,  NULL);


   /** Lo que sigue será el hilo OBSERVADOR **/
   /*** TERMINAR ****/ 
   // Poner el ciclo del hilo que muestra la Ganacia Total solamente cuando realmente
   while(gananciaTotal < 5000){
      sem_wait(&hayViaje);
      printf("\nLa ganancia total actual: %d \n", gananciaTotal);
   }
   // se hace un viaje.    
   /*** Fin TERMINAR ***/


   /*** TERMINAR ****/ 
   // Esperar a TODOS los hilos productores y consumidores
   /*** Fin TERMINAR ***/


   destruyeBuffer(&bufPeticiones);
   sem_destroy(&semLugares);
   sem_destroy(&semSolicitudes);
   pthread_mutex_destroy(&mutexBuffer);
   pthread_mutex_destroy(&mutexGanancia);
   printf("\n\nFIN.\n\n");

   return 0;
}

/* Definición de la tarea que hará un conductor */
void consumidor(void* ptr) {
   /*** TERMINAR ****/ 
   // La tarea para sacar y procesar viajes de ambos tipos de Uber.
   Producto viaje;

   bool salir = false;

   while(!salir){
      sem_wait(&semSolicitudes);

      pthread_mutex_lock(&mutexBuffer);
      sacarProducto(&bufPeticiones, &viaje);
      pthread_mutex_unlock(&mutexBuffer);

      if(viaje.destino == 'x'){
         salir = true;

         // EL consumidor vuelve a meter la solicitud 'x'
         sem_wait(&semLugares);
         pthread_mutex_lock(&mutexBuffer);
         sacarProducto(&bufPeticiones, &viaje);
         pthread_mutex_unlock(&mutexBuffer); 
         sem_post(&semSolicitudes);
      }
      else{
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
   // Usar la variable de Ganancia Total para la condición
   // para terminar el ciclo de abajo.
   /*** Fin TERMINAR ***/
   bool salir = false;

   while (!salir) {
      
      viaje.destino=aleatorioEn('a', 'e');
      pthread_mutex_lock(&mutexGanancia);
      if(gananciaTotal >= 5000){
         salir = true;
         viaje.destino = 'x';
      }
         
      pthread_mutex_unlock(&mutexGanancia);
      /*** TERMINAR ****/ 
      // Asignar a 'viaje' un destino (char entre 'a' y 'e'),  
      // un tiempo (entre 2 y 6 segs.) y ...

      viaje.tiempo=aleatorioEn(2, 4);
      /*** Fin TERMINAR ***/
      
      viaje.tipoUber = UBERX; // ... el tipo de Uber.

      /*** TERMINAR ****/ 
      // Uso de semáforos ANTES de meter el viaje
      printf("Productor verificando si hay lugares...\n"); fflush(stdout);
      sem_wait(&semLugares);
      /*** Fin TERMINAR ***/

      pthread_mutex_lock(&mutexBuffer);
      meterProducto(&bufPeticiones, viaje);
      pthread_mutex_unlock(&mutexBuffer);
      
      /*** TERMINAR ****/ 
      // Uso de semáforos DESPUÉS de meter el viaje
      sem_post(&semSolicitudes);
      /*** Fin TERMINAR ***/
      
      sleep( aleatorioEn(1, 2) );
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
      
      viaje.destino=aleatorioEn('a', 'e');
      pthread_mutex_lock(&mutexGanancia);
      if(gananciaTotal >= 5000){
         salir = true;
         pthread_mutex_unlock(&mutexGanancia);
         return;
         //viaje.destino = 'x';
      }
         
      pthread_mutex_unlock(&mutexGanancia);
      /*** TERMINAR ****/ 
      // Asignar a 'viaje' un destino (char entre 'a' y 'e'),  
      // un tiempo (entre 2 y 6 segs.) y ...

      viaje.tiempo=aleatorioEn(2, 4);
      /*** Fin TERMINAR ***/
      
      viaje.tipoUber = UBERBLACK; // ... el tipo de Uber.

      /*** TERMINAR ****/ 
      // Uso de semáforos ANTES de meter el viaje
      printf("Productor verificando si hay lugares...\n"); fflush(stdout);
      sem_wait(&semLugares);
      /*** Fin TERMINAR ***/

      pthread_mutex_lock(&mutexBuffer);
      meterProducto(&bufPeticiones, viaje);
      pthread_mutex_unlock(&mutexBuffer);
      
      /*** TERMINAR ****/ 
      // Uso de semáforos DESPUÉS de meter el viaje
      sem_post(&semSolicitudes);
      /*** Fin TERMINAR ***/
      
      sleep( aleatorioEn(1, 2) );
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
