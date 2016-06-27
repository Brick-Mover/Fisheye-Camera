//
//  camera.cpp
//  Fisheye Camera
//
//  Created by SHAO Jiuru on 6/25/16.
//  Copyright © 2016 UCLA. All rights reserved.
//


#include "fisheye.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <math.h>

using namespace std;


void eat_comment(ifstream &f);
void load_ppm(Wall* img, const string &name);
void saveImg(vector<Pixel> pic, int length, int height);
void initializeWalls(const Surrounding& s);


int main(int argc, const char * argv[])
{
}






void Fisheye::render()
{
    // On normalized view plane, render each pixel
    for (int r = 0; r < yDim; r++) {
        for (int c = 0; c < yDim; c++) {
            renderPixel(r, c);
        }
    }
}


void Fisheye::renderPixel(int row, int col)
{
    // convert (r,c) to normalized coordinate [-1, 1]
    float xn = 2*(float)row/xDim - 1;
    float yn = 2*(float)col/yDim - 1;
    float r = sqrtf(xn*xn + yn*yn);
    if (r > 1)
        setColor(row, col, grey_pixel);
    // theta: [0,2π]
    float theta = atan2f(yn, xn);
    // phi: [0,π]
    float phi = r * aperture / 2;
    
}


void Fisheye::setColor(int row, int col, const Pixel& color)
{
    imagePlane[row*xDim + col] = color;
}


Wall::Wall(int l, int h, int n, WALLTYPE t)
{
    length = l;
    height = h;
    near = n;
    type = t;
    // Ax + By + Cz = D
    switch (t) {
        case FRONT:
            A = 0.0;
            B = 1.0;
            C = 0.0;
            D = near;
            break;
        case BACK:
            A = 0.0;
            B = -1.0;
            C = 0.0;
            D = near;
            break;
        case LEFT:
            A = -1.0;
            B = 0.0;
            C = 0.0;
            D = near;
            break;
        case RIGHT:
            A = 1.0;
            B = 0.0;
            C = 0.0;
            D = near;
            break;
        case CEIL:
            break;
        case FLOOR:
            break;
    }
}


/* trivial member functions such as assign overloading and constructors */
Point& Point::operator = (Point other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}


Pixel& Pixel::operator = (Pixel other)
{
    R = other.R;
    G = other.G;
    B = other.B;
    return *this;
}


Surrounding::Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right, Wall* Ceil, Wall* Floor)
{
    this->Front = Front;
    this->Back = Back;
    this->Left = Left;
    this->Right = Right;
    this->Ceil = Ceil;
    this->Floor = Floor;
}


/* BELOW: load and save images */
void saveImg(vector<Pixel> pic, int length, int height)
{
    FILE *fp = fopen("result.ppm", "wb");
    (void) fprintf(fp, "P6\n%d %d\n255\n", length, height);
    int count = 0;
    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < length; ++i)
        {
            static unsigned char color[3];
            color[0] = pic[count].R;  /* red */
            color[1] = pic[count].G;  /* green */
            color[2] = pic[count].B;  /* blue */
            (void) fwrite(color, 1, 3, fp);
            count++;
        }
    }
    (void) fclose(fp);
}


void initializeWalls(const Surrounding& s)
{
    load_ppm(s.Front, "front.ppm");
    load_ppm(s.Back, "back.ppm");
    load_ppm(s.Left, "left.ppm");
    load_ppm(s.Right, "right.ppm");
    load_ppm(s.Ceil, "ceil.ppm");
    load_ppm(s.Floor, "floor.ppm");
}


void eat_comment(ifstream &f)
{
    char linebuf[1024];
    char ppp;
    while (ppp = f.peek(), ppp == '\n' || ppp == '\r')
        f.get();
    if (ppp == '#')
        f.getline(linebuf, 1023);
}


void load_ppm(Wall* img, const string &name)
{
    ifstream f(name.c_str(), ios::binary);
    if (f.fail())
    {
        cout << "Could not open file: " << name << endl;
        return;
    }
    
    // get type of file
    eat_comment(f);
    int mode = 0;
    string s;
    f >> s;
    if (s == "P3")
        mode = 3;
    else if (s == "P6")
        mode = 6;
    
    // get w
    eat_comment(f);
    f >> img->length;
    
    // get h
    eat_comment(f);
    f >> img->height;
    
    // get bits
    eat_comment(f);
    int bits = 0;
    f >> bits;
    
    // error checking
    if (mode != 3 && mode != 6)
    {
        cout << "Unsupported magic number" << endl;
        f.close();
        return;
    }
    if (img->length < 1)
    {
        cout << "Unsupported width: " << img->length << endl;
        f.close();
        return;
    }
    if (img->height < 1)
    {
        cout << "Unsupported height: " << img->height << endl;
        f.close();
        return;
    }
    if (bits < 1 || bits > 255)
    {
        cout << "Unsupported number of bits: " << bits << endl;
        f.close();
        return;
    }
    
    // load image data
    img->pixels.resize(img->length * img->height);
    
    if (mode == 6)
    {
        f.get();
        f.read((char*)&img->pixels[0], img->pixels.size() * 3);
    }
    else if (mode == 3)
    {
        for (int i = 0; i < img->pixels.size(); i++)
        {
            int v;
            f >> v;
            img->pixels[i].R = v;
            f >> v;
            img->pixels[i].G = v;
            f >> v;
            img->pixels[i].B = v;
        }
    }
    
    // close file
    f.close();
}