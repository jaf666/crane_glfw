#ifndef VAOS_H
#define VAOS_H

// VAOs globales
extern unsigned int VAO_esfera;
extern unsigned int VAOCubo;				// Vertex Array Object para los vertices del cubo
extern unsigned int VAO;					// Vertex Array Object para los vertices
extern unsigned int VAOCuadradoXZ;			// Vertex Array Object para los vertices del cuadrado
extern unsigned int vaoArbol;				// Vertex Array Object para los vertices del arbol
// Inicializa todos los VAOs necesarios
void initGeometria();

#endif