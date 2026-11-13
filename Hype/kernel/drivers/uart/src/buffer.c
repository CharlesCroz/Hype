#include "buffer.h"

static unsigned int min(unsigned int a, unsigned int b){
    return (a < b ? a : b);
}

int buffer_init(struct rolling_buffer *buffer, char *data, unsigned len){
    if(buffer == 0 || data == 0 || len == 0) return -1;
    /* The end of data is beyond 32bits memory address limits !*/
    if(data + len < data) return -1;
    
    buffer->data = data;
    buffer->data_end = ((char *)data) + len;
    buffer_clear(buffer);
    
    return 0;
}

void buffer_clear(struct rolling_buffer *buffer){
    if(buffer == 0) return;
    buffer->head = buffer->data;
    buffer->tail = buffer->data;
    buffer->full = 0;
}

int buffer_push(struct rolling_buffer *buffer, const char *data, unsigned len){
    unsigned int space, amount;

    if(buffer == 0 || data == 0 || buffer->full == 1) return len;
    
    /* Fill from tail to data_end */
    space = buffer->data_end - buffer->tail;
    amount = min(space, len);

    /* Copy amount bytes at tail, it's not efficient, I know */
    for(; amount != 0; amount--){
        *buffer->tail = *data;
        buffer->tail++;
        data++;
        len--;
    }
    if(buffer->tail == buffer->data_end){
        buffer->tail = buffer->data;
    }

    /* work the rest, maybe, from data to head */
    space = buffer->head - buffer->tail;
    amount = min(space, len);

    for(; amount != 0; amount--){
        *buffer->tail = *data;
        buffer->tail++;
        data++;
        len--;
    }

    if(buffer->head == buffer->tail){
        buffer->full = 1;
    }

    /* Return the amount of bytes not copied */
    return len;
}

int buffer_pop(struct rolling_buffer *buffer, char *data, unsigned len){
    if(buffer == 0 || data == 0) return len;
    
    while(len && ((buffer->tail != buffer->head) || buffer->full)){
        buffer->full = 0;
        *data = *buffer->head;
        buffer->head++;
        data++;
        len--;
        if(buffer->head == buffer->data_end){
            buffer->head = buffer->data;
        }
    }

    return len;
}

int buffer_is_empty(struct rolling_buffer *buffer){
    return (buffer->head == buffer->tail) && (!buffer->full);
}


