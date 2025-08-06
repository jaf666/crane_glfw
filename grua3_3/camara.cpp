#include <stdio.h>
#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad.h>
#include <glm/glm.hpp>
#include "camara.h"

#define SHADER_VIEW_VAR "view"
#define SHADER_PROJECTION_VAR "projection"
// Dependencias externas
extern GLuint shaderProgram;
float alpha = 0.35;
float beta = 0.78;
bool modoNoche = false; // modo noche por defecto


void myCamara() {
	// Proyection matrix
	glm::mat4 projection;
	projection = glm::mat4();
	// Localizo la variable en el shader
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, SHADER_PROJECTION_VAR);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
	// Mapeo la variable en el shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// View matrix
	glm::mat4 view;
	view = glm::mat4();
	// Localizo la variable en el shader
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, SHADER_VIEW_VAR);
	// La función lookAt recibe la posición de la cámara, el punto al que mira y el vector que indica la dirección de arriba
	// La cámara se situa en la posición en coordenadas polares: DISTANCIA * cos(alpha) * sin(beta), DISTANCIA * cos(beta), DISTANCIA * sin(alpha) * sin(beta)
	// El punto al que mira es el origen de coordenadas
	// El vector que indica la dirección de arriba es el eje Y, al cual está orientado
	view = glm::lookAt(glm::vec3(DISTANCIA * cos(alpha) * sin(beta), DISTANCIA * cos(beta), DISTANCIA * sin(alpha) * sin(beta)), // Posición de la cámara
		glm::vec3(0.0f, 0.0f, 0.0f), // Punto al que mira
		glm::vec3(0.0, 1.0, 0.0));  // Vector que indica la dirección de arriba, en este caso se usa el eje Y para mantener dicho eje como la vertical

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void myPrimeraPersona(objeto coche_h) {
	// Proyection matrix
	glm::mat4 projection;
	projection = glm::mat4();
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, SHADER_PROJECTION_VAR);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// View matrix
	glm::mat4 view;
	view = glm::mat4();
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, SHADER_VIEW_VAR);
	view = glm::lookAt(glm::vec3(coche_h.px, coche_h.py + 10, coche_h.pz),
		glm::vec3(coche_h.px + sin((coche_h.angulo_trans) * GradosToRadianes), 10, coche_h.pz + cos((coche_h.angulo_trans) * GradosToRadianes)),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void myTerceraPersona(objeto coche_h) {
	// Proyection matrix
	glm::mat4 projection;
	projection = glm::mat4();
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, SHADER_PROJECTION_VAR);
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// View matrix
	glm::mat4 view;
	view = glm::mat4();
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, SHADER_VIEW_VAR);
	view = glm::lookAt(glm::vec3(coche_h.px - 20 * sin(coche_h.angulo_trans * GradosToRadianes), coche_h.py + 10, coche_h.pz - 20 * cos(coche_h.angulo_trans * GradosToRadianes)),
		glm::vec3(coche_h.px + sin((coche_h.angulo_trans) * GradosToRadianes), 10, coche_h.pz + cos((coche_h.angulo_trans) * GradosToRadianes)),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

}

//	Gestión de teclado de movimiento de la grúa y la cámara
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			// Teclas para el desplazamiento de la grúa, tanto hacia adelante como hacia los lados
		case GLFW_KEY_S:				// Mover hacia adelante
			velocidad -= .1f;
			break;
		case GLFW_KEY_W:				// Mover hacia atrás
			velocidad += .1f;
			break;
		case GLFW_KEY_A:				// Girar a la izquierda
			base.angulo_trans += 5.0f;
			break;
		case GLFW_KEY_D:				// Girar a la derecha
			base.angulo_trans -= 5.0f;
			break;
			// Movimiento de el brazo del la grua
		case GLFW_KEY_E:				// Mover brazo de la grua hacia adelante
			if (Brazo.angulo_trans <= 30) Brazo.angulo_trans++;
			break;
		case GLFW_KEY_R:				// Mover brazo de la grua hacia atras
			if (Brazo.angulo_trans >= -85) Brazo.angulo_trans--;
			break;
			// Teclas para cambiar el modo de camara
		case GLFW_KEY_1:				// Cámara exterior
			camara = 1;
			break;
		case GLFW_KEY_2:				// Cámara en primera persona
			camara = 2;
			break;
		case GLFW_KEY_3:				// Cámara en tercera persona
			camara = 3;
			break;
			// Mueve la cámara
		case GLFW_KEY_UP:
			beta += 0.05;
			break;
		case GLFW_KEY_DOWN:
			beta -= 0.05;
			break;
		case GLFW_KEY_LEFT:
			alpha -= 0.05;
			break;
		case GLFW_KEY_RIGHT:
			alpha += 0.05;
			break;
		case GLFW_KEY_N:
			modoNoche = !modoNoche;
			if (modoNoche) {
				glUniform1i(glGetUniformLocation(shaderProgram, "modoNoche"), true);
				std::cout << "Modo cambiado a: " << (modoNoche ? "Noche" : "Día") << std::endl;
			}
			else {
				glUniform1i(glGetUniformLocation(shaderProgram, "modoNoche"), false);
				std::cout << "Modo cambiado a: " << (modoNoche ? "Noche" : "Día") << std::endl;
			}
			break;

		default:
			break;
		}
	}
	if (action == GLFW_RELEASE) {
		base.velocidad = 0.0f; // Detener la grúa cuando se suelta la tecla
	}
}

void resize(GLFWwindow* window, int width, int height) {
	// Cambiamos la anchura
	SCR_WIDTH = width;
	// Cambiamos la altura
	SCR_HEIGHT = height;
}