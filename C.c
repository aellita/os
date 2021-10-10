#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "zmq.h"

int main (int argc, char const *argv[])
{
    
    //присоединяемся, как диктор для А
    void* context2 = zmq_ctx_new();
    void* publisher = zmq_socket(context2, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:4041");
    
    //присоединяемся, как диктор для B
    void* context3 = zmq_ctx_new();
    void* publisher3 = zmq_socket(context3, ZMQ_PUB);
    zmq_bind(publisher3, "tcp://*:4043");
    printf("Устанавливаем соединение между C и B..\n");
    sleep(3);
    
    //присоединяемся, как слушатель А
    void* context = zmq_ctx_new();
    void* subscriber = zmq_socket(context, ZMQ_SUB);
    int conn = zmq_connect(subscriber, "tcp://localhost:4040");
    conn = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    int i = 0;
    for(i = 0; i < 10; i++)
    {
        sleep(3);
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, subscriber, 0);
        int length = zmq_msg_size(&reply);
        char* value = malloc(length+1);
        memcpy(value, zmq_msg_data(&reply), length);
        zmq_msg_close(&reply);
        printf("%s\n", value);
        free(value);
        
        //после каждого получения отправляем уведомление об этом в А
        char *name = "С получила строку от А";
        printf("C отправляет A сообщение: %s\n", name);
        zmq_msg_t message;
        zmq_msg_init_size(&message, strlen(name)+1);
        memcpy(zmq_msg_data(&message), name, strlen(name)+1);
        zmq_msg_send(&message, publisher, 0);
        zmq_msg_close(&message);
        
        
        //а также отправляем, сколько получено символов, в B
        //не нашла быстро, как отправлять число, будем отправлять строку а там уже считать
        zmq_msg_t messageB;
        zmq_msg_init_size(&messageB, length);
        memcpy(zmq_msg_data(&messageB), value, length);
        zmq_msg_send(&messageB, publisher3, 0);
        zmq_msg_close(&messageB);
        
        
    }
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
    
    zmq_close(publisher);
    zmq_ctx_destroy(context2);
    
    zmq_close(publisher3);
    zmq_ctx_destroy(context3);
}
