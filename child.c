//
//  child.c
//  os-2
//
//  Created by Аэлита Лукманова on 05.08.2021.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    //MARK: -открытие файлов на запись
    FILE *fp;
    if ((fp = fopen(argv[1], "w")) == NULL)
    {
        printf("Не удалось открыть файл на запись");
        getchar();
        return 0;
    }
    
    
    printf("\nHello, I'm %s\nThis is result of my work:\n", argv[0]);
    for (int i = 2; i < argc; i++) {
        for (int j = 0, k = strlen(argv[i]) - 1; j < k; j++, k--)
                {
                    char temp = argv[i][j];
                    argv[i][j] = argv[i][k];
                    argv[i][k] = temp;
                }
        printf("%s\n", argv[i]);
        
        fprintf(fp, "%s\n", argv[i]);
    }
    
    fclose(fp);
    printf("%s say goodbye\n", argv[0]);
    return 0;
}
