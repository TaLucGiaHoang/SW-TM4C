/* datatypes_dep.h */

#ifndef DATATYPES_DEP_H
#define DATATYPES_DEP_H

typedef enum {FALSE=0, TRUE} Boolean;
typedef char Octet;
typedef signed char Integer8;
typedef signed short Integer16;
typedef signed int Integer32;
typedef unsigned char UInteger8;
typedef unsigned short UInteger16;
typedef unsigned int UInteger32;

typedef struct {
  Integer32  nsec_prev, y;
} offset_from_master_filter;

typedef struct {
  Integer32  nsec_prev, y;
  Integer32  s_exp;
} one_way_delay_filter;

typedef struct {
    void        *pbuf[PBUF_QUEUE_SIZE];
    Integer32   get;
    Integer32   put;
    Integer32   count;
} BufQueue;

typedef struct {
  Integer32 multicastAddr;
  Integer32 unicastAddr;
  void      *eventPcb;
  void      *generalPcb;
  BufQueue  eventQ;
  BufQueue  generalQ;
  void      *eventTxBuf;
  void      *generalTxBuf;
} NetPath;

#endif
