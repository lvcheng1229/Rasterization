#include<iostream>
#include<fstream>
using namespace std;
//输出图像的宽度，输出图像的高度
const int imageWidth=200;
const int imageHeight=100;
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
float EdgeFunction(const Vector3 &A,const Vector3 &B,const Vector3 &P)
{
	Vector3 vec1 = sub(B, A);
	Vector3 vec2 = sub(P, A);
	return Cross(vec2, vec1).z;
}
int main()
{
	Vector3 vertPosA(0, 0, 0);
	Vector3 vertPosB(imageWidth/2, imageHeight, 0);
	Vector3 vertPosC(imageWidth, 0, 0);

	Vector3 ColorA(255, 0, 0);
	Vector3 ColorB(0, 255, 0);
	Vector3 ColorC(0, 0, 255);

	ofstream outfile("Code4.ppm");
	outfile<<"P3\n"<<imageWidth<<endl<<imageHeight<<endl<<"255\n";
	Vector3 image[imageWidth][imageHeight];

	for(int i=0;i<imageWidth;i++)
	{
		for(int j=0;j<imageHeight;j++)
		{			
			Vector3 Pixel(i+0.5f, j+0.5f, 0);
			float weight3 = EdgeFunction(vertPosA, vertPosB, Pixel)/2.0;
			float weight1 = EdgeFunction(vertPosB, vertPosC, Pixel)/2.0;
			float weight2 = EdgeFunction(vertPosC, vertPosA, Pixel)/2.0;

			//如果三个重心坐标都大于0则该像素点在三角形内
			if (weight1 >= 0 && weight2 >= 0 && weight3 >= 0)
			{
				float triangleArea = EdgeFunction(vertPosA, vertPosB,vertPosC)/2.0;

				//计算重心坐标
				weight1 /= triangleArea;
				weight2 /= triangleArea;
				weight3 /= triangleArea;
				
				image[i][j] = ColorA * weight1 + ColorB * weight2 + ColorC * weight3;
			}
			else
			{
				image[i][j] = Vector3(50, 50, 50);
			}

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