//
//  SquareRuntime.c
//  os-5
// Tests libSquare.dylib 1.0 as a runtime-loaded library.
//  Created by Аэлита Лукманова on 24.08.2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include "Square.h"


int main(int argc, char **argv) {
    printf("[start_test]\n");
 
    // Open the library.
    char *lib_name = "./libSquare.dylib";
    void *lib_handle = dlopen(lib_name, RTLD_NOW);
    
    if (lib_handle) {
        printf("[%s] dlopen(\"%s\", RTLD_NOW): Successful\n", __FILE__, lib_name);
    }
    else {
        printf("[%s] Unable to open library: %s\n",
            __FILE__, dlerror());
        exit(EXIT_FAILURE);
    }
 
    // Get the symbol addresses.
    float (*squareOfFigure)(int, float, float) = dlsym(lib_handle, "squareOfFigure");
    if (squareOfFigure) {
        printf("[%s] dlsym(lib_handle, \"squareOfFigure\"): Successful\n", __FILE__);
    }
    else {
        printf("[%s] Unable to get symbol: %s\n",
            __FILE__, dlerror());
        exit(EXIT_FAILURE);
    }
    
    
    
    int typeOfFigure;
    float side1;
    float side2;
    float result;
    char mode;
    
    printf("Hello, you are in the runtime-loaded realization of library with calculating square\n");
    printf("Do you want to change mode to dependent library? Y/N\n");
    scanf("%s", &mode);
    if (mode == 'Y' || mode == 'y') {
        execv("./SquareDependent", NULL);
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
        printf("Result: %f\n", squareOfFigure(typeOfFigure, side1, side2));
    }
 
    
    
    // Close the library.
    if (dlclose(lib_handle) == 0) {
        printf("[%s] dlclose(lib_handle): Successful\n", __FILE__);
    }
    else {
        printf("[%s] Unable to open close: %s\n",
            __FILE__, dlerror());
    }
 
    printf("[end_test]\n");
    return 0;
}
