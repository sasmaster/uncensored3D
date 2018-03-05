#include <stdio.h>



#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif // _EMSCRIPTEN_

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>
#include "gllib.h"
#include "linmath.h"

//Emscripten:
//The VS2015 plugin is available from here: https://github.com/crosire/vs-toolsets
static void output_error(int error, const char * msg)
{
	fprintf(stderr, "Error: %s\n", msg);
}
GLFWwindow * window;

#define WIN_W 1280
#define WIN_H 720

int main()
{

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

	glfwSetErrorCallback(output_error);
	if (!glfwInit())
	{
		fputs("Faileid to initialize GLFW", stderr);
		//emscripten_force_exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(WIN_W, WIN_H, "CEngine", NULL, NULL);

	if (!window) {
		fputs("Failed to create GLFW window", stderr);
		glfwTerminate();
		//	emscripten_force_exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	StandardShaderProgram prog;

	create_shader_program(&prog, vertex_shader_text, fragment_shader_text);
	Mesh mesh;
	create_plane_mesh(&mesh);

	mat4x4 perspMatrix;

	mat4x4_perspective(perspMatrix, LIN_TO_RADIANS(46.0f), (float)WIN_W / (float)WIN_H, 0.5f, 2000.0f);

	mat4x4 modelMatrix;
	mat4x4_identity(modelMatrix);
	vec3 scale = { 100.0f,100.0f,1.0f };

	mat4x4_translate(modelMatrix, 0.0f, 0.0f, -200.0f);
	mat4x4_scale_by_vec(modelMatrix, scale);


	mat4x4 mvp;
	mat4x4_mul(mvp, perspMatrix, modelMatrix);

	mat4x4 translationMatrix;
	mat4x4 scaleMatrix;
	mat4x4_identity(scaleMatrix);
	mat4x4 rotMatrix;
	vec4 color = { 0.0f,1.0f,0.0f,1.0f };
	float angle = 0.0f;
	printf("Hello, world!\n");
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, WIN_W, WIN_H);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//scale
		mat4x4_identity(modelMatrix);
		mat4x4_scale(scaleMatrix, modelMatrix, 100.0f);

		//rotation
		mat4x4_identity(rotMatrix);
		mat4x4_rotate(rotMatrix, modelMatrix, 0, 1.0f, 0.0f, LIN_TO_RADIANS(angle));
		angle += 2.44f;


		//translation
		mat4x4_identity(translationMatrix);
		mat4x4_translate(translationMatrix, 100.0f, 50.0f, -500.0f);

		//T * R * S
		mat4x4_mul(modelMatrix, translationMatrix, rotMatrix);
		mat4x4_mul(modelMatrix, modelMatrix, scaleMatrix);


		//
		//mat4x4_scale_by_vec(modelMatrix, scale);
		mat4x4_mul(mvp, perspMatrix, modelMatrix);
		glUseProgram(prog.prog);
		glUniformMatrix4fv(prog.MVP_LOC, 1, GL_FALSE, (const GLfloat*)mvp);
		glUniform4fv(prog.COLOR_LOC, 1, (const GLfloat*)color);

		glBindVertexArray(mesh.vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	return 0;
}