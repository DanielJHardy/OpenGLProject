#include "Animation.h"

#include "gl_core_4_4.h"

#include "Gizmos.h"
#include "Utility.h"

#include "glm_header.h"

#include <GLFW\glfw3.h>
#include <cstdio>

Animation::Animation()
{

}
Animation::~Animation()
{
	
}

bool Animation::Startup()
{
	if (Application::Startup() == false)
	{
		return false;
	}

	m_backColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);

	glClearColor(m_backColor.r, m_backColor.g, m_backColor.b, m_backColor.a);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	//Camera
	m_sceneCam = FlyCamera(glm::radians(60.0f));
	m_sceneCam.setLookAt(vec3(-1, 0, -1), vec3(0, 0, 0), vec3(0, 1, 0));

	//GUI
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_bar = TwNewBar("Awesome Bar");
	TwAddVarRW(m_bar, "Clear Color", TW_TYPE_COLOR4F, &m_backColor, "");
	TwAddSeparator(m_bar, "sep1", "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");
	//

	//Load shaders
	LoadShaders("./data/shaders/animation_vertex.glsl", "./data/shaders/animation_fragment.glsl", nullptr, &m_programID);
	
	//Load FBX
	m_fbx = new FBXFile();
	m_fbx->load("./data/models/Pyro/pyro.fbx");
	m_fbx->initialiseOpenGLTextures();
	createOpenGLBuffers(m_fbx);

	//init variables
	m_timer = 0;

	//
	glfwSetTime(0.0);
	return true;
}
void Animation::Shutdown()
{
	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();
}

bool Animation::Update()
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
	//

	m_timer += dt;

	//grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

	//evaluate the animation to update bones
	EvaluateSkeleton(animation, skeleton, m_timer);

	for (unsigned int i = 0; i < skeleton->m_boneCount; ++i)
	{
		skeleton->m_nodes[i]->updateGlobalTransform();


		mat4 node_global = skeleton->m_nodes[i]->m_globalTransform;
		vec3 node_pos = node_global[3].xyz;


		if (skeleton->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_pos = skeleton->m_nodes[i]->m_parent->m_globalTransform[3].xyz;

		}
	}



	//Grid
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
void Animation::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	//bind camera
	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&m_sceneCam.getProjectionView());

	int diffuse_uniform = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(diffuse_uniform, 0);


	//grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	UpdateBones(skeleton);

	int bones_location = glGetUniformLocation(m_programID, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //normals
	glEnableVertexAttribArray(2); //tangents
	glEnableVertexAttribArray(3); //texcoords
	glEnableVertexAttribArray(4); //weights
	glEnableVertexAttribArray(5); //indices

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);


	//bind the vertex array and draw mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		FBXMaterial* mesh_material = mesh->m_material;

		if (mesh_material->textures[FBXMaterial::DiffuseTexture])
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh_material->textures[FBXMaterial::DiffuseTexture]->handle);

			mat4 world_transform = mesh->m_globalTransform;
			int world_uniform = glGetUniformLocation(m_programID, "world");

			glUniformMatrix4fv(world_uniform, 1, GL_FALSE, (float*)&world_transform);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

		}

	}


	Gizmos::draw(m_sceneCam.getProjectionView());
	TwDraw();

	glfwSwapBuffers(this->m_window);
	glfwPollEvents();


}

void Animation::createOpenGLBuffers(FBXFile* fbx)
{
	//Create the Gl VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		//storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;

	}
}

void Animation::cleanupOpenGLBuffers(FBXFile* fbx)
{
	//clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}


void Animation::EvaluateSkeleton(FBXAnimation* anim,
	FBXSkeleton *skeleton,
	float timer)
{
	float fps = 24.f;
	int current_frame = (int)(timer * fps);

	//loop through all the tracks.
	for (unsigned int track_index = 0;
		track_index < anim->m_trackCount;
		++track_index)
	{
		//wrap back to the start of the track if we've gone too far
		int track_frame_count = anim->m_tracks[track_index].m_keyframeCount;
		int track_frame = current_frame % track_frame_count;

		//find what keyframes are effecting the current the bone
		FBXKeyFrame curr_frame =
			anim->m_tracks[track_index].m_keyframes[track_frame];
		FBXKeyFrame next_frame =
			anim->m_tracks[track_index].m_keyframes[(track_frame + 1) % track_frame_count];

		//find out how far between keyframes we are
		float time_since_frame_flip = timer - (current_frame / fps);
		float t = time_since_frame_flip * fps;

		//interpolate between those keyframes to generate the matrix
		//for the current pose.
		vec3 new_pos = glm::mix(curr_frame.m_translation,
			next_frame.m_translation, t);
		vec3 new_scale = glm::mix(curr_frame.m_scale,
			next_frame.m_scale, t);
		glm::quat new_rot = glm::slerp(curr_frame.m_rotation,
			next_frame.m_rotation, t);

		mat4 local_transform = glm::translate(new_pos) *
			glm::toMat4(new_rot) *
			glm::scale(new_scale);

		//get the right bone for the given track
		int bone_index = anim->m_tracks[track_index].m_boneIndex;

		//set the FBXnode's local transform to match
		if (bone_index < skeleton->m_boneCount)
		{
			skeleton->m_nodes[bone_index]->m_localTransform = local_transform;
		}
	}
}

void Animation::UpdateBones(FBXSkeleton* skeleton)
{
	//loop through the nodes in the skeleton
	for (unsigned int bone_index = 0;
		bone_index < skeleton->m_boneCount;
		++bone_index)
	{
		//generate their global transforms
		int parent_index = skeleton->m_parentIndex[bone_index];

		if (parent_index == -1)
		{
			skeleton->m_bones[bone_index] = skeleton->m_nodes[bone_index]->m_localTransform;
		}
		else
		{
			skeleton->m_bones[bone_index] =
				skeleton->m_bones[parent_index] * skeleton->m_nodes[bone_index]->m_localTransform;
		}
	}

	for (unsigned int bone_index = 0;
		bone_index < skeleton->m_boneCount;
		++bone_index)
	{
		//multiply the global transform by the inverse bind pose.
		skeleton->m_bones[bone_index] = skeleton->m_bones[bone_index] * skeleton->m_bindPoses[bone_index];
	}
}