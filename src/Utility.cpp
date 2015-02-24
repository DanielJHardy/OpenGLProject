#include "Utility.h"

#include <cstdio>
#include "gl_core_4_4.h"

bool LoadShaders(char* a_vertex_filename, char* a_fragment_filename, GLuint* a_result)
{
	bool succeded = false;
	FILE* vertex_file = fopen(a_vertex_filename, "r");
	FILE* fragment_file = fopen(a_fragment_filename, "r");

	if (vertex_file == 0 || fragment_file == 0)
	{
		printf("failed to open shader file");
	}
	else
	{
		fseek(vertex_file, 0, SEEK_END);
		int vertex_file_length = ftell(vertex_file);
		fseek(vertex_file, 0, SEEK_SET);


		fseek(fragment_file, 0, SEEK_END);
		int fragment_file_length = ftell(fragment_file);
		fseek(fragment_file, 0, SEEK_SET);

		char *vs_source = new char[vertex_file_length];
		char *fs_source = new char[fragment_file_length];

		vertex_file_length = fread(vs_source, 1, vertex_file_length, vertex_file);
		fragment_file_length = fread(fs_source, 1, fragment_file_length, fragment_file);

		succeded = true;
		unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertex_shader, 1, &vs_source, &vertex_file_length);
		glCompileShader(vertex_shader);
		glShaderSource(fragment_shader, 1, &fs_source, &fragment_file_length);
		glCompileShader(fragment_shader);

		*a_result = glCreateProgram();
		glAttachShader(*a_result, vertex_shader);
		glAttachShader(*a_result, fragment_shader);
		glLinkProgram(*a_result);

		//error checking
		int success = GL_FALSE;
		glGetProgramiv(*a_result, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			int infoLogLength = 0;
			glGetProgramiv(*a_result, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* infoLog = new char[infoLogLength];
			glGetProgramInfoLog(*a_result, infoLogLength, 0, infoLog);
			printf("ERROR: Failed\n\n");
			printf("%s", infoLog);
			delete[] infoLog;
			succeded = false;
		}
		glDeleteShader(fragment_shader);
		glDeleteShader(vertex_shader);
		delete[] vs_source;
		delete[] fs_source;
	}
	fclose(vertex_file);
	fclose(fragment_file);
	return succeded;

}