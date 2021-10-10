#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define NUMBER_OF_STRING 25
#define MAX_STRING_SIZE 200


//Вариант 21

int main(int argc, char* argv[]) {
    
    //MARK: -названия файлов на запись
    char firstChildFile[200];
    char secChildFile[200];
    printf("Введите имя файла на запись для первого дочернего процесса: ");
    fgets(firstChildFile, 200, stdin);
    firstChildFile[strlen(firstChildFile) - 1] = '\0';
    
    printf("Спасибо, введите имя файла на запись для второго дочернего процесса: ");
    fgets(secChildFile, 200, stdin);
    secChildFile[strlen(secChildFile) - 1] = '\0';

    
    //MARK: -инициализация
    int fd[2];
    int fd2[2];
    if (pipe(fd) == -1) {
        return 1;
    }
    if (pipe(fd2) == -1) {
        return 1;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        return 2;
    }
    
    
    if (pid == 0) { //child1
        printf("child1");
        
        close(fd[1]);
//        char string[200];
        char arrayOfString1[NUMBER_OF_STRING][MAX_STRING_SIZE];
        int arrayCount1;

        
        if (read(fd[0], &arrayCount1, sizeof(int)) < 0) { return 6; }
        if (read(fd[0], arrayOfString1, sizeof(char) * NUMBER_OF_STRING * MAX_STRING_SIZE) < 0) { return 6; }
        
        char *argv[200];
        
        argv[0] = "./child1";
        argv[1] = firstChildFile;
        for (int i = 2; i < arrayCount1+1; i++) {
            argv[i] = &arrayOfString1[i-2][0];
        }
  
        argv[arrayCount1+1] = NULL;
        wait(NULL);
        execv("./child", argv);
    }
    
    if (pid > 0) { //parent
        
        pid_t ch2 = fork();
        
        if (ch2 == 0) { //child2
            
            printf("child2");
            close(fd2[1]);
//            char string[200];
            int arrayCount2;
            char arrayOfString2[NUMBER_OF_STRING][MAX_STRING_SIZE];
            
            if (read(fd2[0], &arrayCount2, sizeof(int)) < 0) { return 6; }
            if (read(fd2[0], arrayOfString2, sizeof(char) * NUMBER_OF_STRING * MAX_STRING_SIZE) < 0) { return 6; }
            
            char *argv[200];
            
            argv[0] = "./child2";
            argv[1] = secChildFile;
            for (int i = 2; i < arrayCount2+1; i++) {
                argv[i] = &arrayOfString2[i-2][0];
            }
      
            argv[arrayCount2+1] = NULL;
            //sleep(3);
            execv("./child", argv);
            
        } else {
            
            //MARK: -работа с pid
            printf("Hello, I'm a parent\n");
            
            close(fd[0]);
            close(fd2[0]);
            
//            char string[200];
            
            char arrayOfString[50][200];
            printf("Input strings: \n");
            int arrayCount = 0;
            char line[200];
            while(fgets(line, 200, stdin)){
                //чтобы не было в конце перевода строки
                line[strlen(line) - 1] = '\0';
                strcpy(arrayOfString[arrayCount], line);
                arrayCount++;
            }
                        
            char arrayOfString1[NUMBER_OF_STRING][MAX_STRING_SIZE];
            char arrayOfString2[NUMBER_OF_STRING][MAX_STRING_SIZE];
            char currentStr[200];
            
            int arrayCount1 = 1;
            int arrayCount2 = 1;
            for (int i = 0; i < arrayCount; i++) {
                strcpy(currentStr, arrayOfString[i]);
                if (i%2 == 0) {
                    strcpy(arrayOfString1[arrayCount1-1], currentStr);
                    arrayCount1++;
                } else {
                    strcpy(arrayOfString2[arrayCount2-1], currentStr);
                    arrayCount2++;
                }
            }
            
            write(fd[1], &arrayCount1, sizeof(int));
            write(fd2[1], &arrayCount2, sizeof(int));
            write(fd[1], &arrayOfString1, sizeof(arrayOfString1));
            write(fd2[1],&arrayOfString2, sizeof(arrayOfString2));
            
            close(fd[1]);
            close(fd2[1]);
            printf("\n parent say goodbye\n");
            waitpid(ch2, NULL, 0);
            waitpid(pid, NULL, 0);
        }
    }
}

//коротко, что тут делается
//беру введенные строки, создаю два массива из четных и нечетных строк
//передаю первый в один процесс, второй в другой
//процессы просто все переворачивают и выводят
//зачем здесь может быть нужен map?
//мне нужно чтобы взаимодействие между процессами было через mmap files












//
//
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <errno.h>
//#include <fcntl.h>
//
//#define NUMBER_OF_STRING 25
//#define MAX_STRING_SIZE 200
//
/////////
////Вариант 21
////#define BUF_SIZE 64
//
//
////void read_file_name(char file_name[BUF_SIZE]) {
////    char ch;
////    int count = 0;
////    while (read(0, &ch, 1) > 0 && ch != '\n') {
////        file_name[count++] = ch;
////    }
////    file_name[count] = '\0';
////}
//
//int main(int argc, char* argv[]) {
//    //MARK: -открытие файлов на запись
//    FILE *fp1;
//    FILE *fp2;
//    char firstChildFile[200];
//    char secChildFile[200];
//    printf("Введите имя файла на запись для первого дочернего процесса: ");
//    fgets(firstChildFile, 200, stdin);
//
//    firstChildFile[strlen(firstChildFile) - 1] = '\0';
//    if ((fp1 = fopen(firstChildFile, "w")) == NULL)
//    {
//        printf("Не удалось открыть файл на запись");
//        getchar();
//        return 0;
//    }
//    printf("Спасибо, введите имя файла на запись для второго дочернего процесса: ");
//    fgets(secChildFile, 200, stdin);
//
//    secChildFile[strlen(secChildFile) - 1] = '\0';
//    if ((fp2 = fopen(secChildFile, "w")) == NULL)
//    {
//        printf("Не удалось открыть файл на запись");
//        getchar();
//        return 0;
//    }
//
//    //MARK: -инициализация
//    int fd[2];
//    int fd2[2];
//    if (pipe(fd) == -1) {
//        return 1;
//    }
//    if (pipe(fd2) == -1) {
//        return 1;
//    }
//
//
//
//
//        pid_t pid = fork();
//        if (pid == -1) {
//            return 2;
//        }
//
//
//    if (pid == 0) { //child1
//        printf("child1");
//        //while (1) {
//            close(fd[1]);
//            char string[200];
//        char arrayOfString1[NUMBER_OF_STRING][MAX_STRING_SIZE];
//            int n;
//
//        if (read(fd[0], arrayOfString1, sizeof(char) * NUMBER_OF_STRING * MAX_STRING_SIZE) < 0) { return 6; }
//
////            if (read(fd[0], &n, sizeof(int)) < 0) { return 5; }
////            if (read(fd[0], string, sizeof(char) * n) < 0) { return 6; }
//
//            //string[strlen(string) - 1] = 'o';
//
//        for (int i = 0; i < NUMBER_OF_STRING; i++) {
//            strcpy(string, arrayOfString1[i]);
//            printf("New child1 Resived: %s\n", string);
//        }
//
////            printf("Resived: %s\n", string);
//            fprintf(fp1, "%s", string);
//            fclose(fp1);
//
//            close(fd[0]);
//        printf("bye child1");
//            waitpid(pid, NULL, 0);
//        //}
//
//    } if (pid > 0) { //parent
//
//        pid_t ch2 = fork();
//
//        if (ch2 == 0) { //child2
//            printf("child2");
//            close(fd2[1]);
//            char string[200];
//            char arrayOfString2[NUMBER_OF_STRING][MAX_STRING_SIZE];
//            int n;
//
//            if (read(fd2[0], arrayOfString2, sizeof(char) * NUMBER_OF_STRING * MAX_STRING_SIZE) < 0) { return 6; }
//
////            if (read(fd2[0], &n, sizeof(int)) < 0) { return 5; }
////            if (read(fd2[0], string, sizeof(char) * n) < 0) { return 6; }
//
//            for (int i = 0; i < NUMBER_OF_STRING; i++) {
//                strcpy(string, arrayOfString2[i]);
//                printf("New child2 Resived: %s\n", string);
//            }
//
//            //string[strlen(string) - 1] = 'r';
//            //printf("Resived: %s\n", string);
//            fprintf(fp2, "%s", string);
//            fclose(fp2);
//
//            close(fd2[0]);
//            waitpid(ch2, NULL, 0);
//            waitpid(pid, NULL, 0);
//
//        } else {
//
//            //MARK: -работа с pid
//            printf("parent");
//            //while (1) {
//
//                close(fd[0]);
//                close(fd2[0]);
//
//                char string[200];
//            //есть массив строк
//            char arrayOfString[50][200];
//            printf("Input stringS: \n");
//            int arrayCount = 0;
//            char line[200];
//            while(fgets(line, 200, stdin)){
//                //чтобы не было в конце перевода строки
//                line[strlen(line) - 1] = '\0';
//                strcpy(arrayOfString[arrayCount], line);
//                arrayCount++;
//            }
//            printf("arrayCount: %d", arrayCount);
//
//                printf("Input string: ");
//                fgets(string, 200, stdin);
//                string[strlen(string) - 1] = '\0';
//
//                unsigned long n = strlen(string) + 1;
//
//
//
//            char str1[200];
//            char str2[200];
//
//            char arrayOfString1[NUMBER_OF_STRING][MAX_STRING_SIZE];
//            char arrayOfString2[NUMBER_OF_STRING][MAX_STRING_SIZE];
//            char currentStr[200];
//
//            int arrayCount1 = 1;
//            int arrayCount2 = 1;
//            for (int i = 0; i < arrayCount; i++) {
//                strcpy(currentStr, arrayOfString[i]);
//                printf("currentStr: %s\n", currentStr);
//                if (i%2 == 0) {
//                    strcpy(arrayOfString1[arrayCount1-1], currentStr);
//                    arrayCount1++;
//                } else {
//                    strcpy(arrayOfString2[arrayCount2-1], currentStr);
//                    arrayCount2++;
//                }
//            }
//            //на этом этапе у меня есть 2 массива, которые я хочу теперь передать
//
////            int count1 = 1;
////            int count2 = 1;
////            for (int i = 0; i < n-1; i++) {
////                char ch = string[i];
////                printf("%s\n", &ch);
////                if (i%2 == 0) {
////                    str1[count1-1] = ch;
////                    count1++;
////                } else {
////                    str2[count2-1] = ch;
////                    count2++;
////                }
////            }
//
//            //выяснить размер массива
//
////            write(fd[1], &arrayCount1, sizeof(int));
////            write(fd2[1], &arrayCount2, sizeof(int));
//            write(fd[1], &arrayOfString1, sizeof(arrayOfString1));
//            write(fd2[1],&arrayOfString2, sizeof(arrayOfString2));
//
////                write(fd[1], &n, sizeof(unsigned long));
////                write(fd[1], string, sizeof(char) * n);
////                write(fd2[1], &n, sizeof(unsigned long));
////                write(fd2[1], string, sizeof(char) * n);
//
//
////                if (write(fd[1], &n, sizeof(unsigned long)) < 0) {
////                    printf("%lu", n);
////                    return 4; }
////
////                if (write(fd[1], string, sizeof(char) * n) < 0) {
////                    printf("%s", string);
////                    return 3;
////                }
////                if (write(fd2[1], &n, sizeof(unsigned long)) < 0) {
////                    printf("%s", "goodbye3");
////                    return 4; }
////
////                if (write(fd2[1], string, sizeof(char) * n) < 0) {
////                    printf("%s", "goodbye4");
////                    return 3;
////                }
//                close(fd[1]);
//                close(fd2[1]);
//                waitpid(ch2, NULL, 0);
//                waitpid(pid, NULL, 0);
//            //}
//        }
//    }
//}
//        
//
//
//
//        printf("parent");
//        close(fd[0]);
//        close(fd2[0]);
//
//        char string[200];
//        printf("Input string: ");
//        fgets(string, 200, stdin);
//        string[strlen(string) - 1] = '\0';
//
//        unsigned long n = strlen(string) + 1;
//        write(fd[1], &n, sizeof(unsigned long));
//        write(fd[1], string, sizeof(char) * n);
//        write(fd2[1], &n, sizeof(unsigned long));
//        write(fd2[1], string, sizeof(char) * n);
//
//        close(fd[1]);
//        close(fd2[1]);
//        wait(NULL);
////        waitpid(ch2, NULL, 0);
////        waitpid(pid, NULL, 0);
//
//
//        if (pid == 0) { //child1
//
//            printf("here child1");
//
//            //while (1) {
//                close(fd[1]);
//                char string[200];
//                unsigned long n;
//
//                if (read(fd[0], &n, sizeof(unsigned long)) < 0) { return 5; }
//                if (read(fd[0], string, sizeof(char) * n) < 0) { return 6; }
//
//                string[strlen(string) - 1] = 'o';
//                printf("Resived: %s\n", string);
//                fprintf(fp1, "%s", string);
//                fclose(fp1);
//
//                close(fd[0]);
//                waitpid(pid, NULL, 0);
//            //}
//        }
//
//        if (ch2 == 0) { //child2
//            printf("here child2");
//            close(fd2[1]);
//            char string[200];
//            unsigned long n;
//
//            if (read(fd2[0], &n, sizeof(unsigned long)) < 0) { return 5; }
//            if (read(fd2[0], string, sizeof(char) * n) < 0) { return 6; }
//
//            string[strlen(string) - 1] = 'r';
//            printf("Resived: %s\n", string);
//            fprintf(fp2, "%s", string);
//            fclose(fp2);
//
//            close(fd2[0]);
//            waitpid(ch2, NULL, 0);
//            waitpid(pid, NULL, 0);
//        }
//
//
//
//
//
//
//    }
//}
//
////у нас есть input и output
//// между ними pipe
////pipe это типа виртуальный файл
////в него нельзя записывать и читать одновременно
////мы можем читать, но если там ничего не написано, то читающий ждет
//
////Parameters :
////fd[0] will be the fd(file descriptor) for the
////read end of pipe.
////fd[1] will be the fd for the write end of pipe.
////Returns : 0 on Success.
////-1 on error.
