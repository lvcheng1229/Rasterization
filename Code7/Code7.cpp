#include<iostream>
#include<fstream>
//载入图像数据需要的头文件
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
using namespace std;
//输出图像的宽度，输出图像的高度
const int imageWidth=200;
const int imageHeight=100;
//纹理贴图的宽度，纹理贴图的高度
int textureWidth;
int textureHeight;
//三维向量结构体
struct Vector3 {
	float x, y, z;
	Vector3(float x0,float y0,float z0):x(x0),y(y0),z(z0) {};
	Vector3() { x = 0; y = 0; z = 0; };
	Vector3 operator*(float temp)
	{
		return Vector3(x*temp, y*temp, z*temp);
	}
	Vector3 operator+(Vector3 temp)
	{
		return Vector3(temp.x + x, temp.y + y, temp.z + z);
	}
};
//二维向量结构体
struct Vector2 {
	float x, y;
	Vector2(float x0,float y0):x(x0),y(y0){};
	Vector2() { x = 0; y = 0;};
	Vector2 operator*(float temp)
	{
		return Vector2(x*temp, y*temp);
	}
	Vector2 operator+(Vector2 temp)
	{
		return Vector2(temp.x + x, temp.y + y);
	}
};
//顶点
struct Vertex {
	//顶点属性

	//顶点位置
	Vector3 Position;
	//顶点颜色
	Vector3 Color;
	//顶点UV值
	Vector2 UV;
	Vertex() {};
	Vertex(Vector3 Position,Vector3 Color):Position(Position),Color(Color) {};
	Vertex(Vector3 Position,Vector2 UV):Position(Position),UV(UV) {};
};
struct Triangle
{
	Vertex v1, v2, v3;
	Triangle() {};
	Triangle(Vertex v1,Vertex v2,Vertex v3):v1(v1),v2(v2),v3(v3) {};
};
//向量减法
Vector3 sub(const Vector3 &A,const Vector3 &B)
{
	return Vector3(A.x-B.x, A.y-B.y, A.z-B.z);
}
//叉乘运算
Vector3 Cross(const Vector3 &A,const Vector3 &B)
{
	return Vector3(A.y*B.z - A.z*B.y, A.z*B.x - A.x*B.z, A.x*B.y - A.y*B.x);
}
//根据UV值进行纹理贴图采样
Vector3 texture(float u, float v, unsigned char *data)
{
	int i = u * textureWidth;
	int j = (1 - v) * textureHeight - 0.001f;
	if (i < 0)i = 0;
	if (j < 0)j = 0;
	if (i > textureWidth - 1)i = textureWidth - 1;
	if (j > textureHeight - 1)j = textureHeight - 1;
	float r = int(data[3 * i + j * textureWidth * 3]);
	float g = int(data[3 * i + j * textureWidth * 3 + 1]);
	float b = int(data[3 * i + j * textureWidth * 3 + 2]);
	return Vector3(r, g, b);
}
float EdgeFunction(const Vector3 &A,const Vector3 &B,const Vector3 &P)
{
	Vector3 vec1 = sub(B, A);
	Vector3 vec2 = sub(P, A);
	return Cross(vec2, vec1).z;
}
//把顶点投影到二维
void ProjectVertexTo2D(Vertex &vertex,float distance=1.0)
{
	//将顶点投影到成像平面，其中1e-5是为了防止除零错误
	vertex.Position.x = vertex.Position.x / (vertex.Position.z / distance+1e-5);
	vertex.Position.y = vertex.Position.y / (vertex.Position.z / distance+1e-5);

	//将顶点范围从[-1,1]^2，映射到[imageWidth，imageHeight]^2
	vertex.Position.x = (imageWidth + vertex.Position.x*imageWidth) / 2;
	vertex.Position.y = (imageHeight + vertex.Position.y*imageHeight) / 2;

	//将顶点的Z值变成其倒数以便后续插值计算
	vertex.Position.z = 1 / vertex.Position.z;
}
//把顶点投影到三维
void ProjectTriangleTo2D(Triangle &triangle)
{
	ProjectVertexTo2D(triangle.v1);
	ProjectVertexTo2D(triangle.v2);
	ProjectVertexTo2D(triangle.v3);
}
//着色函数
Vector3 Color(const Vector3 &Pixel, Triangle &triangle,unsigned char *data=NULL)
{
	Vector3 Color(0, 0, 0);

	float weight3 = EdgeFunction(triangle.v1.Position, triangle.v2.Position, Pixel)/2.0;
	float weight1 = EdgeFunction(triangle.v2.Position, triangle.v3.Position, Pixel)/2.0;
	float weight2 = EdgeFunction(triangle.v3.Position, triangle.v1.Position, Pixel)/2.0;

	//如果三个重心坐标都大于0则该像素点在三角形内
	if (weight1 >= 0 && weight2 >= 0 && weight3 >= 0)
	{
		float triangleArea = EdgeFunction(triangle.v1.Position, triangle.v2.Position,triangle.v3.Position)/2.0;

		//计算重心坐标
		weight1 /= triangleArea;
		weight2 /= triangleArea;
		weight3 /= triangleArea;

		//对三角形的Z值进行插值，z1Inverse为顶点1的Z值的倒数
		float z1Inverse = triangle.v1.Position.z;
		float z2Inverse = triangle.v2.Position.z;
		float z3Inverse = triangle.v3.Position.z;
		float z = z1Inverse*weight1 + z2Inverse*weight2 + z3Inverse*weight3;

		z = 1.0 / z;
		//对顶点属性进行插值
		Vector2 UV=(triangle.v1.UV*weight1*z1Inverse 
			+ triangle.v2.UV*weight2*z2Inverse 
			+ triangle.v3.UV*weight3*z3Inverse)*z;

		Color = texture(UV.x, UV.y, data);
		return Color;
	}
	return Color;

}
Vector3 rotate_z(Vector3 vec,float theta)
{
	float xx, yy;
	theta = (theta / 180)*3.1415926535;
	xx = vec.x*cos(theta) - vec.y*sin(theta);
	yy=  vec.x*sin(theta) + vec.y*cos(theta);
	return Vector3(xx, yy, vec.z);
}

Vector3 rotate_y(Vector3 vec, float theta)
{
	float xx, zz;
	theta = (theta / 180)*3.1415926535;
	xx = vec.x*cos(theta) + vec.z*sin(theta);
	zz = -vec.x*sin(theta) + vec.z*cos(theta);
	return Vector3(xx, vec.y, zz);
}

Vector3 rotate_x(Vector3 vec, float theta)
{
	float yy, zz;
	theta = (theta / 180)*3.1415926535;
	yy = vec.y*cos(theta) - vec.z*sin(theta);
	zz = vec.y*sin(theta) + vec.z*cos(theta);
	return Vector3(vec.x, yy, zz);
}

Vector3 rotate(Vector3 vec, Vector3 rot)
{
	return rotate_z(rotate_y(rotate_x(vec, rot.x),rot.y),rot.z);
}

Vector3 translate(Vector3 vec, Vector3 offset)
{
	return vec + offset;
}
int main()
{
	Vector3 vertPosA(0, 2, 4);
	Vector3 vertPosB(1, -1, 2);
	Vector3 vertPosC(-1, -1, 2);
	
	vertPosA = translate(vertPosA, Vector3(-1, 0, 0));
	vertPosB = translate(vertPosB, Vector3(-1, 0, 0));
	vertPosC = translate(vertPosC, Vector3(-1, 0, 0));
	
	vertPosA = rotate(vertPosA, Vector3(0, 0, 30));
	vertPosB = rotate(vertPosB, Vector3(0, 0, 30));
	vertPosC = rotate(vertPosC, Vector3(0, 0, 30));

	Vertex vertA(vertPosA, Vector2(0.5, 1.0));
	Vertex vertB(vertPosB, Vector2(0.0, 0.0));
	Vertex vertC(vertPosC, Vector2(1.0, 0.0));

	Triangle triangle(vertA, vertB, vertC);
	ProjectTriangleTo2D(triangle);
	ofstream outfile("Code7.ppm");
	Vector3 image[imageWidth][imageHeight];
	outfile<<"P3\n"<<imageWidth<<endl<<imageHeight<<endl<<"255\n";

	int nchannel;
	unsigned char *data = stbi_load("container.jpg", &textureWidth, &textureHeight, &nchannel, 0);

	for(int i=0;i<imageWidth;i++)
	{
		for(int j=0;j<imageHeight;j++)
		{
			Vector3 Pixel(i+0.5f, j+0.5f, 0);

			image[i][j] = Color(Pixel, triangle,data);
		}
	}
	for (int j = imageHeight - 1; j >= 0; j--)
	{
		for (int i = 0; i < imageWidth; i++)
		{
			outfile << (int)image[i][j].x << " " << (int)image[i][j].y << " " << (int)image[i][j].z <<endl;
		}
	}
	outfile.close();
}