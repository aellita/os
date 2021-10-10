#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

#include "Derivative.h"


int main(int argc, char **argv) {
    printf("[start_test]\n");
 
    // Open the library.
    char *lib_name = "./libDerivative.dylib";
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
    float (*derivativeCosWay1)(float, float) = dlsym(lib_handle, "derivativeCosWay1");
    if (derivativeCosWay1) {
        printf("[%s] dlsym(lib_handle, \"derivativeOfCos\"): Successful\n", __FILE__);
    }
    else {
        printf("[%s] Unable to get symbol: %s\n",
            __FILE__, dlerror());
        exit(EXIT_FAILURE);
    }
    
    float (*derivativeCosWay2)(float, float) = dlsym(lib_handle, "derivativeCosWay2");
    if (derivativeCosWay2) {
        printf("[%s] dlsym(lib_handle, \"derivativeOfCos\"): Successful\n", __FILE__);
    }
    else {
        printf("[%s] Unable to get symbol: %s\n",
            __FILE__, dlerror());
        exit(EXIT_FAILURE);
    }
    
    
    
    printf("Hello, you are in the runtime-loaded realization of library with calculating derivative of cosine\n");
    printf("Do you want to change mode to dependent library? Y/N\n");
    char libMode;
    scanf("%s", &libMode);
    if (libMode == 'Y' || libMode == 'y') {
        if (execv("./DerivativeDependent", NULL)) // TODO: - remove if
        {
            printf("ERROR: Failed to change mode to dependent library\n");
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
        printf("Hey");
        printf("Result: %f\n", derivativeCosWay1(A, deltaX));
    } else if (way == 2) {
        printf("Hey");
        printf("Result: %f\n", derivativeCosWay2(A, deltaX));
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



