#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "zmq.h"

int main (int argc, char const *argv[]) {
    
    //присоединяемся, как слушатель A
    void* context1 = zmq_ctx_new();
    void* subscriber1 = zmq_socket(context1, ZMQ_SUB);
    int conn1 = zmq_connect(subscriber1, "tcp://localhost:4042");
    conn1 = zmq_setsockopt(subscriber1, ZMQ_SUBSCRIBE, "", 0);
    
    //присоединяемся, как слушатель C
    void* context2 = zmq_ctx_new();
    void* subscriber2 = zmq_socket(context2, ZMQ_SUB);
    int conn2 = zmq_connect(subscriber2, "tcp://localhost:4043");
    conn2 = zmq_setsockopt(subscriber2, ZMQ_SUBSCRIBE, "", 0);
    
    
    while(1) {
        //печать, сколько символов отправлено A
        zmq_msg_t replyA;
        zmq_msg_init(&replyA);
        zmq_msg_recv(&replyA, subscriber1, 0);
        int length1 = zmq_msg_size(&replyA);
        zmq_msg_close(&replyA);
        printf("Send from A: %d symbols\n", length1);
        
        
        //печать, сколько символов получено C
        zmq_msg_t replyC;
        zmq_msg_init(&replyC);
        zmq_msg_recv(&replyC, subscriber2, 0);
        int length2 = zmq_msg_size(&replyC);
        zmq_msg_close(&replyC);
        printf("Recieved by C: %d symbols\n\n", length2);
    }
    
    zmq_close(subscriber1);
    zmq_ctx_destroy(context1);
    
    zmq_close(subscriber2);
    zmq_ctx_destroy(context2);
    
}
