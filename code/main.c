#include <stdio.h>


#ifndef EM_PORT_API
#	if defined(__EMSCRIPTEN__)
#		include <emscripten.h>
#		if defined(__cplusplus)
#			define EM_PORT_API(rettype) extern "C" rettype EMSCRIPTEN_KEEPALIVE
#		else
#			define EM_PORT_API(rettype) rettype EMSCRIPTEN_KEEPALIVE
#		endif
#	else
#		if defined(__cplusplus)
#			define EM_PORT_API(rettype) extern "C" rettype
#		else
#			define EM_PORT_API(rettype) rettype
#		endif
#	endif
#endif

#include <stdio.h>


#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gllib.h"
#include "utils.h"
#include "scene.h"
#include "glsl.h"
#include "path.h"

GLFWwindow* window = NULL;
#define WIN_W 1280
#define WIN_H 720

void render();

mat4x4 perspMatrix;

float angle = 0.0f;
Mesh mesh;
StandardShaderProgram prog;
Texture tex1;
Texture texFromJs = {};



//Emscripten:
//The VS2015 plugin is available from here: https://github.com/crosire/vs-toolsets

//these functions are called from JS side 
#if __EMSCRIPTEN__

EM_PORT_API(void) hellow_world()
{
	printf("Hello world\n");
}

/**
* ON JS side: deprecated Pointer_stringify (use UTF8ToString instead)
*/
EM_PORT_API(const char*) get_string()
{
	static const char str[] = "I am static const C string";
	return str;
}

EM_PORT_API(void) print_string_c(char* str)
{
	printf("%s\n", str);
}

EM_PORT_API(void) register_gl_tex_handle(unsigned int handle, int w, int h, int num_channels)
{
	texFromJs.handle = handle;
	texFromJs.w = w;
	texFromJs.h = h;
	texFromJs.num_channels = num_channels;
	printf("Registered JS created Texture with Handle id:%i\n", handle);
}

#else
static void output_error(int error, const char* msg)
{
	fprintf(stderr, "Error: %s\n", msg);
}
#endif//__EMSCRIPTEN__


int main()
{
	//Spatial graphics_node1;


	unsigned char* img_data = stbi_load("assets/test.png", &tex1.w, &tex1.h, &tex1.num_channels, 4);

	Path path1;

	initPath(&path1);


#ifndef __EMSCRIPTEN__
	glfwSetErrorCallback(output_error);
#endif // __EMSCRIPTEN__

	if (!glfwInit())
	{
		fputs("Failed to initialize GLFW", stderr);
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

	//	glfwSwapInterval(0);



	create_shader_program(&prog, vertex_shader_text, fragment_shader_text);

	create_plane_mesh(&mesh);

	create_texture(&tex1, true, img_data);

	STBI_FREE(img_data);

	mat4x4_perspective(perspMatrix, LIN_TO_RADIANS(54.0f), (float)WIN_W / (float)WIN_H, 0.1f, 2000.0f);


	//-------------------   set basic state ------------------//
	//turn on alpha blending:
	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);//we want this for path rendering
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

#ifdef U_BENCHMARK
	double t_start = get_time_in_milliseconds();
#endif

	glViewport(0, 0, WIN_W, WIN_H);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//for some reason the sync happens at glClear color

	mat4x4 mvp;
	{
		mat4x4 modelMatrix;
		mat4x4 rotMatrix;

		//scale
		mat4x4_identity(modelMatrix);
	 
		if (texFromJs.handle)
		{
			vec3  scale = { (float)texFromJs.w, (float)texFromJs.h, 1.0f };
			mat4x4_scale_by_vec(modelMatrix, scale);
		}
		else
		{

			vec3  scale = { 100.0f, 100.0f, 1.0f };
			mat4x4_scale_by_vec(modelMatrix, scale);
		}



		//rotation
		 mat4x4_rotate(rotMatrix, modelMatrix, 0, 1.0f, 0.0f, LIN_TO_RADIANS(angle));
		 //angle += 2.44f;

		//translation
		mat4x4_translate(modelMatrix, 0.0f, 0.0f, -1800.0f);

		//T * R * S  -> P
		mat4x4_mul(modelMatrix, modelMatrix, rotMatrix);
		mat4x4_mul(mvp, perspMatrix, modelMatrix);
	}


	glUseProgram(prog.handle);
	if (texFromJs.handle)
	{
		glBindTexture(GL_TEXTURE_2D, texFromJs.handle);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, tex1.handle);
	}


	glUniform1i(prog.COLOR_TEX_LOC, 0);
	glUniformMatrix4fv(prog.MVP_LOC, 1, GL_FALSE, (const GLfloat*)mvp);
	{
		const vec4 color = { 1.0f,1.0f,1.0f,1.0f };
		glUniform4fv(prog.COLOR_LOC, 1, (const GLfloat*)color);
	}
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glfwPollEvents();
	glfwSwapBuffers(window);


#ifdef U_BENCHMARK
	double frame_time = static_cast<double>(get_time_in_milliseconds()) - t_start;
	printf("Frame time:%f\n", frame_time);
	char time_str[20];
	snprintf(time_str, 19, "Frame time: %.2f", frame_time);
	glfwSetWindowTitle(window, time_str);
#endif





	}