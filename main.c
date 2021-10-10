//
//  main.c
//  os-3FF
//
//  Created by Аэлита Лукманова on 17.08.2021.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

struct arg_struct {
    int * x;
    int * total;
    int size;
    int j;
    int k;
    int * smallArray;
    int numberOfThreads;
};


void* changeTotalArrayByConcat(void *arguments) {

    struct arg_struct *args = (struct arg_struct *)arguments;
    
    // сколько массивов в переданном smallArray
    int sizeOfSmallArrayColumns;
    
    //сколько массивов прошло
    int initColumnIndex;
    int r = args->k % args->numberOfThreads;
    
    if (args->j < r) {
        sizeOfSmallArrayColumns = args->k / args->numberOfThreads + 1;
        initColumnIndex = args->j*sizeOfSmallArrayColumns;
    } else {
        sizeOfSmallArrayColumns = args->k / args->numberOfThreads;
        initColumnIndex = r*(sizeOfSmallArrayColumns+1) + (args->j-r)*sizeOfSmallArrayColumns;
    }
    
    //uncoment to see work of threads:
    //printf("this is sizeOfSmallArrayColumns: %d\n", sizeOfSmallArrayColumns);
    
    int *smarr = args->smallArray;
    
    int indexForTotal = args->size * initColumnIndex;
    
    int sizeOfSmallArr = args->size * sizeOfSmallArrayColumns;
    
    memcpy((args->total) + indexForTotal,
           smarr,
           sizeOfSmallArr * sizeof(int));
    
    //uncoment to see work of threads:
//    for (int i = 0; i < args->size * args->k; i++) {
//
//        printf("f: %d : %d\n", i, (args->total)[i]);
//    }
    
    free(arguments);
    return NULL;
}

int main(int argc, const char * argv[]) {
//    clock_t start_time;
    int n;
    int k;
    int numberOfThreads;

    printf("How many arrays?:");
    scanf("%d",&k);
    
    printf("How many elements in array?:");
    scanf("%d",&n);
    
    printf("How many threads?:");
    scanf("%d",&numberOfThreads);
    
    //create array of arrays:
    int arrayOfArrays[k][n];
    
    srand((unsigned int)time( NULL ));
    
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            arrayOfArrays[i][j] = rand()%100 + 1;
        }
    }
    
    pthread_t th[k];
    
    //fill structure:
    struct arg_struct args;
    args.size = n;
    args.k = k;
    args.total = malloc(args.size * args.k * sizeof(int));
    args.numberOfThreads = numberOfThreads;
    
    clock_t start_time = clock();
    double setUpStructureTime = 0;
    
    for (int j = 0; j < args.numberOfThreads; j++) {
        clock_t setUpStructureTime0 = clock();
        int x;
        int y = args.size;
        
        //сколько массивов прошло
        int initColumnIndex;
        //remainder
        int r = args.k % args.numberOfThreads ;
        
        if (j < r) {
            x = args.k / args.numberOfThreads + 1;
            initColumnIndex = j*x;
        } else {
            x = args.k / args.numberOfThreads;
            initColumnIndex = r*(x+1) + (j-r)*x;
        }
        
        int smallArray[x][y];
        
        for (int col = initColumnIndex;
             col < initColumnIndex + x;
             col++) {
            for (int row = 0; row < y; row++) {
                smallArray[col-initColumnIndex][row] = arrayOfArrays[col][row];
            }
        }
        
        int * smAr = malloc(sizeof(smallArray));
        memcpy(smAr, smallArray, sizeof(smallArray));
        args.smallArray = smAr;
        args.j = j;
        
        struct arg_struct* tmpArgs = malloc(sizeof(struct arg_struct));
        *tmpArgs = args;
        
       
        //printf("setUpStructureTime in %f seconds\n", (double)(clock() - setUpStructureTime0) / CLOCKS_PER_SEC);
        setUpStructureTime += (double)(clock() - setUpStructureTime0) / CLOCKS_PER_SEC;
        pthread_create(&th[j], NULL, &changeTotalArrayByConcat, tmpArgs);

        //uncoment to see work of threads:
        //sleep(1);

    }
    
    for (int j = 0; j < args.numberOfThreads; j++) {
        if (pthread_join(th[j], NULL) != 0 ) {
            perror("Failed to join thread");
        }
    }
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC - setUpStructureTime;
    double elapsed_time2 = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("All Done in %f seconds\n", elapsed_time2);
    printf("SetUpStructure Done in %f seconds\n", setUpStructureTime);
    printf("Concat Done in %f seconds\n", elapsed_time);
    
    return 0;
    
}

