/*************************************************************************************************\
*                                                                                                 *
*  "main.cpp" - Main file of the 'Racing Line Finder' program.                                    *
*               Contains the main() function plus initialisation and event handling functions.    *
*                                                                                                 *
*      Author - Tom McDonnell 2004                                                                *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "racingline.h"
#include "gui.h"

#include <TomsLibrary/vector.h>
#include <TomsLibrary/geometry.h>

#include <GL/glut.h>

#include <assert.h>
#include <iostream.h>

// GLOBAL VARIABLE DEFINITIONS ////////////////////////////////////////////////////////////////////

rect worldRect = {-1.0, 1.0, 1.0, -1.0}; // definition of max & min world coordinates
                                         // also used in "gui.cpp"

// STATIC GLOBAL VARIABLE DECLARATIONS ////////////////////////////////////////////////////////////

static int mainWindowID;

static rec2vector mainWinDim(600, 600);

// STATIC FUNCTION DECLARATIONS ///////////////////////////////////////////////////////////////////

static void initLightSources(void);
static void display(void);
static void reshape(int w, int h);
static void mouse(int button, int state, int x, int y);
static void mousePassiveMove(int x, int y);
static void setupBirdsEyeView(void);
static void scaleWindowToWorld(rec2vector &);

// FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////////////////

/*
 *
 */
void main(int argc, char *argv[])
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
   
   // create main window
   glutInitWindowSize(mainWinDim.x, mainWinDim.y);
   glutInitWindowPosition(0, 0);
   mainWindowID = glutCreateWindow("Racing Line Finder");

   // register functions for main window
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc(mouse);
   glutPassiveMotionFunc(mousePassiveMove);

   // settings for main window
   //glEnable(GL_DEPTH_TEST);

   initGUI();

   glutMainLoop();
}

// STATIC FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 *
 */
static void display(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   drawWindow();

   setupBirdsEyeView();

   glutSwapBuffers();
}

/*
 * PROBLEM: Unfinished function.
 */
static void reshape(int w, int h)
{
   mainWinDim.x = w;
   mainWinDim.y = h;

   glutPostRedisplay();
}

/*
 *
 */
static void mouse(int button, int state, int x, int y)
{
   rec2vector mousePos(x, y);
   scaleWindowToWorld(mousePos);

   switch (button)
   {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN)
      {
         processLeftClick(mousePos);
      }
      break;
   }
}

/*
 *
 */
static void mousePassiveMove(int x, int y)
{
   rec2vector mousePos(x, y);
   scaleWindowToWorld(mousePos);
   processMouseMove(mousePos);
}

/*
 * Set up modelview matrix for viewing from birds eye perspective (from overhead).
 */
static void setupBirdsEyeView(void)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   double halfW = (worldRect.r - worldRect.l) / 2.0,
          halfH = (worldRect.t - worldRect.b) / 2.0;
   glOrtho( -halfW, halfW,   // left, right
            -halfH, halfH,   // top, bottom
               0.0,   2.0 ); // near, far

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(halfW + worldRect.l, halfH + worldRect.b, 1.0,   // look from
             halfW + worldRect.l, halfH + worldRect.b, 0.0,   // look at
                             0.0,                 1.0, 0.0 ); // up vector
}

/*
 * Scale point p from window coordinates to world coordinates.
 */
static void scaleWindowToWorld(rec2vector &p)
{
   assert(0 <= p.x && p.x <= mainWinDim.x);
   assert(0 <= p.y && p.y <= mainWinDim.y);

   p.x = ((worldRect.r - worldRect.l) *                 p.x)  / mainWinDim.x + worldRect.l;
   p.y = ((worldRect.t - worldRect.b) * (mainWinDim.y - p.y)) / mainWinDim.y + worldRect.b;
}

/*******************************************END*OF*FILE*******************************************/
