#include <uv-msgpack.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct{
    uvm_read_callback_t cb;
    void* data;
}connection_request_t;

void
cb_connect(uv_connect_t* req, int status){
    connection_request_t* con;
    con = (connection_request_t *)req->data;
    con->cb(req->handle, CONNECT, NULL, con->data, status);
    if(!status){
        /* Start reader */
        uvm_start_reader_new(con->cb, req->handle, NULL);
    }
    free(req);
    free(con);
}

int
uvm_start_client(uvm_read_callback_t cb, 
                 char* host, int port, void* data){
    struct sockaddr_in sin;
    uv_tcp_t* sock = malloc(sizeof(uv_tcp_t));
    uv_connect_t* connect_req = malloc(sizeof(uv_connect_t));
    connection_request_t* req = malloc(sizeof(connection_request_t));
    if(!req){
        /* FIXME: set error here */
        return -1;
    }
    connect_req->data = req;
    req->cb = cb;
    req->data = data;

    /* Resolve name */
    sin = uv_ip4_addr(host,port);

    /* Init connection state */
    uv_tcp_init(uv_default_loop(), sock);

    /* Enqueue connection request */
    return uv_tcp_connect(connect_req, sock, sin, cb_connect);
}

