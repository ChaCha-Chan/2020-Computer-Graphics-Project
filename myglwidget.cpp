#include "myglwidget.h"
#include "object.h"

MyGLWidget::MyGLWidget(QWidget *parent)
	:QOpenGLWidget(parent)
{
	timer = new QTimer(this); 
	timer->start(30);
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
	gluLookAt(0.0f, 1.5f, 17.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
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
void MyGLWidget::s_r_t_set_matrix(GLfloat s1, GLfloat s2, GLfloat s3, GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3, GLfloat t1, GLfloat t2, GLfloat t3) {
	GLfloat modelview_matrix[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(t1, t2, t3);
	glRotatef(r0, r1, r2, r3);
	glScalef(s1, s2, s3);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix);
	glPopMatrix();
}
void MyGLWidget::r_s_r_set_matrix(GLfloat s1, GLfloat s2, GLfloat s3, GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3) {
	GLfloat modelview_matrix[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotatef(r0, r1, r2, r3);
	glScalef(s1, s2, s3);
	glRotatef(45.0f, 0.0f, 1.0f, 0.0f);
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

//初始法线得多算两个点
void MyGLWidget::get_cylinder_v_i(GLfloat r, GLfloat h, GLuint u_num, GLfloat* temp_v, GLuint* temp_i){
	for (int i = 0; i < 2 * u_num + 2; i++) {
		if (i <= u_num) {
			temp_v[i * 8] = r * cos(PI * i * 2 / u_num);
			temp_v[i * 8 + 1] = h;
			temp_v[i * 8 + 2] = -r * sin(PI * i * 2 / u_num);
			temp_v[i * 8 + 6] = 1.0f * (double)i / u_num;
			temp_v[i * 8 + 7] = 1.0f;
		}
		else {
			temp_v[i * 8] = r * cos(PI * (i - u_num - 1) * 2 / u_num);
			temp_v[i * 8 + 1] = 0.0f;
			temp_v[i * 8 + 2] = -r * sin(PI * (i - u_num - 1) * 2 / u_num);
			temp_v[i * 8 + 6] = 1.0f * (double)(i - u_num - 1) / u_num;
			temp_v[i * 8 + 7] = 0.0f;
		}
		temp_v[i * 8 + 3] = 1.0f;
		temp_v[i * 8 + 4] = 1.0f;
		temp_v[i * 8 + 5] = 1.0f;
	}
	for (int i = 0; i < 6 * u_num; i += 6) {
		temp_i[i] = i / 6;
		temp_i[i + 1] = i / 6 + 1;
		temp_i[i + 2] = temp_i[i] + u_num + 1;
		temp_i[i + 3] = temp_i[i + 1];
		temp_i[i + 4] = temp_i[i + 1] + u_num + 1;
		temp_i[i + 5] = temp_i[i + 2];	
	}
}
//初始点得多算一个点，由于多了个圆心，所以应该有u_num + 1 + 1 个点

void MyGLWidget::get_cycle_v_i(GLfloat r, GLuint u_num, GLfloat* temp_v, GLuint* temp_i) {
	for (int i = 0; i <= u_num; i++) {
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
	temp_v[(u_num + 1) * 8] = 0.0f;
	temp_v[(u_num + 1) * 8 + 1] = 0.0f;
	temp_v[(u_num + 1) * 8 + 2] = 0.0f;
	temp_v[(u_num + 1) * 8 + 3] = 1.0f;
	temp_v[(u_num + 1) * 8 + 4] = 1.0f;
	temp_v[(u_num + 1) * 8 + 5] = 1.0f;
	temp_v[(u_num + 1) * 8 + 6] = 0.5f;
	temp_v[(u_num + 1) * 8 + 7] = 0.5f;

	for (int i = 0; i < u_num; i++) {
		temp_i[i * 3] = i;
		temp_i[i * 3 + 1] = i + 1;
		temp_i[i * 3 + 2] = u_num + 1;
	}
}

void MyGLWidget::draw_gate() {
	static bool open_flag1 = FALSE;
	static bool openning_flag1 = TRUE;
	static bool close_flag1 = FALSE;
	static bool closing_flag1 = FALSE;

	static bool open_flag2 = FALSE;
	static bool openning_flag2 = TRUE;
	static bool close_flag2 = FALSE;
	static bool closing_flag2 = FALSE;
	//估计要有四个状态 正在开 正在关 开 关
	//闸机两侧
	int u_num = 16;
	GLfloat* side_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* side_i = new GLuint[6 * u_num ];
	get_cylinder_v_i(0.15f, 0.6f, u_num, side_v, side_i);
	char path1[] = "pics\\闸机侧面.png";
	object side1(side_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, side_i, sizeof(GLuint) * 6 * u_num , path1);
	//闸机1
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 6.2f);
	side1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 7.7f);
	side1.draw();
	//闸机2
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.0f, 6.2f);
	side1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.0f, 7.7f);
	side1.draw();

	GLfloat* top_v = new GLfloat[(u_num + 2) * 8];
	GLuint* top_i = new GLuint[3 * u_num];
	get_cycle_v_i(0.15f, u_num, top_v, top_i);
	char path2[] = "pics\\闸机上面.png";
	object top1(top_v, sizeof(GLfloat) * (u_num + 2) * 8, top_i, sizeof(GLuint) * 3 * u_num, path2);
	//闸机1
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.6f, 6.2f);
	top1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.6f, 7.7f);
	top1.draw();
	//闸机2
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.6f, 6.2f);
	top1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.6f, 7.7f);
	top1.draw();

	//闸机屏幕
	char path3[] = "pics\\闸机屏幕.png";
	object side2(side_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, side_i, sizeof(GLuint) * 6 * u_num, path3);
	//闸机1
	s_r_t_set_matrix(0.2f, 0.5f, 0.8f, 180.0f, 0.0f, 1.0f, 0.0f, 1.5f, 0.6f, 6.2f);
	side2.draw();
	s_r_t_set_matrix(0.2f, 1.0f, 0.8f, 180.0f, 0.0f, 1.0f, 0.0f, 1.5f, 0.9f, 6.2f);
	top1.draw();
	//闸机2
	s_r_t_set_matrix(0.2f, 0.5f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f, -1.5f, 0.6f, 6.2f);
	side2.draw();
	s_r_t_set_matrix(0.2f, 1.0f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f, -1.5f, 0.9f, 6.2f);
	top1.draw();

	//闸机门
	int u_num1 = 4;
	GLfloat* gate_v = new GLfloat[2 * (u_num1 + 1) * 8];
	GLuint* gate_i = new GLuint[6 * u_num1];
	get_cylinder_v_i(0.4f, 0.4f, u_num1, gate_v, gate_i);
	char path4[] = "pics\\闸机门.png";
	object gate(gate_v, sizeof(GLfloat) * 2 * (u_num1 + 1) * 8, gate_i, sizeof(GLuint) * 6 * u_num1, path4);
	static GLfloat angle1 = 0.0f;
	static GLfloat angle2 = 0.0f;
	set_alpha(0.5f);
	glMatrixMode(GL_MODELVIEW);
	//闸机1
	if (close_flag1) {
		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 6.6f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 7.3f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();
	}
	else if (openning_flag1) {
		angle1 += 10.0f;
		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 6.6f);
		glTranslatef(0.02f, 0.0f, -0.2f);
		glRotatef(-angle1, 0.0f, 1.0f, 0.0f);
		glTranslatef(-0.02f, 0.0f, 0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 7.3f);
		glTranslatef(0.02f, 0.0f, 0.2f);
		glRotatef(angle1, 0.0f, 1.0f, 0.0f);
		glTranslatef(-0.02f, 0.0f, -0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		if (abs(angle1 - 90.0f) < 1e-4) {
			openning_flag1 = FALSE;
			open_flag1 = TRUE;
		}
	}
	else if (open_flag1) {
		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 6.6f);
		glTranslatef(0.02f, 0.0f, -0.2f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-0.02f, 0.0f, 0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 7.3f);
		glTranslatef(0.02f, 0.0f, 0.2f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(-0.02f, 0.0f, -0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();
	}
	else if(closing_flag1){
		angle1 -= 10.0f;
		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 6.6f);
		glTranslatef(0.02f, 0.0f, -0.2f);
		glRotatef(-angle1, 0.0f, 1.0f, 0.0f);
		glTranslatef(-0.02f, 0.0f, 0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(1.5f, 0.2f, 7.3f);
		glTranslatef(0.02f, 0.0f, 0.2f);
		glRotatef(angle1, 0.0f, 1.0f, 0.0f);
		glTranslatef(-0.02f, 0.0f, -0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		if (abs(angle1 - 0.0f) < 1e-4) {
			closing_flag1 = FALSE;
			close_flag1 = TRUE;
		}
	}
	//闸机2
	if (close_flag2) {
		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 6.6f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 7.3f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();
	}
	else if (openning_flag2) {
		angle2 += 10.0f;
		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 6.6f);
		glTranslatef(-0.02f, 0.0f, -0.2f);
		glRotatef(angle2, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.02f, 0.0f, 0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 7.3f);
		glTranslatef(-0.02f, 0.0f, 0.2f);
		glRotatef(-angle2, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.02f, 0.0f, -0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		if (abs(angle2 - 90.0f) < 1e-4) {
			openning_flag2 = FALSE;
			open_flag2 = TRUE;
		}
	}
	else if (open_flag2) {
		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 6.6f);
		glTranslatef(-0.02f, 0.0f, -0.2f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.02f, 0.0f, 0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 7.3f);
		glTranslatef(-0.02f, 0.0f, 0.2f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.02f, 0.0f, -0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();
	}
	else if (closing_flag2) {
		angle2 -= 10.0f;
		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 6.6f);
		glTranslatef(-0.02f, 0.0f, -0.2f);
		glRotatef(angle2, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.02f, 0.0f, 0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-1.5f, 0.2f, 7.3f);
		glTranslatef(-0.02f, 0.0f, 0.2f);
		glRotatef(-angle2, 0.0f, 1.0f, 0.0f);
		glTranslatef(0.02f, 0.0f, -0.2f);
		r_s_r_set_matrix(1.0f, 1.0f, 0.1f, -90.0f, 0.0f, 1.0f, 0.0f);
		gate.draw();
		glPopMatrix();

		if (abs(angle2 - 0.0f) < 1e-4) {
			closing_flag2 = FALSE;
			close_flag2 = TRUE;
		}
	}
	set_alpha(1.0f);

	delete[] side_v;
	delete[] side_i;
	delete[] top_v;
	delete[] top_i;
	delete[] gate_v;
	delete[] gate_i;
}

void MyGLWidget::draw_buildings() {
	int u_num = 4;
	GLfloat* building_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* building_i = new GLuint[6 * u_num];
	get_cylinder_v_i(1.0f, 1.0f, u_num, building_v, building_i);
	GLfloat* top_v = new GLfloat[(u_num + 2) * 8];
	GLuint* top_i = new GLuint[3 * u_num];
	get_cycle_v_i(1.0f, u_num, top_v, top_i);

	char path_zhishan[] = "pics\\至善.png";
	object building_zhishan(building_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, building_i, sizeof(GLuint) * 6 * u_num, path_zhishan);
	char path_zhishantop[] = "pics\\至善楼顶.png";
	object top_zhishan(top_v, sizeof(GLfloat) * (u_num + 2) * 8, top_i, sizeof(GLuint) * 3 * u_num, path_zhishantop);
	//至善
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(1.2f, 0.0f, -4.3f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	building_zhishan.draw();
	glTranslatef(0.0f, 2.3f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	top_zhishan.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.2f, 0.0f, -4.3f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	building_zhishan.draw();
	glTranslatef(0.0f, 2.3f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	top_zhishan.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5.2f, 0.0f, -4.3f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	building_zhishan.draw();
	glTranslatef(0.0f, 2.3f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	top_zhishan.draw();
	glPopMatrix();

	//明德
	glPushMatrix();
	glTranslatef(3.4f, 0.0f, 3.5f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	building_zhishan.draw();
	glTranslatef(0.0f, 2.3f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	top_zhishan.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.4f, 0.0f, 5.5f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	building_zhishan.draw();
	glTranslatef(0.0f, 2.3f, 0.0f);
	r_s_r_set_matrix(2.0f, 2.3f, 0.5f, -90.0f, 0.0f, 1.0f, 0.0f);
	top_zhishan.draw();
	glPopMatrix();

	delete[] building_v;
	delete[] building_i;
	delete[] top_v;
	delete[] top_i;
}

void MyGLWidget::draw_p(GLuint number) {
	char *path1, *path2;
	if (number == 1) {
		char p1[] = "pics\\p1纯色.png";
		char p2[] = "pics\\p1.png";
		path1 = p1, path2 = p2;
	}
	else {
		char p1[] = "pics\\p2纯色.png";
		char p2[] = "pics\\p2.png";
		path1 = p1, path2 = p2;
	}
	int u_num = 16;
	GLfloat* side_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* side_i = new GLuint[6 * u_num];
	GLfloat* face_v = new GLfloat[(u_num + 2) * 8];
	GLuint* face_i = new GLuint[3 * u_num];

	get_cylinder_v_i(0.25f, 0.05f, u_num, side_v, side_i);
	object side1(side_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, side_i, sizeof(GLuint) * 6 * u_num, path1);
	get_cycle_v_i(0.25f, u_num, face_v, face_i);
	object top1(face_v, sizeof(GLfloat) * (u_num + 2) * 8, face_i, sizeof(GLuint) * 3 * u_num, path2);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(0.0f, 0.8f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.025f, 0.0f);
	side1.draw();
	r_t_set_matrix(180.0f, 0.0f, 0.0f, 1.0f, 0.0f, -0.025f, 0.0f);
	top1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.025f, 0.0f);
	top1.draw();
	glPopMatrix();

	s_r_t_set_matrix(0.08f, 12.0f, 0.08f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	side1.draw();
	s_r_t_set_matrix(0.4f, 2.0f, 0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	side1.draw();
	s_r_t_set_matrix(0.4f, 1.0f, 0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f);
	top1.draw();

	delete[] side_v;
	delete[] side_i;
	delete[] face_v;
	delete[] face_i;
}

void MyGLWidget::turn_p(GLuint number, GLint dir) {
	static GLfloat angle = 0.0f;
	GLfloat start = (dir - 2) * 90.0f;
	GLfloat offset = 0.20f;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	if (number == 1) {
		glTranslatef(offset, 0.0f, offset);
	}
	else {
		glTranslatef(-offset, 0.0f, -offset);
	}
	glPushMatrix();
	glRotatef(start + angle, 0.0f, 1.0f, 0.0f);
	draw_p(number);
	glPopMatrix();
	glPopMatrix();
	angle += 90.0f / 5;
	if (abs(angle - 90.0f) < 1e-3) {
		angle = 0.0f;
	}
}

void MyGLWidget::move_p(GLuint number, GLint dir, GLfloat x, GLfloat z) {
	static GLfloat step = 0.0f;
	GLfloat angle = (dir - 2) * 90.0f;
	GLfloat height = - step * step + step;
	GLfloat offset = 0.20f;
	GLfloat x_dir[5] = { 0.0f, -1.0f, 0.0f, 1.0f, 0.0f};
	GLfloat z_dir[5] = { 0.0f,  0.0f, 1.0f, 0.0f, -1.0f};
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	if (number == 1) {
		glTranslatef(offset, 0.0f, offset);
	}
	else {
		glTranslatef(-offset, 0.0f, -offset);
	}
	glPushMatrix();
	glTranslatef(x + x_dir[dir] * step, height, z + z_dir[dir] * step);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	draw_p(number);
	glPopMatrix();
	glPopMatrix();
	step += 0.20f;
	if (abs(step - 1.0f) < 1e-3) {
		step = 0.0f;
	}
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

	draw_map();
	draw_roadside();
	draw_buildings();
	draw_gate();
	move_p(2, 2, 0, 0);
}

void MyGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	update();
}

