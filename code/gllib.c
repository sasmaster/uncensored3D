#include  "gllib.h"

//proper way of inlining in C: https://stackoverflow.com/questions/23699719/inline-vs-static-inline-in-header-file
extern inline int create_texture(Texture* tex, unsigned char* data);
extern inline void render_path(RenderState* render_state, Shape* shape);

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

	prog->handle        = compile_shader_program(vertStr, fragStr);
	prog->COLOR_LOC     = glGetUniformLocation(prog->handle, "uColor");
	prog->COLOR_TEX_LOC = glGetUniformLocation(prog->handle, "uColorTex");
	prog->MVP_LOC       = glGetUniformLocation(prog->handle, "uMVP");

}

 void create_plane_mesh(Mesh* mesh)
{

	const float vertices[8] = {
		-1.0f,  1.0f, // 0,1
		1.0f,   1.0f,
		1.0f,  -1.0f,
		-1.0f, -1.0f
	};

  //  	-1.0f, -1.0f,  0,0
		//1.0f, -1.0f, 1,0
		//1.0f, 1.0f,  1,1
		//-1.0f, 1.0f  0,1

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

void create_sphere_mesh(float radius, unsigned int rings, unsigned int sectors)
 {
	float* vertices = NULL;
	float* normals = NULL;
	float* texcoords = NULL;
	unsigned short* indices = NULL;


	float const R = 1.0f / (float)(rings - 1);
	float const S = 1.0f / (float)(sectors - 1);
	unsigned int r, s;

	vertices = malloc(rings * sectors * 3 * sizeof(float));
	normals = malloc(rings * sectors * 3 * sizeof(float));
	texcoords = malloc(rings * sectors * 2 * sizeof(float));
	float* v = vertices;
	float* n = normals;
	float* t = texcoords;

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const y = (float)sin(-M_PI_2 + M_PI * r * R);
			float const x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
			float const z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

			*t++ = s * S;
			*t++ = r * R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;

		}
	}

	indices = malloc(rings * sectors * 4 * sizeof(unsigned short));

	unsigned short* i = indices;
	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}
	}

	assert(i);
 }