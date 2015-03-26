#include "DeferredRendering.h"

#include "gl_core_4_4.h"
#include "Gizmos.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include <GLFW\glfw3.h>
#include "tiny_obj_loader.h"

#include "Utility.h"

#include <cstdio>

DeferredRendering::DeferredRendering()
{

}
DeferredRendering::~DeferredRendering()
{
}

bool DeferredRendering::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	m_backColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	m_lightPos = vec3(0,3,4);

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(0, 0, 1), vec3(0, 0, 0), vec3(0, 1, 0));

	//GUI
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_bar = TwNewBar("Awesome Bar");
	TwAddVarRW(m_bar, "Clear Color", TW_TYPE_COLOR4F, &m_backColor, "");
	TwAddSeparator(m_bar, "sep1", "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");
	TwAddVarRW(m_bar, "light Pos", TW_TYPE_DIR3F, &m_lightPos, "");

	//mesh
	buildMesh();
	buildQuad();
	buildCube();

	//buffers
	buildGbuffer();
	buildLightBuffer();

	LoadShaders("./data/shaders/gbuffer_vertex.glsl", "./data/shaders/gbuffer_fragment.glsl", 0, &m_gbuffer_program);
	LoadShaders("./data/shaders/composite_vertex.glsl", "./data/shaders/composite_fragment.glsl", 0, &m_composite_program);
	LoadShaders("./data/shaders/composite_vertex.glsl", "./data/shaders/directional_light_fragment.glsl", 0, &m_directional_light_program);
	LoadShaders("./data/shaders/point_light_vertex.glsl", "./data/shaders/point_light_fragment.glsl",0, &m_point_light_program);

	glEnable(GL_CULL_FACE);

	//
	glfwSetTime(0.0);
	return true;
}
void DeferredRendering::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();
}

bool DeferredRendering::Update()
{
	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_fps = 1 / dt;

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);

	if (Application::Update() == false)
	{
		return false;
	}

	
	m_sceneCam.Update(dt);

	vec4 white(1);
	vec4 black(0.1f, 0.1f, 0.1f, 1);

	for (int i = 0; i <= 20; i++)
	{
		Gizmos::addLine(vec3(-10 + i, -1, -10), vec3(-10 + i, -1, 10), i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, -1, -10 + i), vec3(10, -1, -10 + i), i == 10 ? white : black);
	}
	Gizmos::addTransform(mat4(1), 1);

	return true;

}

void DeferredRendering::renderDirectionalLight(vec3 light_dir, vec3 light_color)
{
	vec4 viewspace_light_dir = m_sceneCam.getView() * vec4(light_dir, 0);

	int light_dir_uniform = glGetUniformLocation(m_directional_light_program, "light_dir");
	int light_color_uniform = glGetUniformLocation(m_directional_light_program, "light_color");

	glUniform3fv(light_dir_uniform, 1, (float*)&viewspace_light_dir);
	glUniform3fv(light_color_uniform, 1, (float*)&light_color);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void DeferredRendering::renderPointLight(vec3 position, float radius, vec3 diffuse)
{
	vec4 view_space_pos = m_sceneCam.getView() * vec4(position,1);

	int pos_uniform = glGetUniformLocation(m_point_light_program, "light_position");
	int view_pos_uniform = glGetUniformLocation(m_point_light_program, "light_view_position");
	int light_diffuse_uniform = glGetUniformLocation(m_point_light_program, "light_diffuse");
	int light_radius_uniform = glGetUniformLocation(m_point_light_program, "light_radius");

	glUniform3fv(pos_uniform, 1, (float*)&position);
	glUniform3fv(view_pos_uniform,1, (float*)&view_space_pos);
	glUniform3fv(light_diffuse_uniform, 1, (float*)&diffuse);
	glUniform1f(light_radius_uniform,radius);

	glBindVertexArray(m_light_cube.m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void DeferredRendering::Draw()
{
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);
	glClearColor(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT);

	vec4 clear_color = vec4(0,0,0,0);
	vec4 clear_normal = vec4(0.5f, 0.5f, 0.5f, 0.5f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 1, (float*)&clear_color);
	glClearBufferfv(GL_COLOR, 2, (float*)&clear_normal);

	glUseProgram(m_gbuffer_program);

	int view_uniform = glGetUniformLocation(m_gbuffer_program, "view");
	int view_proj_uniform = glGetUniformLocation(m_gbuffer_program, "view_proj");

	glUniformMatrix4fv(view_uniform, 1, GL_FALSE, (float*)&m_sceneCam.getView());
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_index_count, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_sceneCam.getProjectionView());


	//light buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);

	//addative blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_directional_light_program);

	int position_tex_uniform = glGetUniformLocation(m_directional_light_program, "position_tex");
	int normals_tex_uniform = glGetUniformLocation( m_directional_light_program, "normals_tex");

	glUniform1i(position_tex_uniform, 0);
	glUniform1i(normals_tex_uniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);

	//render light/////////////////////////
	//renderDirectionalLight(vec3(0,0,1), vec3(1,1,1));
	//renderDirectionalLight(m_lightDir, vec3(0, 1, 0));

	glUseProgram(m_point_light_program);

	///
	view_proj_uniform = glGetUniformLocation(m_point_light_program, "proj_view");

	position_tex_uniform = glGetUniformLocation(m_point_light_program, "position_texture");
	normals_tex_uniform = glGetUniformLocation(m_point_light_program, "normal_texture");

	glUniformMatrix4fv(view_proj_uniform,1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());
	glUniform1i(position_tex_uniform,0);
	glUniform1i(normals_tex_uniform,1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);


	renderPointLight(m_lightPos, 10, vec3(1,0,0));
	renderPointLight(vec3(0, 0, -4), 10, vec3(0, 1, 0));
	renderPointLight(vec3(-7, 6, 0), 10, vec3(0, 0, 1));
	

	glDisable(GL_BLEND);
	///
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_composite_program);

	int albedo_tex_uniform = glGetUniformLocation(m_composite_program, "albedo_tex");
	int light_tex_uniform = glGetUniformLocation(m_composite_program, "light_tex");


	 glUniform1i(albedo_tex_uniform, 0);
	 glUniform1i(light_tex_uniform, 1);

	 glActiveTexture(GL_TEXTURE0);
	 glBindTexture(GL_TEXTURE_2D, m_albedo_texture);

	 glActiveTexture(GL_TEXTURE1);
	 glBindTexture(GL_TEXTURE_2D, m_light_texture);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Gizmos::draw(m_sceneCam.getProjectionView());
	//TwDraw();

	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}

void DeferredRendering::buildGbuffer()
{
	//framebuffer
	glGenFramebuffers(1, &m_gbuffer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer_fbo);

	//textures
	//albedo, position, normal, depth()

	//albedo
	glGenTextures(1, &m_albedo_texture);
	glBindTexture(GL_TEXTURE_2D, m_albedo_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//position
	glGenTextures(1, &m_position_texture);
	glBindTexture(GL_TEXTURE_2D, m_position_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//normals
	glGenTextures(1, &m_normals_texture);
	glBindTexture(GL_TEXTURE_2D, m_normals_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//depth
	glGenRenderbuffers(1, &m_gbuffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gbuffer_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

	//attach textures to frambuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedo_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_position_texture,0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normals_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gbuffer_depth);

	GLenum targets[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

	glDrawBuffers(3, targets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error in creating gbuffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//check that it worked
}

void DeferredRendering::buildMesh()
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector<float> vertex_data;
	tinyobj::LoadObj(shapes, materials, "./data/models/dragon.obj");

	m_bunny.m_index_count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;
	vertex_data.reserve(m_bunny.m_index_count);

	vertex_data.insert(vertex_data.end(), mesh->positions.begin(), mesh->positions.end());
	vertex_data.insert(vertex_data.end(), mesh->normals.begin(), mesh->normals.end());


	glGenVertexArrays(1, &m_bunny.m_VAO);
	glBindVertexArray(m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*m_bunny.m_index_count, vertex_data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[0].mesh.indices.size()*sizeof(unsigned int), shapes[0].mesh.indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //positions
	glEnableVertexAttribArray(1); //normals

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[0].mesh.positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DeferredRendering::buildLightBuffer()
{
	//create the fbo
	glGenFramebuffers(1, &m_light_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_fbo);

	//create textures
	//light texture
	glGenTextures(1, &m_light_texture);
	glBindTexture(GL_TEXTURE_2D, m_light_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//attach textures
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_light_texture, 0);

	GLenum light_target = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &light_target);

	//check for success
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error - light framebuffer incorrect");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRendering::buildQuad()
{
	vec2 halfTexel = 1.0f / (vec2(1280, 720) * 0.5f);
	float vertexData[] =
	{
		-1, -1, 0, 1,			halfTexel.x, halfTexel.y,
		1, -1, 0, 1,			1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1,				1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1,			halfTexel.x, 1 - halfTexel.y,
	};
	unsigned int indexData[] = {
		0, 1, 2, 0, 2, 3
	};
	glGenVertexArrays(1, &m_screenspace_quad.m_VAO);

	glGenBuffers(1, &m_screenspace_quad.m_VBO);
	glGenBuffers(1, &m_screenspace_quad.m_IBO);

	glBindVertexArray(m_screenspace_quad.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_screenspace_quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenspace_quad.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void DeferredRendering::buildCube()
{
	float vertexData[] =
	{
		//bottom
		-1,-1,1,1,
		1,-1,1,1,
		1,-1,-1,1,
		-1,-1,-1,1,

		//top
		-1,1,1,1,
		1,1,1,1,
		1,1,-1,1,
		-1,1,-1,1,
	};
	unsigned int indexData[] = {
		4,5,0,
		5,1,0,
		5,6,1,
		6,2,1,
		6,7,2,
		7,3,2,
		7,4,3,
		4,0,3,
		7,6,4,
		6,5,4,
		0,1,3,
		1,2,3
	
	};

	//unsigned int indexData[] = { 0, 5, 4, 0, 1, 5, 1, 6, 5, 1, 2, 6, 2, 7, 6, 2, 3, 7, 3, 4, 7, 3, 0, 4, 4, 6, 7, 4, 5, 6, 3, 1, 0, 3, 2, 1 };
	glGenVertexArrays(1, &m_light_cube.m_VAO);

	glGenBuffers(1, &m_light_cube.m_VBO);
	glGenBuffers(1, &m_light_cube.m_IBO);

	glBindVertexArray(m_light_cube.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_light_cube.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_light_cube.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}