#version 330 core

in vec3 ourNormal;
in vec3 FragPos;
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 luzDir;

// Variable para establecer el modo noche al obtener un evento de teclado
uniform bool modoNoche;
uniform float opacidad;

uniform sampler2D ourTexture;

void main()

{
vec3 luzFinal = lightColor;
float ambientI = 0.5;

// Si está activado el modo noche, se ajusta la luz final
if (modoNoche) {
    luzFinal = vec3(0.2, 0.2, 0.4);
    ambientI = 0.2;
}

vec3 ambient = ambientI * luzFinal;


	// hay que cambiar la posicion de la luz por su direccion para que sea direccional
	// Se calcula el ángulo entre la dirección de la luz direccional y el vector fd
    // Si el ángulo es menor a 15 grados, se activa iluminación completa (efecto foco)
	vec3 ld = luzDir;												// dirección de la luz direccional
	vec3 fd = normalize(vec3(FragPos - lightPos));					// dirección desde la luz puntual al fragmento
	// angulo de 15 grados
	if (acos(dot(ld, fd)) < radians(15.0)) {
		// diffuse
		vec3 norm = normalize(ourNormal);							// normal normalizada
		vec3 lightDir = normalize(lightPos - FragPos);				// dirección luz -> fragmento
		float diff = max(dot(norm, lightDir), 0.0);					// componente difusa (dot product)
		vec3 diffuse = diff * lightColor;							// luz difusa

		// specular
		float specularStrength = 1.0;
		vec3 viewDir = normalize(viewPos - FragPos);				// dirección vista -> fragmento
		vec3 reflectDir = reflect(-lightDir, norm);					// vector reflejado de la luz
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);	// intensidad especular
		vec3 specular = specularStrength * spec * lightColor;
		
		// result
		// Combinamos las tres luces: ambiental + difusa + especular
		vec3 result = (ambient + diffuse + specular) * ourColor;

		// Obtenemos el color de la textura en este fragmento
		vec4 texColor = texture(ourTexture, TexCoord);
		
		 // Si la transparencia de la textura es menor a 0.1, descartamos el fragmento
		if(texColor.a < 0.1) discard;

		// Mezclamos el color resultante con la textura (50% cada uno)
		else	FragColor = mix(vec4(result, 1.0), texColor, .5);

	} else{
		// Si el ángulo es mayor a 15 grados, sólo se aplica luz ambiental
		vec3 result = (ambient) * ourColor;
		
		vec4 texColor = texture(ourTexture, TexCoord);
		
		if(texColor.a < 0.1) discard;
		 // Mezcla la luz ambiental con la textura, con 80% peso para la textura
		else	FragColor = mix(vec4(result, 1.0), texColor, .8);
	}
	
}