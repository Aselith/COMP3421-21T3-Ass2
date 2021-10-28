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

#version 330 core

in vec2 vTexCoord;

uniform sampler2D uTex;

out vec4 fFragColor;

void main() {
    fFragColor = texture(uTex, vTexCoord);
}
