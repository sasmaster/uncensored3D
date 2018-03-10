#include  "gllib.h"











GLuint compile_shader_program(const char* vert_str, const char* frag_str)
{

#define GL_COMPILE_STATUS_CHECK(shader)                                                 \
	do{                                                                                 \
	GLint status = 0;                                                                   \
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);                                  \
	if (status != GL_TRUE) {                                                            \
		GLint log_length, chars_written;                                                \
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);                         \
		char* info_log = (char*)malloc(log_length);                                     \
		glGetShaderInfoLog(shader, log_length, &chars_written, info_log);               \
		printf("Shader compile error:%s\n", info_log);                                  \
		free(info_log);                                                                 \
		assert(0);                                                                      \
	}                                                                                   \
	}while(0);



	GLuint prog;
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vert_str, NULL);
	glCompileShader(vert);

	GL_COMPILE_STATUS_CHECK(vert);


	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_str, NULL);
	glCompileShader(frag);

	GL_COMPILE_STATUS_CHECK(frag);

#undef GL_COMPILE_STATUS_CHECK

	prog = glCreateProgram();

	// Attach the fragment and vertex shaders to it.
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);

	// Link the program.
	glLinkProgram(prog);


	GLint linkStatus = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint info_log_length, chars_written;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &info_log_length);
		char* info_log = (char*)malloc(info_log_length);
		glGetProgramInfoLog(prog, info_log_length, &chars_written, info_log);
		printf("Shader link error:%s\n", info_log);
		free(info_log);
		assert(0);
		goto clear;
	}


clear:
	glDeleteShader(vert);
	glDeleteShader(frag);




	return prog;

}



void create_vbo(GLuint *vbo, GLuint *ibo, const void* vertices, const size_t vertex_size,
	const int num_vertices, const void* indices, const size_t index_size, const int num_indices)
{

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_size * num_vertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (num_indices)
	{
		glGenBuffers(1, ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * num_indices, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}





void create_shader_program(StandardShaderProgram* prog, const char* vertStr, const char* fragStr)
{

	prog->prog = compile_shader_program(vertStr, fragStr);
	prog->COLOR_LOC = glGetUniformLocation(prog->prog, "uColor");
	prog->COLOR_TEX_LOC = glGetUniformLocation(prog->prog, "uColorTex");
	prog->MVP_LOC = glGetUniformLocation(prog->prog, "uMVP");

}

 void create_plane_mesh(Mesh* mesh)
{

	const float vertices[8] = {
		-1.0f, -1.0f,
		1.0f,  -1.0f,
		1.0f,   1.0f,
		-1.0f,  1.0f
	};

	create_vbo(&mesh->vbo, NULL, vertices, sizeof(float) * 2, 4, NULL, 0, 0);


	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	mesh->draw_type = TRIANGLE_FAN;
	mesh->num_indices = 0;
	mesh->num_vertices = 4;
}