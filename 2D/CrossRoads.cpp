/*
 *在这个项目中实现毕业设计中所要求的几个场景
 *首先是最简单的人群过马路的场景
 *为了增加难度，我应该实现一个通过十字路口的场景
 */
#include<windows.h>
#include"GL/glut.h"
#include"../毕设依赖文件/rvo2-2.0.2/RVO2/src/RVO.h"

#include<vector>
#include<map>
#include<cmath>
#include <cstdlib>
#include<ctime>
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



//初始化场景
void myinit()
{
    glClearColor(0,0.250,0.154,1); //这句话设置了背景的颜色。
}
//绘制坐标轴
void draw_xy()
{
    glPointSize(5);
    glColor3f(0,0,0);
    glBegin(GL_LINES);
        glVertex2f(-1,0);
        glVertex2f(1,0);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(0,1);
        glVertex2f(0,-1);
    glEnd();

    glPointSize(10);
    glBegin(GL_POINTS);
        glVertex2f(0,0);
    glEnd();
}

//绘制agent,参数为指定的坐标位置
void draw_agent(float x,float y)
{
    x = x*bili;
    y = y*bili;
    //绘制点
    glBegin(GL_POINTS);
        //glColor3f(1,0,0);//红色
        glVertex2f(x,y);
    glEnd();
}

//绘制障碍物，主要是障碍物这里我不懂得如何将障碍物添加进去，我目前只是画了一个没有实体的东西，而障碍物应当是具有实体的
void draw_obstacles()
{
    size_t obstaclesNums = sim->getNumObstacleVertices();
    for(int i =0;i<obstaclesNums/4;i++)
    {
        glBegin(GL_QUADS);
            for(int j =0;j<4;j++)
            {
                RVO::Vector2 obstacles_pos = sim->getObstacleVertex(i*4+j);
                glVertex2f(obstacles_pos.x()*bili,obstacles_pos.y()*bili);
            }
        glEnd();
    }
}


//TODO:绘制斑马线
void drawRoadWhite()
{
    //斑马线一个四个不同位置
    //上
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
        glVertex2f(20*bili,20*bili);
        glVertex2f(20*bili,35*bili);
        glVertex2f(-20*bili,35*bili);
        glVertex2f(-20*bili,20*bili);
    glEnd();
    //左
    glBegin(GL_QUADS);
        glVertex2f(-20*bili,20*bili);
        glVertex2f(-35*bili,20*bili);
        glVertex2f(-35*bili,-20*bili);
        glVertex2f(-20*bili,-20*bili);
    glEnd();
    //下
    glBegin(GL_QUADS);
        glVertex2f(20*bili,-20*bili);
        glVertex2f(-20*bili,-20*bili);
        glVertex2f(-20*bili,-35*bili);
        glVertex2f(20*bili,-35*bili);
    glEnd();
    //右
    glBegin(GL_QUADS);
        glVertex2f(35*bili,20*bili);
        glVertex2f(20*bili,20*bili);
        glVertex2f(20*bili,-20*bili);
        glVertex2f(35*bili,-20*bili);
    glEnd();
}

//场景绘制函数
void display()
{

    //前期设置放在display当中
    //清屏
	glClear(GL_COLOR_BUFFER_BIT);

    //绘制坐标轴，帮助观察。
    //draw_xy();

    //获取坐标并绘制障碍物
    glColor3f(0,0.197,0.205);
    draw_obstacles();//这样只是简单的画出来，并没有体现出是障碍物啊。
    //绘制斑马线
    drawRoadWhite();
    //获取坐标并绘制每个agents
    glPointSize(15);
    for(int i =0;i<agents;i++)
    {
        //绘制agents
        agent_pos = sim->getAgentPosition(i);
        if(i%3 ==0)
            glColor3f(1,0,0);
        if(i%3 ==1)
            glColor3f(0,1,0);
        if(i%3 ==2)
            glColor3f(0,0,1);
        //std::cout<<"agents:"<<i<<"-----"<<agent_pos.x()<<","<<agent_pos.y()<<std::endl;
        draw_agent(agent_pos.x(),agent_pos.y());
    }
    //最后交换缓冲
    glutSwapBuffers();
}
//在这里构造场景障碍
void makeStructure()
{
/* 障碍物 */
    //中
    ob.push_back(RVO::Vector2(20.0f,20.0f));
    ob.push_back(RVO::Vector2(-20.0f,20.0f));
    ob.push_back(RVO::Vector2(-20.0f,-20.0f));
    ob.push_back(RVO::Vector2(20.0f,-20.0f));
    sim->addObstacle(ob);
    ob.clear();
    //上
    ob.push_back(RVO::Vector2(20.0f,35.0f));
    ob.push_back(RVO::Vector2(20.0f,100.0f));
    ob.push_back(RVO::Vector2(-20.0f,100.f));
    ob.push_back(RVO::Vector2(-20.0f,35.0f));
    sim->addObstacle(ob);
    ob.clear();
    //下
    ob.push_back(RVO::Vector2(20.0f,-35.0f));
    ob.push_back(RVO::Vector2(-20.0f,-35.0f));
    ob.push_back(RVO::Vector2(-20.0f,-100.0f));
    ob.push_back(RVO::Vector2(20.0f,-100.0f));
    sim->addObstacle(ob);
    ob.clear();
    //左上
    ob.push_back(RVO::Vector2(-35.0f,35.0f));
    ob.push_back(RVO::Vector2(-35.0f,100.0f));
    ob.push_back(RVO::Vector2(-100.0f,100.0f));
    ob.push_back(RVO::Vector2(-100.0f,35.0f));
    sim->addObstacle(ob);
    ob.clear();
    //左中
    ob.push_back(RVO::Vector2(-35.0f,-20.0f));
    ob.push_back(RVO::Vector2(-35.0f,20.0f));
    ob.push_back(RVO::Vector2(-100.0f,20.0f));
    ob.push_back(RVO::Vector2(-100.0f,-20.0f));
    sim->addObstacle(ob);
    ob.clear();
    //左下
    ob.push_back(RVO::Vector2(-35.0f,-35.0f));
    ob.push_back(RVO::Vector2(-100.0f,-35.0f));
    ob.push_back(RVO::Vector2(-100.0f,-100.0f));
    ob.push_back(RVO::Vector2(-35.0f,-100.0f));
    sim->addObstacle(ob);
    ob.clear();
    //右上
    ob.push_back(RVO::Vector2(35.0f,35.0f));
    ob.push_back(RVO::Vector2(100.0f,35.0f));
    ob.push_back(RVO::Vector2(100.0f,100.0f));
    ob.push_back(RVO::Vector2(35.0f,100.0f));
    sim->addObstacle(ob);
    ob.clear();
    //右中
    ob.push_back(RVO::Vector2(35.0f,20.0f));
    ob.push_back(RVO::Vector2(35.0f,-20.0f));
    ob.push_back(RVO::Vector2(100.0f,-20.0f));
    ob.push_back(RVO::Vector2(100.0f,20.0f));
    sim->addObstacle(ob);
    ob.clear();
    //右下
    ob.push_back(RVO::Vector2(35.0f,-35.0f));
    ob.push_back(RVO::Vector2(35.0f,-100.0f));
    ob.push_back(RVO::Vector2(100.0f,-100.0f));
    ob.push_back(RVO::Vector2(100.0f,-35.0f));
    sim->addObstacle(ob);
    ob.clear();
}

//在这里指定目标位置
void generateGoals()
{
    //在这里设定三个位置为指定的终点
    v.position = RVO::Vector2(90.0f,-30.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-30.0f,-90.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(30.0f,90.0f);
    roadmap.push_back(v);
}

//在这里指定地图顶点位置
void generateRoadMap()
{
    //左上
    v.position = RVO::Vector2(-33.0f,98.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-33.0f,33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-98.0f,33.0f);
    roadmap.push_back(v);
    //左
    v.position = RVO::Vector2(-98.0f,22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-33.0f,22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-33.0f,-22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-98.0f,-22.0f);
    roadmap.push_back(v);
    //左下
    v.position = RVO::Vector2(-98.0f,-33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-33.0f,-33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-33.0f,-98.0f);
    roadmap.push_back(v);
    //上
    v.position = RVO::Vector2(22.0f,98.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(22.0f,33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-22.0f,33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-22.0f,98.0f);
    roadmap.push_back(v);
    //中
    v.position = RVO::Vector2(22.0f,22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-22.0f,22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-22.0f,-22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(22.0f,-22.0f);
    roadmap.push_back(v);
    //下
    v.position = RVO::Vector2(22.0f,-98.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(22.0f,-33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-22.0f,-33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(-22.0f,-98.0f);
    roadmap.push_back(v);
    //右上
    v.position = RVO::Vector2(33.0f,98.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(33.0f,33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(98.0f,33.0f);
    roadmap.push_back(v);
    //右
    v.position = RVO::Vector2(98.0f,22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(33.0f,22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(33.0f,-22.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(98.0f,-22.0f);
    roadmap.push_back(v);
    //右下
    v.position = RVO::Vector2(98.0f,-33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(33.0f,-33.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(33.0f,-98.0f);
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
    sim->setAgentDefaults(10.0f, 10, 5.0f, 5.0f, 2.0f, 2.0f);
    //添加agent，指定初始位置，并存储终点为环境中对称位置
    for (size_t i = 0; i < 30; ++i) //三个地点，每个地点添加5个人
    {
			sim->addAgent(RVO::Vector2(-30.0f,90.0f));
			goals.push_back(std::rand()%3);//0号终点？

			sim->addAgent(RVO::Vector2(90.0f,30.0f));
			goals.push_back(std::rand()%3);//1号终点

			sim->addAgent(RVO::Vector2(-90.0f,-30.0f));
			goals.push_back(std::rand()%3);
	}
}
//创建底层路线图指导路径规划
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

	for (int i = 0; i < 3; ++i) {
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
//设置首选速度
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
//检测是否满足到达的条件（一旦靠近指定范围，那就可以让他的坐标就是当前的位置了！）
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
//写出回调函数
void myIdle()
{
    //设置首选速度
    //setPreferredVolicaties();
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
	glutCreateWindow("十字路口人行道-test");

    //初始化画面
    myinit();
    //指定绘制函数
    glutDisplayFunc(display);
    //调用回调函数
    glutIdleFunc(myIdle);
	
    glutMainLoop();
    delete sim;
    return 0;
}
























