old version
1、修改#define PBUF_POOL_SIZE         20//10
2、中断中增加
  CPU_SR_ALLOC(); 
  CPU_CRITICAL_ENTER();                                   
  OSIntEnter(); 
  CPU_CRITICAL_EXIT(); 
  
3、OSIntExit();  
中减短SnmpDataRefresh，临界区的长度。

2019-04-23 LB 
1.tcp_keepalive ->tcp_output_alloc_header (in fuction)：
      tcphdr->wnd = htons(pcb->rcv_ann_wnd);
modified as below:

      tcphdr->wnd = htons(pcb->rcv_ann_wnd + 1);

2.opt.h 
#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              1
#endif

modified as below.

#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              0
#endif

3.httpd.c  httpd_init -->httpd_init_addr(in function)

removed:
pcb->so_options |= SOF_KEEPALIVE;

4.tpc.c  tcp_kill_prio(in function)
 if (pcb->prio <= prio &&
  pcb->prio <= mprio &&
(u32_t)(tcp_ticks - pcb->tmr) >= inactivity)

modified as below,resoved the http send RST ACK:

 if (pcb->prio < prio &&
  pcb->prio <= mprio &&
(u32_t)(tcp_ticks - pcb->tmr) >= inactivity)


