/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   �� rs_ringbuffer.h
  * @brief  �� Single ring buffer for communiction service.
 *  @Changes��
 *  Date           Author   Version    Notes
 *  2019-06-13     Manny    0.0.1      first version
 ********************************************************
 */
#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"
#include "string.h"

/* Private  ------------------------------------------------------------------*/
/*���ݻ���������(�û���������ͨ�ŷ���)*/
#define DATA_BUF_LENGTH              512

/*���λ�������ؽṹ��*/
typedef struct {
    volatile uint8_t  head;                                            /* ������ͷ�� */
    volatile uint8_t  tail;                                            /* ������β�� */
    volatile uint8_t  dataLen;                                         /* �����������ݳ��� */
    volatile uint8_t  readWriteMutexFlag;                              /* ��д�����־ */
    uint8_t           aFrameLen[40];                                   /* �洢����֡�ĳ��� */
    volatile uint8_t  frameNum;                                        /* ������������ */
    uint8_t           ringBuf[DATA_BUF_LENGTH];                        /* ������ */
}ringBufType_t;

/*�໺��*/
typedef struct {
    ringBufType_t RingBuf_1;
    ringBufType_t RingBuf_2;
}multRingBufType_t;

#ifndef COUNTOF
    #define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#endif

/* Exported ------------------------------------------------------------------*/
bool    rs_WriteDataToRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len);
uint8_t rs_ReadDataFromRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len);
  
#ifdef __cplusplus
}
#endif

#endif  

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
