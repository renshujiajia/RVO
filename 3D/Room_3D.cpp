/*
 *在这个项目中实现毕业设计中所要求的几个场景
 *首先是最简单的人群过马路的场景
 *为了增加难度，我应该实现一个通过十字路口的场景
 */
#include<windows.h>
#include"GL/glut.h"
#include "../毕设依赖文件/rvo2-2.0.2/RVO2/src/RVO.h"
#include<vector>
#include<map>
#include<cmath>
#include <cstdlib>
#include<ctime>

#ifndef M_PI
static const float M_PI = 3.14159265358979323846f;
#endif
using namespace std;
/*
 *   几个需要使用的变量
 */



//画面坐标比例
float bili = 1.0/100.0;
//实例化sim
RVO::RVOSimulator * sim= new RVO::RVOSimulator();
//使用vector存储目标地goals的坐标
std::vector<int> goals;
//使用vector来存储障碍物的坐标
std::vector<RVO::Vector2> ob;
//用来存放斑马线坐标
std::vector<RVO::Vector2> banma;
//agents的个数
size_t agents;
//记录一个agent的坐标对象
RVO::Vector2 agent_pos;
//RoadMapVertex的结构体
class RoadMapVertex
{
public:
    RVO::Vector2 position;//表示一个顶点位置
    std::vector<int> neighbors;//邻居个数
    std::vector<float> distToGoal;//距终点距离
};
//存储路径图
std::vector<RoadMapVertex> roadmap;
//路线图对象
RoadMapVertex v;
//用来测试ob的个数
std::vector<vector<RVO::Vector2> > obs;


//初始化场景
void myinit()
{

    //glClearColor(0.0, 1.0, 0.0, 0.0);  //背景色
	// LIGHTING
    GLfloat lightpos[4] = { 1.0, 1.0, 1.0, 1.0 };     // light position
    GLfloat lightamb[4] = { 0.0, 0.0, 0.0, 1.0 };     // ambient colour
    GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };     // diffuse colour
    GLfloat global_ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);//环境光参数


    glEnable(GL_LIGHTING);//开关：使用光
    glEnable(GL_LIGHT0);//打开0#灯
    glEnable(GL_DEPTH_TEST);//打开深度测试

    glMatrixMode(GL_PROJECTION);//投影
    //视觉范围，显示界面比例，近处的截面，远处的截面
    gluPerspective(60.0, 800./600., 1.0, 5000.0);//?
	glMatrixMode(GL_MODELVIEW);//?

	glEnable ( GL_COLOR_MATERIAL ); //?
	glColorMaterial( GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE ); //设置材质
	glShadeModel(GL_SMOOTH);//多边形填充方式
}
//camera values
double xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0, angle=0.0;
int lastx, lasty;
double xrotrad, yrotrad;

// swiftless tutorial for camera - direct copy (see top comment )
void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-xpos,-ypos,-zpos); //translate the screen to the position of our camera
}

// swiftless tutorial for camera - direct copy (see top comment )
void mouseMovement(int x, int y) {
    int diffx=x-(int)lastx; //check the difference between the current x and the last x position
    int diffy=y-(int)lasty; //check the difference between the current y and the last y position
    lastx=x; //set lastx to the current x position
    lasty=y; //set lasty to the current y position
    xrot += (float) diffy; //set the xrot to xrot with the addition of the difference in the y position
    yrot += (float) diffx;    //set the xrot to yrot with the addition of the difference in the x position
}


//绘制地面
void draw_floor()
{
    glDisable(GL_LIGHTING);//暂时关闭灯源
	glColor3ub(160,82,45);
    //绘制一个四边形
	glBegin(GL_QUADS);   //这个位置是如何确定的。
	  glVertex3f(150.0f, -0.01f, 150.0f); //后面绘制是否都是根据这个坐标来的？
	  glVertex3f(150.0f, -0.01f, -150.0f);
	  glVertex3f(-150.0f, -0.01f, -150.0f);
	  glVertex3f(-150.0f, -0.01f, 150.0f);
	glEnd();
	glEnable(GL_LIGHTING);//打开灯源

}
//绘制agent,参数为指定的坐标位置
//绘制agent
void draw_agent(float x,float y)
{
    //我再这里尝试绘制我所制定的位置的agent--绘制球体
	glPushMatrix(); //首先转移到对应的位置
			glTranslatef(x, 0.0f, y);
			glutSolidSphere(1, 5, 5);//画一个球体
			glColor4f(1, .5, .5, 1); // 决定头的颜色
			glTranslatef(0.0f, 0.5f, 0.0f);//头的位置
			glutSolidSphere(.5, 5, 5);//画小球
	glPopMatrix();
}
//写一个函数
//传入参数为四个点坐标
void draw_aob(RVO::Vector2 pos1,RVO::Vector2 pos2,RVO::Vector2 pos3,RVO::Vector2 pos4)
{
    //求出中心的坐标
    RVO::Vector2 centor = RVO::Vector2((pos1.x()+pos3.x())/2.0,(pos1.y()+pos3.y())/2.0);
    //求出方块的x差值和y差值
    float x0 = centor.x();float z0 = centor.y(); //中心
    float dx = abs(pos1.x()-pos3.x())/2; //x变量值
    float dz = abs(pos1.y()-pos3.y())/2; //z变量值
    //根据中心值重构出一个立方体的坐标，并根据坐标绘制立方体
    //上
    glBegin(GL_QUADS);
        glVertex3f(x0-dx,15,z0+dz);
        glVertex3f(x0-dx,15,z0-dz);
        glVertex3f(x0+dx,15,z0-dz);
        glVertex3f(x0+dx,15,z0+dz);
    glEnd();
    //左
    glBegin(GL_QUADS);
        glVertex3f(x0-dx,0,z0+dz);
        glVertex3f(x0-dx,0,z0-dz);
        glVertex3f(x0-dx,15,z0-dz);
        glVertex3f(x0-dx,15,z0+dz);
    glEnd();
    //后
    glBegin(GL_QUADS);

        glVertex3f(x0-dx,0,z0-dz);
        glVertex3f(x0+dx,0,z0-dz);
        glVertex3f(x0+dx,15,z0-dz);
        glVertex3f(x0-dx,15,z0-dz);
    glEnd();
    //右
    glBegin(GL_QUADS);
        glVertex3f(x0+dx,0,z0-dz);
        glVertex3f(x0+dx,0,z0+dz);
        glVertex3f(x0+dx,15,z0+dz);
        glVertex3f(x0+dx,15,z0-dz);
    glEnd();
    //前
    glBegin(GL_QUADS);
        glVertex3f(x0+dx,0,z0+dz);
        glVertex3f(x0-dx,0,z0+dz);
        glVertex3f(x0-dx,15,z0+dz);
        glVertex3f(x0+dx,15,z0+dz);
    glEnd();
}


//绘制障碍物，主要是障碍物这里我不懂得如何将障碍物添加进去，我目前只是画了一个没有实体的东西，而障碍物应当是具有实体的
void draw_buildings(){
	//glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
    //我应该拿出ob找那个的建筑物，然后绘制建筑物，每个建筑物绘制5个面,
    //建筑物的颜色
    size_t obstaclesNums = obs.size();
    glColor3ub(95,158,160);
    for(int i =0;i<obstaclesNums;i++)
    {
        draw_aob(obs[i][0],obs[i][1],obs[i][2],obs[i][3]);
    }
	glEnable(GL_LIGHTING);
}



void draw_obstacles()
{
    glDisable(GL_LIGHTING);
    size_t obstaclesNums = obs.size();
    //建筑颜色
    glColor3ub(0,0,0);
    for(int i =0;i<obstaclesNums;i++)
    {
        glBegin(GL_QUADS);
            for(int j =0;j<4;j++)
            {
                RVO::Vector2 obstacles_pos = obs[i][j];
                glVertex3f(obstacles_pos.x(),0.1,obstacles_pos.y());
            }
        glEnd();
    }
    glEnable(GL_LIGHTING);

}

//场景绘制函数
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    gluLookAt(10.0, 200.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //根据鼠标调整观察位置
    camera();


    //首先怎样绘制floor
    draw_floor();
    //画障碍
    draw_obstacles();
    //画斑马线
    //draw_aob(obs[0][0],obs[0][1],obs[0][2],obs[0][3]);

    //绘制建筑
    //draw_xybuilding(3);


    draw_buildings();
    //绘制agent
    //draw_agent();

#pragma omp parallel for
    for(int i =0;i<agents;i++)
    {
        //绘制agents
        agent_pos = sim->getAgentPosition(i);
        if(i%7 ==0)
            glColor3f(0,0,0);
        if(i%7 ==1)
            glColor3f(0,0,1);
        if(i%7 ==2)
            glColor3f(0,1,0);
        if(i%7 ==3)
            glColor3f(0,1,1);
        if(i%7 ==4)
            glColor3f(0,0,0);
        if(i%7 ==5)
            glColor3f(0,0,1);
        if(i%7 ==6)
            glColor3f(0,1,0);
        //std::cout<<"agents:"<<i<<"-----"<<agent_pos.x()<<","<<agent_pos.y()<<std::endl;
        draw_agent(agent_pos.x(),agent_pos.y());
    }

    glutSwapBuffers();
}

//在这里构造场景障碍
void makeStructure()
{
/* 障碍物 */
    //1
    ob.clear();
    ob.push_back(RVO::Vector2(-78.0f,50.0f));
    ob.push_back(RVO::Vector2(-82.0f,50.0f));
    ob.push_back(RVO::Vector2(-82.0f,-70.0f));
    ob.push_back(RVO::Vector2(-78.0f,-70.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //2
    ob.push_back(RVO::Vector2(-100.0f,2.0f));
    ob.push_back(RVO::Vector2(-100.0f,-2.0f));
    ob.push_back(RVO::Vector2(-20.0f,-2.0f));
    ob.push_back(RVO::Vector2(-20.0f,2.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //3
    ob.push_back(RVO::Vector2(-100.0f,2.0f));
    ob.push_back(RVO::Vector2(-96.0f,2.0f));
    ob.push_back(RVO::Vector2(-96.0f,72.0f));
    ob.push_back(RVO::Vector2(-100.0f,72.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //4
    ob.push_back(RVO::Vector2(-96.0f,72.0f));
    ob.push_back(RVO::Vector2(-96.0f,68.0f));
    ob.push_back(RVO::Vector2(-18.0f,68.0f));
    ob.push_back(RVO::Vector2(-18.0f,72.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //14
    ob.push_back(RVO::Vector2(-22.0f,20.0f));
    ob.push_back(RVO::Vector2(-18.0f,20.0f));
    ob.push_back(RVO::Vector2(-18.0f,72.0f));
    ob.push_back(RVO::Vector2(-22.0f,72.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //5
    ob.push_back(RVO::Vector2(-82.0f,-74.0f));
    ob.push_back(RVO::Vector2(100.0f,-74.0f));
    ob.push_back(RVO::Vector2(100.0f,-70.0f));
    ob.push_back(RVO::Vector2(-82.0f,-70.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //6
    ob.push_back(RVO::Vector2(-22.0f,-70.0f));
    ob.push_back(RVO::Vector2(-18.0f,-70.0f));
    ob.push_back(RVO::Vector2(-18.0f,-18.0f));
    ob.push_back(RVO::Vector2(-22.0f,-18.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
     //7
    ob.push_back(RVO::Vector2(28.0f,-70.0f));
    ob.push_back(RVO::Vector2(32.0f,-70.0f));
    ob.push_back(RVO::Vector2(32.0f,-18.0f));
    ob.push_back(RVO::Vector2(28.0f,-18.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //8
    ob.push_back(RVO::Vector2(78.0f,-70.0f));
    ob.push_back(RVO::Vector2(82.0f,-70.0f));
    ob.push_back(RVO::Vector2(82.0f,-40.0f));
    ob.push_back(RVO::Vector2(78.0f,-40.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //9
    ob.push_back(RVO::Vector2(96.0f,-70.0f));
    ob.push_back(RVO::Vector2(100.0f,-70.0f));
    ob.push_back(RVO::Vector2(100.0f,2.0f));
    ob.push_back(RVO::Vector2(96.0f,2.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //10
     ob.push_back(RVO::Vector2(-22.0f,-18.0f));
    ob.push_back(RVO::Vector2(-22.0f,-22.0f));
    ob.push_back(RVO::Vector2(10.0f,-22.0f));
    ob.push_back(RVO::Vector2(10.0f,-18.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //11
    ob.push_back(RVO::Vector2(50.0f,-18.0f));
    ob.push_back(RVO::Vector2(50.0f,-22.0f));
    ob.push_back(RVO::Vector2(78.0f,-22.0f));
    ob.push_back(RVO::Vector2(78.0f,-18.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //12
    ob.push_back(RVO::Vector2(100.0f,-2.0f));
    ob.push_back(RVO::Vector2(100.0f,2.0f));
    ob.push_back(RVO::Vector2(82.0f,2.0f));
    ob.push_back(RVO::Vector2(82.0f,-2.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //15
    ob.push_back(RVO::Vector2(20.0f,72.0f));
    ob.push_back(RVO::Vector2(20.0f,68.0f));
    ob.push_back(RVO::Vector2(82.0f,68.0f));
    ob.push_back(RVO::Vector2(82.0f,72.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //16
    ob.push_back(RVO::Vector2(82.0f,68.0f));
    ob.push_back(RVO::Vector2(78.0f,68.0f));
    ob.push_back(RVO::Vector2(78.0f,-22.0f));
    ob.push_back(RVO::Vector2(82.0f,-22.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //17
    ob.push_back(RVO::Vector2(-100.0f,-2.0f));
    ob.push_back(RVO::Vector2(-100.0f,2.0f));
    ob.push_back(RVO::Vector2(-80.0f,2.0f));
    ob.push_back(RVO::Vector2(-80.0f,-2.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //18
    ob.push_back(RVO::Vector2(-22.0f,-18.0f));
    ob.push_back(RVO::Vector2(-22.0f,-22.0f));
    ob.push_back(RVO::Vector2(10.0f,-22.0f));
    ob.push_back(RVO::Vector2(10.0f,-18.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
}

//在这里指定目标位置
void generateGoals()
{
    v.position = RVO::Vector2(-40.0f,130.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(40.0f,130.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-130.0f,100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(130.0f,100.0f);
    roadmap.push_back(v);
}

//在这里指定地图顶点位置
void generateRoadMap()
{
    //1
    v.position = RVO::Vector2(-97.0f,3.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-85.0f,3.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-85.0f,55.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-97.0f,63.0f);
    roadmap.push_back(v);
    //2
    v.position = RVO::Vector2(-75.0f,5.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-15.0f,5.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-25.0f,15.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-25.0f,63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-75.0f,63.0f);
    roadmap.push_back(v);
    //3
    v.position = RVO::Vector2(-15.0f,15.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-15.0f,63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(65.0f,63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(15.0f,63.0f);
    roadmap.push_back(v);
    //4
    v.position = RVO::Vector2(-75.0f,-5.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-75.0f,-65.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-25.0f,-65.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-25.0f,-15.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(15.0f,-15.0f);
    roadmap.push_back(v);
    //5
    v.position = RVO::Vector2(15.0f,-25.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-15.0f,-25.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-15.0f,-63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(25.0f,-63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(25.0f,-15.0f);
    roadmap.push_back(v);
    //6
    v.position = RVO::Vector2(35.0f,-15.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(35.0f,-63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(75.0f,-63.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(75.0f,-35.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(45.0f,-25.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-25.0f,-15.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(75.0f,-15.0f);
    roadmap.push_back(v);
    //7
    v.position = RVO::Vector2(85.0f,-25.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(85.0f,-5.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(93.0f,-5.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(93.0f,-65.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(85.0f,-65.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(85.0f,-35.0f);
    roadmap.push_back(v);
       //外部添加两个
    v.position = RVO::Vector2(-10.0f,80.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(10.0f,80.0f);
    roadmap.push_back(v);
    //再添加两个用来进行后续导航
    v.position = RVO::Vector2(-80.0f,100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(80.0f,100.0f);
    roadmap.push_back(v);

}

//设置场景属性
void setupScenario(RVO::RVOSimulator *sim)
{
    //指定模拟的全局时间步长
    sim->setTimeStep(0.5f);
    //添加障碍物
    makeStructure();
    //加载障碍物
    sim->processObstacles();
    /* 路线图 */
    RoadMapVertex v;
    //添加agent的终点坐标
    generateGoals();
    //添加障碍物周围的路线图顶点
    generateRoadMap();

    //指定agent的默认参数(邻居距离，邻居个数，时间地平线，时间平线布斯特，半径，最大速度，康斯特矢量）
    //sim->setAgentDefaults(200.0f, 3, 10.0f, 10.0f, 3.0f, 2.0f);
    sim->setAgentDefaults(10.0f, 10, 5.0f, 10.0f, 2.0f, 2.0f);
    //添加agent，指定初始位置，并存储终点为环境中对称位置
#pragma omp parallel for
    for (size_t i = 0; i < 50; ++i) //三个地点，每个地点添加5个人
    {
            //1
			sim->addAgent(RVO::Vector2(-90.0f,35.0f));
			goals.push_back(0);
        	//2
        	sim->addAgent(RVO::Vector2(-50.0f,35.0f));
			goals.push_back(1);
        	//3
        	sim->addAgent(RVO::Vector2(40.0f,25.0f));
			goals.push_back(2);
        	//4
        	sim->addAgent(RVO::Vector2(-50.0f,-35.0f));
			goals.push_back(3);
            //5
            sim->addAgent(RVO::Vector2(0.0f,-45.0f));
			goals.push_back(0);
        	//6
        	sim->addAgent(RVO::Vector2(55.0f,-45.0f));
			goals.push_back(1);
        	//7
        	sim->addAgent(RVO::Vector2(90.0f,-35.0f));
			goals.push_back(2);

	}//
}

//创建底层路线图指导路径规划--need change
void buildRoadmap(RVO::RVOSimulator *sim)
{
	/* Connect the roadmap vertices by edges if mutually visible. 如果互相可见，则按边连接路线图顶点*/
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(roadmap.size()); ++i) {
		for (int j = 0; j < static_cast<int>(roadmap.size()); ++j) {
			if (sim->queryVisibility(roadmap[i].position, roadmap[j].position, sim->getAgentRadius(0))) {
				roadmap[i].neighbors.push_back(j);
			}
		}

		/*
		 * Initialize the distance to each of the four goal vertices at infinity
		 * (9e9f).
		 */
		roadmap[i].distToGoal.resize(4, 9e9f);
	}

	/*
	 * Compute the distance to each of the four goals (the first four vertices)
	 * for all vertices using Dijkstra's algorithm.
	 */

#pragma omp parallel for

	for (int i = 0; i < 5; ++i) {
		std::multimap<float, int> Q;
		std::vector<std::multimap<float, int>::iterator> posInQ(roadmap.size(), Q.end());

		roadmap[i].distToGoal[i] = 0.0f;
		posInQ[i] = Q.insert(std::make_pair(0.0f, i));

		while (!Q.empty()) {
			const int u = Q.begin()->second;
			Q.erase(Q.begin());
			posInQ[u] = Q.end();

			for (int j = 0; j < static_cast<int>(roadmap[u].neighbors.size()); ++j) {
				const int v = roadmap[u].neighbors[j];
				const float dist_uv = RVO::abs(roadmap[v].position - roadmap[u].position);

				if (roadmap[v].distToGoal[i] > roadmap[u].distToGoal[i] + dist_uv) {
					roadmap[v].distToGoal[i] = roadmap[u].distToGoal[i] + dist_uv;

					if (posInQ[v] == Q.end()) {
						posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
					}
					else {
						Q.erase(posInQ[v]);
						posInQ[v] = Q.insert(std::make_pair(roadmap[v].distToGoal[i], v));
					}
				}
			}
		}
	}
}


//设置首选速度--nochange
void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the visible roadmap vertex that is on the shortest path to the
	 * goal.将首选速度设置为单位大小(速度)的矢量，指向目标的最短路径上的可见路线图顶点的方向
	 */

#pragma omp parallel for

	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i) {
		float minDist = 9e9f;
		int minVertex = -1;

		for (int j = 0; j < static_cast<int>(roadmap.size()); ++j) {
			if (RVO::abs(roadmap[j].position - sim->getAgentPosition(i)) + roadmap[j].distToGoal[goals[i]] < minDist &&
				sim->queryVisibility(sim->getAgentPosition(i), roadmap[j].position, sim->getAgentRadius(i))) {

				minDist = RVO::abs(roadmap[j].position - sim->getAgentPosition(i)) + roadmap[j].distToGoal[goals[i]];
				minVertex = j;
			}
		}

		if (minVertex == -1) {
			/* No roadmap vertex is visible; should not happen. */
			sim->setAgentPrefVelocity(i, RVO::Vector2(0, 0));
		}
		else {
			if (RVO::absSq(roadmap[minVertex].position -
			               sim->getAgentPosition(i)) == 0.0f) {
				if (minVertex == goals[i]) {
					sim->setAgentPrefVelocity(i, RVO::Vector2());
				}
				else {
					sim->setAgentPrefVelocity(i, RVO::normalize(roadmap[goals[i]].position - sim->getAgentPosition(i)));
				}
			}
			else {
				sim->setAgentPrefVelocity(i, RVO::normalize(roadmap[minVertex].position - sim->getAgentPosition(i)));
			}
		}

		/*
		 * Perturb a little to avoid deadlocks due to perfect symmetry.
		 */
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;

		sim->setAgentPrefVelocity(i, sim->getAgentPrefVelocity(i) +
		                          dist * RVO::Vector2(std::cos(angle), std::sin(angle)));
	}
}

//检测是否满足到达的条件-nochange
void checkGoals(){
    //先创建pagent的以及目标的vector2类型位置对象
	RVO::Vector2 tempAgent, tempGoal;
	//浮点类型距离
	float dist;

	//遍历agents
	for( size_t i = 0; i < sim->getNumAgents(); i++) {
		//获取每个agent和对应目标的位置对象
		tempAgent = sim->getAgentPosition(i);
		tempGoal = roadmap[goals[i]].position;

		//求出距离
		dist = sqrt( (tempGoal.x()-tempAgent.x())*(tempGoal.x()-tempAgent.x()) + (tempGoal.y()-tempAgent.y())*(tempGoal.y()-tempAgent.y()) );

		//如果到达了目的地，
		// if goal is reached, warp agent to oposite side (maybe consider randomizing this
		/*if(dist < 1.5f) {
            //获取agent的位置，将agent移动到指定的位置（这里是他这个程序的想法）
			tempAgent = RVO::Vector2(tempAgent.x()*-1, tempAgent.y()*-1);
			sim->setAgentPosition(i, tempAgent);
		}*/
	}
}

//写出回调函数--need change
void myIdle()
{
    //设置首选速度
    setPreferredVelocities(sim);
    //模拟一步，坐标发生了改变
    sim->doStep();
    checkGoals();
    //再次调用绘制函数重绘
    glutPostRedisplay();
}

int main()
{
    //设置模拟场景的参数
    setupScenario(sim);
    //获取agents的个数
    agents = sim->getNumAgents();
    //初始化底层路径图
    buildRoadmap(sim);

    //初始化opengl
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,1000);
	glutCreateWindow("人群3D-test");

    //初始化画面
    myinit();
    glutPassiveMotionFunc(mouseMovement);
    //指定绘制函数
    glutDisplayFunc(display);
    //调用回调函数
    glutIdleFunc(myIdle);
    glutMainLoop();

    delete sim;
    return 0;
}
