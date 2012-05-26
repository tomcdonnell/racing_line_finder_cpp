/*************************************************************************************************\
*                                                                                                 *
* "gui.cpp" -                                                                                     *
*                                                                                                 *
*    Author - Tom McDonnell 2003                                                                  *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "gui.h"
#include "racingline.h"

#include <TomsLibrary/geometry.h>

#include <GL/glut.h>

#include <math.h>
#include <assert.h>
#include <iostream.h>

// EXTERNAL GLOBAL VARIABLE DECLARATIONS //////////////////////////////////////////////////////////

extern rect worldRect; // max and min world coordinates
                       // defined in "main.cpp"

// STATIC GLOBAL VARIABLES ////////////////////////////////////////////////////////////////////////

static enum {moveTrkE, moveTrkC, moveTrkX, moveTrkW, normal} mouseMoveMode = normal;

// variables defining racetrack corner
static rec2vector trkE(-0.5, -1.0), // track screen entry  point (default)
                  trkC(-0.5,  0.5), // track corner centre point (default)
                  trkX( 1.0,  0.5), // track screen exit   point (default)
                  trkA;             // track apex
static double     trkW = 0.05;      // track width

// variables defining racing line
static racingLine rL[5];
static       int   nRacingLines;
static const int maxRacingLines = 5;

// variables defining buttons
static rec2vector buttonDim;            // default button dimensions

static rect  simBut,  clrBut,  accBut,  // simulate, clear & acceleration direction buttons
             trkButE, trkButC, trkButX, // track entry, centre, & exit buttons
             trkButW;                   // track width slider button

// STATIC FUNCTION DECLARATIONS ///////////////////////////////////////////////////////////////////

static void drawRaceTrack(void);

// FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////////////////

/*
 *
 */
void initGUI()
{
   rec2vector worldDim(worldRect.r - worldRect.l, worldRect.t - worldRect.b),
               accButCentre(   0.85 * worldDim.x + worldRect.l, 0.20 * worldDim.y + worldRect.b),
               clrButCentre(accButCentre.x - 0.10 * worldDim.x, 0.10 * worldDim.y + worldRect.b),
               simButCentre(accButCentre.x + 0.10 * worldDim.x,                  clrButCentre.y),
              trkWButCentre(                    accButCentre.x, 0.05 * worldDim.y + worldRect.b);

   buttonDim = 0.01 * worldDim;

   // calculate dimensions and position of racing line selection buttons
   surrPointWithRect( accButCentre, 0.1 *  worldDim, accBut );
   surrPointWithRect( clrButCentre, 1.5 * buttonDim, clrBut );
   surrPointWithRect( simButCentre, 1.5 * buttonDim, simBut );
   surrPointWithRect(trkWButCentre,       buttonDim, trkButW);

   // calculate dimensions and initial position of track entry, centre, & exit buttons
   surrPointWithRect(trkE, buttonDim, trkButE);
   surrPointWithRect(trkC, buttonDim, trkButC);
   surrPointWithRect(trkX, buttonDim, trkButX);
  
   nRacingLines = 0;
}

/*
 *
 */
void drawWindow(void)
{
   // draw x, y axes
   glColor3f(0.2, 0.2, 0.2);
   drawLine( 0.0, -1.0, 0.0, 1.0);
   drawLine(-1.0,  0.0, 1.0, 0.0);

   // draw racetrack
   drawRaceTrack();

   // draw acc button
   double r = (accBut.r - accBut.l) / 2;
   drawCircle(accBut.l + r, accBut.t - r, r);

   // draw bounding lines for valid angle selection
   drawArrow(accBut.l + r, accBut.b + r, angle(trkC, trkE), r, 0);
   drawArrow(accBut.l + r, accBut.b + r, angle(trkC, trkX), r, 0);
   
   // draw simulate button
   drawRect(simBut);

   // draw clear button
   drawRect(clrBut);

   // draw track width slider button
   drawRect(trkButW);
   drawLine(0.5, trkButW.b + 0.02, 0.9, trkButW.b + 0.02);

   // draw track movement buttons
   glColor3f(1.0, 0.0, 0.0);
   drawWireRect(trkButE);
   drawWireRect(trkButC);
   drawWireRect(trkButX);
}

/*
 *
 */
void processLeftClick(rec2vector mousePos)
{
   assert(worldRect.l <= mousePos.x && mousePos.x <= worldRect.r);
   assert(worldRect.b <= mousePos.y && mousePos.y <= worldRect.t);

   // return to normal mode if modifying corner
   if (mouseMoveMode != normal)
   {
      mouseMoveMode = normal;
      return;
   }

   // test for button clicks
   enum {clr, sim, acc, trkEb, trkCb, trkXb, trkWb, none} butPressed;
        if (insideRect(mousePos, clrBut )) butPressed = clr;
   else if (insideRect(mousePos, simBut )) butPressed = sim;
   else if (insideRect(mousePos, accBut )) butPressed = acc;
   else if (insideRect(mousePos, trkButE)) butPressed = trkEb;
   else if (insideRect(mousePos, trkButC)) butPressed = trkCb;
   else if (insideRect(mousePos, trkButX)) butPressed = trkXb;
   else if (insideRect(mousePos, trkButW)) butPressed = trkWb;
   else                                    butPressed = none;

   // deal with button clicks
   switch (butPressed)
   {
    case clr:
      nRacingLines = 0;
      break;

    case sim:
      syncRacingLines(rL, nRacingLines);
      plotRacingLines(rL, nRacingLines);
      break;

    case acc:
      if (nRacingLines < maxRacingLines)
      {
         double r = (accBut.r - accBut.l) / 2;
         rL[nRacingLines].setF(atan2(mousePos.y - (accBut.t - r), mousePos.x - (accBut.l + r)));

         //drawArrow(accBut.l + r, accBut.t - r, rL[nRacingLines].getF(), r, 0.05);

         line lineE = findLineEquation(trkE, trkC);
         line lineX = findLineEquation(trkC, trkX);

         if (rL[nRacingLines].fitToCorner(lineE, lineX, trkA))
         {
            //rL[nRacingLines].plot();
            ++nRacingLines;
         }
      }
      break;

    case trkEb:
      if (mouseMoveMode != moveTrkE)
      {
         mouseMoveMode = moveTrkE;
         nRacingLines  = 0;
      }
      break;

    case trkCb:
      if (mouseMoveMode != moveTrkC)
      {
         mouseMoveMode = moveTrkC;
         nRacingLines  = 0;
      }
      break;

    case trkXb:
      if (mouseMoveMode != moveTrkX)
      {
         mouseMoveMode = moveTrkX;
         nRacingLines  = 0;
      }
      break;

    case trkWb:
      if (mouseMoveMode != moveTrkW)
      {
         mouseMoveMode = moveTrkW;
         nRacingLines  = 0;
      }
      break;
   }
}

/*
 *
 */
void processMouseMove(rec2vector mousePos)
{
   assert(worldRect.l <= mousePos.x && mousePos.x <= worldRect.r);
   assert(worldRect.b <= mousePos.y && mousePos.y <= worldRect.t);

   rec2vector temp;

   switch (mouseMoveMode)
   {
    case moveTrkE:
      temp = lineIntersectRect(trkC, mousePos, worldRect); // find new track screen entry point
      if (temp != trkX) trkE = temp;                       // avoid situation where trkE = trkX
      surrPointWithRect(trkE, buttonDim, trkButE);         // update track screen entry button
      glutPostRedisplay();
      break;

    case moveTrkC:
      trkC = mousePos;                                     // update track corner centre point
      surrPointWithRect(trkC, buttonDim, trkButC);         // update track corner centre button
      glutPostRedisplay();
      break;

    case moveTrkX:
      temp = lineIntersectRect(trkC, mousePos, worldRect); // find new track screen exit point
      if (temp != trkE) trkX = temp;                       // avoid situation where trkE = trkX
      surrPointWithRect(trkX, buttonDim, trkButX);         // update track screen exit button
      glutPostRedisplay();
      break;

    case moveTrkW:
      if      (mousePos.x < 0.5) trkW = 0.01;
      else if (mousePos.x > 0.9) trkW = 0.20;
      else                       trkW = (0.19 * (mousePos.x - 0.5)) / (0.9 - 0.5) + 0.01;
      glutPostRedisplay();
      break;

    case normal:
      // do nothing
      break;
   }
}

// STATIC FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 *
 */
static void drawRaceTrack()
{
   // find apex of corner
   rec2vector eRec, xRec, trkCtoArec;
   pol2vector ePol, xPol, trkCtoApol;

   // create unit vectors in direction of entry and exit track segments
   eRec   = trkC - trkE;     xRec   = trkX - trkC;
   ePol   = convToPol(eRec); xPol   = convToPol(xRec);
   ePol.r = 1.0;             xPol.r = 1.0;
   eRec   = convToRec(ePol); xRec   = convToRec(xPol);

   // find corner angle
   // check that result is in range  (pi -> -pi)
   double angle = ePol.angle - xPol.angle;
   if      (angle < -pi) angle += 2 * pi;
   else if (angle >  pi) angle -= 2 * pi;

   // create vector in direction trkC->trkA
   trkCtoArec   = xRec - eRec;
   trkCtoApol   = convToPol(trkCtoArec);
   trkCtoApol.r = trkW / cos(angle / 2.0);
   trkCtoArec   = convToRec(trkCtoApol);

   // define track border points.
   // eg. IEoffScr = point where Inside Entry line meets Screen boundary
   //     OXoffScr = point where Outside eXit line meets Screen boundary
   //     IEXmeet  = point where Inside Entry and eXit lines meet
   rec2vector IEXmeet  = trkC + trkCtoArec,
              IEoffScr = lineIntersectRect(IEXmeet, IEXmeet - eRec, worldRect),
              IXoffScr = lineIntersectRect(IEXmeet, IEXmeet + xRec, worldRect),
              OEXmeet  = trkC - trkCtoArec,
              OEoffScr = lineIntersectRect(OEXmeet, OEXmeet - eRec, worldRect),
              OXoffScr = lineIntersectRect(OEXmeet, OEXmeet + xRec, worldRect);

   // draw corner
   glColor3f(0.3, 0.3, 0.3);
   glBegin(GL_QUADS);
     glVertex3f(IEoffScr.x, IEoffScr.y, 0.0);
     glVertex3f(OEoffScr.x, OEoffScr.y, 0.0);
     glVertex3f( OEXmeet.x,  OEXmeet.y, 0.0);
     glVertex3f( IEXmeet.x,  IEXmeet.y, 0.0);

     glVertex3f(IXoffScr.x, IXoffScr.y, 0.0);
     glVertex3f(OXoffScr.x, OXoffScr.y, 0.0);
     glVertex3f( OEXmeet.x,  OEXmeet.y, 0.0);
     glVertex3f( IEXmeet.x,  IEXmeet.y, 0.0);
   glEnd();

   glColor3f(1.0, 1.0, 1.0);
   drawLine(trkE, trkC);
   drawLine(trkC, trkX);
}

/*****************************************END*OF*FILE*********************************************/
