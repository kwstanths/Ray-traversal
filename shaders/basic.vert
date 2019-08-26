#version 330

uniform mat4 projection, model, view;
uniform mat3 normalMatrix;

in vec3 position;
in vec3 normal;
in vec3 vertex_color;

out vec3 normalFrag;
out vec3 color;

void main() {
	
    color = vertex_color;
    normalFrag = normalMatrix * normal;
	gl_Position = projection * view * model * vec4(position, 1.0);
}

