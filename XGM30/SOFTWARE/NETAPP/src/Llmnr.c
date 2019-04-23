/***************************************************************************** 
* Llmnr.c - Link-Local Multicast Name Resolution (LLMNR) Protocol.
*
*
* portions Copyright (c) 2011 by Michael Vysotsky.
*
******************************************************************************/
#include <includes.h>

#include "netconf.h"

/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/

/*                      LLMNR Header          

        1  1  1  1  1  1
        5  4  3  2  1  0  9  8  7  6  5  4  3  2  1  0
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                      ID                       |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |QR|   Opcode  | C|TC| T| Z| Z| Z| Z|   RCODE   |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    QDCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    ANCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    NSCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    ARCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

*/
typedef struct _tagLLMNR_HDR{
  WORD    Id;   /*  A 16-bit identifier assigned by the program that generates
           any kind of query.  This identifier is copied from the query
           to the response and can be used by the sender to match
           responses to outstanding queries.  The ID field in a query
           SHOULD be set to a pseudo-random value.  For advice on
           generation of pseudo-random values, please consult [RFC4086]. */
  union{
    struct{    
      WORD  Rcode:4; /*Response code.  This 4-bit field is set as part of LLMNR
                       responses.  In an LLMNR query, the sender MUST set RCODE to
                       zero; the responder ignores the RCODE and assumes it to be
                       zero.  The response to a multicast LLMNR query MUST have
                       RCODE set to zero.  A sender MUST silently discard an LLMNR
                       response with a non-zero RCODE sent in response to a
                       multicast query.
            
                       If an LLMNR responder is authoritative for the name in a
                       multicast query, but an error is encountered, the responder
                       SHOULD send an LLMNR response with an RCODE of zero, no RRs
                       in the answer section, and the TC bit set.  This will cause
                       the query to be resent using TCP, and allow the inclusion of
                       a non-zero RCODE in the response to the TCP query.
                       Responding with the TC bit set is preferable to not sending a
                       response, since it enables errors to be diagnosed.  This may
                       be required, for example, when an LLMNR query includes a TSIG
                       RR in the additional section, and the responder encounters a
                       problem that requires returning a non-zero RCODE.  TSIG error
                       conditions defined in [RFC2845] include a TSIG RR in an
                       unacceptable position (RCODE=1) or a TSIG RR that does not
                       validate (RCODE=9 with TSIG ERROR 17 (BADKEY) or 16
                       (BADSIG)).
            
                       Since LLMNR responders only respond to LLMNR queries for
                       names for which they are authoritative, LLMNR responders MUST
                       NOT respond with an RCODE of 3; instead, they should not
                       respond at all.
            
                       LLMNR implementations MUST support EDNS0 [RFC2671] and
                       extended RCODE values. */ 
      WORD  Z:4;    /* Reserved for future use.  Implementations of this
                       specification MUST set these bits to zero in both queries and
                       responses.  If these bits are set in a LLMNR query or
                       response, implementations of this specification MUST ignore
                       them.  Since reserved bits could conceivably be used for
                       different purposes than in DNS, implementers are advised not
                       to enable processing of these bits in an LLMNR implementation
                       starting from a DNS code base. */
      WORD  T:1;    /* Tentative.  The 'T'entative bit is set in a response if the
                       responder is authoritative for the name, but has not yet
                       verified the uniqueness of the name.  A responder MUST ignore
                       the 'T' bit in a query, if set.  A response with the 'T' bit
                       set is silently discarded by the sender, except if it is a
                       uniqueness query, in which case, a conflict has been detected
                       and a responder MUST resolve the conflict as described in
                       Section 4.1.(RFC 4795) */
   
      WORD  TC:1;   /* TrunCation.  The 'TC' bit specifies that this message was
                       truncated due to length greater than that permitted on the
                       transmission channel.  The 'TC' bit MUST NOT be set in an
                       LLMNR query and, if set, is ignored by an LLMNR responder.
                       If the 'TC' bit is set in an LLMNR response, then the sender
                       SHOULD resend the LLMNR query over TCP using the unicast
                       address of the responder as the destination address.  If the
                       sender receives a response to the TCP query, then it SHOULD
                       discard the UDP response with the TC bit set.  See  [RFC2181]
                       and Section 2.4 of this specification for further discussion
                       of the 'TC' bit. */
      WORD  C:1;    /* Conflict.  When set within a query, the 'C'onflict bit
                       indicates that a sender has received multiple LLMNR responses
                       to this query.  In an LLMNR response, if the name is
                       considered UNIQUE, then the 'C' bit is clear; otherwise, it
                       is set.  LLMNR senders do not retransmit queries with the 'C'
                       bit set.  Responders MUST NOT respond to LLMNR queries with
                       the 'C' bit set, but may start the uniqueness verification
                       process, as described in Section 4.2. (RFC 4795) */
  
      WORD  Opcode:4;/*A 4-bit field that specifies the kind of query in this
                       message.  This value is set by the originator of a query and
                       copied into the response.  This specification defines the
                       behavior of standard queries and responses (opcode value of
                       zero).  Future specifications may define the use of other
                       opcodes with LLMNR.  LLMNR senders and responders MUST
                       support standard queries (opcode value of zero).  LLMNR
                       queries with unsupported OPCODE values MUST be silently
                       discarded by responders.*/
  
      WORD  QR:1;   /* Query/Response.  A 1-bit field, which, if set, indicates that
                       the message is an LLMNR response; if clear, then the message
                       is an LLMNR query. */
    }FLAGS;
    WORD  Flags;
  }QFLAGS;
  WORD  Qdcount;  /* An unsigned 16-bit integer specifying the number of entries
                     in the question section.  A sender MUST place only one
                     question into the question section of an LLMNR query.  LLMNR
                     responders MUST silently discard LLMNR queries with QDCOUNT
                     not equal to one.  LLMNR senders MUST silently discard LLMNR
                     responses with QDCOUNT not equal to one. */
  WORD Ancount;   /* An unsigned 16-bit integer specifying the number of resource
                     records in the answer section.  LLMNR responders MUST
                     silently discard LLMNR queries with ANCOUNT not equal to
                     zero. */
  WORD Nscount;   /* An unsigned 16-bit integer specifying the number of name
                     server resource records in the authority records section.
                     Authority record section processing is described in Section
                     2.9. (RFC 4795)  LLMNR responders MUST silently discard LLMNR 
                     queries with NSCOUNT not equal to zero. */

  WORD Arcount;   /* An unsigned 16-bit integer specifying the number of resource
                     records in the additional records section.  Additional record
                     section processing is described in Section 2.9. (RFC 4795) */

}LLMNR_HDR;

typedef struct{
  WORD    q_type;
  WORD    q_class;
}LLMNR_QUERY;

typedef struct _tagLLMNR_RES_REC{
  WORD  rr_name;
  WORD  rr_type;        /* Resource record type code */
  WORD  rr_class;       /* Resource record class code */
  WORD  TTL[2];         /* The Time To Live of a the resource record's
                           name. */
  WORD  Rdlengh;        /* The length of Resource data */
}LLMNR_RES_REC;

#define LLMNR_MULTICAST_ADDRESS          "224.0.0.252"
#define LLMNR_UDP_PORT                    5355
#define LLMNR_TIME_INTERVAL               10*OS_CFG_TICK_RATE_HZ  /* 7 sec */
/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/
const BYTE  LLMNRMAC[] = {0x01,0x00,0x5E,0x00,0x00,0xFC};
struct udp_pcb  * udp_llmnr=NULL;
struct ip_addr llmr_multicast;

/*----------------------------------------------------------------------------*/
/*                      PROTOTYPES                                            */
/*----------------------------------------------------------------------------*/
static void   Llmnr_Udp_Listner(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
static void   Llmnr_RequestResponse(struct ip_addr * dip, WORD dport, WORD id, bool request);
static void   LlmnrRun(void * arg);

void LlmnrInit(struct netif * net)
{
  /* Internet Broadcast LLMNR MAC address config */
  ETH_MACAddressConfig(ETH_MAC_Address3,(BYTE*)LLMNRMAC);
  ETH_MACAddressPerfectFilterCmd(ETH_MAC_Address3,ENABLE);
  ETH_MACAddressFilterConfig(ETH_MAC_Address3,ETH_MAC_AddressFilter_DA);
  net->flags |= NETIF_FLAG_IGMP;
  if((udp_llmnr = udp_new())){
    udp_recv(udp_llmnr, Llmnr_Udp_Listner, NULL);
    udp_bind(udp_llmnr, IP_ADDR_ANY, LLMNR_UDP_PORT);
    udp_llmnr->ttl = 1;
  }
  /* llmnr multicast address */
  IP4_ADDR(&llmr_multicast,224,0,0,252);
  tcpip_timeout(LLMNR_TIME_INTERVAL,LlmnrRun,NULL);
}
void LlmnrDown(void)
{
  ETH_MACAddressPerfectFilterCmd(ETH_MAC_Address3,DISABLE);
  udp_remove(udp_llmnr);
  udp_llmnr=NULL;
#if LWIP_IGMP  
  igmp_leavegroup(Localhost(),&llmr_multicast);
#endif  
  tcpip_untimeout(LlmnrRun,NULL);
}

static  void  Llmnr_Udp_Listner(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  /* check if datagram is really directed at us (including broadcast requests) */
  LLMNR_HDR *hdr = (LLMNR_HDR*)p->payload;
  NTOHS(hdr->QFLAGS.Flags);
  int q_coint = htons(hdr->Qdcount);
  if(hdr->QFLAGS.FLAGS.QR || hdr->QFLAGS.FLAGS.Rcode ||
     !q_coint )  { pbuf_free(p); return; }  /* This is or response, or error */
  while(q_coint--){
    char * name = (char*)((char*)p->payload + sizeof(LLMNR_HDR));
    BYTE namelen = *(BYTE*)name++;
    char * hostname = GetHostName();
    if(strlen(name) != strlen(hostname))
      { pbuf_free(p); return; }
    int i;
    for(i=0; i< namelen; ++i){
      if(tolower(name[i]) != tolower(hostname[i])){ 
        pbuf_free(p); return; 
      }
    }
    LLMNR_QUERY query;
    memcpy(&query,(BYTE*)(name + namelen +1),sizeof(LLMNR_QUERY));  
    if(htons(query.q_type) != 1 || htons(query.q_class) != 1)
      { pbuf_free(p); return; }
    WORD id =htons((*(WORD*)p->payload)); 
    udp_llmnr->ttl = UDP_TTL;
    Llmnr_RequestResponse(addr,port,id,false);
  }
  pbuf_free(p);
}

static void Llmnr_RequestResponse(struct ip_addr * dip, WORD dport, WORD id, bool request)
{

  struct pbuf * udpbuf = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
  if(udpbuf == NULL){
    return;
  }
  int buf_len = sizeof(LLMNR_HDR)+sizeof(LLMNR_QUERY)+strlen(GetHostName())+sizeof(LLMNR_RES_REC)+ 6;
  BYTE * buf = mem_malloc(buf_len);
  if(buf == NULL){
    pbuf_free(udpbuf);
    return;
  }
  memset(buf,0,buf_len);
  LLMNR_HDR *hdr = (LLMNR_HDR*)buf;
  hdr->Id = htons(id);
  hdr->Qdcount = htons(1);      /* number questions */
  if(!request){
    hdr->Ancount = htons(1);      /* number answers */
    hdr->QFLAGS.FLAGS.QR=1; /* response */
    NTOHS(hdr->QFLAGS.Flags);
  }else
    hdr->Arcount = htons(1);
  char * qrecord = (char*)(buf+ sizeof(LLMNR_HDR));
  *qrecord++ = strlen(GetHostName());
  strcpy(qrecord,GetHostName());
  BYTE * query_ptr = (BYTE*)(qrecord+strlen(GetHostName())+1);
  LLMNR_QUERY query;  
  query.q_type = htons(1); /* IpV4 address type */
  query.q_class = htons(1); /* Internet class */
  memcpy(query_ptr,&query,sizeof(LLMNR_QUERY));
  BYTE *rr_rec_ptr = query_ptr + sizeof(LLMNR_QUERY);
  LLMNR_RES_REC rr_rec;
  rr_rec.rr_name = htons(0xC00C);
  rr_rec.rr_type = htons(1);
  rr_rec.rr_class = htons(1);
  rr_rec.Rdlengh = htons(4); 
  DWORD ttl = htonl(30);
  memcpy(&rr_rec.TTL,&ttl,4);
  memcpy(rr_rec_ptr,&rr_rec,sizeof(LLMNR_RES_REC));
  BYTE * ip_addr = rr_rec_ptr + sizeof(LLMNR_RES_REC);
  DWORD locip = Localhost()->addr;
  memcpy(ip_addr,&locip,4);
  udpbuf->payload = (void*)buf;
  udpbuf->len = udpbuf->tot_len = buf_len;
  udp_sendto(udp_llmnr,udpbuf,dip,dport);
  mem_free(buf);
  udpbuf->payload = NULL;
  pbuf_free(udpbuf); 
}

static void LlmnrRun(void * arg)
{
  if(Localhost()){
#if LWIP_IGMP      
    igmp_joingroup(Localhost(),&llmr_multicast);
#endif 
    udp_llmnr->ttl = 1;
    Llmnr_RequestResponse(&llmr_multicast,
                          LLMNR_UDP_PORT,(rand()&0xFFFF),true);
  }
  tcpip_timeout(LLMNR_TIME_INTERVAL,LlmnrRun,NULL);
}
