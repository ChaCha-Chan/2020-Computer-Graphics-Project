#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#ifdef MAC_OS
#include <QtOpenGL/QtOpenGL>
#else
#include <GL/glew.h>
#endif
#include <QtGui>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
#define PI 3.1415926

class MyGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

protected:
    void initializeGL();
    void init_shaders(const char* v_path, const char* f_path);
    void set_alpha(GLfloat alpha);
    void set_matrix();
    void draw_map();
    void draw_roadside();
    void get_cylinder_v_i(GLfloat r, GLfloat h, GLuint u_num, GLfloat* temp_v, GLuint* temp_i);
    void get_cycle_v_i(GLfloat r, GLuint u_num, GLfloat* temp_v, GLuint* temp_i);
    void draw_gate();
    void r_t_set_matrix(GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3, GLfloat t1, GLfloat t2, GLfloat t3);
    void s_r_t_set_matrix(GLfloat s1, GLfloat s2, GLfloat s3, GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3, GLfloat t1, GLfloat t2, GLfloat t3);
    void r_s_r_set_matrix(GLfloat s1, GLfloat s2, GLfloat s3, GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3);
    void paintGL();
    void resizeGL(int width, int height);

    QTimer* timer;
    GLuint shader_program;
};
#endif // MYGLWIDGET_H
