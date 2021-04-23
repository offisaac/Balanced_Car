/**
 ********************************************************
 * Copyright (c) 2019-2019, RobotLab Development Team
  * @file   �� rs_ringbuffer.cpp
  * @brief  �� Single ring buffer for communiction service.
 *  @Changes��
 *  Date           Author   Version    Notes
 *  2019-06-13     Manny    0.0.1      first version
 ********************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "rs_ringbuffer.h"

/* Functions -----------------------------------------------------------------*/
/**
* @brief  д�����ݵ����λ�����
* @param  pBuf: ��д�����ݻ�����
*              len:    ��д�����ݳ���
* @retval ִ��״̬
*/
bool rs_WriteDataToRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len)
{
    if (pRingBuf == NULL) { return false; }
    if (pBuf == NULL) { return false; }
    if (len <= 0) { return false; }

    if (len > (COUNTOF(pRingBuf->ringBuf) - pRingBuf->dataLen)) { return false; }                   /* д��ᵼ�»�������� */
    if (pRingBuf->frameNum >= COUNTOF(pRingBuf->aFrameLen)) { return false; }                       /* ��д��ᵼ���峤�洢��������� */
    if (pRingBuf->readWriteMutexFlag) { return false; }                                             /* ��д���� */

    uint32_t tmp = 0;

    pRingBuf->readWriteMutexFlag = true;                                                            /*�򿪻����־*/
    pRingBuf->aFrameLen[pRingBuf->frameNum] = len;                                                  /*�洢�峤*/
    
    /* ʣ�໺���㹻װ���������� */
    if ((COUNTOF(pRingBuf->ringBuf) - pRingBuf->tail) >= pRingBuf->aFrameLen[pRingBuf->frameNum])
    {
        memcpy(&(pRingBuf->ringBuf[pRingBuf->tail]), pBuf, pRingBuf->aFrameLen[pRingBuf->frameNum]);
        pRingBuf->tail += pRingBuf->aFrameLen[pRingBuf->frameNum];
        
        /* �������һȦ��ص�"ͷ��",����жϺ�����Ч */
        if (pRingBuf->tail == COUNTOF(pRingBuf->ringBuf))
        {
            pRingBuf->tail = 0;
        }
    }
    /* ʣ�໺�治����װ���������ݣ��ֿ�������װ�� */
    else
    {
        memcpy(&(pRingBuf->ringBuf[pRingBuf->tail]), pBuf, COUNTOF(pRingBuf->ringBuf) - pRingBuf->tail);
        tmp = COUNTOF(pRingBuf->ringBuf) - pRingBuf->tail;

        pRingBuf->tail = 0;
        memcpy(&(pRingBuf->ringBuf[pRingBuf->tail]), &pBuf[tmp], pRingBuf->aFrameLen[pRingBuf->frameNum] - tmp);
        pRingBuf->tail = pRingBuf->aFrameLen[pRingBuf->frameNum] - tmp;
    }
    
    pRingBuf->dataLen += pRingBuf->aFrameLen[pRingBuf->frameNum];                                       /*���´洢���ݳ���*/
    pRingBuf->frameNum++;                                                                               /*����֡��*/

    pRingBuf->readWriteMutexFlag = false;/*�رջ����־*/

    return true;
}

/**
* @brief  �ӻ��λ�������������
* @param  pBuf: ��Ŷ������ݻ�����
*              len:    ��Ŷ������ݻ���������
* @retval ʵ�ʶ���������
*/
uint8_t rs_ReadDataFromRingBuffer(ringBufType_t *pRingBuf, uint8_t *pBuf, uint8_t len)
{
    if (pRingBuf == NULL) { return 0x00; }
    if (pBuf == NULL) { return 0x00; }
    
    if (len < pRingBuf->aFrameLen[0]) { return 0x00; }                                                  /*��������С*/
    if (pRingBuf->dataLen <= 0) { return 0x00; }                                                        /*�����ݿɶ�*/
    if (pRingBuf->readWriteMutexFlag) { return 0x00; }                                                  /*��д����*/

    uint32_t ret = 0;
    uint32_t tmp = 0;

    pRingBuf->readWriteMutexFlag = true;                                                                /*�򿪻����־*/
    pRingBuf->dataLen -= pRingBuf->aFrameLen[0];                                                        /*���´洢���ݳ���*/
    ret = pRingBuf->aFrameLen[0];                                                                       /*��ȡ֡��*/

    /* �������һȦ��ֱ�Ӷ�ȡ */
    if ((COUNTOF(pRingBuf->ringBuf) - pRingBuf->head) >= pRingBuf->aFrameLen[0])
    {
        memcpy(pBuf, &(pRingBuf->ringBuf[pRingBuf->head]), pRingBuf->aFrameLen[0]);
        pRingBuf->head += pRingBuf->aFrameLen[0];
        /*�������һȦ��ص�"β��"*/
        if (pRingBuf->head == COUNTOF(pRingBuf->ringBuf))
        {
            pRingBuf->head = 0;
        }
    }
    /*����һȦ��ֳ������ֶ�ȡ*/
    else
    {
        memcpy(pBuf, &(pRingBuf->ringBuf[pRingBuf->head]), COUNTOF(pRingBuf->ringBuf) - pRingBuf->head);
        tmp = COUNTOF(pRingBuf->ringBuf) - pRingBuf->head;
        pRingBuf->head = 0;
        memcpy(&pBuf[tmp], &(pRingBuf->ringBuf[pRingBuf->head]), pRingBuf->aFrameLen[0] - tmp);
        pRingBuf->head = pRingBuf->aFrameLen[0] - tmp;
    }

    for (uint8_t i = 0; i < (pRingBuf->frameNum - 1); i++)
    {
        /*��δ���������峤ʼ������ǰ��*/
        pRingBuf->aFrameLen[i] = pRingBuf->aFrameLen[i + 1];
    }
    pRingBuf->aFrameLen[pRingBuf->frameNum - 1] = 0x00;/*֡���������������ƺ� 0*/

    pRingBuf->frameNum--;                                                                               /*��ȡ��֡���� 1*/

    pRingBuf->readWriteMutexFlag = false;                                                               /*�رջ����־*/

    return ret;
}

/************************ COPYRIGHT(C) SCUT-ROBOTLAB **************************/
