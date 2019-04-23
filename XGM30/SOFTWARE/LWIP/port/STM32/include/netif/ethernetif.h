#ifndef _ETHERNETIF_H
#define _ETHERNETIF_H
#include <includes.h>
#include "lwip/err.h"
#include "lwip/inet.h"
#include "netif.h"

/* Forward declarations. */
err_t  ethernetif_input(struct netif *inp);
err_t  ethernetif_init(struct netif *net);
pbuf_t * low_level_input(struct netif *netif);
err_t  low_level_output(struct netif *netif, struct pbuf *p);
void ethernetif_update_config(struct netif *netif);

#endif /* _ETHERNETIF_H */