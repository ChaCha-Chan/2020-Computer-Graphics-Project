#include "myglwidget.h"
#include "object.h"

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <type_traits>

int player_now = 1;
double count_step = 0.0;
bool press_flag = true;
bool IsDraw_turntable = false;
int turntable_number = 1;
int count_round = 0;
bool round_flag = false;
int direction = 1;
bool is_running = true;
GLfloat plate_angle = 30.0f;
bool number_flag = false;
bool is_moving = false;
GLfloat step[2] = { 0.0f,0.0f };

bool open_flag1 = FALSE;
bool openning_flag1 = false;
bool close_flag1 = true;
bool closing_flag1 = FALSE;

bool open_flag2 = FALSE;
bool openning_flag2 = false;
bool close_flag2 = true;
bool closing_flag2 = FALSE;
bool winner_flag = false;
int player_winner = 0;

GLfloat angle1 = 0.0f;
GLfloat angle2 = 0.0f;


MyGLWidget::MyGLWidget(QWidget* parent)
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
	initialGame();
}

void MyGLWidget::init_shaders(const char* v_path, const char* f_path) {
	//????????
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

	// ?????????
	GLuint v_shader;
	v_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_shader, 1, &v_shader_source, NULL);
	glCompileShader(v_shader);

	//????????
	GLuint f_shader;
	f_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f_shader, 1, &f_shader_source, NULL);
	glCompileShader(f_shader);

	//?????????
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

void MyGLWidget::set_matrix(GLdouble camera_x, GLdouble camera_z, GLdouble camera_x_at, GLdouble camera_z_at) {
	//ï¿½ï¿½È¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Í¶Ó°ï¿½ï¿½ï¿½ï¿½
	GLfloat projection_matrix[16];
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width() / (GLfloat)height(), 0.1f, 200.0f);
	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, projection_matrix);

	//ï¿½ï¿½È¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½modelviewï¿½ï¿½ï¿½ï¿½
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat modelview_matrix[16];
	//gluLookAt(0.0f, 5.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt(camera_x, 1.5f, camera_z, camera_x_at, 0.0f, camera_z_at, 0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix);

	//ï¿½ï¿½ï¿½Ã²ï¿½Í¸ï¿½ï¿½ï¿½ï¿½
	set_alpha(1.0f);
}

void MyGLWidget::draw_map() {
	GLfloat map_v[] = {
		// positions          // normals           // texture coords
		 10.0f,  0.0f,-10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
		 10.0f,  0.0f,10.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-10.0f,  0.0f,10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // bottom left
		-10.0f,  0.0f,-10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};

	GLuint map_i[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};

	static GLuint index = 0;

	char path1[] = "pics\\µØÍ¼1.png";
	char path2[] = "pics\\µØÍ¼2.png";
	char path3[] = "pics\\µØÍ¼3.png";
	char path4[] = "pics\\µØÍ¼4.png";
	char path5[] = "pics\\µØÍ¼5.png";
	char* path[] = { path1, path2, path3, path4, path5 };

	object map(map_v, sizeof(map_v), map_i, sizeof(map_i), path[index]);
	map.draw();
	index += 1;
	if (index == 5)index = 0;
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
	char path1[] = "pics\\Ë®ÄàÂ·.png";
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

//?????????????????
void MyGLWidget::get_cylinder_v_i(GLfloat r, GLfloat h, GLuint u_num, GLfloat* temp_v, GLuint* temp_i) {
	for (int i = 0; i < 2 * u_num + 2; i++) {
		if (i <= u_num) {
			temp_v[i * 8] = r * cos(PI * i * 2 / u_num);
			temp_v[i * 8 + 1] = h;
			temp_v[i * 8 + 2] = -r * sin(PI * i * 2 / u_num);
			temp_v[i * 8 + 6] = 1.0f * (double)i / u_num;
			temp_v[i * 8 + 7] = 1.0f;
			temp_v[i * 8 + 3] = cos(PI * i * 2 / u_num);
			temp_v[i * 8 + 4] = 0.0f;
			temp_v[i * 8 + 5] = -sin(PI * i * 2 / u_num);
		}
		else {
			temp_v[i * 8] = r * cos(PI * (i - u_num - 1) * 2 / u_num);
			temp_v[i * 8 + 1] = 0.0f;
			temp_v[i * 8 + 2] = -r * sin(PI * (i - u_num - 1) * 2 / u_num);
			temp_v[i * 8 + 6] = 1.0f * (double)(i - u_num - 1) / u_num;
			temp_v[i * 8 + 7] = 0.0f;
			temp_v[i * 8 + 3] = cos(PI * (i - u_num - 1) * 2 / u_num);
			temp_v[i * 8 + 4] = 0.0f;
			temp_v[i * 8 + 5] = -sin(PI * (i - u_num - 1) * 2 / u_num);
		}
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
//???????????????????????????????????u_num + 1 + 1 ????

void MyGLWidget::get_cycle_v_i(GLfloat r, GLuint u_num, GLfloat* temp_v, GLuint* temp_i) {
	for (int i = 0; i <= u_num; i++) {
		temp_v[i * 8] = r * cos(PI * i * 2 / u_num);
		temp_v[i * 8 + 1] = 0.0f;
		temp_v[i * 8 + 2] = -r * sin(PI * i * 2 / u_num);
		temp_v[i * 8 + 3] = 0.0f;
		temp_v[i * 8 + 4] = 1.0f;
		temp_v[i * 8 + 5] = 0.0f;
		temp_v[i * 8 + 6] = cos(PI * i * 2 / u_num) / 2 + 0.5f;
		temp_v[i * 8 + 7] = sin(PI * i * 2 / u_num) / 2 + 0.5f;
	}
	//???
	temp_v[(u_num + 1) * 8] = 0.0f;
	temp_v[(u_num + 1) * 8 + 1] = 0.0f;
	temp_v[(u_num + 1) * 8 + 2] = 0.0f;
	temp_v[(u_num + 1) * 8 + 3] = 0.0f;
	temp_v[(u_num + 1) * 8 + 4] = 1.0f;
	temp_v[(u_num + 1) * 8 + 5] = 0.0f;
	temp_v[(u_num + 1) * 8 + 6] = 0.5f;
	temp_v[(u_num + 1) * 8 + 7] = 0.5f;

	for (int i = 0; i < u_num; i++) {
		temp_i[i * 3] = i;
		temp_i[i * 3 + 1] = i + 1;
		temp_i[i * 3 + 2] = u_num + 1;
	}
}

void MyGLWidget::draw_gate() {
	//ï¿½ï¿½ï¿½ï¿½Òªï¿½ï¿½ï¿½Ä¸ï¿½×´Ì¬ ï¿½ï¿½ï¿½Ú¿ï¿½ ï¿½ï¿½ï¿½Ú¹ï¿½ ï¿½ï¿½ ï¿½ï¿½
	//Õ¢ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	int u_num = 16;
	GLfloat* side_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* side_i = new GLuint[6 * u_num];
	get_cylinder_v_i(0.15f, 0.6f, u_num, side_v, side_i);
	char path1[] = "pics\\Õ¢»ú²àÃæ.png";
	object side1(side_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, side_i, sizeof(GLuint) * 6 * u_num, path1);
	//Õ¢ï¿½ï¿½1
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 6.2f);
	side1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.0f, 7.7f);
	side1.draw();
	//Õ¢ï¿½ï¿½2
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.0f, 6.2f);
	side1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.0f, 7.7f);
	side1.draw();

	GLfloat* top_v = new GLfloat[(u_num + 2) * 8];
	GLuint* top_i = new GLuint[3 * u_num];
	get_cycle_v_i(0.15f, u_num, top_v, top_i);
	char path2[] = "pics\\Õ¢»úÉÏÃæ.png";
	object top1(top_v, sizeof(GLfloat) * (u_num + 2) * 8, top_i, sizeof(GLuint) * 3 * u_num, path2);
	//Õ¢ï¿½ï¿½1
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.6f, 6.2f);
	top1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 1.5f, 0.6f, 7.7f);
	top1.draw();
	//Õ¢ï¿½ï¿½2
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.6f, 6.2f);
	top1.draw();
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, -1.5f, 0.6f, 7.7f);
	top1.draw();

	//Õ¢ï¿½ï¿½ï¿½ï¿½Ä»
	char path3[] = "pics\\Õ¢»úÆÁÄ».png";
	object side2(side_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, side_i, sizeof(GLuint) * 6 * u_num, path3);
	//Õ¢ï¿½ï¿½1
	s_r_t_set_matrix(0.2f, 0.5f, 0.8f, 180.0f, 0.0f, 1.0f, 0.0f, 1.5f, 0.6f, 6.2f);
	side2.draw();
	s_r_t_set_matrix(0.2f, 1.0f, 0.8f, 180.0f, 0.0f, 1.0f, 0.0f, 1.5f, 0.9f, 6.2f);
	top1.draw();
	//Õ¢ï¿½ï¿½2
	s_r_t_set_matrix(0.2f, 0.5f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f, -1.5f, 0.6f, 6.2f);
	side2.draw();
	s_r_t_set_matrix(0.2f, 1.0f, 0.8f, 0.0f, 0.0f, 1.0f, 0.0f, -1.5f, 0.9f, 6.2f);
	top1.draw();

	//Õ¢ï¿½ï¿½ï¿½ï¿½
	int u_num1 = 4;
	GLfloat* gate_v = new GLfloat[2 * (u_num1 + 1) * 8];
	GLuint* gate_i = new GLuint[6 * u_num1];
	get_cylinder_v_i(0.4f, 0.4f, u_num1, gate_v, gate_i);
	char path4[] = "pics\\Õ¢»úÃÅ.png";
	object gate(gate_v, sizeof(GLfloat) * 2 * (u_num1 + 1) * 8, gate_i, sizeof(GLuint) * 6 * u_num1, path4);
	set_alpha(0.5f);
	glMatrixMode(GL_MODELVIEW);
	//Õ¢ï¿½ï¿½1
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
	else if (closing_flag1) {
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
	//Õ¢ï¿½ï¿½2
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

	//????
	GLfloat hpc_v[] = {
		-2.0f,  0.0f, 0.0f,  -2.0f, 0.0f, 0.0f,   0.0f, 0.0f,
		-2.0f,  0.0f, 1.0f,  -2.0f,  0.0f, 1.0f,   0.17f, 0.0f,
		-1.2f,  0.0f, 2.0f,  -1.2f,  0.0f, 2.0f,   0.33f, 0.0f,
		 1.2f,  0.0f, 2.0f,  1.2f,  0.0f, 2.0f,   0.50f, 0.0f,
		 2.0f,  0.0f, 1.0f, 2.0f,  0.0f, 1.0f,   0.67f, 0.0f,
		 2.0f,  0.0f, 0.0f,  2.0f,  0.0f, 0.0f,   0.83f, 0.0f,
		-2.0f,  0.0f, 0.0f,  -2.0f,  0.0f, 0.0f,   1.0f, 0.0f,

		-2.0f,  2.0f, 0.0f,  -2.0f,  2.0f, 0.0f,   0.0f, 1.0f,
		-2.0f,  2.0f, 1.0f,  -2.0f,  2.0f, 1.0f,   0.17f, 1.0f,
		-1.2f,  2.0f, 2.0f,  -1.2f,  2.0f, 2.0f,   0.33f, 1.0f,
		 1.2f,  2.0f, 2.0f,  1.2f,  2.0f, 2.0f,   0.50f, 1.0f,
		 2.0f,  2.0f, 1.0f, 2.0f,  2.0f, 1.0f,    0.67f, 1.0f,
		 2.0f,  2.0f, 0.0f,  2.0f,  2.0f, 0.0f,   0.83f, 1.0f,
		-2.0f,  2.0f, 0.0f,  -2.0f,  2.0f, 0.0f,   1.0f, 1.0f
	};
	GLuint hpc_i[] = {
		0, 1, 7,
		1, 8, 7,
		1, 2, 8,
		2, 9, 8,
		2, 3, 9,
		3, 10, 9,
		3, 4, 10,
		4, 11, 10,
		4, 5, 11,
		5, 12, 11,
		5, 6, 12,
		6, 13, 12
	};
	GLfloat hpc_top_v[] = {
	-2.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
	-2.0f,  2.0f, 1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.5f,
	-1.2f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f,   0.25f, 0.0f,
	 1.2f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f,   0.75f, 0.0f,
	 2.0f,  2.0f, 1.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.5f,
	 2.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
	};
	GLuint hpc_top_i[] = {
		0, 5, 1,
		1, 5, 2,
		2, 5, 3,
		3, 5, 4
	};
	char path_hpc1[] = "pics\\³¬Ëã.png";
	object hpc1(hpc_v, sizeof(hpc_v), hpc_i, sizeof(hpc_i), path_hpc1);
	char path_hpc2[] = "pics\\³¬ËãÉÏÃæ.png";
	object hpc2(hpc_top_v, sizeof(hpc_top_v), hpc_top_i, sizeof(hpc_top_i), path_hpc2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-2.8f, 0.0f, 0.8f);
	glRotatef(-135.0f, 0.0f, 1.0f, 0.0f);
	r_t_set_matrix(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	hpc1.draw();
	hpc2.draw();
	glPopMatrix();


	//????
	char path_lib2[] = "pics\\Í¼Êé¹İ2.png";
	object lib2(building_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, building_i, sizeof(GLuint) * 6 * u_num, path_lib2);
	object lib_top(top_v, sizeof(GLfloat) * (u_num + 2) * 8, top_i, sizeof(GLuint) * 6 * u_num, path_lib2);
	char path_lib3[] = "pics\\Í¼Êé¹İ3.png";
	object lib3(building_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, building_i, sizeof(GLuint) * 6 * u_num, path_lib3);
	char path_lib4[] = "pics\\Í¼Êé¹İ4.png";
	object lib4(building_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, building_i, sizeof(GLuint) * 6 * u_num, path_lib4);
	GLfloat lib1_v[] = {
		-1.2f,  0.2f, 2.0f,  -1.2f,  0.2f, 2.0f,   0.0f, 0.0f,
		-0.4f,  0.2f, 1.8f,  -0.4f,  0.2f, 1.8f,   0.33f, 0.0f,
		-1.2f,  0.2f, 2.0f,  -1.2f,  0.2f, 2.0f,   0.66f, 0.0f,
		-1.2f,  0.2f, 2.0f,  -1.2f,  0.2f, 2.0f,   1.0f, 0.0f,
		-1.2f,  1.4f, 2.0f,  -1.2f,  1.4f, 2.0f,   0.0f, 1.0f,
		-0.4f,  1.4f, 1.8f,  -0.4f,  1.4f, 1.8f,   0.33f, 1.0f,
		-1.2f,  1.4f, 2.0f,  -1.2f,  1.4f, 2.0f,   0.66f, 1.0f,
		-1.2f,  1.4f, 2.0f,  -1.2f,  1.4f, 2.0f,    1.0f, 1.0f
	};
	GLuint lib1_i[] = {
		0, 1, 4,
		1, 5, 4,
		1, 2, 5,
		2, 6, 5,
		2, 3, 6,
		3, 7, 6
	};
	char path_lib1[] = "pics\\Í¼Êé¹İ1.png";
	object lib1(lib1_v, sizeof(lib1_v), lib1_i, sizeof(lib1_i), path_lib1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-4.0f, 0.0f, 4.0f);
	//???????
	s_r_t_set_matrix(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	lib1.draw();
	glPushMatrix();
	glScalef(-1.0f, 1.0f, 1.0f);
	s_r_t_set_matrix(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	lib1.draw();
	glPopMatrix();
	//???????
	r_s_r_set_matrix(2.0f, 1.6f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib2.draw();

	glPushMatrix();
	glTranslatef(0.0f, 0.2f, 1.0f);
	r_s_r_set_matrix(2.0f, 0.2f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib_top.draw();
	glTranslatef(0.0f, 1.4f, 0.0f);
	r_s_r_set_matrix(2.0f, 0.2f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib_top.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 1.0f);
	r_s_r_set_matrix(2.0f, 0.2f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib2.draw();
	glTranslatef(0.0f, 1.4f, 0.0f);
	r_s_r_set_matrix(2.0f, 0.2f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib2.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.2f, 0.0f, 1.0f);
	r_s_r_set_matrix(0.2f, 1.6f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib2.draw();
	glTranslatef(2.4f, 0.0f, 0.0f);
	r_s_r_set_matrix(0.2f, 1.6f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib2.draw();
	glPopMatrix();

	//????????
	glPushMatrix();
	glTranslatef(0.0f, 0.1f, 1.6f);
	r_s_r_set_matrix(0.5f, 1.4f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib3.draw();
	glPopMatrix();
	//???????
	glPushMatrix();
	glTranslatef(-0.6f, 0.1f, 2.0f);
	r_s_r_set_matrix(0.07f, 1.4f, 0.07f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib4.draw();
	glTranslatef(1.2f, 0.0f, 0.0f);
	r_s_r_set_matrix(0.07f, 1.4f, 0.07f, 0.0f, 0.0f, 1.0f, 0.0f);
	lib4.draw();
	glPopMatrix();
	glPopMatrix();

	//????
	char path_zhishan[] = "pics\\ÖÁÉÆ.png";
	object building_zhishan(building_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, building_i, sizeof(GLuint) * 6 * u_num, path_zhishan);
	char path_zhishantop[] = "pics\\ÖÁÉÆÂ¥¶¥.png";
	object top_zhishan(top_v, sizeof(GLfloat) * (u_num + 2) * 8, top_i, sizeof(GLuint) * 3 * u_num, path_zhishantop);
	//????
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

	//????
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
	char* path1, * path2;
	if (number == 1) {
		char p1[] = "pics\\p1´¿É«.png";
		char p2[] = "pics\\p1.png";
		path1 = p1, path2 = p2;
	}
	else {
		char p1[] = "pics\\p2´¿É«.png";
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


void MyGLWidget::move_p(GLuint number, GLint dir, GLfloat x, GLfloat z, bool Ismove) {

	GLfloat angle = (dir - 2) * 90.0f;
	GLfloat height = -step[number - 1] * step[number - 1] + step[number - 1];
	GLfloat offset = 0.20f;
	GLfloat x_dir[5] = { 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
	GLfloat z_dir[5] = { 0.0f,  0.0f, 1.0f, 0.0f, -1.0f };
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	if (number == 1) {
		glTranslatef(offset, 0.0f, offset);
	}
	else {
		glTranslatef(-offset, 0.0f, -offset);
	}
	glPushMatrix();
	glTranslatef(x + x_dir[dir] * step[number - 1], height, z + z_dir[dir] * step[number - 1]);
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	draw_p(number);
	glPopMatrix();
	glPopMatrix();
	//printf("number:%d,move:%d,step:%.3f\n", number, Ismove, step[number - 1]);
	if (Ismove == true)
	{
		step[number - 1] += 0.20f;
		if (abs(step[number - 1] - 1.0f) < 1e-3) {
			step[number - 1] = 0.0f;
		}
	}
	else
	{
		height = 0.0;
	}
}


void MyGLWidget::draw_bush()
{
	int u_num = 8;
	GLfloat* bush_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* bush_i = new GLuint[6 * u_num];
	get_cylinder_v_i(1.0f, 1.0f, u_num, bush_v, bush_i);
	GLfloat* top_v = new GLfloat[(u_num + 2) * 8];
	GLuint* top_i = new GLuint[3 * u_num];
	get_cycle_v_i(1.0f, u_num, top_v, top_i);

	char path_bush[] = "pics\\purple_bush.png";
	object bush(bush_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, bush_i, sizeof(GLuint) * 6 * u_num, path_bush);
	char path_bushtop[] = "pics\\purple_bush.png";
	object top_bush(top_v, sizeof(GLfloat) * (u_num + 2) * 8, top_i, sizeof(GLuint) * 3 * u_num, path_bushtop);

	GLfloat pos[] = {
						2.1f, 4.5f,
						3.0f, 4.5f,
						3.9f, 4.5f,
						4.8f, 4.5f,
						2.2f, -5.6f,
						2.2f, -4.7f,
						2.2f, -3.8f,
						2.2f, -2.9f,
						4.2f, -2.9f,
						4.2f, -3.8f,
						4.2f, -4.7f,
						4.2f, -5.6f,
						-6.0f, 0.0f,
						-6.0f, -3.0f,
						-6.0f, -4.0f,
						-6.0f, -5.0f,
						-6.0f, 1.0f,
						-6.0f, 2.0f,
						-6.0f, 3.0f,
						-6.0f, 4.0f,
						-6.0f, 5.0f
	};

	for (int i = 0; i < sizeof(pos) / sizeof(int) / 2; i++)
	{
		GLfloat x = pos[2 * i];
		GLfloat z = pos[2 * i + 1];
		glPushMatrix();
		glTranslatef(x, 0.0f, z);
		r_s_r_set_matrix(0.5f, 0.5f, 0.5f, -22.5f, 0.0f, 1.0f, 0.0f);
		bush.draw();
		glTranslatef(0.0f, 0.5f, 0.0f);
		r_s_r_set_matrix(0.5f, 0.5f, 0.5f, -22.5f, 0.0f, 1.0f, 0.0f);
		top_bush.draw();
		glPopMatrix();
	}
}

void MyGLWidget::draw_parterre()
{
	int u_num = 16;
	GLfloat* base_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* base_i = new GLuint[6 * u_num];
	get_cylinder_v_i(1.0f, 1.0f, u_num, base_v, base_i);
	GLfloat* basetop_v = new GLfloat[(u_num + 2) * 8];
	GLuint* basetop_i = new GLuint[3 * u_num];
	get_cycle_v_i(1.0f, u_num, basetop_v, basetop_i);

	char path_base[] = "pics\\parterre_side.png";
	object base(base_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, base_i, sizeof(GLuint) * 6 * u_num, path_base);
	char path_basetop[] = "pics\\parterre_top.png";
	object top_base(basetop_v, sizeof(GLfloat) * (u_num + 2) * 8, basetop_i, sizeof(GLuint) * 3 * u_num, path_basetop);
	//????
	glPushMatrix();
	glTranslatef(4.0f, 0.0f, 0.0f);
	r_s_r_set_matrix(2.1f, 0.1f, 2.1f, 0.0f, 0.0f, 1.0f, 0.0f);
	base.draw();
	glTranslatef(0.0f, 0.1f, 0.0f);
	r_s_r_set_matrix(2.1f, 0.1f, 2.1f, 0.0f, 0.0f, 1.0f, 0.0f);
	top_base.draw();
	glPopMatrix();

	GLfloat* grass_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* grass_i = new GLuint[6 * u_num];
	get_cylinder_v_i(1.0f, 1.0f, u_num, grass_v, grass_i);
	GLfloat* grasstop_v = new GLfloat[(u_num + 2) * 8];
	GLuint* grasstop_i = new GLuint[3 * u_num];
	get_cycle_v_i(1.0f, u_num, grasstop_v, grasstop_i);

	char path_grass[] = "pics\\purple_bush.png";
	object grass(grass_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, grass_i, sizeof(GLuint) * 6 * u_num, path_grass);
	object top_grass(grasstop_v, sizeof(GLfloat) * (u_num + 2) * 8, grasstop_i, sizeof(GLuint) * 3 * u_num, path_grass);
	//????
	glPushMatrix();
	glTranslatef(4.0f, 0.15f, 0.0f);
	r_s_r_set_matrix(1.5f, 0.2f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	grass.draw();
	glTranslatef(0.0f, 0.2f, 0.0f);
	r_s_r_set_matrix(1.5f, 0.2f, 1.5f, 0.0f, 0.0f, 1.0f, 0.0f);
	top_grass.draw();
	glPopMatrix();
}

void MyGLWidget::draw_trees()
{
	int u_num = 16;
	GLfloat* base_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* base_i = new GLuint[6 * u_num];
	get_cylinder_v_i(1.0f, 1.0f, u_num, base_v, base_i);
	GLfloat* basetop_v = new GLfloat[(u_num + 2) * 8];
	GLuint* basetop_i = new GLuint[3 * u_num];
	get_cycle_v_i(1.0f, u_num, basetop_v, basetop_i);

	char path_base[] = "pics\\Ê÷¸É.png";
	object base(base_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, base_i, sizeof(GLuint) * 6 * u_num, path_base);
	object top_base(basetop_v, sizeof(GLfloat) * (u_num + 2) * 8, basetop_i, sizeof(GLuint) * 3 * u_num, path_base);

	GLfloat* grass_v = new GLfloat[2 * (u_num + 1) * 8];
	GLuint* grass_i = new GLuint[6 * u_num];
	get_cylinder_v_i(1.0f, 1.0f, u_num, grass_v, grass_i);
	GLfloat* grasstop_v = new GLfloat[(u_num + 2) * 8];
	GLuint* grasstop_i = new GLuint[3 * u_num];
	get_cycle_v_i(1.0f, u_num, grasstop_v, grasstop_i);

	char path_grass[] = "pics\\bush.png";
	object grass(grass_v, sizeof(GLfloat) * 2 * (u_num + 1) * 8, grass_i, sizeof(GLuint) * 6 * u_num, path_grass);
	object top_grass(grasstop_v, sizeof(GLfloat) * (u_num + 2) * 8, grasstop_i, sizeof(GLuint) * 3 * u_num, path_grass);
	GLfloat pos[] = {
						1.0f,-2.0f,
						1.0f,-1.0f,
						1.0f, 0.0f,
						1.0f, 1.0f,
						1.0f, 2.0f,
						1.0f, 3.0f,
						1.0f, 4.0f,
						1.0f, 5.0f,
						-1.0f, -2.0f,
						-1.0f, -3.0f,
						-1.0f, -4.0f,
						-1.0f, -5.0f,
						-1.0f, -1.0f,
						-1.0f, 2.0f,
						-1.0f, 3.0f,
						-1.0f, 4.0f,
						-1.0f, 5.0f,
						3.5f, 0.0f,
						4.5f, 0.0f,
						4.0f, 0.5f,
						4.0f, -0.5f
	};

	for (int i = 0; i < sizeof(pos) / sizeof(int) / 2; i++)
	{
		GLfloat x = pos[2 * i];
		GLfloat z = pos[2 * i + 1];
		glPushMatrix();
		glTranslatef(x, 0.0f, z);
		r_s_r_set_matrix(0.1f, 0.5f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f);
		base.draw();
		glTranslatef(0.0f, 0.5f, 0.0f);
		r_s_r_set_matrix(0.1f, 0.5f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f);
		top_base.draw();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(x, 0.55f, z);
		r_s_r_set_matrix(0.5f, 0.15f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
		grass.draw();
		glTranslatef(0.0f, 0.15f, 0.0f);
		r_s_r_set_matrix(0.5f, 0.15f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
		top_grass.draw();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(x, 0.75f, z);
		r_s_r_set_matrix(0.3f, 0.2f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0f);
		grass.draw();
		glTranslatef(0.0f, 0.2f, 0.0f);
		r_s_r_set_matrix(0.3f, 0.2f, 0.3f, 0.0f, 0.0f, 1.0f, 0.0f);
		top_grass.draw();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(x, 1.0f, z);
		r_s_r_set_matrix(0.15f, 0.22f, 0.15f, 0.0f, 0.0f, 1.0f, 0.0f);
		grass.draw();
		glTranslatef(0.0f, 0.22f, 0.0f);
		r_s_r_set_matrix(0.15f, 0.12f, 0.15f, 0.0f, 0.0f, 1.0f, 0.0f);
		top_grass.draw();
		glPopMatrix();
	}
}

void MyGLWidget::draw_skybox() {
	GLfloat skybox_top_v[] = {
		// positions          // colors           // texture coords
		 10.0f,  15.0f,-10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
		 10.0f,  15.0f,10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f, // bottom right
		-10.0f,  15.0f,10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.0f,  15.0f,-10.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f  // top left 
	};

	GLuint skybox_top_i[] = {
	   3, 1, 0, // first triangle
	   3, 2, 1  // second triangle
	};

	char path_top[] = "pics\\skybox_top.png";
	object skybox_top(skybox_top_v, sizeof(skybox_top_v), skybox_top_i, sizeof(skybox_top_i), path_top);
	r_t_set_matrix(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	skybox_top.draw();

	GLfloat skybox_side_v[] = {
		// positions          // colors           // texture coords
		-10.0f,  15.0f,-10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
		-10.0f,  0.0f,-10.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-10.0f,  0.0f,10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.0f,  15.0f,10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
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

void MyGLWidget::draw_winner(GLint player)
{
	GLfloat winner_v[] = {
		// positions          // colors           // texture coords
		0.0f,  0.3f,-0.3f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.0f,  0.0f,-0.3f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		0.0f,  0.0f,0.3f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		0.0f,  0.3f,0.3f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
	};

	GLuint winner_i[] = {
	   0, 1, 3, // first triangle
	   1, 2, 3  // second triangle
	};

	char path_winner1[] = "pics\\winner_p1.png";
	char path_winner2[] = "pics\\winner_p2.png";

	if (player == 1)
	{
		object winner1(winner_v, sizeof(winner_v), winner_i, sizeof(winner_i), path_winner1);
		glPushMatrix();
		r_t_set_matrix(20.0f, 0.0f, 0.0f, 10.0f, 1.825f, 1.235f, -7.0f);
		winner1.draw();
		glPopMatrix();
	}

	else if (player == 2)
	{
		object winner2(winner_v, sizeof(winner_v), winner_i, sizeof(winner_i), path_winner2);
		glPushMatrix();
		r_t_set_matrix(20.0f, 0.0f, 0.0f, 10.0f, 1.825f, 1.235f, -7.0f);
		winner2.draw();
		glPopMatrix();
	}
}

void MyGLWidget::draw_plate(GLint target_num, GLfloat x, GLfloat y, GLfloat z, GLint direction)
{
	GLfloat d[] = { 0.0f,0.0f,90.0f,180.0f,270.0f };
	GLfloat cam_angle = d[direction];
	GLfloat target_angle = 30.0f - target_num * 60.0f;
	if (is_running)
	{
		//printf("is_running:%d", is_running);
		if (plate_angle <= target_angle)
		{
			is_running = false;
		}
		plate_angle -= 10.0f;
	}
	GLint u_num = 180;
	GLfloat r = 0.3f;
	GLfloat angle = 30.0f;
	GLfloat* plate_v = (GLfloat*)malloc(sizeof(GLfloat) * (u_num + 2) * 8);
	GLuint* plate_i = (GLuint*)malloc(sizeof(GLuint) * u_num * 3);
	get_cycle_v_i(r, u_num, plate_v, plate_i);
	char path_plate[] = "pics\\plate.png";
	object plate(plate_v, sizeof(GLfloat) * (u_num + 2) * 8, plate_i, sizeof(GLuint) * u_num * 3, path_plate);

	GLfloat modelview_matrix[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(cam_angle, 0.0f, 10.0f, 0.0f);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 0.0f, 10.0f);
	glRotatef(angle, 0.0f, 0.0f, 10.0f);
	glRotatef(plate_angle, 0.0f, 10.0f, 0.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix);
	glPopMatrix();
	plate.draw();

	GLfloat arrow_v[] = {
		// positions          // colors           // texture coords
		 0.0f,  -r, r / 6.0f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f,
		 0.0f,  -r, r / -6.0f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f,
		 0.0f,  r / 6.0f - r , 0.0f,  1.0f, 1.0f, 1.0f,   0.5f, 1.0f,
	};

	GLuint arrow_i[] = {
		0,1,2
	};

	char path_arrow[] = "pics\\arrow.png";
	object arrow(arrow_v, sizeof(arrow_v), arrow_i, sizeof(arrow_i), path_arrow);

	GLfloat modelview_matrix_arrow[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(cam_angle, 0.0f, 10.0f, 0.0f);
	glRotatef(angle, 0.0f, 0.0f, 10.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix_arrow);
	glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelview"), 1, GL_FALSE, modelview_matrix_arrow);
	glPopMatrix();
	arrow.draw();
}

void MyGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	set_matrix(camera_x[player_now], camera_z[player_now], camera_x_at[player_now], camera_z_at[player_now]);

	draw_map();
	draw_skybox();
	draw_roadside();
	draw_buildings();
	////printf("angle_1: %.4f\n",angle1);
	////printf("close_flag1£º%d,close_flag2: %d\n",close_flag1,close_flag2);
	if (abs(playerPos[player_now][2] - 7.0) < 1e-6 && playerPos[player_now][0] > -3.0 && close_flag2 == true)
	{
		//printf("ÏÖÔÚÔÚ3ºÅÏßÂ·ÁË£¡\n");
		openning_flag2 = true;
		close_flag2 = false;
	}
	if (abs(playerPos[player_now][2] - 7.0) < 1e-6 && playerPos[player_now][0] > -0.5 && close_flag1 == true)
	{
		//printf("ÏÖÔÚÔÚ¹ıÖĞÏßÁË£¡\n");
		openning_flag1 = true;
		close_flag1 = false;
	}
	else
	{
		if (open_flag1 == true)
		{
			closing_flag1 = true;
		}
		if (open_flag2 == true)
		{
			closing_flag2 = true;
		}
	}
	draw_gate();
	Game();
	// winner_flag =true;
	// player_winner = 1;
	// player_now = 0;
	if (winner_flag == true)
	{
		draw_winner(player_winner);
	}
}

void MyGLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	update();
}

void MyGLWidget::initialGame()
{
	for (int i = 0; i < 3; i++)
	{
		playerPos[i][0] = -1.0;
		playerPos[i][1] = 1.5;
		playerPos[i][2] = -7.0;
	}

	for (int i = 0; i < 3; i++)
	{
		camera_x[i] = playerPos[i][0] + 3;
		camera_z[i] = playerPos[i][2];
		camera_x_at[i] = playerPos[i][0];
		camera_z_at[i] = playerPos[i][2];
	}

	endPos[0] = 1.0;
	endPos[2] = -7.0;
	endPos[1] = 1.5;

}

void MyGLWidget::keyPressEvent(QKeyEvent* e) {
	//Press 0 or 1 to switch the scene
	if (press_flag == true)
	{
		if (e->key() == Qt::Key_Space) {
			IsDraw_turntable = true;
			press_flag = false;
			is_running = true;
			number_flag = true;
			//printf("\n\n\npress_flag = 0,is_running = %d\n\n\n\n", is_running);
		}
	}

}

void MyGLWidget::Game()
{
	if (player_now == 1 || player_now == 2) {
		int i = player_now;
		////printf("\n\n\nÕâÊÇÍæ¼Ò£º%d\n",i);
		//printf("player : (x,y) = (%.3f,%.3f)\ncamera : (x,y) = (%.3f,%.3f)\n", playerPos[i][0], playerPos[i][2], camera_x[i], camera_z[i]);

		//ÅĞ¶Ïµ±Ç°µÄµÀÂ·
		if (abs(playerPos[i][2] + 7.0) < 0.0000001 && playerPos[i][0] < 0.0 && playerPos[i][0] > -7.0)
		{
			direction = 1;
		}
		else if (abs(playerPos[i][0] + 7.0) < 0.0000001 && playerPos[i][2] < 7.0)
		{
			direction = 2;
		}
		else if (abs(playerPos[i][2] - 7.0) < 0.0000001 && playerPos[i][0] < 7.0)
		{
			direction = 3;
		}
		else if (abs(playerPos[i][0] - 7.0) < 0.0000001 && playerPos[i][2] > -7.0)
		{
			direction = 4;
		}
		else if (abs(playerPos[i][2] + 7.0) < 0.0000001 && playerPos[i][0] > 0.0)
		{
			direction = 5;
		}
		int obj_dir = 0;
		if (direction == 1 || direction == 5)
		{
			obj_dir = 1;
			move_p(1, obj_dir, playerPos[1][0], playerPos[1][2], (!press_flag && player_now == 1 && is_moving));
			move_p(2, obj_dir, playerPos[2][0], playerPos[2][2], (!press_flag && player_now == 2 && is_moving));
		}
		else
		{
			obj_dir = direction;
			move_p(1, obj_dir, playerPos[1][0], playerPos[1][2], (!press_flag && player_now == 1 && is_moving));
			move_p(2, obj_dir, playerPos[2][0], playerPos[2][2], (!press_flag && player_now == 2 && is_moving));
		}

		//Èç¹ûÃ»ÓĞ°´¼üÁË
		if (press_flag == true)
		{
			int delay = 1000000;
			while (delay)
			{
				delay--;
			}
			////printf("delay½áÊøÁË\n");
		}
		else {//°´¼üÁË
			if (number_flag)
			{
				//Éú³ÉËæ»úÊı
				srand(time(NULL));
				turntable_number = rand() % 6 + 1;
				number_flag = false;
			}
			if (IsDraw_turntable == true /*&&  turntable_down == true*/)
			{
				//×ªÅÌ
				//printf("random_number:%d\n", turntable_number);
				if (abs(playerPos[i][0] + 7.0) < 0.00000001 && abs(playerPos[i][2] + 7.0) < 1e-6)
				{
					obj_dir = 1;
				}
				else if (abs(playerPos[i][0] + 7.0) < 0.0000001 && abs(playerPos[i][2] - 7.0) < 1e-6)
				{
					obj_dir = 2;
				}
				else if (abs(playerPos[i][0] - 7.0) < 1e-6 && abs(playerPos[i][2] - 7.0) < 1e-6)
				{
					obj_dir = 3;
				}
				else if (abs(playerPos[i][0] - 7.0) < 1e-6 && abs(playerPos[i][2] + 7.0) < 1e-6)
				{
					obj_dir = 4;
				}

				draw_plate(turntable_number, 0.2 * playerPos[i][0] + 0.8 * camera_x[i], 1.2f, 0.2 * playerPos[i][2] + 0.8 * camera_z[i], obj_dir);
				//×ª½áÊøÁË
				if (is_running == false)
				{
					plate_angle = 30.0f;
					IsDraw_turntable = false;
					is_running = true;
					is_moving = true;
				}

			}
			else//Ç°½ø
			{
				//printf("count_step:%.4f\n", count_step);
				if (count_step + 0.2 < turntable_number || abs(count_step + 0.2 - turntable_number) < 0.0000001)
				{
					if (direction == 1)
					{
						camera_x[i] -= 0.2;
						playerPos[i][0] -= 0.2;
						camera_x_at[i] = playerPos[i][0];
						count_step += 0.2;
					}
					else if (direction == 2)
					{
						if ((abs(playerPos[i][0] - (-7.0)) < 0.0000001 && camera_x[i] > -6.9 && abs(playerPos[i][2] + 7.0) < 0.0000001))
						{
							camera_x[i] -= 0.6;
							camera_z[i] -= 0.6;
							//turn_p(player_now, obj_dir, playerPos[i][0], playerPos[i][2]);
						}
						else
						{
							camera_z[i] += 0.2;
							playerPos[i][2] += 0.2;
							camera_z_at[i] = playerPos[i][2];
							count_step += 0.2;
						}

					}
					else if (direction == 3)
					{
						if ((abs(playerPos[i][0] - (-7.0)) < 0.0000001 && camera_x[i] > -9.9 && abs(playerPos[i][2] - 7.0) < 0.0000001))
						{
							camera_x[i] -= 0.6;
							camera_z[i] += 0.6;
						}
						else
						{
							camera_x[i] += 0.2;
							playerPos[i][0] += 0.2;
							camera_x_at[i] = playerPos[i][0];
							count_step += 0.2;
						}
					}
					else if (direction == 4)
					{
						if ((abs(playerPos[i][0] - 7.0) < 0.0000001 && camera_x[i] < 6.9 && abs(playerPos[i][2] - 7.0) < 0.0000001))
						{
							camera_x[i] += 0.6;
							camera_z[i] += 0.6;
						}
						else
						{
							camera_z[i] -= 0.2;
							playerPos[i][2] -= 0.2;
							camera_z_at[i] = playerPos[i][2];
							count_step += 0.2;
						}
					}
					else if (direction == 5)
					{
						if ((abs(playerPos[i][0] - 7.0) < 0.0000001 && camera_x[i] < 9.9 && abs(playerPos[i][2] + 7.0) < 0.0000001))
						{
							camera_x[i] += 0.6;
							camera_z[i] -= 0.6;
						}
						else
						{
							camera_x[i] -= 0.2;
							playerPos[i][0] -= 0.2;
							if (abs(playerPos[i][0] - endPos[0]) < 0.0000001)
							{
								//printf("you win!");
								winner_flag = true;
								player_winner = player_now;
								player_now = 0;
							}
							camera_x_at[i] = playerPos[i][0];
							count_step += 0.2;
						}
					}
				}
				else
				{
					count_step = 0.0;
					press_flag = true;
					is_moving = false;
					int delay = 100000000;
					step[player_now - 1] = 0;
					while (delay)
					{
						delay--;
					}
					//printf("delay½áÊøÁË\n");
					if (i == 1)
					{
						player_now = 2;
					}
					else
					{
						player_now = 1;
					}
				}

			}
		}
	}


}

