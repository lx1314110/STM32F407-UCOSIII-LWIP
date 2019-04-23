#ifndef   _SNMP_H_                                                   // 头文件保护
#define   _SNMP_H_                                                   // 头文件保护



#define LITTLE


// 添加自己的代码

#define MAXOID 	20
#define MAXKEY 	4
#define MAXKLEN	22
#define MAXVAR 	60

#define scompare(a, b) strcmp((const char *)a, (const char *)b)
#define sncompare(a, b, c) strncmp((const char *)a, (const char *)b, c)
#define slength(a) strlen((const char *)a)
#define scopy(a, b) strcpy((char *)a, (const char *)b)
#define sncopy(a, b, c) strncpy((char *)a, (const char *)b, c)
//
//add the value type 
#define Integer     0x02
#define String      0x04
#define Null        0x05
#define Identifier  0x06
#define OctetString 0x14
#define Sequence    0x30
#define IpAddress   0x40
#define Counter     0x41
#define Counter32   0x41
#define Gauge       0x42
#define Ticks       0x43
//
//add snmpV3 value type
#define Opaque      0x44
#define Counter64   0x46
#define Uinteger32  0x47

//add the pdu type
#define GetRequest      0xa0
#define GetNextRequest  0xa1
#define GetResponse     0xa2
#define SetRequest      0xa3
#define TrapV1          0xa4
#define GetBulkRequest  0xa5
#define InformRequest   0xa6
//snmpV2 Trap
#define TrapV2          0xa7
//snmpV3 Report
#define Report          0xa8


#define DECODE_NO_ERROR             0
#define DECODE_ASN_PARSE_ERROR      -1
#define DECODE_ALLOCATION_FAILURE   -2
#define DECODE_UNKNOWN_PDU_HANDLERS -3
#define DECODE_UNKNOWN_CONTEXTS     -4
/* Version 2 error values */
#define tooBig              1
#define noSuchName          2
#define badValue            3
#define readOnly            4
#define genErr              5
#define noAccess            6
#define wrongType           7
#define wrongLength         8
#define wrongEncoding       9
#define wrongValue          10
#define noCreation          11
#define inconsistentValue   12
#define resourceUnavailable 13
#define commitFailed        14
#define undoFailed          15
#define authorizationError  16
#define notWritable         17
#define inconsistentName    18

#define IMMED     0x01          /* Immediate value in mvp->len */
#define IMMED2    0x02          /* Immediate value in mvp->type + len */
#define BASE1     0x03          /* Base 0 in data space, base 1 in MIB */
#define SCALAR    0x04          /* Table not indexed (no offset) */
#define W         0x08          /* Write allowed */
#define SX        0x10          /* Sequential table index inferred */
#define NWORDER   0x20          /* Network byte ordering for basic type */
#define CAR       0x40          /* Call application after read */
#define CAW       0x80          /* Call application before write */
#define NOACCESS   0x200

/* VACM return values */
#define accessAllowed   0
#define noSuchContext   1

struct COUNTER64
{
    u32_t hi, lo;
};

typedef struct
{
    u8_t nlen, name[MAXOID];
} OID;

typedef struct
{
    OID oid;                    /* Object ID name and length */
    u8_t nix;                  /* Number of indices for table */
    u16_t ix[MAXKEY];          /* Index values (offsets) */
    u16_t len;                 /* Length of table */
    void *empty;		/* empty flag */
} MIBTAB;

typedef struct
{
    OID oid;                    /* Object ID name and length */
    u16_t opt;                 /* Options (with choice) */
    u8_t type;                 /* Type of variable */
    s16_t len;                 /* Length of pointer field */
    void *ptr;                  /* Pointer to possible variable data */
} MIBVAR;

/* Define SNMP access to a particular MIB */
typedef struct
{
    const MIBVAR *mvp;          /* MIB variables */
    s16_t (*numvars)(void);    /* Number of variables */
    const MIBTAB *mtp;          /* MIB tables */
    s16_t (*numtabs)(void);    /* Number of tables */
    void (*get)(s16_t varix, s16_t tabix, u8_t **vvptr);
    s16_t (*set)(s16_t varix, s16_t tabix);
    s16_t (*index)(s16_t varix, s16_t index);
    void (*init)(u16_t type);  /* Initialize the MIB */
    s16_t (*check)(s16_t varix, s16_t tabix, const u8_t *inp);
} MIB;

typedef struct
{
    const MIB **mibs;               /* Array of pointers to host MIBs */
    u16_t nummibs;                 /* Number of host MIBs */
    u16_t trapv;            		/* Trap version and startup trap type */
} AGENT_CONTEXT;

/* Function prototypes */
s16_t inReqOutRep(u8_t **obp, u16_t olen, const u8_t *ibp, u16_t ilen);

/* Mid-level API */
void ussSNMPAgentInit(void);
s16_t ussSNMPAgentTrap(u8_t type, u8_t spec, u8_t*contextName,
    const u8_t *vbs, u16_t len);

/* Utility */
void snmpEncodeIndex(u8_t **pp, const MIB *mibp, const MIBTAB *mtp,
    s16_t tabix, const MIBVAR *mvpold);
    
s32_t snmpVCompare(const u8_t *op1, s16_t len1, const u8_t *op2, s16_t len2);

s32_t snmpFindOID(const u8_t **retp, const u8_t *base, s16_t osize,
    s16_t onum, const u8_t *valp, s16_t vlen);
s32_t snmpFindIndex(s16_t *tabixp, const MIBTAB *mtp,
    const MIB *mibp, const MIBVAR *mvp,
    const u8_t *reqixname, u8_t reqixlen, u8_t nflag);
    
void snmpEncodeID(u8_t **pp, u8_t olen, u32_t val);

s16_t snmpReadLength(const u8_t **pp, u16_t type);
s16_t snmpReadInt(u32_t *outp, u8_t olen, const u8_t **inp, u16_t type);
s16_t snmpReadVal(u8_t *outp, u8_t olen, const u8_t **inp, u16_t type);

void snmpRWriteLength(u8_t **pp, u16_t type, s16_t len);
void snmpRWriteInt(u8_t **pp, u32_t val, u16_t type, s16_t len);
void snmpRWriteVal(u8_t **pp, const u8_t *vp, u16_t type, s16_t len);

/* VACM module */
s16_t isAccessAllowed(const u8_t *contextName);
    
#endif
