#ifndef _EMITTER_H_
#define _EMITTER_H_


#include "glm_header.h"
#include "Vertex.h"

struct Particle
{
	vec4 position;
	vec4 color;

	vec4 velocity;
	
	float size;
	float lifetime;
	float lifespan;
};

//Contains particles it controls and creates and updates and renders it particals
class Emitter
{
public:
	Emitter();
	~Emitter();

	void Init(unsigned int a_max_particles, vec3 a_position,
		float a_emit_rate,
		float a_min_lifespan, float a_max_lifespan,
		float a_min_velocity, float a_max_velocity,
		float a_start_size, float a_end_size,
		vec4 a_start_color, vec4 a_end_color);

	void EmitParticles();
	void Update(float a_dt);
	void Render();

	void UpdateVertexData(vec3 cam_pos, vec3 cam_up);

	//particle info
	Particle* m_particles;
	unsigned int m_max_particles;
	unsigned int m_alive_count;

	//openGL info
	OpenGLInfo m_buffers;
	VertexParticle* m_vertex_data;
	unsigned int *m_index_data;

	//Emitter data
	vec4 m_position;

	float m_emit_rate;
	float m_emit_timer;

	float m_lifespan_min;
	float m_lifespan_max;

	float m_velocity_min;
	float m_velocity_max;

	float m_start_size;
	float m_end_size;

	vec4 m_start_color;
	vec4 m_end_color;


};


#endif // !_EMITTER_H_
