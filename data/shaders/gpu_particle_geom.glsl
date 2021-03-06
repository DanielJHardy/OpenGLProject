#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

//input from vertex shader
in vec3 geo_position[];
in float geo_lifetime[];
in float geo_lifespan[];

//output to fragment shader
out vec4 frag_color;

//Uniforms
uniform mat4 projection_view;
uniform mat4 camera_world;

uniform float size_start;
uniform float size_end;

uniform vec4 color_start;
uniform vec4 color_end;


void main()
{
	//calc 0-1 value for interpolation of variables
	float t = geo_lifetime[0] / geo_lifespan[0];

	//interpolate color
	frag_color = mix(color_start, color_end, t);

	//interpolate half size
	float half_size = mix(size_start, size_end, t) * 0.5f;

	//create quad verts
	vec3 corners[4];
	corners[0] = vec3(half_size, -half_size,0);
	corners[1] = vec3(half_size, half_size,0);
	corners[2] = vec3(-half_size, -half_size,0);
	corners[3] = vec3(-half_size, half_size,0);

	//build the rotation matrix
	vec3 forward = normalize(camera_world[3].xyz - geo_position[0]);
	//vec3 right = cross(forward,camera_world[1].xyz);
	vec3 right = cross(camera_world[1].xyz, forward);
	vec3 up = cross(forward, right);

	mat3 billboard_rot = mat3(right, up, forward);

	//apply rotation matrix
	corners[0] = billboard_rot * corners[0];
	corners[1] = billboard_rot * corners[1];
	corners[2] = billboard_rot * corners[2];
	corners[3] = billboard_rot * corners[3];

	corners[0] += geo_position[0];
	corners[1] += geo_position[0];
	corners[2] += geo_position[0];
	corners[3] += geo_position[0];

	//transform position to world
	gl_Position = projection_view * vec4(corners[0],1);
	EmitVertex();
	gl_Position = projection_view * vec4(corners[1],1);
	EmitVertex();
	gl_Position = projection_view * vec4(corners[2],1);
	EmitVertex();
	gl_Position = projection_view * vec4(corners[3],1);
	EmitVertex();
}