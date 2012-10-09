#ifndef _UV_MSGPACK_H
#define _UV_MSGPACK_H

#include <uv.h>
#include <msgpack.h>

/* server messages */

typedef enum {
    MESSAGE = 0,
    CONNECT,
    DISCONNECT,
    ERROR,
} uvm_reader_message_t;

/* server callbacks */
typedef void (*uvm_read_callback_t)(uv_stream_t* target,
                                    uvm_reader_message_t msg,
                                    msgpack_object* obj,
                                    void* data,
                                    int status);
                                      
/* state */
typedef struct {
    void* userdata;
    uvm_read_callback_t proc;
    msgpack_unpacker unpacker;
    msgpack_unpacked unpacked;
}uvm_con_state;

/* client callbacks */

/* client callback. target->data initialized as user specified. */
/*
typedef void (*uvm_connection_callback_t)(uv_stream_t* target,
                                          msgpack_packer* pac,
                                          void* data);
*/
typedef void (*uvm_write_callback_t)(uv_stream_t* target,
                                     void* data,
                                     int status);

/* i/f */
int uvm_start_client(uvm_read_callback_t,char*,int,void*);
int uvm_start_server(uvm_read_callback_t,char*,int,void*);
void uvm_start_reader(uvm_read_callback_t,uv_stream_t*);
void uvm_start_reader_new(uvm_read_callback_t,uv_stream_t*,void*);
int uvm_new_connection(uvm_read_callback_t,char*,int,void*);
msgpack_packer* uvm_new_packer(void);
void uvm_free_packer(msgpack_packer* pac);
int uvm_write(uvm_write_callback_t,uv_stream_t*,msgpack_packer*);
void uvm_reset_packer(msgpack_packer*);

/* chime */
typedef struct{
    uv_sem_t sem;
    void* value;
}uvm_chime_t;
void uvm_chime_init(uvm_chime_t*);
void uvm_chime_destroy(uvm_chime_t*);
void* uvm_chime_wait(uvm_chime_t*);
void uvm_chime_fill(uvm_chime_t*,void*);

#endif
