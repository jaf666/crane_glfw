#include "vaos.h"
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "esfera.h"  // contiene vertices_esfera y vertices_cubo
#include "defini.h"

// Definición de los VAOs
unsigned int VAO_esfera;
unsigned int VAOCubo;				// Vertex Array Object para los vertices del cubo
unsigned int VAO;					// Vertex Array Object para los vertices
unsigned int VAOCuadradoXZ;			// Vertex Array Object para los vertices del cuadrado
unsigned int vaoArbol;				// Vertex Array Object para los vertices del arbol

// Función que crea los VAO en funcion del numero de atributos que tenga
void _crearVAO(unsigned int& VAO, float* vertices, size_t verticesSize,
	int posSize, int colorSize, int normalSize, int texSize) {
	unsigned int VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);

	int stride = (posSize + colorSize + normalSize + texSize) * sizeof(float);
	int offset = 0;

	if (posSize > 0) {
		glVertexAttribPointer(0, posSize, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(0);
		offset += posSize * sizeof(float);
	}
	if (colorSize > 0) {
		glVertexAttribPointer(1, colorSize, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(1);
		offset += colorSize * sizeof(float);
	}
	if (normalSize > 0) {
		glVertexAttribPointer(2, normalSize, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(2);
		offset += normalSize * sizeof(float);
	}
	if (texSize > 0) {
		glVertexAttribPointer(3, texSize, GL_FLOAT, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(3);
		offset += texSize * sizeof(float);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}

// Funcion para dibujar una esfera empleando esfera.h
void _dibuja_esfera() {
	_crearVAO(VAO_esfera, vertices_esfera, sizeof(vertices_esfera), 3, 0, 3, 2);
}

void _dibujaCubo() {
	_crearVAO(VAOCubo, vertices_cubo, sizeof(vertices_cubo), 3, 3, 3, 2);
}

void _dibujaArbol() {
	_crearVAO(vaoArbol, verticesArbol, sizeof(verticesArbol), 3, 3, 3, 2);
}

void _dibujaCuadradoXZ() {
	_crearVAO(VAOCuadradoXZ, verticesCuadrado, sizeof(verticesCuadrado), 3, 3, 3, 2);
}

// Función privada que inicializa los VAOs correspondientes a cada parte de la grúa
void _initVaos() {
	base.VAO = VAOCubo;
	cabina.VAO = VAOCubo;
	Articulacion.VAO = VAO_esfera;
	Brazo.VAO = VAOCubo;
}

void initGeometria() {
	// Dibujo todo
	_dibujaCubo();
	_dibujaCuadradoXZ();
	_dibujaArbol();
	_dibuja_esfera();
	// Asigno cada VAO
	_initVaos();
}