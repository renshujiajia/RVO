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
	  glVertex3f(100.0f, -0.01f, 100.0f); //后面绘制是否都是根据这个坐标来的？
	  glVertex3f(100.0f, -0.01f, -100.0f);
	  glVertex3f(-100.0f, -0.01f, -100.0f);
	  glVertex3f(-100.0f, -0.01f, 100.0f);
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

//绘制指定ob中的建筑物 ----需要修改
void draw_bigbuilding(int index)
{
    RVO::Vector2 thepos;
    float tx,tz;
    //顶
    thepos = obs[index][0];
    tx = thepos.x();
    tz = thepos.y();

    glBegin(GL_QUADS);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz+200);
        glVertex3f(tx-60,15,tz+200);
        glVertex3f(tx-60,15,tz);
    glEnd();
    //右
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,0,tz+200);
        glVertex3f(tx,15,tz+200);
        glVertex3f(tx,15,tz);
    glEnd();
    //后
    thepos = obs[index][1];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx-60,0,tz);
        glVertex3f(tx-60,15,tz);
        glVertex3f(tx,15,tz);
    glEnd();

    //左
    thepos = obs[index][2];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz-200);
        glVertex3f(tx,0,tz-200);
    glEnd();

    //前
    thepos = obs[index][3];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx+60,0,tz);
        glVertex3f(tx+60,15,tz);
        glVertex3f(tx,15,tz);
    glEnd();
}

void draw_smallbuilding(int index)
{
    RVO::Vector2 thepos;
    float tx,tz;
    //顶
    thepos = obs[index][0];
    tx = thepos.x();
    tz = thepos.y();

    glBegin(GL_QUADS);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz+85);
        glVertex3f(tx-50,15,tz+85);
        glVertex3f(tx-50,15,tz);
    glEnd();
    //右
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,0,tz+85);
        glVertex3f(tx,15,tz+85);
        glVertex3f(tx,15,tz);
    glEnd();
    //后
    thepos = obs[index][1];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx-50,0,tz);
        glVertex3f(tx-50,15,tz);
        glVertex3f(tx,15,tz);
    glEnd();
    //左
    thepos = obs[index][2];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz-85);
        glVertex3f(tx,0,tz-85);
    glEnd();
    //前
    thepos = obs[index][3];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx+50,0,tz);
        glVertex3f(tx+50,15,tz);
        glVertex3f(tx,15,tz);
    glEnd();

}

//绘制障碍物，主要是障碍物这里我不懂得如何将障碍物添加进去，我目前只是画了一个没有实体的东西，而障碍物应当是具有实体的---需要修改
void draw_buildings(){
	//glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
    //我应该拿出ob找那个的建筑物，然后绘制建筑物，每个建筑物绘制5个面,
    //建筑物的颜色
    glColor3ub(105,105,105);
    draw_bigbuilding(0);
    draw_bigbuilding(1);
    draw_smallbuilding(2);
    draw_smallbuilding(3);
	glEnable(GL_LIGHTING);
}


void draw_obstacles()
{
    glDisable(GL_LIGHTING);
    size_t obstaclesNums = sim->getNumObstacleVertices();
    //建筑颜色
    glColor3ub(0,0,0);
    for(int i =0;i<obstaclesNums/4;i++)
    {
        glBegin(GL_QUADS);
            for(int j =0;j<4;j++)
            {
                RVO::Vector2 obstacles_pos = sim->getObstacleVertex(i*4+j);
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
    gluLookAt(10.0, 50.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //根据鼠标调整观察位置
    camera();


    //首先怎样绘制floor
    draw_floor();
    //画障碍
    draw_obstacles();

    //绘制建筑
    //draw_xybuilding(3);


    draw_buildings();
    //绘制agent
    //draw_agent();

    for(int i =0;i<agents;i++)
    {
        //绘制agents
        agent_pos = sim->getAgentPosition(i);
        if(i%2 ==0)
            glColor3f(1,0,0);
        if(i%2 ==1)
            glColor3f(0,1,0);
        //std::cout<<"agents:"<<i<<"-----"<<agent_pos.x()<<","<<agent_pos.y()<<std::endl;
        draw_agent(agent_pos.x(),agent_pos.y());
    }

    glutSwapBuffers();
}

//在这里构造场景障碍--需要修改
void makeStructure()
{
/* 障碍物 */
    //左
    ob.push_back(RVO::Vector2(-40.0f,-100.0f));
    ob.push_back(RVO::Vector2(-40.0f,100.0f));
    ob.push_back(RVO::Vector2(-100.0f,100.0f));
    ob.push_back(RVO::Vector2(-100.0f,-100.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //右
    ob.push_back(RVO::Vector2(100.0f,-100.0f));
    ob.push_back(RVO::Vector2(100.0f,100.0f));
    ob.push_back(RVO::Vector2(40.0f,100.0f));
    ob.push_back(RVO::Vector2(40.0f,-100.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //上
    ob.push_back(RVO::Vector2(10.0f,15.0f));
    ob.push_back(RVO::Vector2(10.0f,100.0f));
    ob.push_back(RVO::Vector2(-40.0f,100.0f));
    ob.push_back(RVO::Vector2(-40.0f,15.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //下
    ob.push_back(RVO::Vector2(40.0f,-100.0f));
    ob.push_back(RVO::Vector2(40.0f,-15.0f));
    ob.push_back(RVO::Vector2(-10.0f,-15.0f));
    ob.push_back(RVO::Vector2(-10.0f,-100.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
}

//在这里指定目标位置--需要替换
void generateGoals()
{
    //在这里设定三个位置为指定的终点
    v.position = RVO::Vector2(-25.0f,-100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(25.0f,100.0f);
    roadmap.push_back(v);
}

//在这里指定地图顶点位置--需要替换
void generateRoadMap()
{
    //主要
    v.position = RVO::Vector2(-12.0f,-100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-13.0f,-12.0f);
    //roadmap.push_back(v);
    v.position = RVO::Vector2(38.0f,-13.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(38.0f,100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(12.0f,100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(13.0f,12.0f);
    //roadmap.push_back(v);
    v.position = RVO::Vector2(-38.0f,13.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-38.0f,-100.0f);
    roadmap.push_back(v);

    //添加次要查看效果
    v.position = RVO::Vector2(-30.0f,0.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(30.0f,0.0f);
    roadmap.push_back(v);
}

//设置场景属性
void setupScenario(RVO::RVOSimulator *sim)
{
    //指定模拟的全局时间步长
    sim->setTimeStep(0.25f);
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

    for (size_t i = 0; i < 10; ++i) //2个地点，每个地点添加50个人
    {
			sim->addAgent(RVO::Vector2(-25.0f,-98.0f+rand()%100));
			goals.push_back(1);
			sim->addAgent(RVO::Vector2(25.0f,98.0f-rand()%100));
			goals.push_back(0);
	}
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

	for (int i = 0; i < 2; ++i) {
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
		if(dist < 10.0f) {
            //获取agent的位置，将agent移动到指定的位置（这里是他这个程序的想法）
			tempAgent = RVO::Vector2(tempAgent.x()*-1+rand()%10, tempAgent.y()*-1);
			sim->setAgentPosition(i, tempAgent);
		}
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
	glutCreateWindow("通道人群3D-test");

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
























