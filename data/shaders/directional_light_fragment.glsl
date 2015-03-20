#version 410

in vec2 frag_texCoord;

out vec3 out_color;

uniform vec3 light_dir;
uniform vec3 light_color;

uniform sampler2D position_tex;
uniform sampler2D normals_tex;

void main()
{
	vec3 normal_sample = texture(normals_tex, frag_texCoord).xyz;
	normal_sample *= 2;
	normal_sample -= 1;

	vec3 position_sample = texture(position_tex, frag_texCoord).xyz;

	float d = max(0, dot(-light_dir, normal_sample));

	out_color = light_color * d;
}