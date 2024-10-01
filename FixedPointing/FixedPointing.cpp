// FixedPointing.cpp : Defines the entry point for the console application.
//
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// This can be increased if the run through the number space is taking too long
#define INCREMENT 1
// Stop if this is hit, to save time
#define MAX_ERROR_PERCENT 10
// Print out when this threshold of error occurs
#define PRINT_ERROR_PERCENT 1
// Print at least this many results so that we can see how things are looking
#define PRINT_AT_LEAST 10

// The fixed-pooint operation
#define FIXED_POINT(x) (((uint64_t)(x) * 1000000) >> 15)
// the inverse of the fixed point operation, needed for checking overflow
#define FIXED_POINT_INVERT(x) (((uint64_t)(x) << 15) / 1000000)

int main()
{
    float fNumerator = 1000000;
    float fDenominator = 32768;
    float fResult;
    uint32_t u32Result;
    float fError = 0;
    float fErrorPercent = 0;
    float fWorstError = 0;
    float fWorstResult = 0;
    uint32_t u32WorstResult = 0;
    uint32_t u32WorstX = 0;
    uint32_t u32BiggestNumber = 0xFFFFFFFF;
    uint32_t x;
    bool printIt = false;
    uint32_t nextPrint = 0;

    // Work out what the biggest number that can be represented without overflow is
    x = u32BiggestNumber;
    if ((uint64_t) u32BiggestNumber * fNumerator / fDenominator > u32BiggestNumber)
    {
        u32BiggestNumber = (uint32_t) (u32BiggestNumber * fDenominator / fNumerator);
    }
    if (FIXED_POINT(x) > x)
    {
        x *= FIXED_POINT_INVERT(x);
    }
    if (x < u32BiggestNumber)
    {
        u32BiggestNumber = x;
    }

    nextPrint = u32BiggestNumber / PRINT_AT_LEAST;
    printf ("Multiply by %.0f, divide by %.0f, fixed point version '(uint64_t) * 1000000 >> 15', max number that won't overflow is %u (0x%x).\n\n", fNumerator, fDenominator, u32BiggestNumber, u32BiggestNumber);
    
    // Go through the number space and check what the errors look like
    for (x = 0; (x < 0xFFFFFFFF - INCREMENT) && (x < u32BiggestNumber) && (fErrorPercent < MAX_ERROR_PERCENT); x += INCREMENT)
    {
        fResult = ((float) x) * fNumerator / fDenominator;
        u32Result = (uint32_t) FIXED_POINT(x);
        fError = abs(fResult - u32Result);
        if ((fWorstResult == 0) || ((fError * 100 / fResult) > (fWorstError * 100 / fWorstResult)))
        {
            fWorstError = fError;
            u32WorstX = x;
            fWorstResult = fResult;
            u32WorstResult = u32Result;
            printIt = true;
        }

        if (fResult > 0)
        {
            fErrorPercent = fError * 100 / fResult;
        }

        if (printIt || x > nextPrint || fErrorPercent > PRINT_ERROR_PERCENT)
        {
            printIt = false;
            printf ("%3u * %.6f = float %f or fixed point %3u", x, fNumerator / fDenominator, fResult, u32Result);
            if (fResult > 0)
            {
                printf (" error %f (%f%%).\n", fError, fErrorPercent);
            }
            else
            {
                printf (".\n");
            }

            if (x > nextPrint)
            {
                 nextPrint += u32BiggestNumber / PRINT_AT_LEAST;
            }
        }
    }

    printf ("\n");

    if (fErrorPercent >= MAX_ERROR_PERCENT)
    {
        printf ("Stopped at %u 'cos of hitting worst case error (%f%% when the limit is %u).\n", x, fErrorPercent, MAX_ERROR_PERCENT);
    }
    printf ("Worst case error: %.6f (%f%%) for input %u (0x%x), where result should have been %.6f but was %u.\n", fWorstError, fWorstError * 100 / fWorstResult, u32WorstX, u32WorstX, fWorstResult, u32WorstResult);

	return 0;
}