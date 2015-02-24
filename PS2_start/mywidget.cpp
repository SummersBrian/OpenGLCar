/****************************************************************************
**
Window for OpenGL in QT.
Start code for CMSC 427, Spring 2015
Reference: cube & texture example in Qt Creator
author: Zheng Xu, xuzhustc@gmail.com
**
****************************************************************************/

#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    memset(textures, 0, sizeof(textures));
    txtPath = TXT_IMG_PATH;
    cubNum = CUBE_NUM;
}

MyWidget::~MyWidget()
{
    makeCurrent();
    vbo.destroy();
    delete program;
    for (int j = 0; j < 6; ++j)
        delete textures[j];
    doneCurrent();
}

void MyWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // Enable back face culling
    glEnable(GL_CULL_FACE);

    clearColor.setRgbF(0.1, 0.1, 0.1, 1.0);

    vao.create(); vao.bind();

    loadCubes();
    initMat();
    initShaders();
}

void MyWidget::initMat()
{
    QMatrix4x4 proj;
    QMatrix4x4 view;
    proj.perspective(45.0f, 1.0f, 0.000001f, 100.0f);
    view.lookAt(QVector3D(0.0f, 0.0, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f));
    mvpMat0 = proj * view;
    mvpMat = mvpMat0;
}

void MyWidget::initShaders()
{
#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
            "#version 330\n"
            "layout (location = 0) in vec4 vertex;\n"
            "layout (location = 1) in vec4 texCoord;\n"
            "uniform mat4 matrix;\n"
            "out vec4 tCd;\n"
            "void main(void)\n"
            "{\n"
            "   gl_Position = matrix * vertex;\n"
            "   tCd = texCoord;\n"
            "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
            "#version 330\n"
            "uniform sampler2D fTexture;\n"
            "in vec4 tCd;\n"
            "out vec4 ffColor;\n"
            "void main(void)\n"
            "{\n"
            "   ffColor = texture(fTexture, tCd.st);\n"
            "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QOpenGLShaderProgram;
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("fTexture", 0);
}

void MyWidget::loadCubes()
{
    static const int iCds[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
    };
    float coords[6][4][3];
    QVector<GLfloat> vertData;

    moveCube(iCds, coords, 1.5f, 0.5f, 10.0f, 0.05f);
    addCube(vertData, coords);

    for (int j = 0; j < 6; ++j)
    {
        if(RESOURCE_FLAG)
        {
            textures[j] = new QOpenGLTexture(QImage(QString(":/images/side%1.png").arg(j + 1)).mirrored(true, false));
        }
        else
        {
            textures[j] = new QOpenGLTexture(QImage(txtPath+QString("/side%1.png").arg(j + 1)).mirrored(true, false));
        }
    }

    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}


void MyWidget::addCube(QVector<GLfloat> &vertData,float coords[6][4][3])
{
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 4; ++j) {
            // vertex position
            vertData.append(coords[i][j][0]);
            vertData.append(coords[i][j][1]);
            vertData.append(coords[i][j][2]);
            // texture coordinate
            vertData.append(j == 0 || j == 3);
            vertData.append(j == 0 || j == 1);
        }
    }
}

void MyWidget::moveCube(const int cords[6][4][3], float (&nCds)[6][4][3], float x, float y, float z, float scale)
{
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
                nCds[i][j][0] = (cords[i][j][0]+x)*scale;
                nCds[i][j][1] = (cords[i][j][1]+y)*scale;
                nCds[i][j][2] = (cords[i][j][2]+z)*scale;
        }
    }
}

void MyWidget::paintGL()
{
    //background
    glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->setUniformValue("matrix", mvpMat);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    for (int i = 0; i < 6; ++i) {
        textures[i]->bind();
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    }
}

void MyWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}