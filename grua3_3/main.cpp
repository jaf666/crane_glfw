//Programa que simplemente crea un contexto de trabajo con glfw
#include <stdio.h>
#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include "lecturaShader_0_9.h"
//#include "esfera.h"
#include "camara.h"
#include "defini.h"
#include "texturas.h"
#include "dibuja.h"
#include "vaos.h"
// Para las transformaciones, permiten manejar matrices 4x4 con metodos trasladar, rotar y escalar
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define VERTEX_SHADER_FILE "shader.vert"
#define FRAGMENT_SHADER_FILE "shader.frag"

extern GLuint setShaders(const char* nVertx, const char* nFrag);

GLuint shaderProgram;

// Variables para el calculo de FPS
double lastTime = glfwGetTime();
double currentTime;
double lapsoTime;
int nbFrames = 0;

//GLfloat angleWheel = 0.0f;

// Funcion para calcular el tiempo transcurrido entre frames y el numero de frames por segundo
void tiempo() {
	static float unSegundo = 0;
	// Calculo de FPS
	currentTime = glfwGetTime();
	nbFrames++;
	lapsoTime = currentTime - lastTime;
	unSegundo += lapsoTime;
	if (unSegundo >= 1.0) {
		printf("%f frame/s\n", double(nbFrames));
		nbFrames = 0;
		unSegundo = 0;
	}
	lastTime = currentTime;
}

// Funcion para el movimiento de la grua
void myMovimiento() {
	// El movimiento se produce en los ejes X y Z
	// Espacio final es igual a espacio inicial mas la velocidad por el tiempo transcurrido
	base.px = base.px + velocidad * sin(base.angulo_trans * GradosToRadianes) * lapsoTime;
	base.pz = base.pz + velocidad * cos(base.angulo_trans * GradosToRadianes) * lapsoTime;

	angleWheel += base.velocidad;
	if (angleWheel > 360) angleWheel -= 360;
}

void openGLInit() {
	// Inicializaciones varias
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Se ha desabilitado el CULL FACE para que se vean las caras de adelante y atrás del suelo
	// glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
}

void iluminacion() {
	// El color de la luz ambiente, que se pasa como parámetro al shader
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightLoc, .7f, .7f, .7f);

	// Posición de la luz, que es la de la grúa
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, base.px, base.py, base.pz);

	// Dirección del foco, que es la dirección de movimiento de la grúa, en los ejes X y Z.
	unsigned int lightDirLoc = glGetUniformLocation(shaderProgram, "luzDir");
	glUniform3f(lightDirLoc, sin(base.angulo_trans * GradosToRadianes), 0, cos(base.angulo_trans * GradosToRadianes));

	// Luz especular, necesaria para generar los reflejos. Se emplea la posición de la base de la grúa.
	unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, base.px, base.py, base.pz);

}

void Display() {
	// Dibujo de la escena
	// ------

	// render 
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (camara) {
	case 1: myCamara(); break;
	case 2: myPrimeraPersona(base); break;
	case 3: myTerceraPersona(base); break;
	}

	iluminacion();
	tiempo();
	myMovimiento();

	drawGrua();

	drawRuedas();

	drawArboles();

	glBindVertexArray(0);

}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void gestionShaders() {
	// Creamos una matriz de transformación 
	glm::mat4 transform = glm::mat4(1.0f);
	// Obtenemos la ubicación de la matriz de transformación en el shader
	GLint trans1Loc = glGetUniformLocation(shaderProgram, "transform");

	// Seleccionamos el shader para su uso en el renderizado
	glUseProgram(shaderProgram);
	// Enviamos la matriz de transformación al shader
	glUniformMatrix4fv(trans1Loc, 1, GL_FALSE, glm::value_ptr(transform));
	// Cargamos los shaders
	shaderProgram = setShaders(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();														// inicializa glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// Version del Core de Opengl a usar
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// Cersion del core the Opengl a usar
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Indica que debe crear el opengl contexto CORE

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Clases", NULL, NULL);		// crea la ventana
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);															// crea el contexto de trabajo dentro de la ventana

	glfwSetFramebufferSizeCallback(window, resize);											// Callback para el cambio de tamaño de la ventana			

	// Captura los punteros de opengl y los carga en glad
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// Funcion para la gestion de los shaders	
	shaderProgram = setShaders(VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE);
	if (shaderProgram == 0) {
		std::cerr << "Error: shader program inválido." << std::endl;
		return -1;
	}
	// Se setea la función de teclado para el callback
	glfwSetKeyCallback(window, keyCallback);
	// Inicializaciones varias
	openGLInit();

	// Asignación de VAOs a cada elemento y dibujo de todos los elementos
	initGeometria();

	// Cargo las texturas de cada objeto
	cargarTexturas();
	
	// Lazo principal.
	while (!glfwWindowShouldClose(window))
	{
		// Entrada de teclado
		processInput(window);						// procesamos los eventos de entrada

		Display();

		glfwSwapBuffers(window);					// los eventos de glfw, techlado raton....
		glfwPollEvents();							// los eventos de openGl. 
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAOCuadradoXZ);
	glDeleteVertexArrays(1, &VAO_esfera);
	glDeleteVertexArrays(1, &VAOCubo);
	glDeleteVertexArrays(1, &vaoArbol);

	glfwTerminate();
	return 0;
}

