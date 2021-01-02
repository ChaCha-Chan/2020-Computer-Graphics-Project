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


class MyGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT

public:
    MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget();

protected:
    void initializeGL();
    void init_shaders(const char* v_path, const char* f_path);
    void set_matrix();
    void set_texture();
    void paintGL();
    void resizeGL(int width, int height);

    QTimer* timer;
    GLuint shader_program;
};
#endif // MYGLWIDGET_H
