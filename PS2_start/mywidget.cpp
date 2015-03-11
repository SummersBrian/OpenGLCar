/****************************************************************************
**
CMSC427 Spring 2015 Problem Set 2
Brian Summers - 110656609
summers.brian.cs@gmail.com
3/5/2015


Uses Start code by:
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
    cubNum = 0;

    setFocusPolicy(Qt::StrongFocus);

    //transformation variables
    rot_radians = 3.14f/36.0f;
    automove = false;
    move_speed = 0.03f;
    rot_count = 0;
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
    proj.perspective(45.0f, 1.0f, 0.000001f, 100.0f);
    view.lookAt(QVector3D(0.0f, 0.0, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f));
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
            "uniform vec4 color;\n"
            "in vec4 tCd;\n"
            "out vec4 ffColor;\n"
            "void main(void)\n"
            "{\n"
            "   ffColor = texture(fTexture, tCd.st) * color;\n"
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

    //adding 6 cubes to the world
    moveCube(iCds, coords, 1.5f, 0.5f, 10.0f, 0.05f);
    addCube(vertData, coords);
    cubNum++;

    moveCube(iCds, coords, -5.0f, 0.5f, 15.0f, 0.05f);
    addCube(vertData, coords);
    cubNum++;

    moveCube(iCds, coords, 1.5f, 0.5f, 20.f, 0.05f);
    addCube(vertData, coords);
    cubNum++;

    moveCube(iCds, coords, -5.0f, 0.5f, 25.0f, 0.05f);
    addCube(vertData, coords);
    cubNum++;

    moveCube(iCds, coords, 1.5f, 0.5f, 30.0f, 0.05f);
    addCube(vertData, coords);
    cubNum++;

    moveCube(iCds, coords, -5.0f, 0.5f, 35.0f, 0.05f);
    addCube(vertData, coords);
    cubNum++;

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

    int offset = 0;
    program->setUniformValue("matrix", proj * view * model);
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, offset, 3, 5 * sizeof(GLfloat));
    offset += 3 * sizeof(GLfloat);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, offset, 2, 5 * sizeof(GLfloat));

    //setting colors for each cube
    QVector4D color[6] = {
        QVector4D(1.0f, 0.55f, 0.55f, 0.0f),
        QVector4D(0.55f, 0.55f, 1.0f, 0.0f),
        QVector4D(0.55f, 1.0f, 0.55f, 0.0f),
        QVector4D(1.0f, 0.55f, 1.0f, 0.0f),
        QVector4D(0.55f, 1.0f, 1.0f, 0.0f),
        QVector4D(1.0f, 1.0f, 0.55f, 0.0f),
    };

    //painting textures and colors for each cube
    for (int cube = 0; cube < cubNum; ++cube) {
        for (int i = 0; i < 6; ++i) {
            textures[i]->bind();
            program->setUniformValue("color", color[cube]); //adding color to the vertex shader
            glDrawArrays(GL_TRIANGLE_FAN, (cube*6+i) * 4, 4);
        }
    }
}

void MyWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

// respond to keyboard key presses for translation and rotation
// see translate(), rotateYaw(), rotatePitch() functions
void MyWidget::keyPressEvent(QKeyEvent * event) {

    //translate in forward direction
    if(event->key() == Qt::Key_Up) {
        //changes move speed to translate in proper direction
        if (move_speed < 0)
            move_speed *= -1;

        //forward automove
        if (automove) {
            connect(timer, SIGNAL(timeout()), this, SLOT(translate()));
            timer->start(500);
        } else { //forward manual move
            translate();
        }
    }

    //translate in backward direction
    if (event->key() == Qt::Key_Down) {
        //change move speed to translate in proper direction
        if (move_speed > 0)
            move_speed *= -1;

        if (automove) {
            //change to backward automovement
            connect(timer, SIGNAL(timeout()), this, SLOT(translate()));
            timer->start(500);
        } else { //backward manual move
            translate();
        }
    }

    //rotate to the left
    if (event->key() == Qt::Key_Left)
        rotateYaw(-rot_radians);

    //rotate to the right
    if (event->key() == Qt::Key_Right)
        rotateYaw(rot_radians);

    //toggle automove
    if (event->key() == Qt::Key_Space) {
        //turn off automovement
        if (automove) {
            automove = false;
            disconnect(timer, SIGNAL(timeout()), this, SLOT(translate()));
            timer->~QTimer();
        } else { //turn on autmovement
            //start moving the car forward automatically
            if (move_speed < 0)
                move_speed *= -1;

            automove = true;
            timer = new QTimer();
            connect(timer, SIGNAL(timeout()), this, SLOT(translate()));
            timer->start(500);
        }
    }

    //speed up the car
    if (event->key() == Qt::Key_F)
        move_speed *= 2;

    //slow down the car
    if (event->key() == Qt::Key_S)
        move_speed /= 2;

    //somersault
    if (event->key() == Qt::Key_M) {
        timer2 = new QTimer();
        connect(timer2, SIGNAL(timeout()), this, SLOT(rotatePitch()));
        timer2->start(500);
    }

}

//respond to mouse button presses for rotation
//see rotateYaw() function
void MyWidget::mousePressEvent(QMouseEvent * event) {

    // rotate to the left
    if (event->button() == Qt::LeftButton) {
        rotateYaw(-rot_radians);
    }

    //rotate to the right
    if (event->button() == Qt::RightButton){
        rotateYaw(rot_radians);
    }
}

//translates the world coordinates by move_speed units
void MyWidget::translate() {

    model = QMatrix4x4(1,0,0,0,
                       0,1,0,0,
                       0,0,1,-1.0f * move_speed,
                       0,0,0,1)
            * model;
    update();
}

//rotate the world coordinates by radians about the y-axis
void MyWidget::rotateYaw(float radians) {

    model = QMatrix4x4(qCos(radians),0,qSin(radians),0,
                       0,1,0,0,
                       -qSin(radians),0,qCos(radians),0,
                       0,0,0,1)
            * model;
    update();
}

//rotate the world coordinates by rot_radians around the camera's "x-axis"
void MyWidget::rotatePitch() {
    //rotating 72 times will rotate the somersault by 2pi radians (360 degrees)
    if (rot_count < 72) {
        model = QMatrix4x4(1,0,0,0,
                           0,qCos(rot_radians),-qSin(rot_radians),0,
                           0,qSin(rot_radians),qCos(rot_radians),0,
                           0,0,0,1)
                * model;
        update();
        rot_count++;
    }

    else { //end the somersault because we've reached 2pi radians rotation
        disconnect(timer2, SIGNAL(timeout()), this, SLOT(rotatePitch()));
        timer2->~QTimer();
        rot_count = 0;
    }
}
