/********************************** (C) COPYRIGHT ******************************
* File Name          : KEY.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 KEY SCAN
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "KEY.H"


/*******************************************************************************
* Function Name  : KEY_INIT
* Description    : ����ɨ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KEY_INIT( void )
{

    R32_PD_PU |= (KEY_00 | KEY_11 );                                            /* ���� */
    R32_PD_DIR |= (KEY_00 | KEY_11 );                                           /* ��� */

    R32_PD_PU |= (KEY1 | KEY2 | KEY3 | KEY4);                                   /* ���� */
    R32_PD_DIR &= (~(KEY1 | KEY2 | KEY3 | KEY4));                               /* ���� */
}

/*******************************************************************************
* Function Name  : KEY_SCAN
* Description    : ����ɨ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KEY_SCAN( void )
{
    KEY_OUT_LOW( KEY_00 );                                                      /* ����͵�ƽ */
    KEY_OUT_HIGH( KEY_11 );                                                     /* ����ߵ�ƽ */
    Delay_ms( 1 );
    if( !(R32_PD_PIN&KEY1) ){
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY1) ){
            while( !(R32_PD_PIN&KEY1) );
            PRINT("#1");
        }
    }
    if( !(R32_PD_PIN&KEY2) ){ 
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY2) ){
            while( !(R32_PD_PIN&KEY2) );
            PRINT("#2");
        }
    }
    if( !(R32_PD_PIN&KEY3) ){ 
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY3) ){
            while( !(R32_PD_PIN&KEY3) );
            PRINT("#3");
        }
    }
    if( !(R32_PD_PIN&KEY4) ){ 
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY4) ){
            while( !(R32_PD_PIN&KEY4) );
            PRINT("#4");
        }
    }
    KEY_OUT_HIGH( KEY_00 );                                                        
    KEY_OUT_LOW( KEY_11 );                                                       
    Delay_ms( 1 );
    if( !(R32_PD_PIN&KEY1) ){
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY1) ){
            while( !(R32_PD_PIN&KEY1) );
            PRINT("#5");
        }
    }
    if( !(R32_PD_PIN&KEY2) ){ 
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY2) ){
            while( !(R32_PD_PIN&KEY2) );
            PRINT("#6");
        }
    }
    if( !(R32_PD_PIN&KEY3) ){ 
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY3) ){
            while( !(R32_PD_PIN&KEY3) );
            PRINT("#7");
        }
    }
    if( !(R32_PD_PIN&KEY4) ){ 
        Delay_ms( 1 );
        if( !(R32_PD_PIN&KEY4) ){
            while( !(R32_PD_PIN&KEY4) );
            PRINT("#8");
        }
    }
}

/*********************************** endfile **********************************/