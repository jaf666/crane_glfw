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

	vec3 ld = luzDir;
	vec3 fd = normalize(vec3(FragPos - lightPos));
	// angulo de 15 grados
	if (acos(dot(ld, fd)) < radians(15.0)) {
		// diffuse
		vec3 norm = normalize(ourNormal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightColor;

		// specular
		float specularStrength = 1.0;
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
		vec3 specular = specularStrength * spec * lightColor;
		
		// result
		vec3 result = (ambient + diffuse + specular) * ourColor;
		vec4 texColor = texture(ourTexture, TexCoord);
		if(texColor.a < 0.1) discard;
		else	FragColor = mix(vec4(result, 1.0), texColor, .5);
	} else{
		vec3 result = (ambient) * ourColor;
		vec4 texColor = texture(ourTexture, TexCoord);
		if(texColor.a < 0.1) discard;
		else	FragColor = mix(vec4(result, 1.0), texColor, .8);
	}
	
}