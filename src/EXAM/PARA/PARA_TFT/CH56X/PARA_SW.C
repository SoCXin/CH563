/********************************** (C) COPYRIGHT ******************************
* File Name          : PARA_SW.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : ģ�Ⲣ��
*******************************************************************************/



/******************************************************************************/
#include    "HAL.H"

#define PARA_RS                ( 1 << 16 )                                    /* RS(PA16) */
#define PARA_CS                ( 1 << 19 )                                    /* CS(PA19) */
#define PARA_RD                ( 1 << 20 )                                    /* RD(PA20) */
#define PARA_WR                ( 1 << 21  )                                   /* WR(PA21) */
    

#define PIN_A0_LOW( )     { R32_PA_CLR |= PARA_RS; }  /* ģ�Ⲣ��A0��������͵�ƽ */
#define PIN_A0_HIGH( )    { R32_PA_OUT |= PARA_RS; }  /* ģ�Ⲣ��A0��������ߵ�ƽ */
#define PIN_PCS_LOW( )    { R32_PA_CLR |= PARA_CS; }  /* ģ�Ⲣ��Ƭѡ��������͵�ƽ */
#define PIN_PCS_HIGH( )   { R32_PA_OUT |= PARA_CS; }  /* ģ�Ⲣ��Ƭѡ��������ߵ�ƽ */
#define PIN_RD_LOW( )     { R32_PA_CLR |= PARA_RD; }  /* ģ�Ⲣ��RD��������͵�ƽ */
#define PIN_RD_HIGH( )    { R32_PA_OUT |= PARA_RD; }  /* ģ�Ⲣ��RD��������ߵ�ƽ */
#define PIN_WR_LOW( )     { R32_PA_CLR |= PARA_WR; }  /* ģ�Ⲣ��WR��������͵�ƽ */
#define PIN_WR_HIGH( )    { R32_PA_OUT |= PARA_WR; }  /* ģ�Ⲣ��WR��������ߵ�ƽ */


void CH563_PORT_INIT(void)
{
    R32_PD_DIR &= ~(0xFFFF);                                                    /* ��ֹ������� */                            
    R32_PA_DIR = (1<<21)|( 1<<20 )|( 1<<19 )|(1<<16);                           /* �õ�ַ��WR��RD�ź���� */ 
    R32_PA_PU  = (1<<21)|( 1<<20 )|( 1<<19 )|(1<<16); ;                         /* GPIO A �������ã� ��1��ʾ���� */ 
    PIN_PCS_HIGH( );
    PIN_A0_HIGH( );
    PIN_WR_HIGH( );
    PIN_RD_HIGH( );
}

/*******************************************************************************
* Function Name  : LCD_WR_REG
* Description    : д�Ĵ�������
* Input          : mCmd---Ҫд�������
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WR_REG( UINT16 mCmd )  
{
    R32_PD_OUT = mCmd;                                                          /* ������� */    
    R32_PD_DIR |= 0xFFFF;                                                       /* ���ò��ڷ���Ϊ��� */ 

    PIN_PCS_LOW( );
    PIN_A0_LOW( );

    PIN_WR_LOW( );                                                              /* �����Чд�����ź� */        
    PIN_PCS_LOW( );                                                             /* �ò���������,������ʱ */
    PIN_PCS_LOW( );                                                             /* �ò���������,������ʱ */
    PIN_WR_HIGH( );                                                             /* �����Ч�Ŀ����ź� */
 
    PIN_PCS_HIGH( );
    R32_PD_DIR &= ~(0xFFFF);                                                    /* ��ֹ������� */                            
    Delay_us( 2 );      
}

/*******************************************************************************
* Function Name  : LCD_WR_DATA
* Description    : д����
* Input          : mData---д����
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WR_DATA( UINT16 mData )     
{
    R32_PD_OUT = mData ;                                                        /* ������� */    
    R32_PD_DIR |= 0xFFFF;

    PIN_PCS_LOW( );    
    PIN_A0_HIGH( );    

    PIN_WR_LOW( );                                                              /* �����Чд�����ź�, д���� */        
    PIN_PCS_LOW( );                                                             /* �ò���������,������ʱ */
    PIN_PCS_LOW( );                                                             /* �ò���������,������ʱ */
    PIN_WR_HIGH( );                                                             /* �����Ч�Ŀ����ź� */
 
    PIN_PCS_HIGH( );    
    R32_PD_DIR &= ~(0xFFFF);                                                    /* ��ֹ������� */                            
}

/*******************************************************************************
* Function Name  : LCD_RD_DATA
* Description    : ������
* Input          : None
* Output         : None
* Return         : ���ض�ȡ������
*******************************************************************************/
UINT16 LCD_RD_DATA( void )  
{
    UINT16  mData;

    Delay_us(1);
    R32_PD_DIR &= ~(0xFFFF);                                                    /* ����Ϊ���� */                            
 
    PIN_PCS_LOW( );
  
    PIN_A0_HIGH( );    

    PIN_RD_LOW( );                                                              /* �����Ч�������ź�, ������ */
    PIN_PCS_LOW( );                                                             /* �ò���������,������ʱ */
    PIN_PCS_LOW( );                                                             /* �ò���������,������ʱ */
    mData = (UINT16)( R32_PD_OUT );                                             /* �������� */
    PIN_RD_HIGH( );                                                             /* �����Ч�Ŀ����ź� */
    PIN_PCS_HIGH( );
 
    return( mData );
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
    LCD_WR_DATA( RGB_Code );                                                     /* дʮ��λGRAM    */
}

/*******************************************************************************/