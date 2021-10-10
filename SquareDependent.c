//
//  SquareDependent.c
//  os-5
//  Tests libSquare.dylib 1.0 as a dependent library.
//  Created by Аэлита Лукманова on 24.08.2021.
//

#include <stdio.h>
#include <unistd.h>
#include "Square.h"

int main(int argc, char **argv) {
    printf("[start_test]\n");
 
    int typeOfFigure;
    float side1;
    float side2;
    float result;
    char mode;
    
    printf("Hello, you are in the dependent realization of library with calculating square\n");
    printf("Do you want to change mode to runtime-loaded library? Y/N\n");
    scanf("%s", &mode);
    if (mode == 'Y' || mode == 'y') {
        execv("./SquareRuntime", NULL);
    }
    printf("If you want to know square of rectangle input 1 and if of triangle input 2:\n");
    scanf("%d", &typeOfFigure);
    
    printf("First side:\n");
    scanf("%f", &side1);
    
    printf("Second side:\n");
    scanf("%f", &side2);

    result = squareOfFigure(typeOfFigure, side1, side2);
    if (result == 0) {
        printf("Error: Wrong input");
    } else {
        printf("Result: %f\n", result);
    }
    
    
    printf("[end_test]\n");
    return 0;
}

