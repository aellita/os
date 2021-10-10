//
//  child.c
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


int main(int argc, char* argv[]) {
    
    char * file_name = argv[1];
    
    int fd = open(file_name, O_RDWR | O_CREAT);
    if (fd < 0) {
        printf("can't open %s", file_name);
        return 0;
    }
    
    struct stat st;
    fstat(fd, &st);
    char *contents = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    printf("\nHello, I'm %s\nI've recieved this contents of size %lld :\n%s", argv[0], st.st_size, contents);
    
    int currentIndex = 0;
    char *enter = "\n";
    
    while (currentIndex < (int)st.st_size) {
        char tmpString[100] = {""};
        int ind = 0;
        while (contents[currentIndex] != *enter) {
            //printf("contents[%d]: %c\n", currentIndex, contents[currentIndex]);
            tmpString[ind] = contents[currentIndex];
            currentIndex++;
            ind++;
        }

        for (size_t i = currentIndex-ind; i < currentIndex; i++) {
            contents[i] = tmpString[currentIndex - i - 1];
        }

        currentIndex++;
    }
  
    printf("Then I reversed each string and rewrite them. This is result: \n%s\n", contents);
    munmap(contents, st.st_size);
    close(fd);
    
   
    printf("%s say goodbye\n", argv[0]);
    return 0;
}















//int numLine = 0;
    
//    char *copyOfContents = malloc(st.st_size);
//    *copyOfContents = *contents;
//    printf("POO");
//    printf("copyOfContents: %s\n", copyOfContents);
//
//    char * word = strtok(copyOfContents, "\n");
//    while (word != NULL) {
//        printf("word: %s\n", word);
//        int lenght = (int)strlen(word);
//
//        for(int i = 0, k = lenght - 1; i < (lenght/2); i++, k--) {
//            char temp = word[k];
//            word[k] = word[i];
//            word[i] = temp;
//            contents[k + currentInitIndex] = word[i];
//            contents[i + currentInitIndex] = temp;
//        }
//        currentInitIndex += lenght + 1;
//
//        printf("rev word: %s\n", word);
//        word = strtok(NULL, "\n");
//    }
//




//while (currentIndex <  st.st_size) {
//        char *contentString = contents[numLine];
//        int lenght = (int)strlen(contentString);
//
//        for(int i = 0, k = lenght - 1; i < (lenght/2); i++, k--) {
//            char temp = contentString[k];
//            contentString[k] = contentString[i];
//            contentString[i] = temp;
//        }
//        printf("%s\n", contentString);
//        currentIndex += lenght;
//        numLine++;
//    }
   
//    for(int i = 0, k = (int)st.st_size - 1; i < (st.st_size/2); i++, k--) {
//        char temp = contents[k];
//        contents[k] = contents[i];
//        contents[i] = temp;
//    }
   
//            contents[0] = 'A';
  // printf("File Contents: %s\n", contents);



//    printf("\nHello, I'm %s\nThis is result of my work:\n", argv[0]);
//    for (int i = 2; i < argc; i++) {
//        for (int j = 0, k = strlen(argv[i]) - 1; j < k; j++, k--)
//                {
//                    char temp = argv[i][j];
//                    argv[i][j] = argv[i][k];
//                    argv[i][k] = temp;
//                }
//        printf("%s\n", argv[i]);
//
//        fprintf(fp, "%s\n", argv[i]);
//    }
//
//    fclose(fp);
