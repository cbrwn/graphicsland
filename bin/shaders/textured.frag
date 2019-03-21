#version 410

in vec3 viewPos;
in vec3 lightPos;
in vec3 vertNormal;
in vec3 vertPos;
in vec2 uv;

uniform sampler2D friggenTexture;
uniform vec3 Kd;
uniform vec3 Ka;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(vertNormal);

    vec3 lightCol = vec3(1,1,1);
    vec3 lightDif = normalize(vertPos-lightPos);
    float cosTheta = max(dot(norm, lightDif), 0.0);

    vec3 viewDir = normalize(viewPos - vertPos);
    vec3 reflectDir = reflect(-lightDif, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.2f * spec * lightCol;

    vec3 diffuse = cosTheta * lightCol;

	float ambientStrength = 0.2;

//    vec4 col = texture(friggenTexture, uv) * vec4(Kd, 1.0);
    vec4 col = vec4(Kd, 1.0);

    float dist = 1.0;//dot(lightDif, lightDif);

    fragColor =  vec4(diffuse + specular + vec3(ambientStrength), 1.0) * col;
}