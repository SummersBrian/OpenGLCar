/****************************************************************************
**
Window for OpenGL in QT.
Start code for CMSC 427, Spring 2015
Reference: cube & texture example in Qt Creator
author: Zheng Xu, xuzhustc@gmail.com
**
****************************************************************************/

#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include <QtGui>

#include <QMatrix4x4>
#include <QVector4D>

class MyWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = 0);
    ~MyWidget();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    void initShaders();
    void initMat();

private:
    void loadCubes();
    void moveCube(const int cords[6][4][3], float (&nCds)[6][4][3], float x, float y, float z, float scale);
    void addCube(QVector<GLfloat> &vertData,float coords[6][4][3]);


    QColor clearColor;
    QOpenGLShaderProgram *program;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    QMatrix4x4 mvpMat;
    QMatrix4x4 mvpMat0;
    QOpenGLTexture *textures[6];
    int cubNum;
    QString txtPath;

#define RESOURCE_FLAG true
#define TXT_IMG_PATH "C:/Users/Zheng/Documents/openglTest/images"
#define CUBE_NUM 1
};

#endif  //MYWIDGET_h
