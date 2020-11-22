/*
 *�������Ŀ��ʵ�ֱ�ҵ�������Ҫ��ļ�������
 *��������򵥵���Ⱥ����·�ĳ���
 *Ϊ�������Ѷȣ���Ӧ��ʵ��һ��ͨ��ʮ��·�ڵĳ���
 */
#include<windows.h>
#include"GL/glut.h"
#include "../���������ļ�/rvo2-2.0.2/RVO2/src/RVO.h"
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
 *   ������Ҫʹ�õı���
 */



//�����������
float bili = 1.0/100.0;
//ʵ����sim
RVO::RVOSimulator * sim= new RVO::RVOSimulator();
//ʹ��vector�洢Ŀ���goals������
std::vector<int> goals;
//ʹ��vector���洢�ϰ��������
std::vector<RVO::Vector2> ob;
//������Ű���������
std::vector<RVO::Vector2> banma;
//agents�ĸ���
size_t agents;
//��¼һ��agent���������
RVO::Vector2 agent_pos;
//RoadMapVertex�Ľṹ��
class RoadMapVertex
{
public:
    RVO::Vector2 position;//��ʾһ������λ��
    std::vector<int> neighbors;//�ھӸ���
    std::vector<float> distToGoal;//���յ����
};
//�洢·��ͼ
std::vector<RoadMapVertex> roadmap;
//·��ͼ����
RoadMapVertex v;
//��������ob�ĸ���
std::vector<vector<RVO::Vector2> > obs;


//��ʼ������
void myinit()
{

    //glClearColor(0.0, 1.0, 0.0, 0.0);  //����ɫ
	// LIGHTING
    GLfloat lightpos[4] = { 1.0, 1.0, 1.0, 1.0 };     // light position
    GLfloat lightamb[4] = { 0.0, 0.0, 0.0, 1.0 };     // ambient colour
    GLfloat lightdif[4] = { 1.0, 1.0, 1.0, 1.0 };     // diffuse colour
    GLfloat global_ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);//���������


    glEnable(GL_LIGHTING);//���أ�ʹ�ù�
    glEnable(GL_LIGHT0);//��0#��
    glEnable(GL_DEPTH_TEST);//����Ȳ���

    glMatrixMode(GL_PROJECTION);//ͶӰ
    //�Ӿ���Χ����ʾ��������������Ľ��棬Զ���Ľ���
    gluPerspective(60.0, 800./600., 1.0, 5000.0);//?
	glMatrixMode(GL_MODELVIEW);//?

	glEnable ( GL_COLOR_MATERIAL ); //?
	glColorMaterial( GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE ); //���ò���
	glShadeModel(GL_SMOOTH);//�������䷽ʽ
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


//���Ƶ���
void draw_floor()
{
    glDisable(GL_LIGHTING);//��ʱ�رյ�Դ
	glColor3ub(160,82,45);
    //����һ���ı���
	glBegin(GL_QUADS);   //���λ�������ȷ���ġ�
	  glVertex3f(100.0f, -0.01f, 100.0f); //��������Ƿ��Ǹ�������������ģ�
	  glVertex3f(100.0f, -0.01f, -100.0f);
	  glVertex3f(-100.0f, -0.01f, -100.0f);
	  glVertex3f(-100.0f, -0.01f, 100.0f);
	glEnd();
	glEnable(GL_LIGHTING);//�򿪵�Դ

}
//����agent,����Ϊָ��������λ��
//����agent
void draw_agent(float x,float y)
{
    //�������ﳢ�Ի��������ƶ���λ�õ�agent--��������
	glPushMatrix(); //����ת�Ƶ���Ӧ��λ��
			glTranslatef(x, 0.0f, y);
			glutSolidSphere(1, 5, 5);//��һ������
			glColor4f(1, .5, .5, 1); // ����ͷ����ɫ
			glTranslatef(0.0f, 0.5f, 0.0f);//ͷ��λ��
			glutSolidSphere(.5, 5, 5);//��С��
	glPopMatrix();
}

//����ָ��ob�еĽ����� ----��Ҫ�޸�
void draw_bigbuilding(int index)
{
    RVO::Vector2 thepos;
    float tx,tz;
    //��
    thepos = obs[index][0];
    tx = thepos.x();
    tz = thepos.y();

    glBegin(GL_QUADS);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz+200);
        glVertex3f(tx-60,15,tz+200);
        glVertex3f(tx-60,15,tz);
    glEnd();
    //��
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,0,tz+200);
        glVertex3f(tx,15,tz+200);
        glVertex3f(tx,15,tz);
    glEnd();
    //��
    thepos = obs[index][1];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx-60,0,tz);
        glVertex3f(tx-60,15,tz);
        glVertex3f(tx,15,tz);
    glEnd();

    //��
    thepos = obs[index][2];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz-200);
        glVertex3f(tx,0,tz-200);
    glEnd();

    //ǰ
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
    //��
    thepos = obs[index][0];
    tx = thepos.x();
    tz = thepos.y();

    glBegin(GL_QUADS);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz+85);
        glVertex3f(tx-50,15,tz+85);
        glVertex3f(tx-50,15,tz);
    glEnd();
    //��
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,0,tz+85);
        glVertex3f(tx,15,tz+85);
        glVertex3f(tx,15,tz);
    glEnd();
    //��
    thepos = obs[index][1];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx-50,0,tz);
        glVertex3f(tx-50,15,tz);
        glVertex3f(tx,15,tz);
    glEnd();
    //��
    thepos = obs[index][2];
    tx = thepos.x();
    tz = thepos.y();
    glBegin(GL_QUADS);
        glVertex3f(tx,0,tz);
        glVertex3f(tx,15,tz);
        glVertex3f(tx,15,tz-85);
        glVertex3f(tx,0,tz-85);
    glEnd();
    //ǰ
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

//�����ϰ����Ҫ���ϰ��������Ҳ�������ν��ϰ�����ӽ�ȥ����Ŀǰֻ�ǻ���һ��û��ʵ��Ķ��������ϰ���Ӧ���Ǿ���ʵ���---��Ҫ�޸�
void draw_buildings(){
	//glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
    //��Ӧ���ó�ob���Ǹ��Ľ����Ȼ����ƽ����ÿ�����������5����,
    //���������ɫ
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
    //������ɫ
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


//�������ƺ���
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    gluLookAt(10.0, 50.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //�����������۲�λ��
    camera();


    //������������floor
    draw_floor();
    //���ϰ�
    draw_obstacles();

    //���ƽ���
    //draw_xybuilding(3);


    draw_buildings();
    //����agent
    //draw_agent();

    for(int i =0;i<agents;i++)
    {
        //����agents
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

//�����ﹹ�쳡���ϰ�--��Ҫ�޸�
void makeStructure()
{
/* �ϰ��� */
    //��
    ob.push_back(RVO::Vector2(-40.0f,-100.0f));
    ob.push_back(RVO::Vector2(-40.0f,100.0f));
    ob.push_back(RVO::Vector2(-100.0f,100.0f));
    ob.push_back(RVO::Vector2(-100.0f,-100.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //��
    ob.push_back(RVO::Vector2(100.0f,-100.0f));
    ob.push_back(RVO::Vector2(100.0f,100.0f));
    ob.push_back(RVO::Vector2(40.0f,100.0f));
    ob.push_back(RVO::Vector2(40.0f,-100.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //��
    ob.push_back(RVO::Vector2(10.0f,15.0f));
    ob.push_back(RVO::Vector2(10.0f,100.0f));
    ob.push_back(RVO::Vector2(-40.0f,100.0f));
    ob.push_back(RVO::Vector2(-40.0f,15.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
    //��
    ob.push_back(RVO::Vector2(40.0f,-100.0f));
    ob.push_back(RVO::Vector2(40.0f,-15.0f));
    ob.push_back(RVO::Vector2(-10.0f,-15.0f));
    ob.push_back(RVO::Vector2(-10.0f,-100.0f));
    sim->addObstacle(ob);
    obs.push_back(ob);
    ob.clear();
}

//������ָ��Ŀ��λ��--��Ҫ�滻
void generateGoals()
{
    //�������趨����λ��Ϊָ�����յ�
    v.position = RVO::Vector2(-25.0f,-100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(25.0f,100.0f);
    roadmap.push_back(v);
}

//������ָ����ͼ����λ��--��Ҫ�滻
void generateRoadMap()
{
    //��Ҫ
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

    //��Ӵ�Ҫ�鿴Ч��
    v.position = RVO::Vector2(-30.0f,0.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(30.0f,0.0f);
    roadmap.push_back(v);
}

//���ó�������
void setupScenario(RVO::RVOSimulator *sim)
{
    //ָ��ģ���ȫ��ʱ�䲽��
    sim->setTimeStep(0.25f);
    //����ϰ���
    makeStructure();
    //�����ϰ���
    sim->processObstacles();
    /* ·��ͼ */
    RoadMapVertex v;
    //���agent���յ�����
    generateGoals();
    //����ϰ�����Χ��·��ͼ����
    generateRoadMap();

    //ָ��agent��Ĭ�ϲ���(�ھӾ��룬�ھӸ�����ʱ���ƽ�ߣ�ʱ��ƽ�߲�˹�أ��뾶������ٶȣ���˹��ʸ����
    //sim->setAgentDefaults(200.0f, 3, 10.0f, 10.0f, 3.0f, 2.0f);
    sim->setAgentDefaults(10.0f, 10, 5.0f, 10.0f, 2.0f, 2.0f);
    //���agent��ָ����ʼλ�ã����洢�յ�Ϊ�����жԳ�λ��

    for (size_t i = 0; i < 10; ++i) //2���ص㣬ÿ���ص����50����
    {
			sim->addAgent(RVO::Vector2(-25.0f,-98.0f+rand()%100));
			goals.push_back(1);
			sim->addAgent(RVO::Vector2(25.0f,98.0f-rand()%100));
			goals.push_back(0);
	}
}

//�����ײ�·��ͼָ��·���滮--need change
void buildRoadmap(RVO::RVOSimulator *sim)
{
	/* Connect the roadmap vertices by edges if mutually visible. �������ɼ����򰴱�����·��ͼ����*/
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


//������ѡ�ٶ�--nochange
void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the visible roadmap vertex that is on the shortest path to the
	 * goal.����ѡ�ٶ�����Ϊ��λ��С(�ٶ�)��ʸ����ָ��Ŀ������·���ϵĿɼ�·��ͼ����ķ���
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

//����Ƿ����㵽�������-nochange
void checkGoals(){
    //�ȴ���pagent���Լ�Ŀ���vector2����λ�ö���
	RVO::Vector2 tempAgent, tempGoal;
	//�������;���
	float dist;

	//����agents
	for( size_t i = 0; i < sim->getNumAgents(); i++) {
		//��ȡÿ��agent�Ͷ�ӦĿ���λ�ö���
		tempAgent = sim->getAgentPosition(i);
		tempGoal = roadmap[goals[i]].position;

		//�������
		dist = sqrt( (tempGoal.x()-tempAgent.x())*(tempGoal.x()-tempAgent.x()) + (tempGoal.y()-tempAgent.y())*(tempGoal.y()-tempAgent.y()) );

		//���������Ŀ�ĵأ�
		// if goal is reached, warp agent to oposite side (maybe consider randomizing this
		if(dist < 10.0f) {
            //��ȡagent��λ�ã���agent�ƶ���ָ����λ�ã������������������뷨��
			tempAgent = RVO::Vector2(tempAgent.x()*-1+rand()%10, tempAgent.y()*-1);
			sim->setAgentPosition(i, tempAgent);
		}
	}
}

//д���ص�����--need change
void myIdle()
{
    //������ѡ�ٶ�
    setPreferredVelocities(sim);
    //ģ��һ�������귢���˸ı�
    sim->doStep();
    checkGoals();
    //�ٴε��û��ƺ����ػ�
    glutPostRedisplay();
}

int main()
{
    //����ģ�ⳡ���Ĳ���
    setupScenario(sim);
    //��ȡagents�ĸ���
    agents = sim->getNumAgents();
    //��ʼ���ײ�·��ͼ
    buildRoadmap(sim);

    //��ʼ��opengl
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1000,1000);
	glutCreateWindow("ͨ����Ⱥ3D-test");

    //��ʼ������
    myinit();
    glutPassiveMotionFunc(mouseMovement);
    //ָ�����ƺ���
    glutDisplayFunc(display);
    //���ûص�����
    glutIdleFunc(myIdle);
    glutMainLoop();

    delete sim;
    return 0;
}
























