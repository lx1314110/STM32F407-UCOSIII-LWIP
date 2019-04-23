#ifndef __SNTP_H__
#define __SNTP_H__

#ifdef __cplusplus
extern "C" {
#endif

void sntp_init(void);
//static void sntp_send_request(ip_addr_t *server_addr);
void sntp_stop(void);


#ifdef __cplusplus
}
#endif

#endif /* __SNTP_H__ */
