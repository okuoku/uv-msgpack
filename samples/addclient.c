#include <uv-msgpack.h>

#include <stdio.h>
#include <stdlib.h>

static int32_t seq;

struct _request_queue_entry;
typedef void (*request_callback_t)(struct _request_queue_entry* req,
                                   uv_stream_t* target,
                                   msgpack_packer* pac,
                                   msgpack_object* err,
                                   msgpack_object* obj);


typedef struct{
    msgpack_packer* pac;
    uv_stream_t* target;
}connection_status_t;

typedef enum{
    REQUEST_NEW,
    REQUEST_SENT,
    REQUEST_SUCCESS,
    REQUEST_FAIL
}request_status_t;

typedef enum{
    METHOD_ADD,
}method_t;

typedef struct _request_queue_entry{
    request_status_t status;
    request_callback_t callback;
    int32_t seq;
    struct _request_queue_entry* next;
    struct _request_queue_entry* prev;
    method_t method;
    union{
        struct{
            int a;
            int b;
        }request_add;
    }param;
    union{
        struct{
            int result;
        }result_add;
    }result;
    uvm_chime_t chime;
}request_queue_entry;

uv_async_t async_request_queue;

request_queue_entry* root = NULL;

static void
write_nothing_to_do(uv_stream_t* target,
                    void* data,
                    int status){
    /* Nothing to do */
}

request_queue_entry*
new_request_queue_entry(void){
    request_queue_entry* r;
    r = malloc(sizeof(request_queue_entry));
    uvm_chime_init(&r->chime);
    return r;
}

void
dispose_request_queue_entry(request_queue_entry* me){
    uvm_chime_destroy(&me->chime);
    free(me);
}

void
request_add_cb(request_queue_entry* req, uv_stream_t* target,
               msgpack_packer* pac, msgpack_object* err, msgpack_object* obj){
    /* Dequeue */
    if(req == root){
        if(req->next){
            req->next->prev = NULL;
            root = req->next;
        }else{
            root = NULL;
        }
    }else{
        if(req->prev){
            req->prev->next = req->next;
        }
        if(req->next){
            req->next->prev = req->prev;
        }
    }
    if(!err){
        req->result.result_add.result =
            obj->via.array.ptr[0].via.i64;
        req->status = REQUEST_SUCCESS;
    }else{
        req->status = REQUEST_FAIL;
    }
}

static void
request_issue_add(uv_stream_t* target, msgpack_packer* pac, int32_t seq,
            int a, int b){
    /* Pack request header */
    msgpack_pack_array(pac,4);
    msgpack_pack_uint64(pac,0);
    msgpack_pack_uint64(pac,seq);
    msgpack_pack_raw(pac,3);
    msgpack_pack_raw_body(pac,"add",3);

    /* Pack request parameters */
    msgpack_pack_array(pac,2);
    msgpack_pack_int64(pac,a);
    msgpack_pack_int64(pac,b);

    /* Send */
    uvm_write(&write_nothing_to_do, target, pac);
}

static void
request_issue(uv_stream_t* target, msgpack_packer* pac,
              request_queue_entry* req){
    seq++;
    /* Fill seq */
    req->seq = seq;
    switch(req->method){
        case METHOD_ADD:
            request_issue_add(target,pac,seq,
                              req->param.request_add.a,
                              req->param.request_add.b);
            req->callback = request_add_cb;
            req->status = REQUEST_SENT;
            break;
    }
}

static void
cb_async_request_queue(uv_async_t* handle, int status){
    request_queue_entry* p;
    connection_status_t* st;
    st = (connection_status_t*)handle->data;
    if(st){
        for(p=root;p;p=p->next){
            if(p->status == REQUEST_NEW){
                request_issue(st->target,st->pac,p);
            }
        }
    }
}

static request_queue_entry*
request_locate(uint64_t seq){
    request_queue_entry* p;
    for(p=root;p;p=p->next){
        if(p->seq == seq){
            return p;
        }
    }
    return NULL;
}

static void
request_enqueue(request_queue_entry* me){
    me->status = REQUEST_NEW;
    /* FIXME: Guard here. */
    me->next = root;
    me->prev = NULL;
    if(root){
        root->prev = me;
    }
    root = me;
    /* Chime main thread */
    uv_async_send(&async_request_queue);
}

static int
request_add(int a, int b){
    request_queue_entry* e;
    e = new_request_queue_entry();
    e->method = METHOD_ADD;
    e->param.request_add.a = a;
    e->param.request_add.b = b;
    request_enqueue(e);
    uvm_chime_wait(&e->chime);
    dispose_request_queue_entry(e);
    return e->result.result_add.result;
}

static void
handle_client_message(uv_stream_t* target,
                      msgpack_packer* pac,
                      msgpack_object* obj){
    int type;
    uint64_t seq;
    request_queue_entry* me;
    msgpack_object* err;
    msgpack_object* result;
    if(obj->type == MSGPACK_OBJECT_ARRAY){
        type = obj->via.array.ptr[0].via.i64;
        if(type == 1 /* Resp */){
            seq = obj->via.array.ptr[1].via.u64;
            me = request_locate(seq);
            if(me){
                err = &obj->via.array.ptr[2];
                if(err->type == MSGPACK_OBJECT_NIL){
                    err = NULL;
                }
                result = &obj->via.array.ptr[3];
                me->callback(me, target, pac, err, result);
                uvm_chime_fill(&me->chime, (void*)1);
            }else{
                printf("ERR: Invalid sequence ");
                msgpack_object_print(stdout, *obj);
                printf("\n");
            }
        }
    }else{
        printf("ERR: Malformed packet ");
        msgpack_object_print(stdout, *obj);
        printf("\n");
    }
}

static void
test_thread(void* arg){
    for(;;){
        printf("Request....\n");
        printf("Request add 100 + 20 = %d\n",
               request_add(100,20));
    }
}

static void
start_test_thread(void){
    uv_thread_t tid;
    uv_thread_create(&tid, test_thread, NULL);
}

static void
client_cb(uv_stream_t* target,
          uvm_reader_message_t msg,
          msgpack_object* obj,
          void* p,
          int status){
    connection_status_t* st;
    msgpack_packer* pac = (msgpack_packer*)p;
    switch(msg){
        case MESSAGE:
            if(obj){
                handle_client_message(target,pac,obj);
            }
            break;
        case CONNECT:
            printf("client connect = %lx\n",(unsigned long)target);
            /* Fill current connection status */
            st = malloc(sizeof(connection_status_t));
            st->pac = pac;
            st->target = target;
            async_request_queue.data = st;
            start_test_thread();
            break;
        case DISCONNECT:
            printf("client disconnect = %lx\n",(unsigned long)target);
            async_request_queue.data = NULL;
            break;
        default:
            break;
    }
}

int
main(int ac, char** av){
    int r;
    msgpack_packer* pac;
    pac = uvm_new_packer();
    r = uvm_start_client(client_cb, "127.0.0.1",12345,pac);
    if(r){
        printf("Client error!\n");
        return -1;
    }
    root = NULL;
    async_request_queue.data = NULL;
    uv_async_init(uv_default_loop(), 
                  &async_request_queue,
                  cb_async_request_queue);
    uv_run(uv_default_loop());
    return 0;
}

