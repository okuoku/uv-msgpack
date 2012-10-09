#include <stdio.h>
#include <stdlib.h>
#include <uv-msgpack.h>

static void
write_nothing_to_do(uv_stream_t* target,
                    void* data,
                    int status){
    /* Nothing to do */
}

static void
pack_response_header(msgpack_packer* pac, uint64_t seq){
    msgpack_pack_array(pac,4);
    msgpack_pack_uint64(pac,1); /* Response */
    msgpack_pack_uint64(pac,seq);
    msgpack_pack_nil(pac); /* OK */
    /* To be filled */
}

static void
dispatch(uv_stream_t* target,msgpack_packer* pac,
         uint64_t seq, const char* method, int method_len, 
         msgpack_object* obj){
    int a;
    int b;

    if(!strncmp("add",method,method_len)){
        a = obj->via.array.ptr[0].via.i64;
        b = obj->via.array.ptr[1].via.i64;
        /* Pack response */
        pack_response_header(pac,seq);
        msgpack_pack_array(pac,1);
        msgpack_pack_uint64(pac,a+b);
        /* Send */
        uvm_write(&write_nothing_to_do, target,pac);
    }else if(!strncmp("stradd",method,method_len)){
        /* FIXME: Implement it. */
    }else{
        printf("ERR: Unrecognized method: %d\n",method_len);
    }
}

static void
handle_server_message(uv_stream_t* target, msgpack_packer* pac,
                      msgpack_object* obj){
    /* NB: obj will free()ed once we have returned from this function. */
    /*     Copy if needed */
    int type;
    char* method;
    int method_len;
    uint64_t seq; /* int32 in spec */
    msgpack_object* param;
    /* Every message should be a array */
    if(obj->type == MSGPACK_OBJECT_ARRAY){
        type = obj->via.array.ptr[0].via.i64;
        if(type == 0 /* Request */){
            seq = obj->via.array.ptr[1].via.u64;
            method = (char*)&obj->via.array.ptr[2].via.raw.ptr;
            method_len = obj->via.array.ptr[2].via.raw.size;
            param = &obj->via.array.ptr[3];
            dispatch(target,pac,seq,method,method_len,param);
        }
    }else{
        printf("ERR: Malformed object ");
        msgpack_object_print(stdout, *obj);
        printf("\n");
    }
}

static void
server_cb(uv_stream_t* target,
          uvm_reader_message_t msg,
          msgpack_object* obj,
          void* p,
          int status){
    msgpack_packer* pac = (msgpack_packer *)p;
    switch(msg){
        case SERVER_CONNECT:
            printf("connect = %lx\n",(unsigned long)target);
            break;
        case SERVER_DISCONNECT:
            printf("disconnect = %lx\n",(unsigned long)target);
            break;
        case MESSAGE:
            if(obj){
                handle_server_message(target,pac,obj);
            }
            break;
        default:
            /* Ignore */
            break;
    }
}

int
main(int ac, char* av){
    int r;
    msgpack_packer* pac;
    pac = uvm_new_packer();
    r = uvm_start_server(server_cb,"127.0.0.1",12345,pac);
    if(r){
        printf("Error!\n");
        return -1;
    }
    uv_run(uv_default_loop());
}
