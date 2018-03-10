#include <stdio.h>

//#define __EMSCRIPTEN__ 1

#if __EMSCRIPTEN__

#endif // _EMSCRIPTEN_

#include <emscripten.h>

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include "gllib.h"

//#include "utils.h"
#include "scene.h"

//Emscripten:
//The VS2015 plugin is available from here: https://github.com/crosire/vs-toolsets
static void output_error(int error, const char * msg)
{
	fprintf(stderr, "Error: %s\n", msg);
}
GLFWwindow * window;

#define WIN_W 1280
#define WIN_H 720


void render();
mat4x4 modelMatrix;
mat4x4 perspMatrix;
mat4x4 mvp;
mat4x4 translationMatrix;
mat4x4 scaleMatrix;
mat4x4 rotMatrix;
vec4 color = { 0.0f,1.0f,0.0f,1.0f };
float angle = 0.0f;
Mesh *mesh;
StandardShaderProgram *prog;
int main()
{
	Spatial graphics_node1;



	mesh = malloc(sizeof(Mesh));
	prog = malloc(sizeof(StandardShaderProgram));

	// TODO list:
	// 1. Add GLFW to create gles context
	// 2. Compile shader program
	// 3. create mesh
	// 4. test it
	// 5. test it in emscripten

	const char* vertex_shader_text =
		"#version 300 es\n"
		"uniform highp mat4 uMVP;\n"
		"layout(location = 0)in lowp vec2 vPos;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = uMVP * vec4(vPos, 0.0, 1.0);\n"
		"}\n";
	const char* fragment_shader_text =
		"#version 300 es\n"
		"uniform lowp vec4 uColor;\n"
		"out lowp vec4 o_color;\n"
		"void main()\n"
		"{\n"
		"    o_color = uColor;\n"
		"}\n";


#ifndef __EMSCRIPTEN__
	glfwSetErrorCallback(output_error);
#endif // __EMSCRIPTEN__

	if (!glfwInit())
	{
		fputs("Faileid to initialize GLFW", stderr);
		printf("failed to init glfw\n");
		//emscripten_force_exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(WIN_W, WIN_H, "CEngine", NULL, NULL);

	if (!window)
	{
		fputs("Failed to create GLFW window", stderr);
#ifdef __EMSCRIPTEN__
		emscripten_force_exit(EXIT_FAILURE);
#else
		glfwTerminate();
#endif // __EMSCRIPTEN__

	}

	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);



	create_shader_program(prog, vertex_shader_text, fragment_shader_text);

	create_plane_mesh(mesh);

	//GLuint tex;

	//create_texture(&tex);
	//printf("%i\n",tex);

	mat4x4_perspective(perspMatrix, LIN_TO_RADIANS(46.0f), (float)WIN_W / (float)WIN_H, 0.5f, 2000.0f);


	mat4x4_identity(modelMatrix);
	vec3 scale = { 100.0f,100.0f,1.0f };

	mat4x4_translate(modelMatrix, 0.0f, 0.0f, -200.0f);
	mat4x4_scale_by_vec(modelMatrix, scale);



	mat4x4_mul(mvp, perspMatrix, modelMatrix);

	mat4x4_identity(scaleMatrix);


	printf("Hello, world!\n");
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);


#ifdef __EMSCRIPTEN__

	emscripten_set_main_loop(render, 0, 0);

#else

	while (!glfwWindowShouldClose(window))
	{
		render();
	}

#endif // __EMSCRIPTEN__





	return 0;
}

void render()
{
#ifndef __EMSCRIPTEN__
	double t_start = get_time_in_milliseconds();
#endif // !__EMSCRIPTEN__

	

	glViewport(0, 0, WIN_W, WIN_H);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//for some reason the sync happens at glClear color



	//scale
	mat4x4_identity(modelMatrix);
	mat4x4_scale(scaleMatrix, modelMatrix, 100.0f);

	//rotation
	mat4x4_identity(rotMatrix);
	mat4x4_rotate(rotMatrix, modelMatrix, 0, 1.0f, 0.0f, LIN_TO_RADIANS(angle));
	angle += 2.44f;


	//translation
	mat4x4_identity(translationMatrix);
	mat4x4_translate(translationMatrix, 0.0f, 0.0f, -500.0f);

	//T * R * S
	mat4x4_mul(modelMatrix, translationMatrix, rotMatrix);
	mat4x4_mul(modelMatrix, modelMatrix, scaleMatrix);


	//
	//mat4x4_scale_by_vec(modelMatrix, scale);
	mat4x4_mul(mvp, perspMatrix, modelMatrix);
	glUseProgram(prog->prog);
	glUniformMatrix4fv(prog->MVP_LOC, 1, GL_FALSE, (const GLfloat*)mvp);
	glUniform4fv(prog->COLOR_LOC, 1, (const GLfloat*)color);

	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);



	glfwPollEvents();
	glfwSwapBuffers(window);

	//glFinish();
	
#ifndef __EMSCRIPTEN__
	double frame_time = get_time_in_milliseconds() - t_start;
	printf("Frame time:%f\n", frame_time);
	char time_str[20];
	snprintf(time_str, 19, "Frame time: %.2f", frame_time);
	glfwSetWindowTitle(window, time_str);
#endif // !__EMSCRIPTEN__


}