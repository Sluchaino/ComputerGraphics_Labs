

#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <Math.h>
#include <vector>
#define P 3.14159


	void Figura(std::vector<std::vector<double>> aaa, double height)
	{
		for (int i = 0; i < 2; i++)
		{
			glBegin(GL_POLYGON);
			if (i == 0)
				glColor3d(0, 0.5, 1);
			else
				glColor3d(1, 0.5, 0);
			for (int z = 0; z < aaa.size(); z++)
			{
				glVertex3d((double)aaa[z][0], (double)aaa[z][1], (double)aaa[z][2]);
			}
			glEnd();
			for (int j = 0; j < aaa.size(); j++)
			{
				aaa[j][2] += height;
			}

		}
		for (int j = 0; j < aaa.size(); j++)
		{
			aaa[j][2] -= height;
		}
	}
	void Polygon(std::vector<double> v1, std::vector<double> v2, double height, double color[])
	{
		glBegin(GL_POLYGON);
		glColor3d(color[0], color[1], color[2]);
		glVertex3d(v1[0],v1[1],v1[2]);
		glVertex3d(v2[0], v2[1], v2[2]);
		v2[2] += height;
		v1[2] += height;
		glVertex3d(v2[0], v2[1], v2[2]);
		glVertex3d(v1[0], v1[1], v1[2]);
		glEnd();
		v2[2] -= height;
		v1[2] -= height;
	}
	void Bok(std::vector<std::vector<double>> aaa, double height, int col)
	{
		double color[] = { 1, 0, 0 };
		for (int j = 0; j < aaa.size()-1; j++)
		{
			if (j == 3);
			else if (j >= 6 && j <= col)
			{
				color[0] = 1;
				color[1] = 0;
				color[2] = 0.94;
				Polygon(aaa[j], aaa[j + 1], height, color);

			}
			else
			{
				color[0] = 1;
				color[1] = 0;
				color[2] = 0;
				Polygon(aaa[j], aaa[j + 1], height, color);

			}
		}
		color[0] = 1;
		color[1] = 0;
		color[2] = 0;
		Polygon(aaa[aaa.size() - 1], aaa[0], height, color);
	}
	double** PolKrug(std::vector<double> v1, std::vector<double> v2, double height, double step)
	{
		double** ver = new double* [step + 1];
		for (int i = 0; i < step + 1; i++) {
			ver[i] = new double[3];
		}
		double StepAngel = 180 / step;
		double a[][3] = { {0,0,v1[2]}, {-1,2.5,v1[2]}, {1,-2.5,v1[2]} };
		double r = sqrt(pow(v1[0] - v2[0], 2) + pow(v1[1] - v2[1], 2)) / 2.0;
		double beginUgol = atan(-2.5) * 180.0 / P;
		double angel;
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0, 1, 0);
		glVertex3dv(a[0]);
		int j = 1;
		double g = beginUgol;
		for (int i = 0; i < step + 1; i++)
		{
			angel = g * P / 180;
			double point[] = { r * cos(angel) ,r * sin(angel), a[2][2] };
			glVertex3dv(point);
			ver[i][0] = point[0];
			ver[i][1] = point[1];
			ver[i][2] = point[2];
			g -= StepAngel;
		}
		glEnd();
		return ver;
	}
	void PolKrugPolygon(double** ver, int step, double color[3], double height)
	{

		for (int i = 0; i < step; i++)
		{
			std::vector<double> temp = { ver[i][0], ver[i][1], ver[i][2] };
			std::vector<double> temp1 = { ver[i + 1][0], ver[i + 1][1], ver[i + 1][2] };
			Polygon(temp, temp1, height, color);
		}
	}
	double OnCicle(std::vector<double> v1, double x, double y)
	{
		return pow(v1[0] - x, 2) + pow(v1[1] - y, 2);
	}
	double* FoundXY(std::vector<double> v1, std::vector<double> v2, std::vector<double> M)
	{
		double temp[] = { (v1[0] + v2[0]) / 2,(v1[1] + v2[1]) / 2, 25 };
		for (double x = 11; x <= 30; x += 0.1)
		{
			for (double y = min(v1[1], v2[1]); y <= 30; y += 0.1)
			{
				if (floor(OnCicle(v1, x, y)) == floor(OnCicle(v2, x, y)) && floor(OnCicle(M, x, y)) == floor(OnCicle(v1, x, y)))
				{
					temp[0] = x;
					temp[1] = y;
					temp[2] = OnCicle(v1, x, y);
					return temp;
				}
			}
		}
		return temp;
	}
	void Vognutost(std::vector<double> M, std::vector<std::vector<double>>& ver, std::vector<double> lastVer)
	{
		double* a = FoundXY(ver[6], lastVer, M);
		double O[] = { a[0], a[1], 1 };
		double r = sqrt(a[2]);
		double beginUgol = atan(3.5 / -1.5) * 180.0 / P;
		double angel;
		
		for (int i = beginUgol; i >= beginUgol - 200; i -= 1)
		{
			
			angel = i * P / 180;
			std::vector<double> point = { O[0] + r * cos(angel) ,O[1] + r * sin(angel), O[2] };
			if (point[1] >= min(ver[6][1], lastVer[1]) && point[1] <= max(ver[6][1], lastVer[1]) && point[0] <= max(ver[6][0], lastVer[0]))
			{
				ver.push_back(point);
			}
		}

	}
	void Render(double delta_time)
	{
		double height = 5;
		double z = 1;
		double ColVertexPolCrug = 30;
		std::vector<double> M = { 11.5,9,z };
		std::vector<double> lastVer = { 11, 13, z };

		std::vector<std::vector<double>> aaa;
		aaa = {
			
			{ 6, 12, z },
			{ 2, 17,z },
			{ 4, 10, z },
			{ 0, 5, z },
			{ 2,0, z },
			{6, 7, z},
			{14, 6, z},

		};
		glPushMatrix();

			glTranslated(1, 2.5, 0);

			std::vector<double> v1 = { aaa[3][0], aaa[3][1] ,aaa[3][2] + height };
			std::vector<double> v2 = { aaa[4][0], aaa[4][1] ,aaa[4][2] + height };

			double** ver = PolKrug(aaa[3], aaa[4],height, ColVertexPolCrug);

			PolKrug(v1, v2, height, ColVertexPolCrug);

			double color[] = { 0.3,0.1,0.5 };

			PolKrugPolygon(ver, ColVertexPolCrug, color, height);

		glPopMatrix();


		glPushMatrix();
		Vognutost(M, aaa, lastVer);
		aaa.push_back(lastVer);
		glPopMatrix();



		glPushMatrix();
		Bok(aaa, height, aaa.size());
		Figura(aaa, height);
		glPopMatrix();
	}



