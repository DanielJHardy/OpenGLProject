#version 410

in vec2 frag_texCoord;

out vec4 out_color;

uniform sampler2D albedo_tex;
uniform sampler2D light_tex;

void main()
{

	vec4 albedo_sample = texture(albedo_tex, frag_texCoord);
	vec4 light_sample = texture(light_tex, frag_texCoord);

	out_color = albedo_sample * light_sample;
	//out_color = texture(normals_tex, frag_texCoord);
	//out_color = texture(albedo_tex, frag_texCoord);
}