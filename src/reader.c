/* UV-MSGPACK: Reader - Receive msgpack objects from stream */
#include "uv-msgpack.h"

uv_buf_t
unpacker_alloc_cb(uv_handle_t* handle, size_t sz){
    size_t cap;
    uvm_con_state* cns = (uvm_con_state *)handle->data;
    msgpack_unpacker_reserve_buffer(&cns->unpacker,sz);
    cap = msgpack_unpacker_buffer_capacity(&cns->unpacker);
    return uv_buf_init(msgpack_unpacker_buffer(&cns->unpacker),
                       cap);
}

void
read_cb(uv_stream_t* stream,ssize_t nread, uv_buf_t buf){
    bool b;
    uvm_read_callback_t srv;
    uvm_con_state* cns = (uvm_con_state *)stream->data;
    srv = cns->proc;
    if(nread > 0){
        msgpack_unpacker_buffer_consumed(&cns->unpacker,nread);
        while(msgpack_unpacker_next(&cns->unpacker, &cns->unpacked)){
            srv(stream, MESSAGE, &cns->unpacked.data,cns->userdata,0);
            msgpack_zone_free(msgpack_unpacked_release_zone(&cns->unpacked));
        }
    }else{
        srv(stream, DISCONNECT,0,0,0);
    }
}

void
uvm_start_reader(uvm_read_callback_t cb, uv_stream_t *target){
    uvm_con_state* cns;
    cns = (uvm_con_state *)target->data;
    /* prepare unpacker */
    msgpack_unpacker_init(&cns->unpacker, 65536);
    msgpack_unpacked_init(&cns->unpacked);

    /* issue read request */
    uv_read_start((uv_stream_t *)target, &unpacker_alloc_cb, &read_cb);
}

void
uvm_start_reader_new(uvm_read_callback_t cb, uv_stream_t *target, void* dat){
    uvm_con_state* cns;
    cns = malloc(sizeof(uvm_con_state));
    target->data = cns;
    cns->userdata = dat;
    cns->proc = cb;
    uvm_start_reader(cb, target);
}

void
listen_cb(uv_stream_t* server, int status){
    uv_tcp_t* ns;
    uvm_con_state* cns;
    uvm_read_callback_t srv;
    cns = (uvm_con_state *)server->data;
    srv = cns->proc;

    ns = malloc(sizeof(uv_tcp_t));
    ns->data = cns;
    uv_tcp_init(uv_default_loop(), ns);
    uv_tcp_nodelay(ns, 1);
    uv_accept(server, (uv_stream_t *)ns);
    uvm_start_reader_new(srv, (uv_stream_t *)ns, cns->userdata);
    srv((uv_stream_t *)ns, CONNECT, NULL, cns->userdata, 0);
}

int
uvm_start_server(uvm_read_callback_t server,char* addr,int port,void* data){
    int i;
    struct sockaddr_in sin;
    uv_tcp_t* srv_socket;
    uvm_con_state* cns;
    cns = malloc(sizeof(uvm_con_state));
    srv_socket = malloc(sizeof(uv_tcp_t));
    /* FIXME: v6? */
    sin = uv_ip4_addr(addr,port);

    /* FIXME: Handle error here. */
    uv_tcp_init(uv_default_loop(),srv_socket);
    uv_tcp_bind(srv_socket,sin);
    srv_socket->data = cns;
    cns->userdata = data;
    cns->proc = server;
    return uv_listen((uv_stream_t *)srv_socket,5,&listen_cb);
}
