/*************************************************************************************************\
*                                                                                                 *
* "racingline.h" -                                                                                *
*                                                                                                 *
*         Author - Tom McDonnell 2003                                                             *
*                                                                                                 *
\*************************************************************************************************/

#ifndef RACINGLINE_H
#define RACINGLINE_H

// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include "graphics.h"

#include <TomsLibrary/geometry.h>

// TYPE DEFINIIONS ////////////////////////////////////////////////////////////////////////////////

/*
 *
 */
class racingLine
{
 public:
   racingLine(void)       {A = 1.0; w = 0.0; f = 0.0;}

   void   setF(double f1) {f = f1;}

   double getF(void)      {return f;}

   bool fitToCorner(line eL, line xL, rec2vector apex);

   void plot(void);

   friend void syncRacingLines(racingLine [5], const int);
   friend int  plotRacingLines(racingLine [5], const int);

 private:
   bool fitToCornerWzeroBothYeqMxPlusC(line, line, rec2vector);
   bool fitToCornerWzeroExitYeqMxPlusC(line, line, rec2vector);

   double  w,   f, // variables defining how acceleration angle varies with time (ang = w + ft)
           A,      // magnitude of acceleration;
          tE,  tX, // time of corner entry and exit
          tB;      // (*) time when braking for corner begins

   rec2vector  E,  X, // position at t = tE, tX
              vE, vX, // velocity at t = tE, tX
               B,     // (*) position when braking for corner begins
              vB,     // (*) velocity when braking for corner begins
              start;  // (*) starting position from zero velocity on straight before corner
                      // NOTE: (*) signifies variables used only for syncronising two or more
                      //           racing lines through the same corner so that they start at
                      //           the same time and in the same position.
};

#endif

/*****************************************END*OF*FILE*********************************************/
