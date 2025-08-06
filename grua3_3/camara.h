// Funciones de cámara
#ifndef CAMARA_H
#define CAMARA_H
#include "defini.h"

// Función que coloca la cámara por defecto
void myCamara();
void myPrimeraPersona(objeto coche_h);
void myTerceraPersona(objeto coche_h);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resize(GLFWwindow* window, int width, int height);
#endif

