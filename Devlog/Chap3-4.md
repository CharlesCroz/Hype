# Chapitre 3 - Hyp-0, Part 4

In the previous chapter, I talkedended on a note stating I'll be taking two steps to get Hyp-0 ready: understanding better the MMU, and a better partition packaging. I've kind of changed my mind, and I'll try to do things a bit differently. And first, I want to enrich my drivers a little, with:
- led_read
- led_write
- time_get
- uart_read

## More drivers

### LED read/write

Both operations use a bit mask, and a value. 

```c
int led_read(int * value, int * mask);
int led_write(int value, int mask);
```
After a call to `led_read`:
- the n-th bit of `value` is 1 if the n-th led is on, 0 otherwise;
- the n-th bit of `mask` is 1 if the n-th 'exists', 0 otherwise.

E.g is `value` is 0x5=0b0101 and `mask` is 0xF=0b1111, it means that there are 4 available leds, and led 0 and 2 are on,

Similarly, when calling `led_write`:
- if the n-th bit of value is 1 and the n-th bit of mask is 1, the n-th led is turned on;
- if the n-th bit of value is 0 and the n-th bit of mask is 1, the n-th led is turned off;
- if the n-th bit of mask is 0, the n-th led remains either on or off, depending on previous state.

It's working, but I'm lazy to take a picture of my board with the lights on. Not trusting me? Ok.

### UART Receive

Ok, this one will be a bit more tricky I guess. But the idea will be:
- set a recv_buffer SW buffer for reception;
- activate UART RX interrupt;
- on interrupt, check if TX empty and send_buffer has data vs RX not empty;
- if RX recv_buffer is not empty on uart_read call, copy data to "user" buffer.

Let's... do this?

After a while, I can read inputs:

![UART Recv](./images/Chap_3_4_UartRecv.png)

### Time

Ok, last piece of driver. I'll have to get another HW timer to increment an uptime counter (microseconds and seconds). Here we here we go.

Version 0 of this is actually a couple of counter that increments when I call `time_get`. So the first call will return (0, 0), then (0, 1) etc.

It worked from the partition point of view, but it's obiously wrong. And since the "proof of success" involved me looking at blinking LED... Anyways, back to timer setup!


## ARMv7 context elements

AFAIK, to properly define context and thus handle context switches, I'll need the following:
- ?
