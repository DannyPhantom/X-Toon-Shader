#version 430 core

out vec4 color;

in vec3 N;
in vec3 V;
in vec3 L;

// Material properties
uniform float r = 0.5; 		// |n . v| ^r
uniform sampler2D tex;

void main(void)
{    	
	vec2 uv = vec2(0, 0);
    
    uv.s = max(0, dot(N, L));
    uv.t = min(1, pow(abs(dot(N, V)), r));
    
    color = texture(tex, uv);
}
