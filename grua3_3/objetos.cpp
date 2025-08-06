
#include "defini.h"
// Definicion de los objetos
objeto base = { 0, 1.5, 0.5, 0, 0, 10, 2, 4, 0, 0 };
objeto cabina = { 4.0, 0.5, 0, 0, 0, 2.0f, 3.0f, 4.0f, 0, 0 };
objeto Articulacion = { -.4, 0.4, 0, 35, 0, 1.0, 1.0, 0, 0 };
objeto Brazo = { 0.0, 3, 0, 0, 0, .5, 6.0, .5, 0, 0 };

unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 1200;
unsigned int camara = 1;			// cámara por defecto
extern float velocidad = 0;
// Ángulo de rotación de las ruedas
float angleWheel = 0.0f;