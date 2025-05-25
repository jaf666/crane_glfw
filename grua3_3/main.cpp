
//Programa que simplemente crea un contexto de trabajo con glfw

#include <stdio.h>
#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include "lecturaShader_0_9.h"
#include "esfera.h"
// Para las transformaciones, permiten manejar matrices 4x4 con metodos trasladar, rotar y escalar
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define PI 3.14f
#define RADIO .5f
#define GradosToRadianes 0.0174

bool modoNoche = false; // modo noche por defecto

// Settings
unsigned int SCR_WIDTH = 1200;
unsigned int SCR_HEIGHT = 1200;

extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

// Estructura comun a todos los objetos
typedef struct {
	float px, py, pz;       // posicion inicial
	float angulo_trans;     // angulo giro x
	float velocidad;		// angulo giro z
	float sx, sy, sz;       // escalado en los dos ejes.
	unsigned int VAO;		// VAO
	int textura;
} objeto;

// Definicion de los objetos
objeto base = { 0, 1.5, 0.5, 0, 0, 10, 2, 4, 0, 0 };
objeto cabina = { 4.0, 0.5, 0, 0, 0, 2.0f, 3.0f, 4.0f, 0, 0 };
objeto Articulacion = { -.4, 0.4, 0, 35, 0, 1.0, 1.0, 0, 0 };
objeto Brazo = { 0.0, 3, 0, 0, 0, .5, 6.0, .5, 0, 0 };

int hierba, arbolText, metal, rueda;

// VAOs que corresponden a las listas de render

unsigned int VAO;					// Vertex Array Object para los vertices
unsigned int VAOCuadradoXZ;			// Vertex Array Object para los vertices del cuadrado
unsigned int VAOCubo;				// Vertex Array Object para los vertices del cubo
unsigned int VAO_esfera;
unsigned int vaoArbol;				// Vertex Array Object para los vertices del arbol

unsigned int camara = 1;			// cámara por defecto

float velocidad = 0;
float angulo_giro = 0;

// Variables para el calculo de FPS
double lastTime = glfwGetTime();
double currentTime;
double lapsoTime;
int nbFrames = 0;

GLfloat angleWheel = 0.0f;

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

int myCargaTexturas(const char* nome) {
	GLuint texture;
	int width, height, nrChannels;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	// load image, create texture and generate mipmaps
	unsigned char* data = stbi_load(nome, &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
		return -1;
	}
	stbi_image_free(data);
	return (texture);
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

void dibujaArbol() {
	unsigned int VBO, EBO;
	float vertices[] = {
		-.5f, -.5f, .0f,		1.0f, 1.0f, 1.0f,		0.0f, .0f, 1.0f,	0, 0,
		 .5f, -0.5f, .0f,		1.0f, 1.0f, 1.0f,		0.0f, .0f, 1.0f,	1, 0,
		 .5f,  0.5f, .0f,		1.0f, 1.0f, 1.0f,		0.0f, .0f, 1.0f,	1, 1,

		 .5f,  0.5f, .0f,		1.0f, 1.0f, 1.0f,		0.0f, .0f, 1.0f,	1, 1,
		-.5f,  0.5f, .0f,		1.0f, 1.0f, 1.0f,		0.0f, .0f, 1.0f,	0, 1,
		-.5f,  -0.0f, .0f,		1.0f, 1.0f, 1.0f,		0.0f, .0f, 1.0f,	0, 0
	};

	glGenVertexArrays(1, &vaoArbol);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(vaoArbol);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// position normales
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// position textura
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void dibujaEjes() {
	unsigned int VBO, EBO;
	float vertices[] = {
		// Vertices				// Colores
		0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,	// 0
		.5f, 0.0f, 0.0f,		1.0f, 0.0f, 0.0f,	// x
		.0f, .5f, 0.0f,			0.0f, 1.0f, 0.0f,	// y
		0.0f, .5f, 0.0f,		0.0f, 0.0f, 1.0f,	// z
		.5f, .5f, 0.5f,			1.0f, 1.0f, 1.0f,	// 1,1,1 bueno realmente la mitad de la diagonal
	};
	unsigned int indices[] = {
		0, 1,
		0, 2,
		0, 3,
		0, 4
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void dibujaCuadradoXZ() {
	unsigned int VBO, EBO;
	float vertices[] = {
		-.5f, 0.0f, .5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0, 0,
		 .5f, 0.0f, .5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	10, 0,
		 .5f,  0.0f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	10, 10,

		 -.5f,  0.0f, .5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	10, 10,
		.5f,  0.0f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0, 10,
		-.5f,  0.0f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f, 0.0f,	0, 0
	};

	glGenVertexArrays(1, &VAOCuadradoXZ);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAOCuadradoXZ);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// position normales
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// position textura
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

}

void dibujaCubo() {
	unsigned int VBO, EBO;
	float vertices[] = {
		// Cara frontal (+Z)
		-0.5f, -0.5f,  0.5f, 1,0,0,  0,0,1,   0.0f, 0.0f,  // V0
		 0.5f, -0.5f,  0.5f, 1,0,0,  0,0,1,   1.0f, 0.0f,  // V1
		 0.5f,  0.5f,  0.5f, 1,0,0,  0,0,1,   1.0f, 1.0f,  // V2
		 0.5f,  0.5f,  0.5f, 1,0,0,  0,0,1,   1.0f, 1.0f,  // V2 
		-0.5f,  0.5f,  0.5f, 1,0,0,  0,0,1,   0.0f, 1.0f,  // V3
		-0.5f, -0.5f,  0.5f, 1,0,0,  0,0,1,   0.0f, 0.0f,  // V0 

		// Cara trasera (-Z)
		-0.5f, -0.5f, -0.5f, 1,0,0,  0,0,-1,  0.0f, 0.0f,  // V4
		-0.5f,  0.5f, -0.5f, 1,0,0,  0,0,-1,  0.0f, 1.0f,  // V5
		 0.5f,  0.5f, -0.5f, 1,0,0,  0,0,-1,  1.0f, 1.0f,  // V6
		 0.5f,  0.5f, -0.5f, 1,0,0,  0,0,-1,  1.0f, 1.0f,  // V6 
		 0.5f, -0.5f, -0.5f, 1,0,0,  0,0,-1,  1.0f, 0.0f,  // V7
		-0.5f, -0.5f, -0.5f, 1,0,0,  0,0,-1,  0.0f, 0.0f,  // V4 

		// Cara izquierda (-X)
		-0.5f,  0.5f,  0.5f, 1,0,0,  -1,0,0,  1.0f, 1.0f,  // V3
		-0.5f,  0.5f, -0.5f, 1,0,0,  -1,0,0,  0.0f, 1.0f,  // V5
		-0.5f, -0.5f, -0.5f, 1,0,0,  -1,0,0,  0.0f, 0.0f,  // V4
		-0.5f, -0.5f, -0.5f, 1,0,0,  -1,0,0,  0.0f, 0.0f,  // V4 
		-0.5f, -0.5f,  0.5f, 1,0,0,  -1,0,0,  1.0f, 0.0f,  // V0
		-0.5f,  0.5f,  0.5f, 1,0,0,  -1,0,0,  1.0f, 1.0f,  // V3 

		// Cara derecha (+X)
		 0.5f,  0.5f,  0.5f, 1,0,0,  1,0,0,   0.0f, 1.0f,  // V2
		 0.5f, -0.5f, -0.5f, 1,0,0,  1,0,0,   1.0f, 0.0f,  // V7
		 0.5f,  0.5f, -0.5f, 1,0,0,  1,0,0,   1.0f, 1.0f,  // V6
		 0.5f, -0.5f, -0.5f, 1,0,0,  1,0,0,   1.0f, 0.0f,  // V7 
		 0.5f,  0.5f,  0.5f, 1,0,0,  1,0,0,   0.0f, 1.0f,  // V2 
		 0.5f, -0.5f,  0.5f, 1,0,0,  1,0,0,   0.0f, 0.0f,  // V1

		 // Cara inferior (-Y)
		 -0.5f, -0.5f, -0.5f, 1,0,0,  0,-1,0,  0.0f, 1.0f,  // V4
		  0.5f, -0.5f, -0.5f, 1,0,0,  0,-1,0,  1.0f, 1.0f,  // V7
		  0.5f, -0.5f,  0.5f, 1,0,0,  0,-1,0,  1.0f, 0.0f,  // V1
		  0.5f, -0.5f,  0.5f, 1,0,0,  0,-1,0,  1.0f, 0.0f,  // V1 
		 -0.5f, -0.5f,  0.5f, 1,0,0,  0,-1,0,  0.0f, 0.0f,  // V0
		 -0.5f, -0.5f, -0.5f, 1,0,0,  0,-1,0,  0.0f, 1.0f,  // V4 

		 // Cara superior (+Y)
		 -0.5f,  0.5f, -0.5f, 1,0,0,  0,1,0,   0.0f, 1.0f,  // V5
		  0.5f,  0.5f,  0.5f, 1,0,0,  0,1,0,   1.0f, 0.0f,  // V2
		  0.5f,  0.5f, -0.5f, 1,0,0,  0,1,0,   1.0f, 1.0f,  // V6
		  0.5f,  0.5f,  0.5f, 1,0,0,  0,1,0,   1.0f, 0.0f,  // V2 
		 -0.5f,  0.5f, -0.5f, 1,0,0,  0,1,0,   0.0f, 1.0f,  // V5 
		 -0.5f,  0.5f,  0.5f, 1,0,0,  0,1,0,   0.0f, 0.0f   // V3
	};


	glGenVertexArrays(1, &VAOCubo);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAOCubo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);					// posición
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); // color
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); // normales
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(9 * sizeof(float))); // texturas
	glEnableVertexAttribArray(3);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
}


// Funcion para dibujar una esfera empleando esfera.h

void dibuja_esfera() {
	unsigned int VBO;

	glGenVertexArrays(1, &VAO_esfera);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO_esfera);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	// Normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// textura
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);

}

#define DISTANCIA 50
float alpha = 0.35;
float beta = 0.78;

void openGLInit() {
	// Inicializaciones varias
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);
	// Se ha desabilitado el CULL FACE para que se vean las caras de adelante y atrás del suelo
	// glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

// Funciones de camara para los distintos tipos de camara

void myCamara() {
	// Proyection matrix
	glm::mat4 projection;
	projection = glm::mat4();
	// Localizo la variable en el shader
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
	// Mapeo la variable en el shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// View matrix
	glm::mat4 view;
	view = glm::mat4();
	// Localizo la variable en el shader
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
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
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// View matrix
	glm::mat4 view;
	view = glm::mat4();
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	view = glm::lookAt(glm::vec3(coche_h.px, coche_h.py + 10, coche_h.pz),
		glm::vec3(coche_h.px + sin((coche_h.angulo_trans) * GradosToRadianes), 10, coche_h.pz + cos((coche_h.angulo_trans) * GradosToRadianes)),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void myTerceraPersona(objeto coche_h) {
	// Proyection matrix
	glm::mat4 projection;
	projection = glm::mat4();
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 200.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// View matrix
	glm::mat4 view;
	view = glm::mat4();
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	view = glm::lookAt(glm::vec3(coche_h.px - 20 * sin(coche_h.angulo_trans * GradosToRadianes), coche_h.py + 10, coche_h.pz - 20 * cos(coche_h.angulo_trans * GradosToRadianes)),
		glm::vec3(coche_h.px + sin((coche_h.angulo_trans) * GradosToRadianes), 10, coche_h.pz + cos((coche_h.angulo_trans) * GradosToRadianes)),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

}

// Funcion que dibuja una rueda
void drawWheel() {
	glBindTexture(GL_TEXTURE_2D, rueda);
	glBindVertexArray(VAOCubo);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

// Funcion que dibuja las ruedas del coche mediante la funcion drawWheel y matrices de transformacion
void drawRuedas() {
	// Variables
	glm::mat4 transform;
	glm::mat4 stack;
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "Color");
	// la busco en el shader
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0, 0, 0)));

	transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(base.px, base.py, base.pz));
	transform = glm::rotate(transform, (float)((base.angulo_trans - 90) * GradosToRadianes), glm::vec3(0.0f, 1.0f, 0.0f));

	stack = transform;
	transform = glm::scale(transform, glm::vec3(3.5, -.5, 2.0));
	transform = glm::rotate(transform, (float)((-angleWheel) * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(1.5, 1.5, 1.5));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	drawWheel();

	transform = stack;
	transform = glm::translate(transform, glm::vec3(3.5, -0.5, -2.0));
	transform = glm::rotate(transform, (float)((-angleWheel) * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(1.5, 1.5, 1.5));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	drawWheel();

	transform = stack;
	transform = glm::translate(transform, glm::vec3(-3.5, -0.5, 2.0));
	transform = glm::rotate(transform, (float)((-angleWheel) * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(1.5f, 1.5f, 1.5f));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	drawWheel();

	transform = stack;
	transform = glm::translate(transform, glm::vec3(-3.5, -0.5, -2.0));
	transform = glm::rotate(transform, (float)((-angleWheel) * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(1.5, 1.5, 1.5));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	drawWheel();

	// Dibuja la ultima rueda
	transform = stack;
	transform = glm::translate(transform, glm::vec3(3.5, -0.5, 2.0));
	transform = glm::rotate(transform, (float)((-angleWheel) * GradosToRadianes), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(1.5, 1.5, 1.5));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	drawWheel();

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

void drawArboles() {
	// Draw two trees
	glm::mat4 transform;
	glDisable(GL_CULL_FACE);

	// Localiza la matriz del modelo dentro del vertex shader
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "Color");
	
	transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(10, 3, 10));
	transform = glm::scale(transform, glm::vec3(4, 8, 1));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
	glBindTexture(GL_TEXTURE_2D, arbolText);
	glBindVertexArray(vaoArbol);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(-10, 3, 10));
	transform = glm::scale(transform, glm::vec3(3, 8, 1));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
	glBindVertexArray(vaoArbol);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(10, 3, -10));
	transform = glm::scale(transform, glm::vec3(4, 8, 1));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
	glBindVertexArray(vaoArbol);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	transform = glm::mat4();
	transform = glm::translate(transform, glm::vec3(-10, 3, -10));
	transform = glm::scale(transform, glm::vec3(4, 8, 1));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0.8, 0.8, 0.8)));
	glBindVertexArray(vaoArbol);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, arbolText);


	glEnable(GL_CULL_FACE);
}
void Display() {
	// Dibujo de la escena
	// ------
	glUseProgram(shaderProgram);
	glm::mat4 transform; // es la matriz de transformacion
	glm::mat4 stack; // guardo la matriz de transformacion
	// Localiza la matriz del modelo dentro del vertex shader
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
	unsigned int colorLoc = glGetUniformLocation(shaderProgram, "Color");
	// render 
	// ------
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (camara) {
	case 1:
		myCamara();
		break;
	case 2:
		myPrimeraPersona(base);
		break;
	case 3:
		myTerceraPersona(base);
		break;
	}

	//myTerceraPersona(base);

	iluminacion();
	tiempo();
	myMovimiento();
	//myMovimiento();

	// Dibujo los cuadrados
	int cuboScale = 1;
	transform = glm::mat4(); // identity matrix
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	transform = glm::scale(transform, glm::vec3(5, 5, 5));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 6);


	// SUELO
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	transform = glm::mat4();
	transform = glm::scale(transform, glm::vec3(100, 1, 100));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform3fv(colorLoc, 1, glm::value_ptr(glm::vec3(0, 1, 0)));

	glBindTexture(GL_TEXTURE_2D, hierba);
	glBindVertexArray(VAOCuadradoXZ);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Coche
	// Base
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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


	// Ruedas
	drawRuedas();

	// draw trees
	drawArboles();

	glBindVertexArray(0);

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
			if(Brazo.angulo_trans <= 30) Brazo.angulo_trans++;
			break;
		case GLFW_KEY_R:				// Mover brazo de la grua hacia atras
			if(Brazo.angulo_trans >= -85) Brazo.angulo_trans--;
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


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void resize(GLFWwindow* window, int width, int height) {
	// Cambiamos la anchura
	SCR_WIDTH = width;
	// Cambiamos la altura
	SCR_HEIGHT = height;
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
	shaderProgram = setShaders("shader.vert", "shader.frag");
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
	shaderProgram = setShaders("shader.vert", "shader.frag");

	// Se setea la función de teclado para el callback
	glfwSetKeyCallback(window, keyCallback);
	// Inicializaciones varias
	openGLInit();

	// Dibujar todos los elementos
	// dibujaEjes();
	dibujaCuadradoXZ();
	dibuja_esfera();
	dibujaCubo();
	dibujaArbol();

	// Se puede hacer en el campo de la estructura
	hierba = myCargaTexturas("hierba.jpg");
	arbolText = myCargaTexturas("grassnb.png");
	metal = myCargaTexturas("metal.png");
	rueda = myCargaTexturas("rueda.jpg");
	//arbolText = myCargaTexturas("texturas/arbol.jpg");

	base.VAO = VAOCubo;
	cabina.VAO = VAOCubo;
	Articulacion.VAO = VAO_esfera;
	Brazo.VAO = VAOCubo;

	
	// Lazo principal.
	while (!glfwWindowShouldClose(window))
	{
		// Entrada de teclado
		processInput(window);						// procesamos los eventos de entrada

		// Mis funciones
		//iluminacion();
		//tiempo();
		//myMovimiento();
		Display();

		// Swap
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
