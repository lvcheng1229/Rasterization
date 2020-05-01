#include<iostream>
#include<fstream>
using namespace std;
//输出图像的宽度，输出图像的高度
const int imageWidth=200;
const int imageHeight=100;
int main()
{
	ofstream outfile("Code1.ppm");
	outfile<<"P3\n"<<imageWidth<<endl<<imageHeight<<endl<<"255\n";
	for(int i=0;i<imageWidth;i++)
	{
		for(int j=0;j<imageHeight;j++)
		{
			outfile << "255 0 0" << endl;
		}
	}
	outfile.close();
}