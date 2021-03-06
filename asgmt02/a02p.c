#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RND_CTL_BIT_SHIFT   10

// floating point rounding modes: IA-32 Manual, Vol. 1, p. 4-20
typedef enum {
    ROUND_NEAREST_EVEN =    0 << RND_CTL_BIT_SHIFT,
    ROUND_MINUS_INF =       1 << RND_CTL_BIT_SHIFT,
    ROUND_PLUS_INF =        2 << RND_CTL_BIT_SHIFT,
    ROUND_TOWARD_ZERO =     3 << RND_CTL_BIT_SHIFT
} RoundingMode;

// do not change anything above this comment

double roundD(double n, RoundingMode roundingMode)
{
	// here you will set the FPU rounding mode as specified,
	// and then round n to the nearest integer in the specified mode
    asm (
            "fstcw  %%ax                    \n"
            "fstcw  %%bx                    \n"
            "mov    %[roundingMode], %%cx   \n"
            "or     %%cx, %%ax              \n"
            "fldcw  postconv                \n"
            "fistp %[nin]                   \n"
            "fild  %[nout]                  \n"
            "fldcw  postconv                \n"
            :[nout] "=m"    (n)
            :[nin]  "m"     (n),
             [roundingMode] "m" (round)
            : "ax", "bx"
        );
    return 0.0;
}

double hullSpeed(double lgth)
{
	// here you will compute the hull speed for the specified boat length,
	// implementing the numerical computation in GNU Assembly Language
	// through use of FPU instructions (do NOT call sqrt() from math.h)
    double hullSpeed = 1.0;
    double hullLength = lgth;
    double multForHullSpeed = 1.34;
    asm (
            "fldl   %[hullLengthIn]                 \n"
            "fsqrt                                  \n"
            "fldl   %[hullConst]                    \n"
            "fmulp                                  \n"
            "fstl    %[hullSpeedOut]                 \n"

            :[hullSpeedOut] "=m" (hullSpeed)
            :[hullLengthIn] "m" (hullLength),
             [hullConst]    "m" (multForHullSpeed)
        );
	return hullSpeed;
}

// do not change anything below this comment, except for printing out your name

double hullSpeedC(double lgth)
{
	// this C implementation of the hull speed computation is provided
	// so you can compare its correct results with the results of your code

	return 1.34 * sqrt(lgth);
}

void printRounded(char *label, RoundingMode roundingMode,
				  double lgth, double hullSpd, double hullSpdC)
{
	printf("%s hullSpeed(%.0f) = %.0f, %.0f\n",
		   label, lgth,
		   roundD(hullSpd,  roundingMode),
		   roundD(hullSpdC, roundingMode));
}

int main (int argc, char **argv)
{
	double	lgth;
	double	hullSpd, hullSpdC;

	printf("CS201 - Assignment 02 - David Hartman\n");
	if (argc != 2) {
		printf("need 1 argument: boat length\n");
		return -1;
		}
	lgth = atof(argv[1]);
	hullSpd = hullSpeed(lgth);
	hullSpdC = hullSpeedC(lgth);

	printf("hullSpeed(%.0f) = %.3f, %.3f\n", lgth, hullSpd, hullSpdC);
	printRounded("round even:", ROUND_NEAREST_EVEN, lgth, hullSpd, hullSpdC);
	printRounded("round down:", ROUND_MINUS_INF,    lgth, hullSpd, hullSpdC);
	printRounded("round up:  ", ROUND_PLUS_INF,     lgth, hullSpd, hullSpdC);
	printRounded("round zero:", ROUND_TOWARD_ZERO,  lgth, hullSpd, hullSpdC);

	return 0;
}
