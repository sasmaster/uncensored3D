#ifndef  GL_LIB_H
#define  GL_LIB_H

#include <stdlib.h>
#include <stdio.h>
#include <GLES3/gl3.h>
#include <math.h>
#include <assert.h>


#define M_PI            3.14159265358979323846  /* pi */
#define M_PI_2          1.57079632679489661923  /* half PI */

typedef enum
{
	TRINAGLES,
	TRIANGLE_FAN,
	TRIANGLE_STRIP
}DrawType;

typedef struct _GLFBO
{
	unsigned int handle;

}GLFBO;

typedef struct _Mesh
{
	GLuint vao;
	GLuint vbo;
	DrawType draw_type;
	int num_vertices;
	int num_indices;
}Mesh;

typedef struct _Texture
{
	GLuint handle;
	int w;
	int h;
	int num_channels;
}Texture;

typedef struct _StandardShaderProgram
{
	GLuint handle;
	GLint  MVP_LOC;
	GLint  COLOR_LOC;
	GLint  COLOR_TEX_LOC;
}StandardShaderProgram;

static inline int create_texture(Texture *tex,unsigned char* data)
{
	glGenTextures(1, &tex->handle);
	if (tex->handle == 0)
	{
		printf("failed to gen texture\n");
		return 1;
	}
	glActiveTexture(GL_TEXTURE0);
	
	const int num_mips  = (int) floorf(log2f(fmaxf((float)tex->w,(float)tex->h))) + 1;
	glBindTexture(GL_TEXTURE_2D, tex->handle);
	glTexStorage2D(GL_TEXTURE_2D, num_mips, GL_RGBA8, tex->w, tex->h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->w, tex->h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;

}

GLuint compile_shader_program(const char* vert_str, const char* frag_str);
void create_shader_program(StandardShaderProgram* prog, const char* vertStr, const char* fragStr);

void create_vbo(GLuint *vbo, GLuint *ibo, const void* vertices, const size_t vertex_size,
	const int num_vertices, const void* indices, const size_t index_size, const int num_indices);

void create_plane_mesh(Mesh* mesh);
 

inline void create_sphere_mesh(float radius, unsigned int rings, unsigned int sectors)
{


	float   *vertices = NULL;
	float   *normals = NULL;
	float   *texcoords = NULL;
	unsigned short  *indices = NULL;


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

			*t++ = s*S;
			*t++ = r*R;

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
		for (s = 0; s < sectors; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;


		}
	}

	assert(i);


}

inline void create_fbo()
{

}

#endif // ! GL_LIB


