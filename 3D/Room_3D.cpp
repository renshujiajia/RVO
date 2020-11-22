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
	  glVertex3f(150.0f, -0.01f, 150.0f); //��������Ƿ��Ǹ�������������ģ�
	  glVertex3f(150.0f, -0.01f, -150.0f);
	  glVertex3f(-150.0f, -0.01f, -150.0f);
	  glVertex3f(-150.0f, -0.01f, 150.0f);
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
//дһ������
//�������Ϊ�ĸ�������
void draw_aob(RVO::Vector2 pos1,RVO::Vector2 pos2,RVO::Vector2 pos3,RVO::Vector2 pos4)
{
    //������ĵ�����
    RVO::Vector2 centor = RVO::Vector2((pos1.x()+pos3.x())/2.0,(pos1.y()+pos3.y())/2.0);
    //��������x��ֵ��y��ֵ
    float x0 = centor.x();float z0 = centor.y(); //����
    float dx = abs(pos1.x()-pos3.x())/2; //x����ֵ
    float dz = abs(pos1.y()-pos3.y())/2; //z����ֵ
    //��������ֵ�ع���һ������������꣬�������������������
    //��
    glBegin(GL_QUADS);
        glVertex3f(x0-dx,15,z0+dz);
        glVertex3f(x0-dx,15,z0-dz);
        glVertex3f(x0+dx,15,z0-dz);
        glVertex3f(x0+dx,15,z0+dz);
    glEnd();
    //��
    glBegin(GL_QUADS);
        glVertex3f(x0-dx,0,z0+dz);
        glVertex3f(x0-dx,0,z0-dz);
        glVertex3f(x0-dx,15,z0-dz);
        glVertex3f(x0-dx,15,z0+dz);
    glEnd();
    //��
    glBegin(GL_QUADS);

        glVertex3f(x0-dx,0,z0-dz);
        glVertex3f(x0+dx,0,z0-dz);
        glVertex3f(x0+dx,15,z0-dz);
        glVertex3f(x0-dx,15,z0-dz);
    glEnd();
    //��
    glBegin(GL_QUADS);
        glVertex3f(x0+dx,0,z0-dz);
        glVertex3f(x0+dx,0,z0+dz);
        glVertex3f(x0+dx,15,z0+dz);
        glVertex3f(x0+dx,15,z0-dz);
    glEnd();
    //ǰ
    glBegin(GL_QUADS);
        glVertex3f(x0+dx,0,z0+dz);
        glVertex3f(x0-dx,0,z0+dz);
        glVertex3f(x0-dx,15,z0+dz);
        glVertex3f(x0+dx,15,z0+dz);
    glEnd();
}


//�����ϰ����Ҫ���ϰ��������Ҳ�������ν��ϰ�����ӽ�ȥ����Ŀǰֻ�ǻ���һ��û��ʵ��Ķ��������ϰ���Ӧ���Ǿ���ʵ���
void draw_buildings(){
	//glEnable(GL_LIGHTING);
	glDisable(GL_LIGHTING);
    //��Ӧ���ó�ob���Ǹ��Ľ����Ȼ����ƽ����ÿ�����������5����,
    //���������ɫ
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
    //������ɫ
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

//�������ƺ���
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    gluLookAt(10.0, 200.0, 80.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //�����������۲�λ��
    camera();


    //������������floor
    draw_floor();
    //���ϰ�
    draw_obstacles();
    //��������
    //draw_aob(obs[0][0],obs[0][1],obs[0][2],obs[0][3]);

    //���ƽ���
    //draw_xybuilding(3);


    draw_buildings();
    //����agent
    //draw_agent();

#pragma omp parallel for
    for(int i =0;i<agents;i++)
    {
        //����agents
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

//�����ﹹ�쳡���ϰ�
void makeStructure()
{
/* �ϰ��� */
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

//������ָ��Ŀ��λ��
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

//������ָ����ͼ����λ��
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
       //�ⲿ�������
    v.position = RVO::Vector2(-10.0f,80.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(10.0f,80.0f);
    roadmap.push_back(v);
    //����������������к�������
    v.position = RVO::Vector2(-80.0f,100.0f);
    roadmap.push_back(v);
    v.position = RVO::Vector2(80.0f,100.0f);
    roadmap.push_back(v);

}

//���ó�������
void setupScenario(RVO::RVOSimulator *sim)
{
    //ָ��ģ���ȫ��ʱ�䲽��
    sim->setTimeStep(0.5f);
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
#pragma omp parallel for
    for (size_t i = 0; i < 50; ++i) //�����ص㣬ÿ���ص����5����
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
		/*if(dist < 1.5f) {
            //��ȡagent��λ�ã���agent�ƶ���ָ����λ�ã������������������뷨��
			tempAgent = RVO::Vector2(tempAgent.x()*-1, tempAgent.y()*-1);
			sim->setAgentPosition(i, tempAgent);
		}*/
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
	glutCreateWindow("��Ⱥ3D-test");

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
