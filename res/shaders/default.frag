/*
#version 330 core

in vec2 vTexCoord;

uniform sampler2D uTex;
uniform float uTexFactor;
uniform vec4 uColor;

out vec4 fFragColor;

void main() {
    fFragColor = mix(uColor, texture(uTex, vTexCoord), uTexFactor);
}
*/

/*
#version 330 core

in vec2 vTexCoord;

uniform sampler2D uTex;

out vec4 fFragColor;

void main() {
    fFragColor = texture(uTex, vTexCoord);
}
*/

#version 330 core

#define MAX_LIGHTS 101

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;
out vec4 fFragColor;

uniform sampler2D uTex;
uniform sampler2D uSpec;

struct Material {
    float texFactor;
    float specularFactor;
    vec3 ambient;
    vec4 color;
    vec4 specular;
    vec3 diffuse;
    float phongExp;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float ambient;
};

struct SpotLight {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    float intensity;
};

uniform Material uMat;
uniform DirLight uSun;
uniform vec3 uCameraPos;
uniform SpotLight allLights[MAX_LIGHTS];

vec3 sRGB_to_linear(vec3 col) {
    return pow(col, vec3(2.2));
}

vec3 linear_to_sRGB(vec3 col) {
    return pow(col, vec3(1/2.2));
}

vec3 calcSpotLight(SpotLight light, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 ambient = light.ambient * mat_diffuse * uMat.ambient;
    vec3 light_direction = normalize(vPosition - light.position);
    vec3 diffuse = light.diffuse * mat_diffuse * max(0,dot(-light_direction, vNormal));
    vec3 reflected = reflect(light_direction, vNormal);
    vec3 view = normalize(uCameraPos - vPosition);

    vec3 specular = light.specular * mat_specular * pow(max(0, dot(reflected, view)), uMat.phongExp);

    float distance = distance(light.position, vPosition);
    float attenuation = 1.0f / (1.0f + (0.2 * distance * distance));

    return (ambient + diffuse + specular) * attenuation * light.intensity;
}

void main() {
    
    if (vNormal.x == 0 && vNormal.y == 0 && vNormal.z == 0) {
        fFragColor = texture(uTex, vTexCoord);
    } else {
        vec4 color = mix(uMat.color, texture(uTex, vTexCoord), uMat.texFactor);
        vec3 colorV3 = sRGB_to_linear(color.rgb);

        // Calculating specular
        vec4 mat_specular = mix(uMat.specular, texture(uSpec, vTexCoord), uMat.specularFactor);
        vec3 mat_specularV3 = sRGB_to_linear(mat_specular.rgb);

        vec3 ambient = uSun.color * uSun.ambient;
        vec3 diffuse = uSun.color * uMat.diffuse * max(0, dot(-uSun.direction, vNormal));
        vec3 resultSpotLight = {0, 0, 0};

        if (uMat.texFactor == 1.0f) {
            for (int i = 0; i < MAX_LIGHTS; i++) {
                if (allLights[i].position.x >= 0 && allLights[i].position.y >= 0 && allLights[i].position.z >= 0) {
                    resultSpotLight += sRGB_to_linear(calcSpotLight(allLights[i], colorV3, mat_specularV3));
                }
            }
        }

        fFragColor = vec4((ambient + diffuse + linear_to_sRGB(resultSpotLight)) * linear_to_sRGB(colorV3), color.a);
    }
}
