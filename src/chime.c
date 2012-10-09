#include <uv-msgpack.h>
#include <stdlib.h>

void
uvm_chime_init(uvm_chime_t* in){
    in->value = 0;
    uv_sem_init(&in->sem,0);
}

void
uvm_chime_destroy(uvm_chime_t* c){
    uv_sem_destroy(&c->sem);
}

void*
uvm_chime_wait(uvm_chime_t* c){
    uv_sem_wait(&c->sem);
    return c->value;
}

void
uvm_chime_fill(uvm_chime_t* c, void* value){
    c->value = value;
    uv_sem_post(&c->sem);
}
