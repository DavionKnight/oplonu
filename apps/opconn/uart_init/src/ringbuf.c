#include <stdio.h>
#include <pthread.h>
#include "serial_ser.h"
#include "ringbuf.h"
#include "serial.h"
#include "uart_default.h"

static void
UpdateIndexAtomic(volatile unsigned long *pulVal, unsigned long ulDelta,
                  unsigned long ulSize)
{
    //
    // Update the variable value.
    //
    *pulVal += ulDelta;
    //
    // Correct for wrap. We use a loop here since we don't want to use a
    // modulus operation with interrupts off but we don't want to fail in
    // case ulDelta is greater than ulSize (which is extremely unlikely but...)
    //
    
    while(*pulVal >= ulSize)
    {
        *pulVal -= ulSize;
    }
    //
    // Restore the interrupt state
    //
}



int 
RingBufFull(tRingBufObject *ptRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    pthread_mutex_lock(&s2e_ringbug_sem);
    //
    // Copy the Read/Write indices for calculation.
    //

    ulWrite = ptRingBuf->ulWriteIndex;

	ulRead = ptRingBuf->ulReadIndex;
    pthread_mutex_unlock(&s2e_ringbug_sem);

//printf("ulWrite is %d\nulRead is %d\nulSize is %d\n",ulWrite,ulRead,ptRingBuf->ulSize);
    //
    // Return the full status of the buffer.
    //
    return((((ulWrite + 1) % ptRingBuf->ulSize) == ulRead) ? YES: NO);
}

unsigned long
RingBufUsed(tRingBufObject *ptRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;
    unsigned long ulSize;

    pthread_mutex_lock(&s2e_ringbug_sem);
    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptRingBuf->ulWriteIndex;
    ulRead = ptRingBuf->ulReadIndex;
    pthread_mutex_unlock(&s2e_ringbug_sem);

    //
    // Return the number of bytes contained in the ring buffer.
    //
    return((ulWrite >= ulRead) ? (ulWrite - ulRead) :
           (ptRingBuf->ulSize - (ulRead - ulWrite)));
}


unsigned long
RingBufFree(tRingBufObject *ptRingBuf)
{
    unsigned long free_len;

    //
    // Return the number of bytes available in the ring buffer.
    //
    free_len = (ptRingBuf->ulSize - 1) - RingBufUsed(ptRingBuf);

    return(free_len);
}

void RingBufFlush(tRingBufObject *ptRingBuf)
{
	pthread_mutex_lock(&s2e_ringbug_sem);
	ptRingBuf->ulReadIndex = ptRingBuf->ulWriteIndex;
	pthread_mutex_unlock(&s2e_ringbug_sem);
}



int 
RingBufEmpty(tRingBufObject *ptRingBuf)
{
    unsigned long ulWrite;
    unsigned long ulRead;

    pthread_mutex_lock(&s2e_ringbug_sem);
    //
    // Copy the Read/Write indices for calculation.
    //
    ulWrite = ptRingBuf->ulWriteIndex;
    ulRead = ptRingBuf->ulReadIndex;
    pthread_mutex_unlock(&s2e_ringbug_sem);

    //
    // Return the empty status of the buffer.
    //
    return((ulWrite == ulRead) ? YES:NO);
}

unsigned char
RingBufReadOne(tRingBufObject *ptRingBuf)
{
    unsigned char ucTemp;

    //
    // Verify that space is available in the buffer.
    //
 //   VOS_ASSERT(RingBufUsed(ptRingBuf) != 0);

    pthread_mutex_lock(&s2e_ringbug_sem);
    //
    // Write the data byte.
    //
    ucTemp = ptRingBuf->pucBuf[ptRingBuf->ulReadIndex];

    //
    // Increment the read index.
    //
    UpdateIndexAtomic(&ptRingBuf->ulReadIndex, 1, ptRingBuf->ulSize);
    pthread_mutex_unlock(&s2e_ringbug_sem);

    //
    // Return the character read.
    //
    return(ucTemp);
}

void
RingBufRead(tRingBufObject *ptRingBuf, unsigned char *pucData,
               unsigned long ulLength)
{
    unsigned long ulTemp;

    //
    // Read the data from the ring buffer.
    //
    for(ulTemp = 0; ulTemp < ulLength; ulTemp++)
    {
        pucData[ulTemp] = RingBufReadOne(ptRingBuf);
    }
}


void
RingBufWriteOne(tRingBufObject *ptRingBuf, unsigned char ucData)
{

    pthread_mutex_lock(&s2e_ringbug_sem);
    //
    // Write the data byte.
    //

    ptRingBuf->pucBuf[ptRingBuf->ulWriteIndex] = ucData;

    //
    // Increment the write index.
    //

    UpdateIndexAtomic(&ptRingBuf->ulWriteIndex, 1, ptRingBuf->ulSize);

	pthread_mutex_unlock(&s2e_ringbug_sem);

}

void
RingBufWrite(tRingBufObject *ptRingBuf, unsigned char *pucData,
                unsigned long ulLength)
{
    unsigned long ulTemp;

    //
    // Write the data into the ring buffer.
    //
    for(ulTemp = 0; ulTemp < ulLength; ulTemp++)
    {
        RingBufWriteOne(ptRingBuf, pucData[ulTemp]);
    }
}

void
RingBufInit(tRingBufObject *ptRingBuf, unsigned char *pucBuf,
               unsigned long ulSize)
{
    //
    // Initialize the ring buffer object.
    //
    
    ptRingBuf->ulSize = ulSize;
    ptRingBuf->pucBuf = pucBuf;
    ptRingBuf->ulWriteIndex = ptRingBuf->ulReadIndex = 0;

	
}


