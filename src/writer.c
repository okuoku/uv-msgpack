#include <uv-msgpack.h>

#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 4096
typedef struct{
    int p;
    uv_buf_t buf[MAX_BUFFER];
}packbuffer;

typedef struct{
    uv_write_t wrt;
    uvm_write_callback_t cb;
} my_write_t;

void
uvm_reset_packer(msgpack_packer* pac){
    int i;
    packbuffer* pb = (packbuffer *)pac->data;
    for(i=0;i!=pb->p;i++){
        free(pb->buf[i].base);
    }
    pb->p = 0;
}

void
write_callback(uv_write_t* wrt, int status){
    int i;
    my_write_t* mrt;
    static int count;
    uv_buf_t *bufs;
    uvm_con_state* cns;
    mrt = (my_write_t *)wrt;
    bufs = (uv_buf_t *)wrt->data;
    cns = (uvm_con_state *)wrt->handle->data;
    for(i=0;bufs[i].base;i++){
        free(bufs[i].base);
    }
    free(bufs);
    mrt->cb(wrt->handle, cns->userdata, status);
    free(wrt);
}

int /* FIXME: naive impl (for valgrind check) */
packer(void* data,const char* buf,unsigned int len){
    void* base;
    packbuffer* pb = (packbuffer *)data;
    base = malloc(len);
    memcpy(base,buf,len);
    pb->buf[pb->p] = uv_buf_init((char *)base,len);
    pb->p++;
}

void
connect_cb(uv_connect_t* req, int bogus){
    uvm_read_callback_t cb;
    uvm_con_state* cns;
    cb = (uvm_read_callback_t)req->data;
    cns = (uvm_con_state *)req->handle->data;
    cb(req->handle, SERVER_CONNECT, NULL, cns->userdata, 0);
    uvm_start_reader(cb,req->handle);
    free(req);
}

msgpack_packer*
uvm_new_packer(void){
    msgpack_packer* pac;
    packbuffer* pb;
    pb = malloc(sizeof(packbuffer));
    pb->p = 0;
    pac = msgpack_packer_new(pb, &packer);
    return pac;
}

void
uvm_free_packer(msgpack_packer* pac){
    free(pac->data);
    msgpack_packer_free(pac);
}

int
uvm_write(uvm_write_callback_t cb, uv_stream_t* target,msgpack_packer* pac){
    my_write_t* mrt;
    uv_write_t* wrt;
    uv_buf_t *bufs;
    packbuffer* pb;
    int i;
    pb = (packbuffer*)pac->data;
    if(pb->p){
        mrt = malloc(sizeof(my_write_t));
        wrt = &mrt->wrt;
        mrt->cb = cb;
        bufs = malloc(sizeof(uv_buf_t)*(pb->p+1));
        for(i=0;i!=pb->p;i++){
            bufs[i] = pb->buf[i];
        }
        bufs[i].base = NULL;
        wrt->data = bufs;
        uv_write(wrt,target, bufs, pb->p, &write_callback);
        pb->p = 0;
    }else{
        /* Error: Something wrong. We don't have any datagram */
        return -1;
    }
    return 0;
}

int
uvm_new_connection(uvm_read_callback_t cb,
        char* target,int port,void* data){
    uv_connect_t* req;
    uv_tcp_t* ret;
    uvm_con_state* cns;
    int r;
    req = malloc(sizeof(uv_connect_t));
    ret = malloc(sizeof(uv_tcp_t));
    cns = malloc(sizeof(uvm_con_state));

    ret->data = cns;
    cns->userdata = data;
    uv_tcp_init(uv_default_loop(),ret);
    req->data = cb;
    r = uv_tcp_connect(req, ret, uv_ip4_addr(target,port), &connect_cb);
    if(r){ /* fail */
        free(req);
        free(ret);
    }
    return r;
}

