/*************************************************************************************************\
*                                                                                                 *
* "graphics.h" - Header file for "graphics.cpp"                                                   *
*                                                                                                 *
*       Author - Tom McDonnell 2003                                                               *
*                                                                                                 *
*         Info : Coordinates are scaled so that top left of window is (x = -1, y = 1),            *
*                and bottom right is (x = 1, y = -1).  Centre is thus (x - 0, y = 0).             *
*                All lengths are similarly scaled.                                                *
*                                                                                                 *
\*************************************************************************************************/

#ifndef GRAPHICS_H
#define GRAPHICS_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include <TomsLibrary/vector.h>
#include <TomsLibrary/geometry.h>

// FUNCTION DECLARATIONS //////////////////////////////////////////////////////////////////////////

void drawLine(double x1, double y1, double x2, double y2);
void drawLine(rec2vector p1,        rec2vector p2       );

void drawArrow(double x, double y, double angle, double length, double hLength);
void drawArrow(rec2vector,         double angle, double length, double hLength);
void drawArrow(rec2vector,         pol2vector,                  double hLength);

void drawRect(double x1, double y1, double x2, double y2);
void drawRect(rec2vector p1,        rec2vector p2       );
void drawRect(rect);

void drawWireRect(double x1, double y1, double x2, double y2);
void drawWireRect(rec2vector p1,        rec2vector p2       );
void drawWireRect(rect);

void drawCircle(double x, double y, double r);
void drawCircle(rec2vector,         double r);

void drawText(double x, double y, char *, int messageLength);
void drawText(rec2vector,         char *, int messageLength);

#endif

/*****************************************END*OF*FILE*********************************************/
