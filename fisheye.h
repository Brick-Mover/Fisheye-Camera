//
//  camera.h
//  Fisheye Camera
//
//  Created by SHAO Jiuru on 6/25/16.
//  Copyright © 2016 UCLA. All rights reserved.
//

#ifndef camera_h
#define camera_h

#include <assert.h>
#include <iostream>
#include <vector>

using namespace std;

const float TOLERANCE = float(1.0e-03);
enum WALLTYPE {yPos, yNeg, xNeg, xPos, zPos, zNeg};



struct Point
{
    Point(float x0, float y0, float z0) { x = x0; y = y0; z = z0; }
    Point& operator = (Point other);
    bool operator == (Point other);
    float x;
    float y;
    float z;
    void print() {
        cout << "p.x: " << x << endl;
        cout << "p.y: " << y << endl;
        cout << "p.z: " << z << endl;
    }
};

typedef Point Vec3;

struct Pixel
{
    Pixel(int r, int g, int b) {R = r; G = g; B = b;}
    Pixel() {}
    Pixel& operator = (Pixel other);
    unsigned char R;
    unsigned char G;
    unsigned char B;
    //int a;
    void print() const {
        cout << "p.R: " << int(R) << endl;
        cout << "p.G: " << int(G) << endl;
        cout << "p.B: " << int(B) << endl;
    }
};


const Pixel grey_pixel = Pixel(128, 128, 128);
const Point origin = Point(0.0, 0.0, 0.0);


struct Wall
{
    Wall(int l, int h, int n, WALLTYPE t);
    // Wall: Ax + By + Cz = D
    float A, B, C;
    int D;
    int xDim;
    int yDim;
    int zDim;
    int near;
    // resolution of the picture
    int length;
    int height;
    
    WALLTYPE type;
    vector<Pixel> pixels;
    
    // detect if a given point is on the wall
    bool isOnWall(Point p) const;
    // get the closest pixel to the point on the wall
    Pixel get_pixel(Point p) const;
    // get the intersection point(with a vector in parametrized form)
    Point intersect(Vec3 dir, Point p) const;
    bool getIntersection(Vec3 dir, Point cameraPos, Point& intersection) const;
};


struct Surrounding
{
    Surrounding(Wall* Front, Wall* Back, Wall* Left, Wall* Right, Wall* Ceil, Wall* Floor);
    Surrounding() {};
    Wall* yPos;
    Wall* yNeg;
    Wall* xPos;
    Wall* xNeg;
    Wall* zPos;
    Wall* zNeg;
};


class Fisheye
{
public:
    Fisheye():cameraPos(0.0,0.0,0.0) {}
    vector<Pixel> getImage() const { return imagePlane; }
    void render();
    void renderPixel(int x, int y);
    void setColor(int x, int y, const Pixel& color);

private:
    float aperture;     // range of view
    float viewAngle;    // angle of view direction
    Point cameraPos;
    Surrounding walls;
    int xDim, yDim;     // size of image plane
    
    vector<Pixel> imagePlane; // the final image produced
    
};

#endif /* camera_h */
