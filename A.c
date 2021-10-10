#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "zmq.h"

int main(int argc, const char * argv[]) {
    
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
    
    //присоединяемся, как слушатель C
    void* context2 = zmq_ctx_new();
    void* subscriber = zmq_socket(context2, ZMQ_SUB);
    int conn = zmq_connect(subscriber, "tcp://localhost:4041");
    conn = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    
    //присоединяемся, как диктор для B
    void* context3 = zmq_ctx_new();
    void* publisher3 = zmq_socket(context3, ZMQ_PUB);
    zmq_bind(publisher3, "tcp://*:4042");
    printf("Устанавливаем соединение между A и B..\n");
    sleep(3);
    
    //присоединяемся, как диктор для C
    void* context = zmq_ctx_new();
    void* publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:4040");
    //ждем, когда соединение установится
    printf("Устанавливаем соединение между A и C...\n");
    sleep(3);
    
    int i = 0;
    while(i < arrayCount) {

        printf("A отсылает в С строку номер %d: %s\n", i+1, arrayOfString[i]);
        char *name = arrayOfString[i];
        zmq_msg_t message;
        printf("\n");
        zmq_msg_init_size(&message, strlen(name)+1);
        memcpy(zmq_msg_data(&message), name, strlen(name)+1);
        zmq_msg_send(&message, publisher, 0);
        zmq_msg_close(&message);
        i++;
        
        //После отправления отсылаем в B, сколько символов отправлено
        zmq_msg_t messageB;
        zmq_msg_init_size(&messageB, strlen(name)+1);
        memcpy(zmq_msg_data(&messageB), name, strlen(name)+1);
        zmq_msg_send(&messageB, publisher3, 0);
        zmq_msg_close(&messageB);
        
        
        //После отправления ждем ответа от С, что строка получена
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, subscriber, 0);
        int length = zmq_msg_size(&reply);
        char* value = malloc(length+1);
        memcpy(value, zmq_msg_data(&reply), length);
        zmq_msg_close(&reply);
        printf("%s\n", value);
        free(value);
        
        zmq_sleep(1);
    }
    
    zmq_close(publisher);
    zmq_ctx_destroy(context);
    
    zmq_close(subscriber);
    zmq_ctx_destroy(context2);
    
    zmq_close(publisher3);
    zmq_ctx_destroy(context3);
}
