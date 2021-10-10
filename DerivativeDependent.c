#include <stdio.h>
#include <unistd.h>

#include "Derivative.h"


int main(int argc, char **argv) {
    printf("[start_test]\n");
    
    printf("Hello, you are in the dependent realization of library with calculating derivative of cosine\n");
    printf("Do you want to change mode to runtime-loaded library? Y/N\n");
    char libMode;
    scanf("%s", &libMode);
    if (libMode == 'Y' || libMode == 'y') {
        if (execv("./DerivativeRuntime", NULL)) // todo - remove if
        {
            printf("ERROR: Failed to change mode to runtime-loaded library\n");
        }
    }
    
    printf("Input 1 or 2 to calculate derivative of cosine in point A and delta x with 1st way or 2nd way:\n");
    int way;
    if (scanf("%d", &way) != 1 || !(way == 1 || way == 2) ) {
        printf("Wrong input. The number needs to be \"1\"(1st way) or \"2\"(2nd way)\n");
        return 0;
    }
    
    printf("Point A:\n");
    float A;
    if (scanf("%f", &A) != 1) {
        printf("Wrong input. The point needs to be float number\n");
        return 0;
    }
    
    printf("delta x:\n");
    float deltaX;
    if ((scanf("%f", &deltaX) != 1) || (deltaX==0)) {
        printf("Wrong input. Delta needs to be float non zero number\n");
        return 0;
    }
    
    if (way == 1) {
        printf("Result: %f\n", derivativeCosWay1(A, deltaX));
    } else if (way == 2) {
        printf("Result: %f\n", derivativeCosWay2(A, deltaX));
    }
    
    printf("[end_test]\n");
    return 0;
}

