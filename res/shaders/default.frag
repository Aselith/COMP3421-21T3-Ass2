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

in vec2 vTexCoord;
in vec3 vNormal;
out vec4 fFragColor;

uniform sampler2D uTex;

struct Material {
    float texFactor;
    vec4 color;
    vec3 diffuse;
};

struct DirLight {
    vec3 direction;
    vec3 color;
    float ambient;
};

uniform Material uMat;
uniform DirLight uSun;

void main() {
    
    if (vNormal.x == 0 && vNormal.y == 0 && vNormal.z == 0) {
        fFragColor = texture(uTex, vTexCoord);
    } else {
        vec4 color = mix(uMat.color, texture(uTex, vTexCoord), uMat.texFactor);
        vec3 ambient = uSun.color * uSun.ambient;
        vec3 diffuse = uSun.color * uMat.diffuse * max(0,dot(-uSun.direction, vNormal));
        fFragColor = vec4((ambient + diffuse) * color.rgb, color.a);
    }
}
