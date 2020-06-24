//
// Created by wcg3031 on 2020/4/2.
//

#pragma once

#include "string"

//using namespace std;

static const std::string triangle_vertex_shader = "#version 300 es\n"
									 "layout (location = 0) in vec3 aPos;\n"
									 "void main()\n"
									 "{\n"
									 "    gl_Position = vec4(aPos, 1.0);\n"
									 "}";

static const std::string triangle_fragment_shader = "#version 300 es\n"
											   "precision mediump float;\n"
											   "uniform float g_color;\n"
											   "out vec4 FragColor;\n"
											   "\n"
											   "void main()\n"
											   "{\n"
											   "    FragColor = vec4(1.0f, g_color, 0.2f, 1.0f);\n"
											   "}";



static const std::string texture_vertex_shader = "#version 300 es\n"
											"\n"
											"layout (location = 0) in vec3 aPos;\n"
											"layout (location = 1) in vec3 aColor;\n"
											"layout (location = 2) in vec2 tPos;\n"
											"uniform mat4 MVP;\n"
											"out vec2 TexCoord;\n"
											"out vec3 TexColor;\n"
											"\n"
											"void main()\n"
											"{\n"
											"    gl_Position = MVP * vec4(aPos, 1.0);\n"
											"    TexCoord = tPos;\n"
											"    TexColor = aColor;\n"
											"}";

static const std::string texture_fragment_shader = "#version 300 es\n"
											  "precision mediump float;\n"
											  "\n"
											  "out vec4 FragColor;\n"
											  "in vec2 TexCoord;\n"
											  "in vec3 TexColor;\n"
											  "uniform sampler2D texture1;\n"
											  "\n"
											  "void main()\n"
											  "{\n"
											  "    FragColor = texture (texture1, TexCoord) * vec4 (TexColor, 1.0);\n"
											  "}";


static const std::string fbo_vertex_shader = "#version 300 es\n"
										"\n"
										"layout (location = 0) in vec3 aPos;\n"
										"layout (location = 1) in vec2 aTexCoords;\n"
										"\n"
										"out vec2 TexCoords;\n"
										"\n"
										"void main()\n"
										"{\n"
										"    gl_Position = vec4(aPos, 1.0);\n"
										"    TexCoords = aTexCoords;\n"
										"}";

static const std::string fbo_fragment_shader = "#version 300 es\n"
										  "precision mediump float;\n"
										  "\n"
										  "out vec4 FragColor;\n"
										  "in vec2 TexCoords;\n"
										  "\n"
										  "uniform sampler2D screenTexture;\n"
										  "\n"
										  "void main()\n"
										  "{\n"
										  "    vec4 tempColor = texture (screenTexture, TexCoords);\n"
										  "    float grayValue = tempColor.r * 0.299 + tempColor.g * 0.587 + tempColor.b * 0.114;\n"
										  "    FragColor = vec4 (vec3(grayValue), tempColor.a);\n"
										  "}";

static const std::string fbo_normal_fragment_shader = "#version 300 es\n"
												 "precision mediump float;\n"
												 "out vec4 FragColor;\n"
												 "in vec2 TexCoords;\n"
												 "uniform sampler2D texture1;\n"
												 "void main()\n"
												 "{\n"
												 "    FragColor = texture (texture1, TexCoords) * vec4 (1.0);\n"
												 "}";


