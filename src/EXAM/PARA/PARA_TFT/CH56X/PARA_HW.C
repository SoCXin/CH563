/********************************** (C) COPYRIGHT ******************************
* File Name          : PARA_HW.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/09/2
* Description        : ���߽ӿں���
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include    "HAL.H"

PUINT16V        pXbusPt;

void CH563_PORT_INIT(void)
{

    R32_PA_DIR = (1<<21)|( 1<<20 )|( 1<<19 )|(1<<16);                           /* �õ�ַ��WR��RD��CS�ź���� */ 
    R32_PA_PU  = ((1<<21)|( 1<<20 )|( 1<<19 ))&(~(1<<16));                      /* GPIO A��������,��1��ʾ���� */

    R8_SAFE_ACCESS_SIG = 0x57;                                                  /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8;                                                  /* unlock step 2 */
    R8_XBUS_CONFIG = RB_XBUS_ENABLE | RB_XBUS_ADDR_OE | RB_XBUS_EN_32BIT;       /* �ⲿ����ʹ�� */
    R8_XBUS_SETUP_HOLD = 0x04;                                                  /* 1 setup clocks */
    R8_SAFE_ACCESS_SIG = 0x00;                                                  /* lock, to prevent unexpected writing */
}

/* д�Ĵ������� */
/* CMD:�Ĵ���ֵ */
void  LCD_WR_REG( UINT16 reg )          
{
    pXbusPt = ( PUINT16V )0x00C00000;
    *pXbusPt = reg;    
}

/* дLCD���� */
/* data:Ҫд���ֵ */
void  LCD_WR_DATA( UINT16 mData )          
{
    pXbusPt = ( PUINT16V )0x00C10000;
    *pXbusPt = mData;
}

/* ��LCD���� */
/* ����ֵ:������ֵ */
UINT16 LCD_RD_DATA( void  )          
{
    UINT16 mData;

    pXbusPt = ( PUINT16V )0x00C10000;
    mData = *pXbusPt ;
    return (mData);
}
     
/* д�Ĵ��� */
/* LCD_Reg:�Ĵ�����ַ */
/* LCD_RegValue:Ҫд������� */
void  LCD_WriteReg(UINT16 LCD_Reg, UINT16 LCD_RegValue)
{
    LCD_WR_REG( LCD_Reg );
    LCD_WR_DATA( LCD_RegValue );
}

/* ���Ĵ��� */
/* LCD_Reg:�Ĵ�����ַ */
/* ����ֵ:���������� */
UINT16 LCD_ReadReg(UINT16 LCD_Reg)
{
    LCD_WR_REG( LCD_Reg );
    return( LCD_RD_DATA( ) );
}

/* ��ʼдGRAM */
void  LCD_WriteRAM_Prepare(void)
{
     LCD_WR_REG(lcddev.wramcmd);      
}
     
/* LCDдGRAM */
/* RGB_Code:��ɫֵ */
void  LCD_WriteRAM(UINT16 RGB_Code)
{                                
    LCD_WR_DATA( RGB_Code );                                                    /* дʮ��λGRAM */
}

/*********************************** endfile **********************************/