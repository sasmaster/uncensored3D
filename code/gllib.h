#ifndef  G_LIB_H
#define  G_LIB_H


#include <GLES3/gl3.h>


#include "linmath.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>


//we forward declare array
typedef struct  Array Array;

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

typedef struct _Shape
{
	Mesh   mesh;
	Array*  path;
}Shape;

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

typedef struct _RenderState
{
	vec4 clear_color;
	GLuint clearFlags;
	GLint view_port[2];
}RenderState;

inline int create_texture(Texture *tex,bool mipmaps,unsigned char* data)
{
	glGenTextures(1, &tex->handle);

	if (tex->handle == 0)
	{
		printf("failed to gen texture\n");
		return 1;
	}
	 
	glActiveTexture(GL_TEXTURE0);
	const int num_mips = mipmaps?(int)floorf(log2f(fmaxf((float)tex->w, (float)tex->h))) + 1:1;
	
	glBindTexture(GL_TEXTURE_2D, tex->handle);
	glTexStorage2D(GL_TEXTURE_2D, num_mips, GL_RGBA8, tex->w, tex->h);

	if (data)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex->w, tex->h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps? GL_LINEAR_MIPMAP_LINEAR: GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,	  GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,	  GL_CLAMP_TO_EDGE);

	if (mipmaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	

	glBindTexture(GL_TEXTURE_2D, 0);

	return 0;
}

void create_sphere_mesh(float radius, unsigned int rings, unsigned int sectors);
 

GLuint compile_shader_program(const char* vert_str, const char* frag_str);
void create_shader_program(StandardShaderProgram* prog, const char* vertStr, const char* fragStr);

void create_vbo(GLuint *vbo, GLuint *ibo, const void* vertices, const size_t vertex_size,
	const int num_vertices, const void* indices, const size_t index_size, const int num_indices);

void create_plane_mesh(Mesh* mesh);
 


inline void create_fbo()
{

}

inline void render_path(RenderState* render_state,Shape* shape)
{
	glViewport(0, 0, render_state->view_port[0], render_state->view_port[1]);

	glClearColor(render_state->clear_color[0], render_state->clear_color[1],
		render_state->clear_color[2], render_state->clear_color[3]);

	glClear(render_state->clearFlags);

	

	// Stencil pass:
	//   Always pass
	//   increment for front facing (CCW) triangles
	//   decrement for back facing (CW) triangles
	//   Stencil buffer write only pass

	glStencilMask(0xFF);//255 - stencil write is on
	glDepthMask(GL_FALSE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glStencilFunc(GL_ALWAYS, 0, 255);

	glStencilOpSeparate(GL_FRONT,GL_INCR_WRAP, GL_INCR_WRAP, GL_INCR_WRAP);
	glStencilOpSeparate(GL_BACK,GL_DECR_WRAP, GL_DECR_WRAP, GL_DECR_WRAP);

	//draw stencil curves here

	//andnon


}

#endif // ! GL_LIB


