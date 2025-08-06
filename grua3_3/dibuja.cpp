#include "dibuja.h"
#include "defini.h"
#include "vaos.h"
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#define SHADER_MODEL_VAR "model"
#define SHADER_COLOR_VAR "Color"
// Dependencias externas
extern GLuint shaderProgram;
extern int metal;
extern int rueda;
extern int arbolText;
extern int hierba;
extern unsigned int vaoArbol;
extern float angleWheel;
// Número de árboles que va a haber
const int NUM_ARBOLES = 4;

// Estructura básica para un árbol
struct Arbol {
	glm::vec3 posicion;
	glm::vec3 escala;
};

// Declaramos un array fijo de árboles
Arbol arboles[NUM_ARBOLES] = {
	{ glm::vec3(10, 3,  10), glm::vec3(4, 8, 1) },
	{ glm::vec3(-10, 3,  10), glm::vec3(3, 8, 1) },
	{ glm::vec3(10, 3, -10), glm::vec3(4, 8, 1) },
	{ glm::vec3(-10, 3, -10), glm::vec3(4, 8, 1) },
};

// Función para dibujar un árbol individual
void drawArbol(const Arbol& arbol) {
	glm::mat4 transform = glm::translate(glm::mat4(), arbol.posicion);
	transform = glm::scale(transform, arbol.escala);

	unsigned int transformLoc = glGetUniformLocation(shaderProgram, SHADER_MODEL_VAR);
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, SHADER_COLOR_VAR);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3f(colorLoc, 0.8f, 0.8f, 0.8f);

	glBindTexture(GL_TEXTURE_2D, arbolText);
	glBindVertexArray(vaoArbol);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Función principal que recorre todos los árboles y los dibuja
void drawArboles() {
	glDisable(GL_CULL_FACE);

	for (int i = 0; i < NUM_ARBOLES; i++) {
		drawArbol(arboles[i]);
	}

	glEnable(GL_CULL_FACE);
}

///////////////////////
// Dibujo de las ruedas
///////////////////////
struct Rueda {
	glm::vec3 offset;  // posición relativa desde la base
};
const int NUM_RUEDAS = 5;

Rueda ruedas[NUM_RUEDAS] = {
	{{ 3.5f, -0.5f,  2.0f }},
	{{ 3.5f, -0.5f, -2.0f }},
	{{-3.5f, -0.5f,  2.0f }},
	{{-3.5f, -0.5f, -2.0f }},
	{{ 3.5f, -0.5f,  2.0f }}  
};
void drawWheel() {
	glBindTexture(GL_TEXTURE_2D, rueda);
	glBindVertexArray(VAOCubo);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void drawRueda(const glm::mat4& baseTransform, const Rueda& rueda, GLuint transformLoc) {
	glm::mat4 transform = baseTransform;
	transform = glm::translate(transform, rueda.offset);
	transform = glm::rotate(transform, (float)(-angleWheel * GradosToRadianes), glm::vec3(0, 0, 1));
	transform = glm::scale(transform, glm::vec3(1.5f, 1.5f, 1.5f));

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	drawWheel();
}

void drawRuedas() {
	glm::mat4 transform;
	glm::mat4 stack;

	GLuint transformLoc = glGetUniformLocation(shaderProgram, SHADER_MODEL_VAR);
	GLuint colorLoc = glGetUniformLocation(shaderProgram, SHADER_COLOR_VAR);

	// Color negro
	glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f);

	// Transformación base del coche
	transform = glm::translate(glm::mat4(), glm::vec3(base.px, base.py, base.pz));
	transform = glm::rotate(transform, (float)((base.angulo_trans - 90) * GradosToRadianes), glm::vec3(0, 1, 0));

	stack = glm::scale(transform, glm::vec3(3.5f, -0.5f, 2.0f)); // original, pero la escala no debe estar aquí

	for (int i = 0; i < NUM_RUEDAS; ++i) {
		drawRueda(transform, ruedas[i], transformLoc);
	}
}

void drawGrua() {
	glUseProgram(shaderProgram);
	glm::mat4 transform; // es la matriz de transformacion
	glm::mat4 stack; // guardo la matriz de transformacion
	// Localiza la matriz del modelo dentro del vertex shader
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, SHADER_MODEL_VAR);
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, SHADER_COLOR_VAR);
	// Dibujo los cuadrados
	//int cuboScale = 1;
	//transform = glm::mat4(); // identity matrix
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//transform = glm::scale(transform, glm::vec3(5, 5, 5));
	//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	//glBindVertexArray(VAO);
	//glDrawArrays(GL_LINES, 0, 6);

	// SUELO
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	transform = glm::mat4();
	transform = glm::scale(transform, glm::vec3(100, 1, 100));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0, 1, 0)));

	
	glBindTexture(GL_TEXTURE_2D, hierba);
	glBindVertexArray(VAOCuadradoXZ);
	glEnable(GL_BLEND);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

	// Coche
	// Base
	//glEnable(GL_BLEND);
	transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(base.px, base.py, base.pz));
	transform = glm::rotate(transform, (float)((base.angulo_trans - 90) * GradosToRadianes), glm::vec3(0.0f, 1.0f, 0.0f));
	stack = transform;
	transform = glm::scale(transform, glm::vec3(base.sx, base.sy, base.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(1, 0, 0)));
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, metal);
	glBindVertexArray(base.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Como se hereda la matriz de transformacion, la siguiente transformación se aplica a la base
	transform = stack;
	transform = glm::translate(transform, glm::vec3(cabina.px, cabina.py, cabina.pz));
	transform = glm::rotate(transform, (float)((cabina.angulo_trans)), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::scale(transform, glm::vec3(cabina.sx, cabina.sy, cabina.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(1, .0, .0f)));
	glBindTexture(GL_TEXTURE_2D, metal);
	glBindVertexArray(cabina.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_BLEND);
	// GRUA
	// Articulaciones
	transform = stack;
	transform = glm::translate(transform, glm::vec3(Articulacion.px, Articulacion.py, Articulacion.pz));
	transform = glm::rotate(transform, (float)(Articulacion.angulo_trans * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f));
	stack = transform;
	transform = glm::scale(transform, glm::vec3(Articulacion.sx, Articulacion.sy, Articulacion.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0, 1, 1)));
	glBindTexture(GL_TEXTURE_2D, rueda);
	glBindVertexArray(Articulacion.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 1080);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Brazo
	transform = stack; // stack ya contiene la transformación hasta la articulación
	transform = glm::rotate(transform, (float)(Brazo.angulo_trans * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f)); // rotar primero
	transform = glm::translate(transform, glm::vec3(Brazo.px, Brazo.py, Brazo.pz)); // luego trasladar el brazo
	transform = glm::scale(transform, glm::vec3(Brazo.sx, Brazo.sy, Brazo.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glBindTexture(GL_TEXTURE_2D, rueda);
	glBindVertexArray(Brazo.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_2D, 0);
}

