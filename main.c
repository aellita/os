//
//  main.c
//  os-4
//
//  Created by Аэлита Лукманова on 19.08.2021.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>



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
    
    pid_t pid_ch1 = fork();
    if (pid_ch1 == -1) {
        return 2;
    }
    
    
    if (pid_ch1 == 0) { //child1
        printf("child1");
        
        char *argv[10];
        
        argv[0] = "./child1";
        argv[1] = firstChildFile;
        argv[2] = NULL;
        wait(NULL);
        execv("./child", argv);
    }
    
    if (pid_ch1 > 0) { //parent
        
        pid_t pid_ch2 = fork();
        
        if (pid_ch2 == 0) { //child2
            printf("child2");
            
            char *argv[10];
            
            argv[0] = "./child2";
            argv[1] = secChildFile;
            argv[2] = NULL;
            wait(NULL);
            execv("./child", argv);
            
        } else {
            
            //MARK: -работа с pid
            printf("Hello, I'm a parent\n");
            
            //отсылаем детям сигналы стоп
            kill(pid_ch1, SIGSTOP);
            kill(pid_ch2, SIGSTOP);
            
            //заполняем два массива четными и нечентными строками
            char arrayOfString[50][200];
            printf("Input strings: \n");
            int arrayCount = 0;
            char line[200];
            while(fgets(line, 200, stdin)){
                strcpy(arrayOfString[arrayCount], line);
                arrayCount++;
            }

            char arrayOfString1[NUMBER_OF_STRING][MAX_STRING_SIZE];
            char arrayOfString2[NUMBER_OF_STRING][MAX_STRING_SIZE];
            char currentStr[200];

            int arrayCount1 = 0;
            int arrayCount2 = 0;
            for (int i = 0; i < arrayCount; i++) {
                strcpy(currentStr, arrayOfString[i]);
                if (i%2 == 0) {
                    strcpy(arrayOfString1[arrayCount1], currentStr);
                    arrayCount1++;
                } else {
                    strcpy(arrayOfString2[arrayCount2], currentStr);
                    arrayCount2++;
                }
            }
            
            
            //создадим отображение файлов детей в виртуальной памяти
            char * file_name_ch1 = firstChildFile;
            char * file_name_ch2 = secChildFile;
            
//            O_RDWR нужно писать даже если просто записываем, потому что
            int fd_ch1 = open(file_name_ch1, O_RDWR | O_CREAT | O_TRUNC);
            int fd_ch2 = open(file_name_ch2, O_RDWR | O_CREAT | O_TRUNC);
            if (fd_ch1 < 0) {
                printf("can't open %s for reading", file_name_ch1);
                return 0;
            }
            if (fd_ch2 < 0) {
                printf("can't open %s for reading", file_name_ch2);
                return 0;
            }
            
            //зададим размеры каждому отображению:
            size_t page_size_ch1 = sizeof(arrayOfString1);
            size_t page_size_ch2 = sizeof(arrayOfString2);
            
//            MAP_PRIVATE - изменения видны только моему процессу, то есть даже не попадут в файл, не то что в дети. хотя главное в файл, это как раз то, что нам нужно
            char *contents_ch1 = mmap(NULL, page_size_ch1, PROT_READ | PROT_WRITE, MAP_SHARED, fd_ch1, 0);
            char *contents_ch2 = mmap(NULL, page_size_ch2, PROT_READ | PROT_WRITE, MAP_SHARED, fd_ch2, 0);
            
            if (contents_ch1 == MAP_FAILED){
                    close(fd_ch1);
                    perror("Error mmapping the file");
                    exit(EXIT_FAILURE);
            }
            if (contents_ch2 == MAP_FAILED){
                    close(fd_ch2);
                    perror("Error mmapping the file");
                    exit(EXIT_FAILURE);
            }

            
            //заполним отображения одного файла нечетными строками, другого файла -- четными
            for (int i = 0; i < arrayCount1; i++) {
                write(fd_ch1, arrayOfString1[i], strlen(arrayOfString1[i]));
            }
            for (int i = 0; i < arrayCount2; i++) {
                write(fd_ch2, arrayOfString2[i], strlen(arrayOfString2[i]));
            }
            
            //покажем результат
            printf("File contents_ch1:\n%s\n", contents_ch1);
            printf("File contents_ch2:\n%s\n", contents_ch2);
            
            // Write it now to disk
            if (msync(contents_ch1, page_size_ch1, MS_SYNC) == -1) {
                   perror("Could not sync the file to disk");
            }
            if (msync(contents_ch2, page_size_ch2, MS_SYNC) == -1) {
                   perror("Could not sync the file to disk");
            }
            
            //удалим отображения
            munmap(contents_ch1, page_size_ch1);
            munmap(contents_ch2, page_size_ch2);
            
            //закроем файлы
            close(fd_ch1);
            close(fd_ch2);
            
            //отсылаем детям сигнал продолжать выполнение их процессов
            kill(pid_ch1, SIGCONT);
            kill(pid_ch2, SIGCONT);
            
            waitpid(pid_ch2, NULL, 0);
            waitpid(pid_ch1, NULL, 0);
            
            printf("\nparent say goodbye\n");
        }
    }
}





//коротко, что тут делается
//беру введенные строки, создаю два массива из четных и нечетных строк
//передаю первый в один процесс, второй в другой
//процессы просто все переворачивают и выводят
//зачем здесь может быть нужен map?
//мне нужно чтобы взаимодействие между процессами было через mmap files

//есть отображение, в него записываем что-то, потом в детях в отображении меняем строку на инверсированную





//write(fd[1], &arrayCount1, sizeof(int));
//            write(fd2[1], &arrayCount2, sizeof(int));
//            write(fd[1], &arrayOfString1, sizeof(arrayOfString1));
//            write(fd2[1],&arrayOfString2, sizeof(arrayOfString2));
//
//            close(fd[1]);
//            close(fd2[1]);
            
            
            
//                printf("%s\n", arrayOfString1[i]);
//                offset = strlen(arrayOfString1[i]);
//                write(fd, "\n", 1);
//                memcpy(contents + offset, arrayOfString1[i], strlen(arrayOfString1[i]));
//                printf("offset:%lu\n", offset);
            
            
//            size_t i = 0;
            //            char *str = "Abcввввв\n";
            //            contents[i] = *str;
                        //write(fd, "    ", 1);
                        //unsigned long offset = 0;
                       // printf("\narrayCount1: %d\n", arrayCount1);




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
