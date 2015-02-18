/* Aviation Calculation - AvCalc API v0.01
   Copyright (C) 2015  Arild M Johannessen
 
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/*
 * main.c
 * -------------------------------------------------------------------------------
 *                          Aviation Formulary V1.46
 *                              By Ed Williams
 *             Converted to C by Arild M Johannessen (February 2015)
 *
 * -------------------------------------------------------------------------------
 * Implementation notice
 * -------------------------------------------------------------------------------
 * East/West longitude is rewritten to conform with the defacto standard
 * used in most other applications throughout the world.
 * North and East is positive numbers, South and West is negative.
 * ------------------------------------------------------------------------------- */

#include <math.h>
#include <Windows.h>  //To be removed when speedtesting is complete
#include <stdio.h>    //To be removed when speedtesting is complete
#include <inttypes.h> //To be removed when speedtesting is complete
#include "main.h"





/*
 *--------------------------------------------------------------------------------
 * Some great circle formulae:
 * ----------------------------------------------------------------------------
 */



/*
 * Distance between points
 *
 * The great circle distance d between two points with coordinates {lat1,lon1}
 * and {lat2,lon2} is given by:
 * returns a pointer to a double containing distance expressed as radians
 * ----------------------------------------------------------------------------
 */
double _stdcall Distance(const double* lat1, const double* lon1, const double* lat2, const double* lon2)
{
	return R2D * 2 * asin(sqrt(pow(sin(D2R*(*lat1-*lat2)/2),2) +
			                   pow(sin(D2R*(*lon2-*lon1)/2),2) * cos(D2R* *lat1) * cos(D2R* *lat2)
							  )
						 );
}


/*----------------------------------------------------------------------------
* Course between points

* We obtain the initial course, tc1, (at point 1) from point 1 to point 2
* by the following. The formula fails if the initial point is a pole. We can
* special case this with as IF statement
*----------------------------------------------------------------------------*/


double _stdcall Course (double *lat1, double *lon1, double *lat2, double *lon2)
{
  if (cos(*lat1) < EPS) {     // EPS a small number ~ machine precision
	  if (*lat1 > 0) {
		  return M_PI;        //  Starting from N pole, return true course south
	  } else {
		  return 2*M_PI;      //  starting from S pole, return true course north
	  }
  } else {
	  // Calculate and return the true course
	  return fmod(
				  atan2(sin(*lon2-*lon1) * cos(*lat2),
						cos(*lat1) * sin(*lat2) - sin(*lat1) * cos(*lat2) * cos(*lon2-*lon1)
					   ),
				  2*M_PI
				 );
  }
}

long long SpeedTest(){
	LARGE_INTEGER StartingTime, EndingTime, Frequency;
	uint64_t  ElapsedNanoseconds1, ElapsedNanoseconds2;
	uint64_t  duration1, duration2;

	QueryPerformanceFrequency(&Frequency);

	unsigned long Repetitions = 20e6;



	double x;
	double lat1 = 69;
	double lon1 = 17;
	double lat2 = 70;
	double lon2 = 18;
	x=0;

	//Single run through
	QueryPerformanceCounter(&StartingTime);
	//procedure to be tested
	x=DistanceD(lat1, lon1, lat2, lon2);
	QueryPerformanceCounter(&EndingTime);
	duration1 = EndingTime.QuadPart - StartingTime.QuadPart;




	//20 mill runthroughs
	QueryPerformanceCounter(&StartingTime);
	unsigned long n;
	for(n = 0;n<Repetitions; n++){
		//procedure to be tested
		x=Distance(lat1, lon1, lat2, lon2);
	}
	QueryPerformanceCounter(&EndingTime);
	duration2 = EndingTime.QuadPart - StartingTime.QuadPart;

	ElapsedNanoseconds1 = 1e9 * duration1;
	ElapsedNanoseconds1 /= Frequency.QuadPart;

	ElapsedNanoseconds2 = 1e9 * duration2;
	ElapsedNanoseconds2 /= Frequency.QuadPart;


	double Average = (double) ElapsedNanoseconds2/(double) Repetitions;
	double RepetitionsD = Repetitions;
	printf("Single  runtime = %.2fns\n",(double) ElapsedNanoseconds1 );
	printf("Average runtime over %.2e repetitions = %.8fns\n",RepetitionsD, Average );

}

int main(){
	SpeedTest();
	return 0;
}
