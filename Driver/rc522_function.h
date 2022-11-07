#ifndef __RC522_FUNCTION_H
#define	__RC522_FUNCTION_H


#include "stm32f10x_it.h"


#define          macDummy_Data              0x00


void             PcdReset                   ( void );                       //��λ
void             M500PcdConfigISOType       ( u8 type );                    //������ʽ
char             PcdRequest                 ( u8 req_code, u8 * pTagType ); //Ѱ��
char             PcdAnticoll                ( u8 * pSnr);                   //������
char             IC_CMT                     ( u8 * PID, u8 * KEY, u8 RW, u8 * Dat );
u8 GET_PID( u8 *PID);
unsigned char             IC_test                    ( void );
#endif /* __RC522_FUNCTION_H */

