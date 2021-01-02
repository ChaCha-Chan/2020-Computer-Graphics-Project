#include "myglwidget.h"
#include "object.h"

MyGLWidget::MyGLWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
	timer = new QTimer(this); 
	timer->start(300);
	connect(timer, SIGNAL(timeout()), this, SLOT(update())); 
}

MyGLWidget::~MyGLWidget()
{
	delete this->timer;
}

void MyGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	glViewport(0, 0, width(), height());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	init_shaders("vertex_shader.txt", "fragment_shader.txt");
	set_matrix();
}

void MyGLWidget::init_shaders(const char* v_path, const char* f_path) {
	//读取着色器
	string v_code, f_code;
	ifstream v_shader_file, f_shader_file;
	v_shader_file.open(v_path);
	f_shader_file.open(f_path);
	stringstream v_shader_stream, f_shader_stream;
	v_shader_stream << v_shader_file.rdbuf();
	f_shader_stream << f_shader_file.rdbuf();
	v_shader_file.close();
	f_shader_file.close();
	v_code = v_shader_stream.str();
	f_code = f_shader_stream.str();
	const char* v_shader_source = v_code.c_str();
	const char* f_shader_source = f_code.c_str();

	// 顶点着色器
	GLuint v_shader;
	v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &v_shader_source, NULL);
	glCompileShader(v_shader);

	//片段着色器
	GLuint f_shader;
	f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &f_shader_source, NULL);
	glCompileShader(f_shader);

	//着色器程序
	shader_program = glCreateProgram();
	glAttachShader(shader_program, v_shader);
	glAttachShader(shader_program, f_shader);
	glLinkProgram(shader_program);
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);
	glUseProgram(shader_program);
}

void MyGLWidget::set_matrix() {
	//获取并设置投影矩阵
	GLfloat projection_matrix[16];
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width() / (GLfloat)height(), 0.1f, 200.0f);
	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, projection_matrix);

	//获取并设置modelview矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	GLfloat modelview_matrix[16];
	//gluLookAt(0.0f, 5.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(0.0f, 5.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//glTranslatef(0.0f, 0.0f, -5.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix);
	glPopMatrix();
}

void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	
	GLfloat vertices[] = {
		// positions          // colors           // texture coords
		 10.0f, 10.0f, 0.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 10.0f, -10.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-10.0f, -10.0f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-10.0f, 10.0f, 0.0f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	
	GLfloat vertices1[] = {
		// positions          // colors           // texture coords
		 10.0f,  0.0f,-10.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 10.0f,  0.0f,10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-10.0f,  0.0f,10.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-10.0f,  0.0f,-10.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	
	GLuint indices[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};

	char path[] = "pics\\地图.png";
	//object test(vertices, sizeof(vertices), indices, sizeof(indices), path);
	//test.draw();
	object test1(vertices1, sizeof(vertices), indices, sizeof(indices), path);
	test1.draw();
}

void MyGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	update();
}

