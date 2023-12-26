#include "glsl.h"

const char* vertex_shader_text =
"#version 300 es\n"
"uniform highp mat4 uMVP;\n"
"layout(location = 0)in lowp vec2 vPos;\n"
"const  lowp vec2 uvoff = vec2(0.5,0.5);\n"
"out lowp vec2 uvsOut;\n"
"void main()\n"
"{\n"
" gl_Position = uMVP * vec4(vPos, 0.0, 1.0);\n"
" uvsOut = vPos * uvoff + uvoff;\n"
"}\n";

const char* fragment_shader_text =
"#version 300 es\n"
"uniform lowp vec4 uColor;\n"
"uniform lowp sampler2D uColorTex;\n"
"in lowp vec2 uvsOut;\n"
"out lowp vec4 o_color;\n"
"void main()\n"
"{\n"
"    lowp vec4 tex_color = texture(uColorTex,vec2(uvsOut.s,1.0 - uvsOut.t));\n"
"    o_color = vec4(tex_color.rgb,tex_color.a) * uColor;\n"
"}\n";

//vertex shader for path renderer
const char* vertex_path_curve =
"#version 300 es									\n"
"in highp vec4 vertex;								\n"
"uniform highp mat4 uMVP;							\n"
"in highp vec2 v_uv;								\n"
"void main()										\n"
"{                                                  \n"
"   gl_Position = uMVP * vec4(vertex.xy,0.0,1.0);   \n"
"   v_uv = vec2(vertex.zw);                         \n"
"}                                                  \n"
;

//rasterizer for quadratic path:
const char* fragment_quad_path_curve =
"#version 300 es				\n"
"in highp vec2 v_uv;			\n"
"uniform lowp vec4 u_color;		\n"
"out lowp vec4 o_color;			\n"
"void main(){					\n"
"float a = v_uv.x * v_uv.x;     \n"
"float b = v_uv.y;              \n"
"if(a > b)discard;              \n"
"o_color = u_color;             \n"
"}                              \n"
;

const char* fragment_ellipse_arc =
"#version 300 es				 \n"
"in highp vec2 v_uv;			 \n"
"uniform lowp vec4 u_color;		 \n"
"out lowp vec4 o_color;			 \n"
"void main(){					 \n"
"float a = v_uv.x * v_uv.x;		 \n" //u^2
"float b = v_uv.y * v_uv.y;		 \n" //v^2
"if(a+b > 1.0)discard;			 \n" //discard pixel if u^2 + v^2 > 1
"o_color = u_color;				 \n"
"}                               \n"
;