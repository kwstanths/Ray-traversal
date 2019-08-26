#version 330

in vec3 normalFrag;
in vec3 color;

out vec4 outColor;

void main()
{
    float lighting = 1.0;
  
    vec3 lightDirection = normalize(vec3(1.0, 2.0, 3.0));

    float ambient = 0.2;
    float diffuse = max(0.0, dot(normalize(normalFrag), lightDirection));
    lighting = 0.15f * ambient + 0.85f * diffuse;

	outColor = pow(lighting * vec4(color, 1), vec4(1.0 / 2.1));
}

