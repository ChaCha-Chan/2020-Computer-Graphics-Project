#ifndef OBJECT_H
#define OBJECT_H
#include <GL/glew.h>
#include <QtGui>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

class object : protected QOpenGLExtraFunctions {
public:
	object(GLfloat* vertices, GLuint vertices_size, GLuint* indices, GLuint indices_size, char* path);
	~object();
	void set_texture();
	void draw();

private:
	GLuint VBO, VAO, EBO;
	GLuint texture;
	char* path;

	GLfloat* vertices;
	GLuint* indices;
	GLuint vertices_size, indices_size;
};

#endif
