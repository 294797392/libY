/*
 * 
 * 
 * 弧度：弧长等于半径的弧，其所对的圆心角为1弧度。
 * 
 * 
 * r            =       1弧度           1个半径是1弧度
 * 2πr          =       2π弧度          2π个半径是2π弧度，所以360度所对应的弧度是2π（6.28），180度所对应的弧度是π
 * 
 * 
 * 360角度      =       2π              弧度
 * 180角度      =       π               弧度
 * 1角度        =       π / 180         弧度
 * 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.1415926

double angle2radian(double angle)
{
    return PI / 180 * angle;
}