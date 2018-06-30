#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/*uniform float aspect;
uniform float focal_length;
uniform mat4x4 tm_eye;

out smooth vec3 ray_pos;    
out smooth vec3 ray_dir;*/

void main() {
    	gl_Position = projection * view * vec4(aPos, 1.0f);
	
	/*vec4 p;
	p = tm_eye * vec4(pos.x/aspect, pos.y, pos.z, 1.0);
	ray_pos = p.xyz;
	p -= tm_eye * vec4(0.0, 0.0, -focal_length, 1.0);
	ray_dir = nomarize(p.xyz);*/
//    gl_Position = vec4(aPos, 1.0f);
}
