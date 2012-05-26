/*************************************************************************************************\
*                                                                                                 *
* "racingline.cpp" -                                                                              *
*                                                                                                 *
*           Author - Tom McDonnell 2003                                                           *
*                                                                                                 *
\*************************************************************************************************/

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "racingline.h"
#include "graphics.h"

#include <TomsLibrary/misc.h>
#include <TomsLibrary/geometry.h>

#include <GL/glut.h>

#include <assert.h>
#include <math.h>

// FUNCTION DECLARATIONS //////////////////////////////////////////////////////////////////////////

//void wait(int); // defined in "main.cpp"

// PUBLIC FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 *
 */
bool racingLine::fitToCorner(line eL, line xL, rec2vector apex)
{
   bool success = false;

   if (w == 0)
   {
      switch (whichLinesYeqMxPlusC(eL, xL))
      {
       case both:
         success = fitToCornerWzeroBothYeqMxPlusC(eL, xL, apex);
         break;

       case neither:
         // same function (fitToCornerWzeroBothYeqMxPlusC()) can be used
         // after mirroring all coordinates & vectors about the line y = x

         eL.rep = YeqMxPlusC; xL.rep = YeqMxPlusC;
         swap(apex.x, apex.y);
         f = -f + pi / 2; // mirror angle about y = x line

         success = fitToCornerWzeroBothYeqMxPlusC(eL, xL, apex);

         f = -f + pi / 2; // mirror angle about y = x line
         swap(E.x, E.y); swap(vE.x, vE.y);
         swap(X.x, X.y); swap(vX.x, vX.y);
         break;

       case second:
         success = fitToCornerWzeroExitYeqMxPlusC(eL, xL, apex);
         break;

       case first:
         // same function (fitToCornerWzeroExitYeqMxPlusC()) can be used
         // after mirroring all coordinates & vectors about the line y = x

         eL.rep = XeqMyPlusC; xL.rep = YeqMxPlusC;
         swap(apex.x, apex.y);
         f = -f + pi / 2; // mirror angle about y = x line

         success = fitToCornerWzeroExitYeqMxPlusC(eL, xL, apex);

         f = -f + pi / 2; // mirror angle about y = x line
         swap(E.x, E.y); swap(vE.x, vE.y);
         swap(X.x, X.y); swap(vX.x, vX.y);
         break;
      }
   }

   return success;
}

/*
 *
 */
void racingLine::plot(void)
{
   rec2vector oldPos, newPos;
   rec2vector oldV,   newV;

   double dt = (tX - tE) / 150.0;

   oldPos =  E;
   oldV   = vE;

   // scaling
   oldV = oldV * 0.1;

   for (double t = tE; t <= tX; t += dt)
   {
      newPos.x = (A / 2.0) * cos(f) * pow(t, 2) + vE.x * t + E.x;
      newPos.y = (A / 2.0) * sin(f) * pow(t, 2) + vE.y * t + E.y;

      if (   -1.0 <= newPos.x && newPos.x <= 1.0
          && -1.0 <= newPos.y && newPos.y <= 1.0)
      {
         // draw acceleration vector arrow
         glColor3f(0.0, 0.0, 0.0); drawArrow(oldPos.x, oldPos.y, f, 0.07 * A, 0.03);
         glColor3f(1.0, 0.0, 0.0); drawArrow(newPos.x, newPos.y, f, 0.07 * A, 0.03);

         // calculate velocity vector
         newV.x = A * cos(f) * t + vE.x;
         newV.y = A * sin(f) * t + vE.y;

         // scaling
         newV = newV * 0.1;

         // draw velocity vector arrow
         glColor3f(0.0, 0.0, 0.0); drawArrow(oldPos, convToPol(oldV), 0.03);
         glColor3f(1.0, 0.0, 0.0); drawArrow(newPos, convToPol(newV), 0.03);

         // draw racing line segment
         drawLine(oldPos, newPos);

         glutSwapBuffers();
      }

      oldPos = newPos;
      oldV   = newV;

      //wait(4);
   }
}

// PRIVATE FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////////

/*
 *
 */
bool racingLine::fitToCornerWzeroBothYeqMxPlusC(line eL, line xL, rec2vector apex)
{
   // tApex (intermediate variable) calculated first to simplify later expressions
   double temp = (2 * (apex.x * eL.m - apex.y + eL.c)) / (A * (eL.m * cos(f) - sin(f)));

   if (temp < 0.0) // avoid taking root of negative number
     return false; // calculation has failed because f out of range
                   // or apex on wrong side (convex) of corner
                   // NOTE: this will not catch error where f out of range but still
                   //       possible to pass through apex by accelerating in f direction

   double tApex = sqrt(temp);

   // some simple intermediate calculations to simplify complex expressions later
   double ASinMinMxCos = A * (sin(f) - xL.m * cos(f)),
          mXminMe     = xL.m - eL.m,
          cEminCx     = eL.c - xL.c;

   // (START) solving polynomial (0 = a * tX + b * tX + c) for tX //////////////////////////////
   double a =  -0.5 * (ASinMinMxCos / mXminMe),
          b = tApex * (ASinMinMxCos / mXminMe),
          c = (A / 2.0) * cos(f) * pow(tApex, 2) + cEminCx / mXminMe - apex.x;

   // find which solution of polynomial to use
   // if approach corner from right of screen, sign +.
   // if approach corner from left  of screen, sign -.
   int sign;
   if ((eL < apex && apex < xL) || (xL < apex && apex < eL))
   {
      // corner is acute  angled (because also both lines are YeqMxPlusC)
      // and (entering and leaving) from either the left or right
      if (apex.x > intersection(eL, xL).x) sign =  1; // approach from right
      else                                 sign = -1; // approach from left
   }
   else
   {
      // corner is obtuse angled (because also both lines are YeqMxPlusC)
      // and (entering and leaving) from either the top or bottom
      line bL; // top or bottom screen border line
      if (apex.y > intersection(eL, xL).y) bL.set(0.0,  1.0, YeqMxPlusC); // top    y = 0.0x + 1.0
      else                                 bL.set(0.0, -1.0, YeqMxPlusC); // bottom y = 0.0x - 1.0

      // need to find which direction (left or right) approach to corner is from
      if (   eL.m <= 0.0 && xL.m > 0.0
          || xL.m <= 0.0 && eL.m > 0.0)
      {
         // signs of gradients are different
         if (intersection(eL, bL).x > intersection(xL, bL).x) sign =  1; // approach from right
         else                                                 sign = -1; // approach from left
      }
      else
      {
         // signs of gradients are same
         if (intersection(eL, bL).x > intersection(xL, bL).x) sign = -1; // approach from left
         else                                                 sign =  1; // approach from right
      }
   }

   // solve polynomial
   temp = pow(b, 2) - 4 * a * c; // determinant

   if (temp < 0.0) // avoid taking root of negative number
     return false; // calculation has failed because f out of range
                   // NOTE: this will catch error where f out of range but still
                   //       possible to pass through apex by accelerating in f direction

   tX = (-b + sign * sqrt(temp)) / (2 * a);
   // (FINISH) done solving polynomial /////////////////////////////////////////////////////////

   // calculate rest of required variables
   tE   = 0;
    E.x = (-0.5 * pow(tX, 2) * ASinMinMxCos + cEminCx) / mXminMe;
    E.y = eL.m * E.x + eL.c;
   vE.x = (tX * ASinMinMxCos) / mXminMe;
   vE.y = eL.m * vE.x;
    X.x = (A / 2.0) * cos(f) * pow(tX, 2) + vE.x * tX + E.x;
    X.y = (A / 2.0) * sin(f) * pow(tX, 2) + vE.y * tX + E.y;
   vX.x = A * cos(f) * tX + vE.x;
   vX.y = A * sin(f) * tX + vE.y;

   return true;
}

/*
 *
 */
bool racingLine::fitToCornerWzeroExitYeqMxPlusC(line eL, line xL, rec2vector apex)
{
   // tApex (intermediate variable) calculated first to simplify later expressions
   double temp = (2 * (apex.y * eL.m - apex.x + eL.c)) / (A * (eL.m * sin(f) - cos(f)));

   if (temp < 0.0) // avoid taking root of negative number
     return false; // calculation has failed because f out of range
                   // or apex on wrong side (convex) of corner
                   // NOTE: this will not catch error where f out of range but still
                   //       possible to pass through apex by accelerating in f direction

   double tApex = sqrt(temp);

   // some simple intermediate calculations to simplify complex expressions later
   double ASinMinMxCos = A * (sin(f) - xL.m * cos(f)),
          mXmEminOne  = xL.m * eL.m - 1,
          mXcEplusCx  = xL.m * eL.c + xL.c;

   // (START) solving polynomial (0 = a * tX + b * tX + c) for tX //////////////////////////////
   double a =  -0.5 * (ASinMinMxCos / mXmEminOne),
          b = tApex * (ASinMinMxCos / mXmEminOne),
          c = (A / 2.0) * sin(f) * pow(tApex, 2) - mXcEplusCx / mXmEminOne - apex.y;

   // find which solution of polynomial to use
   // if approach corner from top    of screen, sign +.
   // if approach corner from bottom of screen, sign -.
   int sign;
   if (apex.y > intersection(eL, xL).y) sign =  1;
   else                                 sign = -1;

   // solve polynomial
   temp = pow(b, 2) - 4 * a * c; // determinant

   if (temp < 0.0) // avoid taking root of negative number
     return false; // calculation has failed because f out of range
                   // NOTE: this will catch error where f out of range but still
                   //       possible to pass through apex by accelerating in f direction

   tX = (-b + sign * sqrt(temp)) / (2 * a);
   // (FINISH) done solving polynomial /////////////////////////////////////////////////////////

   // calculate rest of required variables
   tE   = 0;
    E.y = (-0.5 * pow(tX, 2) * ASinMinMxCos - mXcEplusCx) / mXmEminOne;
    E.x = eL.m * E.y + eL.c;
   vE.y = (tX * ASinMinMxCos) / mXmEminOne;
   vE.x = eL.m * vE.y;
    X.y = (A / 2.0) * sin(f) * pow(tX, 2) + vE.y * tX + E.y;
    X.x = (A / 2.0) * cos(f) * pow(tX, 2) + vE.x * tX + E.x;
   vX.y =  A * sin(f) * tX + vE.y;
   vX.x =  A * cos(f) * tX + vE.x;

   return true;
}

// FRIEND FUNCTION DEFINITIONS ////////////////////////////////////////////////////////////////////

/*
 * Syncronise two or more racing lines to enable direct comparison.
 * The racers will all start (with zero velocity) somewhere along the
 * straight line track section leading to the corner being considered.
 * The starting point will be calculated so that one of the racers will reach
 * its entry point at its entry speed exactly (ie. it will not need to brake).
 * Starting positions (start), braking times (tB), entry times (tE),
 * and exit times (tB) are calculated for each racing line.
 * Starting time is assumed to be 0.
 *
 * It is assumed that for each racingLine supplied, fitRacingLine() already been called.
 * Once the racing lines have been syncronised, they may be plotted together
 * by calling plotRacingLines().
 */
void syncRacingLines(racingLine rl[5], const int n)
{
   int sign,
       i;    // counter

   // select racing line which will not need to brake before entering the corner
   int s = 0; // selection
   for (i = 1; i < n; ++i)
   {
      if (fabs(rl[i].vE.y) > fabs(rl[s].vE.y))
        s = i;
   }

   // find starting position (y0) (y0 will be the same for each racing line)
   if (rl[s].E.y > 0) sign =  1;
   else               sign = -1;
   double startPos = rl[s].E.y + sign * 0.5 * pow(rl[s].vE.y, 2);            // eqn 1

   for (i = 0; i < n; ++i)
   {
     // find y0  (starting position) (y0 will be the same for each racing line)
     rl[i].start.y = startPos;

     // find tB  (braking times)
     if (rl[i].E.y > 0) sign = -1;
     else               sign =  1;
     rl[i].tB  = sqrt(fabs(pow(rl[i].vE.y, 2) / 2 + sign * (rl[i].E.y - startPos))); // eqn 9

     // find yB  (braking positions)
     rl[i].B.y  = sign * (0.5 * pow(rl[i].tB, 2) + sign * rl[i].start.y);

     // find vyB (braking velocities)
     rl[i].vB.y = sign * rl[i].tB;

     // find tE  (entry times)
     rl[i].tE  = 2 * rl[i].tB - sign * rl[i].vE.y;                          // eqn 10

     // find tX  (exit times)
     rl[i].tX  = rl[i].tX + rl[i].tE;
   }
}

/*
 *
 */
int plotRacingLines(racingLine rl[5], const int n)
{
   rec2vector oldPos[5], newPos[5];

   double dt, tEnd;
   int    sign,
          i;

   // set time increment
   dt = 0.01; // maximum which results in smooth racing line (trial & error)

   // set end time
   tEnd = 1000;

   // initialise oldX, oldY
   for (i = 0; i < n; ++i)
   {
      oldPos[i].x = rl[i].E.x;     // for right angled corner, start.x will be same as E.x
      oldPos[i].y = rl[i].start.y;
   }

   // plot racing lines
   for (double t = 0; t <= tEnd; t += dt)
   {
      for (i = 0; i < n; ++i)
      {
         if (0 <= t && t <= rl[i].tB)
         {
            // accelerating in phase

            if (rl[i].vE.y > 0) sign =  1;
            else                sign = -1;

            newPos[i].x = rl[i].E.x;
            newPos[i].y = sign * 0.5 * pow(t, 2) + rl[i].start.y;
         }
         else if (rl[i].tB < t && t <= rl[i].tE)
         {
            // braking phase

            if (rl[i].vE.y > 0) sign = -1;
            else                sign =  1;

            newPos[i].x = rl[i].E.x;
            newPos[i].y = sign * 0.5 * pow((t - rl[i].tB), 2) + rl[i].vB.y * (t - rl[i].tB) + rl[i].B.y;
         }
         else if (rl[i].tE < t && t <= rl[i].tX)
         {
            // cornering phase
            newPos[i].x = 0.5 * cos(rl[i].f) * pow((t - rl[i].tE), 2) + rl[i].vE.x * (t - rl[i].tE) + rl[i].E.x;
            newPos[i].y = 0.5 * sin(rl[i].f) * pow((t - rl[i].tE), 2) + rl[i].vE.y * (t - rl[i].tE) + rl[i].E.y;
         }
         else if (rl[i].tX < t && t <= tEnd)
         {
            // accelerating out phase
            if (rl[i].vX.x > 0) sign =  1;
            else                sign = -1;

            newPos[i].x = sign * 0.5 * pow((t - rl[i].tX), 2) + rl[i].vX.x * (t - rl[i].tX) + rl[i].X.x;
            newPos[i].y = rl[i].X.y;
         }

         // detect winner
         if (newPos[i].x > 1.0)
           return i;

         if (   -1.0 < newPos[i].x && newPos[i].x < 1.0
             && -1.0 < newPos[i].y && newPos[i].y < 1.0)
         {
            // draw racing line segment
            drawLine(oldPos[i], newPos[i]);
         }

         oldPos[i] = newPos[i];

         //wait(4);
      }
   }
   return -1; // indicates error
}

/*****************************************END*OF*FILE*********************************************/
