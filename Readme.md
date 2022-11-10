**微观人群模拟方法--基于RVO库&OpenGL库的实现**

国外对该技术的使用和应用已经很常见了，但是在国内目前能在网上查到的相关实际代码应用非常少，要进行使用，需要对其原理进行深入学习并阅读相关文档，比较费事。故将本人的一些产出push上来进行保存并方便了解该技术，通过阅读示例代码应该能快速上手。

示例代码：

exampleGlRvo.cpp

示例结果：

![](https://github.com/renshujiajia/RVO/blob/RVO/pic/test.gif)

**项目中源代码分为2D和3D实现两部分**

​		Room.cpp ：模拟多个房间内多个人群

​		CrossRoads.cpp：模拟十字路口中多个人群

​		Pass.cpp：模拟通道内人多个人群

**结果演示**

​		见pic目录以及ppt文档

**依赖库包含**

​		RVO库 ：https://github.com/renshujiajia/RVO/tree/RVO/2D/rvo2-2.0.2/RVO2/src

​		OpenGL：https://github.com/renshujiajia/RVO/tree/RVO/openGL%E4%BE%9D%E8%B5%96/glut

**参考资料**

1)    Jur van den Berg, Ming C. Lin, and Dinesh Manocha, “Reciprocal velocity obstacles for real-time multi-agent navigation,” in Proc. IEEE Int. Conf. Robotics and Automation, Pasadena, Calif., May 19-23, 2008, pp. 1928-1935.

2)    Jur van den Berg, Sachin Patil, Jason Sewall, Dinesh Manocha, and Ming C. Lin, “Interactive navigation of individual agents in crowded environments,” in Proc. ACM SIGGRAPH Symp. Interactive 3D Graphics and Games, Redwood City, Calif., February 15-17, 2008, pp. 139-147.

3)    http://gamma.cs.unc.edu/RVO2/

4)    http://gamma.cs.unc.edu/RVO/
