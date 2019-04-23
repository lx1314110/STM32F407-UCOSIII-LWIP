// SNMP.C -- SNMP protocol support routines

#include <includes.h>
#include "arch/cc.h"
#include "Snmp\snmp_SNMP.h"
#include "Snmp\snmp_mib.h"
// snmpReadLength()   Read length of BER encoded SNMP data


/***********Fuction   :snmpReadLength      *************/
/***********parameters:*pp  -the point of input buffer,*/
/**********            type -data type     *************/
/**********Return     : the length of data *************/
s16_t snmpReadLength(const u8_t **pp, u16_t type)
{
    const u8_t *p;
    s16_t len, i1;

    p = *pp;
    
    //what you are sure to judge the type .
    if (*p++ != type)
        return -1;
    
    //here is a byte of data that mean length, follow the data.
    len = *p++;
    
    //here is a byte of data that mean short or long code.
    if (len & 0x80)
    {
        i1 = len & 0x7f;
        if (i1 == 0 || i1 > (s16_t)sizeof(s16_t))
            return -1;
        for (len = 0; i1--; )
            len = (len << 8) | (*p++);
    }
    *pp = p;

    return len;
}


// snmpReadInt()    Read integral (32-bit or smaller) type BER encoding

s16_t snmpReadInt(u32_t *outp, u8_t olen, const u8_t **inp, u16_t type)
{
    const u8_t *p;
    s16_t i1, len, sign;
    u32_t ul1;

    ul1 = 0;
    len = snmpReadLength(inp, type);
    if (len >= 0 && len <= olen + 1)
    {
        p = *inp;
        sign = *p >> 7; /* Save sign bit */
        for (i1 = len; i1; i1--)
            ul1 = (ul1 << 8) + *p++;
        *inp = p;
        
        /* Must be signed type, have a sign bit and have room to extend */
        if (type == Integer && sign && len)
            ul1 |= 0xffffffff << (8 * len);
    }
    *outp = ul1;

    return len;
}

// snmpReadVal()    Read OCTET STRING type BER encoding

s16_t snmpReadVal(u8_t *outp, u8_t olen, const u8_t **inp, u16_t type)
{
    s16_t len;

    len = snmpReadLength(inp, type);
    if (len >= 0 && len <= olen)
    {
        memcpy(outp, *inp, len);
        *inp += len;
    }

    return len;
}

// snmpRWriteLen()  Encode length and type with BER

void snmpRWriteLength(u8_t **pp, u16_t type, s16_t len)
{
    u8_t *p = *pp;
    s16_t i1;

    if (len > 0x7f)
    {
        i1 = (len > 255) ? 2 : 1;
        if (i1 == 2)
        {
            *--p = len;
            len >>= 8;
        }
        *--p = len;
        *--p = i1 | 0x80;
    }
    else
        *--p = len;

    *--p = type;
    *pp = p;
}

// snmpRWriteInt()  Encode an Integer type with (minimal) BER encoding

void snmpRWriteInt(u8_t **pp, u32_t val, u16_t type, s16_t len)
{
    u8_t *p;
    s16_t i1, nn;

    p = *pp;

    nn = 1;

    /* If using a possibly signed value */
    if (type == Integer)
    {
        i1 = len - 1;
        if ((val >> i1 * 8) & 0x80) {
            for (i1 = len - 1; i1 > 0; i1--)
                if ((u8_t)(val >> i1 * 8) == 0xff && val >> 8 * (i1 - 1) & 0x80)
                    len--;
                else
                    break;
            nn = 0;
        }
    }

    /*
    ** Encode next higher byte while:
    **  Value has more data and length is not yet fulfilled  OR
    **  Non-negetive required and high bit is true (BER would be negetive)
    */
    i1 = len;
    do
    {
        *--p = (u8_t)val;
        val >>= 8;
        i1--;
    }
    while ((val && i1 > 0) || (nn && *p & 0x80));
    *pp = p;

    snmpRWriteLength(pp, type, len - i1);
}


// snmpRWriteVal()  Encode an String type with BER

void snmpRWriteVal(u8_t **pp, const u8_t *vp, u16_t type, s16_t len)
{
    u8_t *p;
    s16_t i1;
    u8_t count;
    u8_t val;
    
    count = 0;
    if (type == Identifier)
    {	    
        if (*vp ==0x2b)
        {    	
	    	p = *pp;
		    for (i1 = len, vp += len; i1; i1--)
		    {
		        val = *--vp;
		        if (val == 0xff)
		        {
		        	count++;
		        	continue;
		        }	        
		        *--p = val;
		    }
		    
		    *pp = p;

	    	snmpRWriteLength(pp, type, len-count);
        } 
        else
        {   	
	    	p = *pp;
		    for (i1 = len, vp += len; i1>2; i1--)
		    {
		        val = *--vp;
		        if (val == 0xff)
		        {
		        	count++;
		        	continue;
		        }	        
		        *--p = val;
		    }	            
		    *--p = 0x2b;
	    
		    *pp = p;

	    	snmpRWriteLength(pp, type, len-count-1);
	    }
    }
	else
	{
	    p = *pp;
	    for (i1 = len, vp += len; i1; i1--)
	        *--p = *--vp;
	    *pp = p;

    	snmpRWriteLength(pp, type, len);
    }
}

// snmpVCompare()   Compare two variable length values for SNMP

s32_t snmpVCompare(const u8_t *op1, s16_t len1,const u8_t *op2, s16_t len2)
{
    s16_t i1, len;
    s32_t cmp;
    u32_t r1,r2;
    u8_t  v1,v2; 

    /****** Pick the shortest length ********/
    len = (len1 > len2) ? len2 : len1;

    /* Compare for the full short length */
    for (i1 = 0, cmp = 0; i1 < len && cmp == 0; i1++)
    {
    	v1 = *op1++;
    	v2 = *op2++;
    	cmp = v1 -v2;
        
        /*****here is nonequal that can jump into the follow instament.**/
    	if (cmp)
    	{
    		/**it sign "1" at the high 8th of byte that is more than 127**/
    		if ((v1&0x80) || (v2&0x80))
    		{
    			r1 = v1;
    			r2 = v2;
    			
    			if (v1&0x80)
    			{	
                               /***it get the low 7 bit data.             ***/
    				r1 = r1&0x7f;
    				
                                /***it mutilpute 128 that is more than 127  */
    				while (*op1 & 0x80)
        				r1 = (r1 << 7) | (*op1++ & 0x7f);
        			r1 = (r1 << 7) |  *op1++;
    			}
    			
    			if (v2&0x80)
    			{	
    				r2 = r2&0x7f;
    				
    				while (*op2 & 0x80)
        				r2 = (r2 << 7) | (*op2++ & 0x7f);
        			r2 = (r2 << 7) |  *op2++;
    			}
    			
    			if (r1 > r2) cmp = 1;
    			else cmp = -1;
    			break;
    		}
    	}
    }

    /* it Encode values into a single signed 32 bit data */
    cmp <<= 16;
    if (len - i1)
        cmp *= len - i1;
    cmp += len1;
    cmp -= len2;

    return cmp;
}


// snmpFindOID()    Find an OID in an array of sorted objects

s32_t snmpFindOID(const u8_t **retp, const u8_t *base, s16_t osize,
    s16_t onum, const u8_t *valp, s16_t vlen)
{
    const u8_t *low, *high;
    s32_t r = 0;
    
    if (base == 0) 
    {
    	*retp = 0;
    	return 1;
    }

    low = base;
    high = base + (onum - 1) * osize;

   	/* Try to find exact match */
    while (low <= high)
    {
        r = snmpVCompare(valp, vlen, ((OID *)low)->name, ((OID *)low)->nlen);

        if (r == 0)
            break;
        
        low = low + osize;
    }
    
    if (r) low = 0;

    *retp = low;    /* Best match */
    return r;
}

// snmpDecodeID()   Read BER encoded sub-identifier

u32_t snmpDecodeID(const u8_t **inp)
{
    const u8_t *p;
    u32_t val;

    p = *inp;
    val = 0;
    while (*p & 0x80)
        val = (val << 7) | (*p++ & 0x7f);
    val = (val << 7) |  *p++;   /* Grab final byte (7-bits) */
    *inp = p;

    return val;
}

// snmpEncodeID()  Encode an Id with BER

void snmpEncodeID(u8_t **pp, u8_t olen, u32_t val)
{
    u8_t *p;

    p = *pp;
    while (olen--)
    {
        *--p = val | 0x80;  /* Encode 7-bit chunks */
        val >>= 7;
        if (val == 0)
            break;
    }
    *(*pp - 1) &= ~0x80;
    *pp = p;            /* Move pointer */
}

// snmpEncodeIndex()    Encode assicative table indices with BER
u8_t Oid_Len;
u8_t index_out_table_flag = 0;

/***/
void snmpEncodeIndex(u8_t **pp, const MIB *mibp, const MIBTAB *mtp,
    s16_t tabix,const MIBVAR *mvpold)
{
    const MIBVAR *mvp;
    u8_t *p, *vptr;
    u32_t ul1;
    s16_t i1,i2;
    u8_t  count;
    s32_t sl1;

    p = *pp;
    for (i1 = mtp->nix; i1; i1--)
    {
        mvp = &mibp->mvp[mtp->ix[i1 - 1]];
            
       	sl1 = snmpVCompare(mtp->oid.name, mtp->oid.nlen,
        	mvp->oid.name, mvp->oid.nlen-2);
                           
        if (sl1) index_out_table_flag = 1;
       	else index_out_table_flag = 0;
        
        if ((mvp->opt & SX) && (index_out_table_flag == 0))
            snmpEncodeID(&p, 1, 1 + tabix);
        else
        {
            vptr = (u8_t *)mvp->ptr;
            
            if (index_out_table_flag == 0)
            {
	        /* the scalar variables does not have index */
	        if ((mvp->opt & SCALAR) == 0)
	            vptr += mtp->len * tabix;

	        /* Some index variables must be accessed indirectly */
	        if (mvp->opt & CAR)
	            mibp->get(mvp - mibp->mvp, tabix, &vptr);
	    }
	    else
	        mibp->get(mvpold - mibp->mvp, tabix, &vptr);
				
            if ((mvp->opt & BASE1) == BASE1)
                snmpEncodeID(&p, 1, (*vptr) + 1);
            else if ((mvp->type == Integer) || (mvp->type == Gauge) || (mvp->type == Ticks) || (mvp->type == Counter))
            {
                ul1 = 0;
#ifdef LITTLE
                if ((mvp->opt & NWORDER) == 0)
                {
                    switch (mvp->len)
                    {
                        case 1:
                            ul1 = *(u8_t *)vptr;
                            break;
                        case 2:
                            ul1 = *(u16_t *)vptr;
                            break;
                        case 4:
                            ul1 = *(u32_t *)vptr;
                            break;
                        default:
                            break;
                    }
                }
                else
#endif
                {
                    for (i2 = mvp->len; i2; i2--)
                        ul1 = (ul1 << 8) + *vptr++;
                }
                snmpEncodeID(&p, mvp->len + 1, ul1);
            }
            else if (mvp->type == Identifier)
            { 
            	count = 0;
            	
            	for (i2=mvp->len-1; i2>=0; i2--)
            	{
            		if (vptr[i2]==255) continue;
            		else
            		{
            			(*--p) = vptr[i2];
            			count++;
            		}
            	}
            	(*--p) = count - Oid_Len + 1;
            }
            else
                for (i2 = mvp->len - 1; i2 >= 0; i2--)
                    (*--p) = vptr[i2];
       }
    }
    *pp = p;
}


// snmpFindIndex()    Find a table entry index

s32_t snmpFindIndex(s16_t *tabixp, const MIBTAB *mtp,
    const MIB *mibp, const MIBVAR *mvp,
    const u8_t *reqixname, u8_t reqixlen, u8_t nflag)
{
    u8_t *cp;
    s32_t sl1, sl2;
    s16_t i1, i3;
    u8_t ixname[MAXKLEN];

   	/* Default return values */
    *tabixp = -1;
    sl2 = 0;
    
	if (nflag)
	{
	   	/* Search through each table entry's index for the element */
	    for (i1 = 0; ; i1++)
	    {
	        i3 = mibp->index(mvp - mibp->mvp, i1);
	        if (i3 < 0)
	            break;      /* -1 -- End of table reached */
	        if (i3 == 0)
	            continue;   /* 0 -- Skip table entry */

	       	/* Encode the table entry at the current index */
	        cp = ixname + MAXKLEN;
	        snmpEncodeIndex(&cp, mibp, mtp, i1,mvp);

	       	/*
	        ** Compare agent encoding with requested encoding.
	        */
	        sl1 = snmpVCompare(cp, (const u8_t *)&ixname[MAXKLEN] - cp,
	                           reqixname, reqixlen);
			
	       	if (sl1 == 0)
	       	{
	        	i3 = mibp->index(mvp - mibp->mvp, i1+1);
	        	if (i3 > 0)
	        	{
		        	sl2 = 1;
		        	i1++;
			    }	    	
			    
			    goto success;
	       	}
	  	}
    }

   	/* Search through each table entry's index for the element */
    for (i1 = 0; ; i1++)
    {
        i3 = mibp->index(mvp - mibp->mvp, i1);
        if (i3 < 0)
            break;      /* -1 -- End of table reached */
        if (i3 == 0)
            continue;   /* 0 -- Skip table entry */

       	/* Encode the table entry at the current index */
        cp = ixname + MAXKLEN;
        snmpEncodeIndex(&cp, mibp, mtp, i1,mvp);

       	/*
        ** Compare agent encoding with requested encoding.
        */
        sl1 = snmpVCompare(cp, (const u8_t *)&ixname[MAXKLEN] - cp,
                           reqixname, reqixlen);
		
       	if (nflag)
       	{
       		if (sl1 >= 255) 
       		{
       			sl2 = 1;
       			break;
       		}
       	}
       	else
       	{
       		if (sl1 == 0)
       		{
       			sl2 = 1;
       			break;
       		}
       	}
    }
    
success:    
    *tabixp = i1;
    return sl2;
}

