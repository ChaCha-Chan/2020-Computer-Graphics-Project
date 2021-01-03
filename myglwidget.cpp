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
void MyGLWidget::set_alpha(GLfloat alpha) {
	glUniform1f(glGetUniformLocation(shader_program, "alpha"), alpha);
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
	GLfloat modelview_matrix[16];
	//gluLookAt(0.0f, 5.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(0.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix);

	//设置不透明度
	set_alpha(1.0f);
}

void MyGLWidget::draw_map() {
	GLfloat map_v[] = {
		// positions          // colors           // texture coords
		 10.0f,  0.0f,-10.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
		 10.0f,  0.0f,10.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
		-10.0f,  0.0f,10.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-10.0f,  0.0f,-10.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // top left 
	};

	GLuint map_i[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};

	char path[] = "pics\\地图.png";
	object map(map_v, sizeof(map_v), map_i, sizeof(map_i), path);
	map.draw();
}

void MyGLWidget::draw_skybox() {
	GLfloat skybox_top_v[] = {
		// positions          // colors           // texture coords
		 10.0f,  10.0f,-10.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 10.0f,  10.0f,10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // bottom right
		-10.0f,  10.0f,10.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-10.0f,  10.0f,-10.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f  // top left 
	};

	GLuint skybox_top_i[] = {
	   3, 1, 0, // first triangle
	   3, 2, 1  // second triangle
	};

	char path_top[] = "pics\\skybox_top.png";
	object skybox_top(skybox_top_v, sizeof(skybox_top_v), skybox_top_i, sizeof(skybox_top_i), path_top);
	skybox_top.draw();

	GLfloat skybox_side_v[] = {
		// positions          // colors           // texture coords
		-10.0f,  10.0f,-10.0f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		-10.0f,  0.0f,-10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-10.0f,  0.0f,10.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-10.0f,  10.0f,10.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
	};

	GLuint skybox_side_i[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};

	char path_front[] = "pics\\skybox_front.png";
	object skybox_front(skybox_side_v, sizeof(skybox_side_v), skybox_side_i, sizeof(skybox_side_i), path_front);
	r_t_set_matrix(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	skybox_front.draw();

	char path_right[] = "pics\\skybox_right.png";
	object skybox_right(skybox_side_v, sizeof(skybox_side_v), skybox_side_i, sizeof(skybox_side_i), path_right);
	r_t_set_matrix(-90.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	skybox_right.draw();

	char path_back[] = "pics\\skybox_back.png";
	object skybox_back(skybox_side_v, sizeof(skybox_side_v), skybox_side_i, sizeof(skybox_side_i), path_back);
	r_t_set_matrix(180.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	skybox_back.draw();

	char path_left[] = "pics\\skybox_left.png";
	object skybox_left(skybox_side_v, sizeof(skybox_side_v), skybox_side_i, sizeof(skybox_side_i), path_left);
	r_t_set_matrix(90.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	skybox_left.draw();
}

void MyGLWidget::r_t_set_matrix(GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3, GLfloat t1, GLfloat t2, GLfloat t3) {
	GLfloat modelview_matrix[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(t1, t2, t3);
	glRotatef(r0, r1, r2, r3);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix);
	glPopMatrix();
}
void MyGLWidget::draw_roadside() {
	char path1[] = "pics\\水泥路.png";
	GLfloat roadside1_v[] = {
		// positions          // colors           // texture coords
		 6.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 6.0f,
		 6.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 6.0f,
		 0.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

		 6.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 6.0f,
		 6.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 6.0f,
		 0.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f
	};
	GLuint roadside_i[] = {
	   0, 1, 3, 
	   1, 2, 3,
	   1, 5, 2,
	   2, 5, 6,
	   3, 7, 4,
	   3, 4, 0,
	   2, 6, 7,
	   2, 7, 3,
	   1, 0, 4,
	   1, 4, 5
	};
	object road_side1(roadside1_v, sizeof(roadside1_v), roadside_i, sizeof(roadside_i), path1);
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 6.4f);
	road_side1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, -6.5f);
	road_side1.draw();

	GLfloat roadside2_v[] = {
		// positions          // colors           // texture coords
		 13.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 13.0f,
		 13.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 13.0f,
		 0.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

		 13.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 13.0f,
		 13.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 13.0f,
		 0.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f
	};
	object road_side2(roadside2_v, sizeof(roadside2_v), roadside_i, sizeof(roadside_i), path1);
	r_t_set_matrix(90.0f, 0.0f, 1.0f, 0.0f, 6.5f, 0.0f, 6.5f);
	road_side2.draw();

	GLfloat roadside3_v[] = {
		// positions          // colors           // texture coords
		 7.1f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 7.1f,
		 7.1f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 7.1f,
		 0.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

		 7.1f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 7.1f,
		 7.1f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 7.1f,
		 0.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f
	};
	object road_side3(roadside3_v, sizeof(roadside3_v), roadside_i, sizeof(roadside_i), path1);
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 7.4f);
	road_side3.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, -7.5f);
	road_side3.draw();

	GLfloat roadside4_v[] = {
		// positions          // colors           // texture coords
		 15.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 15.0f,
		 15.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 15.0f,
		 0.0f,  0.1f, 0.1f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.0f,  0.1f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,

		 15.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 15.0f,
		 15.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 15.0f,
		 0.0f,  0.0f, 0.1f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f
	};
	object road_side4(roadside4_v, sizeof(roadside4_v), roadside_i, sizeof(roadside_i), path1);
	r_t_set_matrix(90.0f, 0.0f, 1.0f, 0.0f, 7.5f, 0.0f, 7.5f);
	road_side4.draw();

}

void MyGLWidget::get_cylinder_v_i(GLfloat r, GLfloat h, GLuint u_num, GLfloat* temp_v, GLuint* temp_i){
	for (int i = 0; i < 2 * u_num; i++) {
		if (i < u_num) {
			temp_v[i * 8] = r * cos(PI * i * 2 / u_num);
			temp_v[i * 8 + 1] = h;
			temp_v[i * 8 + 2] = r * sin(PI * i * 2 / u_num);
			temp_v[i * 8 + 6] = 1.0f * i / u_num;
			temp_v[i * 8 + 7] = 1.0f;
		}
		else {
			temp_v[i * 8] = r * cos(PI * (i - u_num) * 2 / u_num);
			temp_v[i * 8 + 1] = 0.0f;
			temp_v[i * 8 + 2] = r * sin(PI * (i - u_num) * 2 / u_num);
			temp_v[i * 8 + 6] = 1.0f * (i - u_num) / u_num;
			temp_v[i * 8 + 7] = 0.0f;
		}
		temp_v[i * 8 + 3] = 1.0f;
		temp_v[i * 8 + 4] = 1.0f;
		temp_v[i * 8 + 5] = 1.0f;
	}
	for (int i = 0; i < 6 * u_num; i += 6) {
		if (i == 6 * u_num - 6) {
			temp_i[i + 1] = 0;
		}
		else {
			temp_i[i + 1] = i / 6 + 1;
		}
		temp_i[i] = i / 6;
		temp_i[i + 2] = temp_i[i] + u_num;
		temp_i[i + 3] = temp_i[i + 1];
		temp_i[i + 4] = temp_i[i + 1] + u_num;
		temp_i[i + 5] = temp_i[i + 2];	
	}
}
//由于多了个圆心，所以应该有u_num + 1个点
void MyGLWidget::get_cycle_v_i(GLfloat r, GLuint u_num, GLfloat* temp_v, GLuint* temp_i) {
	for (int i = 0; i < u_num; i++) {
		temp_v[i * 8] = r * cos(PI * i * 2 / u_num);
		temp_v[i * 8 + 1] = 0.0f;
		temp_v[i * 8 + 2] = - r * sin(PI * i * 2 / u_num);
		temp_v[i * 8 + 3] = 1.0f;
		temp_v[i * 8 + 4] = 1.0f;
		temp_v[i * 8 + 5] = 1.0f;
		temp_v[i * 8 + 6] = cos(PI * i * 2 / u_num) / 2 + 0.5f;
		temp_v[i * 8 + 7] = sin(PI * i * 2 / u_num) / 2 + 0.5f;
	}
	//圆心
	temp_v[u_num * 8] = 0.0f;
	temp_v[u_num * 8 + 1] = 0.0f;
	temp_v[u_num * 8 + 2] = 0.0f;
	temp_v[u_num * 8 + 3] = 1.0f;
	temp_v[u_num * 8 + 4] = 1.0f;
	temp_v[u_num * 8 + 5] = 1.0f;
	temp_v[u_num * 8 + 6] = 0.5f;
	temp_v[u_num * 8 + 7] = 0.5f;

	for (int i = 0; i < u_num; i++) {
		if (i == u_num - 1) {
			temp_i[i * 3 + 1] = 0;
		}
		else {
			temp_i[i * 3 + 1] = i + 1;
		}
		temp_i[i * 3] = i;
		temp_i[i * 3 + 2] = u_num;
	}
}

void MyGLWidget::draw_gate() {
	static bool open_flag = FALSE;
	//估计要有四个状态 正在开 正在关 开 关
	//闸机两侧
	int u_num = 4;
	GLfloat* side_v = new GLfloat[2 * u_num * 8];
	GLuint* side_i = new GLuint[6 * u_num];
	get_cylinder_v_i(0.3f, 0.6f, 4, side_v, side_i);
	char path1[] = "pics\\水泥路.png";
	object side1(side_v, sizeof(GLfloat) * 2 * u_num * 8, side_i, sizeof(GLuint) * 6 * u_num, path1);
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f);
	//side1.draw();

	GLfloat* top_v = new GLfloat[(u_num + 1) * 8];
	GLuint* top_i = new GLuint[3 * u_num];
	get_cycle_v_i(0.3f, 4, top_v, top_i);
	object top1(top_v, sizeof(GLfloat) * (u_num + 1) * 8, top_i, sizeof(GLuint) * 3 * u_num, path1);
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	top1.draw();
}

void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	set_matrix();
	draw_skybox();
	draw_map();
	draw_roadside();
	draw_gate();
}

void MyGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	update();
}

