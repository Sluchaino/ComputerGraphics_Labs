#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

#include <Math.h>
#include <vector>
#define P 3.14159

bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint car, stankin, texture[8];
char* names[] = { "stankin.bmp","car.bmp" };
//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE* texarray;
	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("car.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);


	//генерируем ИД для текстуры
	glGenTextures(1, &stankin);
	
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, stankin);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);
	//glBindTexture(GL_TEXTURE_2D, 0);


	free(texCharArray);
	free(texarray);
	
	//OpenGL::LoadBMP("texture[0].bmp", &texW, &texH, &texarray);
	//OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//
	//
	////генерируем ИД для текстуры
	//glGenTextures(1, &texture[0]);
	////биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	//glBindTexture(GL_TEXTURE_2D, texture[0]);

	////загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	////отчистка памяти
	//free(texCharArray);
	//free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void FiguraUp(std::vector<std::vector<double>> aaa, double height)
{
	for (int j = 0; j < aaa.size(); j++)
	{
		aaa[j][2] += height;
	}
	
	
	glEnd();
	for (int j = 0; j < aaa.size(); j++)
	{
		aaa[j][2] -= height;
	}
}
void Figura(std::vector<std::vector<double>> aaa, double height)
{

	double b[][2] = {
		{5.0/17.0, 6.0/14.0},
		{0.0, 2.0/14.0},
		{7.0/17.0, 4.0/14.0},
		{12.0/17.0, 0.0},
		{1.0, 2.0/14.0},
		{10.0 / 17.0, 6.0 / 14.0},
		{11.0/17.0, 1.0},
		{4.0/17.0, 11.0/14.0}
	};
	
	for (int i = 0; i < 2; i++)
	{
		
		glBegin(GL_POLYGON);
		if (i == 0)
		{
			glColor3d(0, 0.5, 1);
			glNormal3d(0, 0, 1);

		}
			
		else
		{
			glColor3d(1, 0.5, 0);
			glNormal3d(0, 0, -1);

		}
		glBindTexture(GL_TEXTURE_2D, 1);
		glBegin(GL_POLYGON);
		glColor3d(0, 0.5, 1);
		for (int z = 0; z < aaa.size(); z++)
		{
			if (z >= 0 && z <= 6)
			{
				glTexCoord2dv(b[z]);
				glVertex3d((double)aaa[z][0], (double)aaa[z][1], (double)aaa[z][2]);
			}
			else if (z == aaa.size()-1)
			{
				glTexCoord2dv(b[7]);
				glVertex3d((double)aaa[z][0], (double)aaa[z][1], (double)aaa[z][2]);
			}
			else
			{
				glTexCoord2d((17.0 - (double)aaa[z][1])/ 17.0, (double)aaa[z][0]/14.0);
				glVertex3d((double)aaa[z][0], (double)aaa[z][1], (double)aaa[z][2]);
			}
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
	glBindTexture(GL_TEXTURE_2D, 1);
	glBegin(GL_POLYGON);
	glColor3d(color[0], color[1], color[2]);
	glNormal3d(v1[1] * v2[2] - v1[2] * v2[1], v1[2]*v2[0] - v1[0]*v2[2], v1[0]*v2[1] - v1[1]*v2[0]);
	glTexCoord2d(0, 0);
	glVertex3d(v1[0], v1[1], v1[2]);
	glTexCoord2d(1, 0);
	glVertex3d(v2[0], v2[1], v2[2]);
	v2[2] += height;
	v1[2] += height;
	glTexCoord2d(1, 1);
	glVertex3d(v2[0], v2[1], v2[2]);
	glTexCoord2d(0, 1);
	glVertex3d(v1[0], v1[1], v1[2]);
	glEnd();
	v2[2] -= height;
	v1[2] -= height;
}
void VogTexture(std::vector<double> v1, std::vector<double> v2, double height, double color[], int col,int index)
{
	glBindTexture(GL_TEXTURE_2D, 1);
	glBegin(GL_POLYGON);
	glColor3d(color[0], color[1], color[2]);
	glNormal3d(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);
	double step = 1 / (double)col;
	double stepNow = 0;
	glTexCoord2d(0, 0);
	glVertex3d(v1[0], v1[1], v1[2]);
	glTexCoord2d(1, 0);
	glVertex3d(v2[0], v2[1], v2[2]);
	v2[2] += height;
	v1[2] += height;
	glTexCoord2d(1, 1);
	glVertex3d(v2[0], v2[1], v2[2]);
	glTexCoord2d(0, 1);
	glVertex3d(v1[0], v1[1], v1[2]);
	glEnd();
	v2[2] -= height;
	v1[2] -= height;
}
void Bok(std::vector<std::vector<double>> aaa, double height, int col)
{
	double color[] = { 1, 0, 0 };
	double Col = (double)col;
	for (int j = 0; j < aaa.size() - 1; j++)
	{
		if (j == 3);
		else if (j >= 6 && j <= col)
		{
			std::vector<double> v1 = aaa[j];
			std::vector<double> v2 = aaa[j+1];
			color[0] = 1;
			color[1] = 0;
			color[2] = 0.94;
			//Polygon(aaa[j], aaa[j + 1], height, color);
			glBindTexture(GL_TEXTURE_2D, 1);
			glBegin(GL_POLYGON);
			glColor3d(color[0], color[1], color[2]);
			glNormal3d(v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]);

			glTexCoord2d((Col - j - 6) / Col + 1 / Col, 0);
			glVertex3d(v1[0], v1[1], v1[2]);

			glTexCoord2d((Col - j - 6) / Col, 0);
			glVertex3d(v2[0], v2[1], v2[2]);

			v2[2] += height;
			v1[2] += height;

			glTexCoord2d((Col - j - 6) / Col, 1);
			glVertex3d(v2[0], v2[1], v2[2]);

			glTexCoord2d((Col - j - 6) / Col + 1 / Col, 1);
			glVertex3d(v1[0], v1[1], v1[2]);
			glEnd();
			v2[2] -= height;
			v1[2] -= height;
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
		color[0] = 1;
		color[1] = 0;
		color[2] = 0.94;
		//Polygon(aaa[j], aaa[j + 1], height, color);
		glBindTexture(GL_TEXTURE_2D, 1);
		glBegin(GL_POLYGON);
		glColor3d(color[0], color[1], color[2]);
		glNormal3d( ver[i][1] *  ver[i+1][2] -  ver[i][2] *  ver[i+1][1],  ver[i][2] *  ver[i+1][0] -  ver[i][0] *  ver[i+1][2],  ver[i][0] *  ver[i+1][1] -  ver[i][1] *  ver[i+1][0]);

		glTexCoord2d(((double)step - i) / (double)step + 1 / (double)step, 0);
		glVertex3d( ver[i][0],  ver[i][1],  ver[i][2]);

		glTexCoord2d(((double)step - i) / (double)step, 0);
		glVertex3d( ver[i+1][0],  ver[i+1][1],  ver[i+1][2]);

		 ver[i+1][2] += height;
		 ver[i][2] += height;

		glTexCoord2d(((double)step - i) / (double)step, 1);
		glVertex3d( ver[i+1][0],  ver[i+1][1],  ver[i+1][2]);

		glTexCoord2d(((double)step - i) / (double)step + 1 / (double)step, 1);
		glVertex3d( ver[i][0],  ver[i][1],  ver[i][2]);
		glEnd();
		 ver[i+1][2] -= height;
		 ver[i][2] -= height;
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


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  

	double height = 5;
	double z = 1;
	double ColVertexPolCrug = 30;
	std::vector<double> M = { 11.5,9.5,z };
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

	double** ver = PolKrug(aaa[3], aaa[4], height, ColVertexPolCrug);

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
	//Начало рисования квадратика станкина
	//double A[2] = { -4, -4 };
	//double B[2] = { 4, -4 };
	//double C[2] = { 4, 4 };
	//double D[2] = { -4, 4 };

	//glBindTexture(GL_TEXTURE_2D, texture[0]);

	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);

	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);

	//glEnd();
	//конец рисования квадратика станкина


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}