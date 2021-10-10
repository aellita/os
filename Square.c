//* Compile with -fvisibility=hidden.

#include "Square.h"

#include <stdio.h>

#define EXPORT __attribute__((visibility("default")))

 
// Initializer.
__attribute__((constructor))
static void initializer(void) {
    printf("[%s] initializer()\n", __FILE__);
}
 
// Finalizer.
__attribute__((destructor))
static void finalizer(void) {
    printf("[%s] finalizer()\n", __FILE__);
}
 
//внутренняя функция, которая будет использоваться функцией из библиотеки
//не будем делать проверку на невалидные данные
float squareOfRect(float side1, float side2) {
    return side1 * side2;
}

//здесь будем делать проверку на невалидные данные, т.к. это функция библиотеки
EXPORT
float squareOfFigure(int typeOfFigure, float side1, float side2) {
    if ((side1 <= 0) || (side2 <= 0)) {
        printf("Проверьте введенные стороны. Они должны быть положительными числами\n");
        return 0;
    }
    switch (typeOfFigure) {
        case 1:
            return squareOfRect(side1, side2);
            break;
        case 2:
            return 1./2*squareOfRect(side1, side2);
            break;
        default:
            printf("Ошибка в вводе. Первый аргумент должен быть \"1\"(площадь прямоугольника) или \"2\"(площадь треугольника)");
            break;
    }
    return 0;
}
