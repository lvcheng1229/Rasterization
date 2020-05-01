#include<iostream>
#include<fstream>
using namespace std;
//输出图像的宽度，输出图像的高度
const int imageWidth=200;
const int imageHeight=100;
int main()
{
	ofstream outfile("Code2.ppm");
	outfile<<"P3\n"<<imageWidth<<endl<<imageHeight<<endl<<"255\n";
	float centerx = imageWidth / 2;
	float centery = imageHeight / 2;
	float r = imageHeight / 5;

	//PPM格式的顺序为从上到下，从左到右
	for (int j=0;j<imageHeight;j++)
	{
		for (int i = imageWidth-1; i>=0; i--)
		{
			if (((i+0.5 - centerx)*(i+0.5 - centerx) + (j+0.5 - centery)*(j+0.5 - centery)) < r*r)
			{
				outfile << "255 0 0" << endl;
			}
			else
			{
				outfile << "50 50 50" << endl;
			}
		}
	}
	outfile.close();
}