#ifndef __UART_RECV_H__
#define __UART_RECV_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int tty1_fd;
extern int tty2_fd;

extern int write_to_uart(int fd, char *buf, int len);

extern int uart1_init();

extern int uart2_init();


#ifdef __cplusplus
}
#endif


#endif /* __UART_RECV_H__ */
