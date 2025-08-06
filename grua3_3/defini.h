#ifndef DEFINI_H
#define DEFINI_H

// Constantes
#define PI 3.14f
#define RADIO .5f
#define GradosToRadianes 0.0174
#define DISTANCIA 50

// Estructura comun a todos los objetos
typedef struct {
	float px, py, pz;       // posicion inicial
	float angulo_trans;     // angulo giro x
	float velocidad;		// angulo giro z
	float sx, sy, sz;       // escalado en los dos ejes.
	unsigned int VAO;		// VAO
	int textura;
} objeto;

// Declaración de objetos globales
extern objeto base;
extern objeto cabina;
extern objeto Articulacion;
extern objeto Brazo;

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern unsigned int camara;			// cámara por defecto
extern float velocidad;
extern float angleWheel;
#endif



