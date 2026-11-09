#ifndef HYPE_UART_BUFFER
#define HYPE_UART_BUFFER

struct rolling_buffer {
    char *data;
    char *data_end;
    char *head;
    char *tail;
    int full;
};

/**
 * @brief Inits the buffer with data as the "storage area" of len bytes
 * 
 * @param buffer 
 * @param data 
 * @param len 
 * @return int 0 on success, non-0 of failure
 */
int buffer_init(struct rolling_buffer *buffer, char *data, unsigned len);

/**
 * @brief Clears the buffer. Silently does nothing if buffer is NULL
 * 
 * @param buffer 
 */
void buffer_clear(struct rolling_buffer *buffer);

/**
 * @brief Copies len bytes from data into the buffer
 * 
 * @param buffer 
 * @param data 
 * @param len 
 * @return int The number of bytes *NOT* copied (0 = succes)
 */
int buffer_push(struct rolling_buffer *buffer, const char *data, unsigned len);


/**
 * @brief Copies len bytes from the buffer into data
 * 
 * @param buffer 
 * @param data 
 * @param len 
 * @return int The number of bytes *NOT* copied (0 = succes)
 */
int buffer_pop(struct rolling_buffer *buffer, char *data, unsigned len);

#endif
