#version 410

in vec3 viewPos;
in vec3 lightPos;
in vec3 vertNormal;
in vec3 vertPos;

out vec4 fragColor;

void main() {
    vec3 norm = normalize(vertNormal);

    vec3 lightCol = vec3(1,1,1);
    vec3 lightDif = normalize(lightPos - vertPos);
    float cosTheta = max(dot(norm, lightDif), 0.0);

    vec3 viewDir = normalize(viewPos - vertPos);
    vec3 reflectDir = reflect(-lightDif, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightCol;

    vec3 diffuse = cosTheta * lightCol;
    vec3 ambient = vec3(0.0);

    vec4 col = vec4(0.4, 0.5, 1.0,1.0);

    float dist = 1.0;//dot(lightDif, lightDif);

    fragColor =  vec4((diffuse + ambient + specular), 1.0) * col;
}