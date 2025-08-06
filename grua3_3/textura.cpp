#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glad.h>
#include <iostream>
#include "texturas.h"

#define TEX_HIERBA "hierba.jpg"
#define GRASS_PNG "grassnb.png"
#define TEX_METAL "metal.png"
#define TEX_RUEDA "rueda.jpg"
// Variables globales definidas aquí para cada textura
int hierba;
int arbolText;
int metal;
int rueda;

int _myCargaTexturas(const char* nome) {
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

int cargarTexturas() {
	hierba = _myCargaTexturas(TEX_HIERBA);
	arbolText = _myCargaTexturas(GRASS_PNG);
	metal = _myCargaTexturas(TEX_METAL);
	rueda = _myCargaTexturas(TEX_RUEDA);
	if (metal < 0 || hierba < 0 || arbolText < 0 || rueda < 0) {
		std::cerr << "Error cargando texturas. Cerrando aplicación." << std::endl;
		return -1;
	}
}