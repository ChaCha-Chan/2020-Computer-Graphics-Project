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
    void set_matrix(GLdouble camera_x,GLdouble camera_z,GLdouble camera_x_at,GLdouble camera_z_at);
    void draw_map();
    void draw_roadside();
    void get_cylinder_v_i(GLfloat r, GLfloat h, GLuint u_num, GLfloat* temp_v, GLuint* temp_i);
    void get_cycle_v_i(GLfloat r, GLuint u_num, GLfloat* temp_v, GLuint* temp_i);
    void draw_gate();
    void draw_buildings();
    void draw_p(GLuint number);
    //void turn_p(GLuint number, GLint dir, GLfloat x, GLfloat z);
    void move_p(GLuint number, GLint dir, GLfloat x, GLfloat z,bool Ismove);
    void r_t_set_matrix(GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3, GLfloat t1, GLfloat t2, GLfloat t3);
    void s_r_t_set_matrix(GLfloat s1, GLfloat s2, GLfloat s3, GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3, GLfloat t1, GLfloat t2, GLfloat t3);
    void r_s_r_set_matrix(GLfloat s1, GLfloat s2, GLfloat s3, GLfloat r0, GLfloat r1, GLfloat r2, GLfloat r3);
    void paintGL();
    void resizeGL(int width, int height);
    void draw_trees();
    void draw_parterre();
    void draw_bush();
    void draw_skybox();
    void initialGame();
    void Game();
    void keyPressEvent(QKeyEvent *e);
    void draw_plate(GLint target_num, GLfloat x, GLfloat y, GLfloat z, GLint direction);
    void draw_winner(GLint player);

    QTimer* timer;
    GLuint shader_program;
    GLdouble camera_x[3];
    GLdouble camera_x_at[3];
    GLdouble camera_z_at[3];
    GLdouble camera_z[3];
    GLdouble playerPos[3][3];
    GLdouble endPos[3];
};
#endif // MYGLWIDGET_H
