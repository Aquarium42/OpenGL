#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <common/shader.hpp>
using namespace glm;

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Playground", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// background
	glClearColor(0.2f, 0.f, 0.3f, 0.0f);


	// �������� ���� �������
	//glEnable(GL_DEPTH_TEST);
	// �������� ����� ���������� ������ � ���, ������, ���� �� ��������� ����� � ������, ��� ����������
	//glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// ������� � ��������������� ���� ��������� ���������
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	// �������� ����� ���������� � �������
   // ������ ���� ��� �� ����� �������������.

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	const float radius = 10.0f;
	// ������������ ������� : 45&deg; ���� ������, 4:3 ����������� ������, �������� : 0.1 ���� <-> 100 ������
	glm::mat4 Projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates


	static const GLfloat g_vertex_buffer_data[] = {
  -1.0f, -1.0f, 0.0f,
  -1.0f, 1.0f, 0.0f,
  0.5f, 0.0f, 0.0f,
  1.0f, -1.0f, 0.0f,
  1.0f, 1.0f, 0.0f,
  -0.5f, 0.0f, 0.0f,
	};
	// ��� ����� ��������������� ������ ������ ������
	GLuint vertexbuffer;
	// �������� 1 ����� � �������� � ���������� vertexbuffer ��� �������������
	glGenBuffers(1, &vertexbuffer);
	// ������� ������ ��� ��������� ����� �������
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// ��������� ���������� � �������� � OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	static const GLfloat g_color_buffer_data[] = {
	0.41f,  0.99f,  1.0f,
	0.41f,  0.99f,  1.0f,
	0.41f,  0.99f,  1.0f,
	1.0f,  0.2f,  0.97f,
	1.0f,  0.2f,  0.97f,
	1.0f,  0.2f,  0.97f,
	
	};

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ������������� ��� ������ �������
		glUseProgram(programID);

		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		glm::mat4 View = glm::lookAt(
			glm::vec3(camX, 0.0, camZ), // ������ ��������� � ������� �����������
			glm::vec3(0, 0, 0), // � ���������� � ������ ���������
			glm::vec3(0, 1, 0)  // "������" ��������� ������
		);
		// ������� ������ : ��������� ������� (������ ��������� � ������ ���������)
		glm::mat4 Model = glm::mat4(1.0f);  // ������������� ��� ������ ������

		// �������� ������� ModelViewProjection, ������� �������� ����������� ������������ ����� ���� ������
		glm::mat4 MVP = Projection * View * Model; // �������, ��� ��������� ������� ������������� � �������� �������


		// �������� ���� ������������� � ������� ������
		// ��� �������� � �������� �����, ��������� ������ ������ ����� ����� ������ MVP-������� (��� ������� ����� M)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// ���������, ��� ������ ������� ��������� ����� �������
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // ������� 0. ��������� �� ���� ����� ���������� � �����, ����������� ��������.
			3,                  // ������
			GL_FLOAT,           // ���
			GL_FALSE,           // ���������, ��� �������� �� �������������
			0,                  // ���
			(void*)0            // �������� ������� � ������
		);
		// ������ ����� ��������� - �����
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // �������. ����� ������������� ��������� 1, �� �������, ����� ��� �������� ��������� � layout � �������..
			3,                                // ������
			GL_FLOAT,                         // ���
			GL_FALSE,                         // ������������?
			0,                                // ���
			(void*)0                          // ��������
		);

		// ������� �����������!
		glDrawArrays(GL_TRIANGLES, 0, 6); // ������� � ������� 0, ����� 3 ������� -> ���� �����������

		glDisableVertexAttribArray(0);
		// Draw nothing, see you in tutorial 2 !

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

