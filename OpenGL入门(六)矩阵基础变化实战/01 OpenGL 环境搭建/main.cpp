#define GL_SILENCE_DEPRECATION
//demo演示了OpenGL背面剔除，深度测试，和多边形模式
#include "GLTools.h"	
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

////设置角色帧，作为相机
GLFrame             viewFrame;
//使用GLFrustum类来设置透视投影
GLFrustum           viewFrustum;
GLTriangleBatch     torusBatch;
GLMatrixStack       modelViewMatix;
GLMatrixStack       projectionMatrix;
GLGeometryTransform transformPipeline;
GLShaderManager     shaderManager;



// 召唤场景
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    //把摄像机矩阵压入模型矩阵中
    modelViewMatix.PushMatrix(viewFrame);
    
    GLfloat vRed[] = { 1.0f, 0.0f ,0.0f, 1.0f};
    
    //使用平面着色器
    //参数1：平面着色器
    //参数2：模型视图投影矩阵
    //参数3：颜色
    //shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    
    //使用默认光源着色器
    //通过光源、阴影效果跟提现立体效果
    //参数1：GLT_SHADER_DEFAULT_LIGHT 默认光源着色器
    //参数2：模型视图矩阵
    //参数3：投影矩阵
    //参数4：基本颜色值
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipeline.GetModelViewMatrix(),transformPipeline.GetProjectionMatrix(),vRed);
    
    //绘制
    torusBatch.Draw();
    //出栈
    modelViewMatix.PopMatrix();
    
    glutSwapBuffers();
    
    
}

// 这个函数不需要初始化渲染
// context. 图像上下文
void SetupRC()
{
    //设置背景颜色
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    
    //初始化c着色器管理器
    shaderManager.InitializeStockShaders();
    
    //将相机向后移动7个单元:肉眼到物体之间的距离
    viewFrame.MoveForward(7.0f);
    
    //创建一个甜甜圈
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
    
    //点的大小
    glPointSize(5.0f);
}

//键位设置，通过不同的键位对其进行设置
//控制Camera的移动，从而改变视口
void SpecialKeys(int key, int x, int y)
{
    if(key == GLUT_KEY_UP)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    //重新刷新window
    glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
    //防止h变为0
    if(h == 0)
        h = 1;
    
    //设置视口窗口尺寸
    glViewport(0, 0, w, h);
    
    //setPerspective函数的参数是一个从顶点方向看去的视场角度（用角度值表示）
    // 设置透视模式，初始化其透视矩阵
    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    
    //把透视矩阵加载到透视矩阵对阵中
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    // 初始化渲染管线
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}


int main(int argc, char* argv[])
{
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Geometry Test Program");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutDisplayFunc(RenderScene);
    
    glutAddMenuEntry("Toggle depth test",1);
    glutAddMenuEntry("Toggle cull backface",2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    return 0;
}
