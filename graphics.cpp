/*************************************************************************************************\
*                                                                                                 *
* "graphics.cpp" -                                                                                *
*                                                                                                 *
*         Author - Tom McDonnell 2003                                                             *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "graphics.h"

#include <TomsLibrary/geometry.h>

#include <GL/glut.h>

#include <assert.h>
#include <math.h>

// FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////

/*
 *
 */
void drawLine(double x1, double y1, double x2, double y2)
{
   glBegin(GL_LINES);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
   glEnd();
}

void drawLine(rec2vector p1, rec2vector p2)
{
   drawLine(p1.x, p1.y, p2.x, p2.y);
}

/*
 *
 */
void drawArrow(double x, double y, double angle, double length, double hLength)
{
   assert(0.0 <=  length &&  length <= 2.0);
   assert(0.0 <= hLength && hLength <= 2.0);

   double x2 = x + length * cos(angle),
          y2 = y + length * sin(angle);

   // draw arrow tail
   drawLine(x, y, x2, y2);

   // draw arrow head
   if (hLength > 0.0)
   {
      drawLine(x2, y2,
               x2 + hLength * cos(angle + 0.75 * pi),
               y2 + hLength * sin(angle + 0.75 * pi) );
      drawLine(x2, y2,
               x2 + hLength * cos(angle - 0.75 * pi),
               y2 + hLength * sin(angle - 0.75 * pi) );
   }
}

void drawArrow(rec2vector p, double angle, double length, double hLength)
{
   drawArrow(p.x, p.y, angle, length, hLength);
}

void drawArrow(rec2vector p, pol2vector v, double hLength)
{
   drawArrow(p.x, p.y, v.angle, v.r, hLength);
}

/*
 *
 */
void drawRect(double xl, double yt, double xr, double yb)
{
   glBegin(GL_POLYGON);
      glVertex2f(xl, yb);
      glVertex2f(xl, yt);
      glVertex2f(xr, yt);
      glVertex2f(xr, yb);
   glEnd();
}

void drawRect(rec2vector p1, rec2vector p2)
{
   drawRect(p1.x, p1.y, p2.x, p2.y);
}

void drawRect(rect r)
{
   drawRect(r.l, r.t, r.r, r.b);
}

/*
 *
 */
void drawWireRect(double xl, double yt, double xr, double yb)
{
   glBegin(GL_LINE_LOOP);
      glVertex2f(xl, yb);
      glVertex2f(xl, yt);
      glVertex2f(xr, yt);
      glVertex2f(xr, yb);
   glEnd();
}

void drawWireRect(rec2vector p1, rec2vector p2)
{
   drawWireRect(p1.x, p1.y, p2.x, p2.y);
}

void drawWireRect(rect r)
{
   drawWireRect(r.l, r.t, r.r, r.b);
}

/*
 *
 */
void drawCircle(double x, double y, double r)
{
   double step   = pi / 30,
          finish = 2 * pi - step;

   rec2vector p1, p2;

   for (double angle = 0; angle < finish; angle += step)
   {
      p1.x = x + r * cos(angle); p2.x = x + r * cos(angle + step);
      p1.y = y + r * sin(angle); p2.y = y + r * sin(angle + step);

      drawLine(p1, p2);
   }
}

void drawCircle(rec2vector p, int r)
{
   drawCircle(p.x, p.y, r);
}

/*****************************************END*OF*FILE*********************************************/
