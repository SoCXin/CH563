/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Examples
*                      (1)��CH563 USB Examples by KEIL3��    
*                      (2)������0��������Ϣ,115200bps��
*                      (3)��USB HOST�Բ�ѯ��ʽ������
*                      (4)����������: 
*                           ��FAT�ļ�ϵͳ��ʽ����U�̡�
*                           ������������ʾ�г�ָ��Ŀ¼�µ������ļ����Լ���������/ö���ļ���
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include "absacc.h"
#include "SYSFREQ.H"                                                            /* ϵͳ�������ͷ�ļ� */
#include "CH563SFR.H"
#include "EHCI_HCD.H"                                                           /* �����������Ͳ�������ͷ�ļ� */
#include "CH563USBSFR.H"                                                        /* оƬUSB�Ĵ������ͷ�ļ� */
#include "Udisk.H"                                                              /* Udisk�������ͷ�ļ� */
#include "PRINTF.H"                                                             /* ���ڴ�ӡ�������ͷ�ļ� */

/******************************************************************************/
/* ������������ */
UINT8 EHCI_Allocate_Buf[ EHCI_ALLOCATE_BUF_LEN ];                               /* �ڴ���仺����(����QH��QTD�Ƚṹ����Ҫ32�ֽڶ���, ����������QH��QTD����ڴ�ʹ��) */
__align( 4096 ) UINT8 EHCI_Data_Buf[ EHCI_DATA_BUF_LEN ];                       /* �ڴ����ݻ�����(��Ҫ4K����) */                                
__align( 4096 ) UINT8 EHCI_PERIODIC_LIST_Buf[ EHCI_PER_LISE_BUF_LEN ];          /* ������֡�б�������(��Ҫ4K����) */                                

/* ����U����ؽṹ�� */
BULK_ONLY_CMD    mBOC;                                                          /* BulkOnly����ṹ���� */
DISK_INFO        UDisk;                                                         /* U����ؽṹ�嶨�� */                                                        

/******************************************************************************/                
/* ���¶������ϸ˵���뿴CH56XUFI.H�ļ� */
#define DISK_BASE_BUF_LEN        20 * 1024                                      /* Ĭ�ϵĴ������ݻ�������СΪ512�ֽ�,����ѡ��Ϊ2048����4096��֧��ĳЩ��������U��,Ϊ0���ֹ��.H�ļ��ж��建��������Ӧ�ó�����pDISK_BASE_BUF��ָ�� */
/* �����Ҫ���ô������ݻ������Խ�ԼRAM,��ô�ɽ�DISK_BASE_BUF_LEN����Ϊ0�Խ�ֹ��.H�ļ��ж��建����,����Ӧ�ó����ڵ���CH56xLibInit֮ǰ��������������õĻ�������ʼ��ַ����pDISK_BASE_BUF���� */

//#define NO_DEFAULT_ACCESS_SECTOR    1                                         /* ��ֹĬ�ϵĴ���������д�ӳ���,���������б�д�ĳ�������� */
#define NO_DEFAULT_FILE_ENUMER        1                                         /* ��ֹĬ�ϵ��ļ���ö�ٻص�����,���������б�д�ĳ�������� */
#define UNSUPPORT_USB_HUB            1                                          /* Ĭ�ϲ�֧��HUB */

#include "CH56XUFI.H"
#include "CH56XUFI.C"

/* ����һ��LED���ڼ����ʾ����Ľ���,�͵�ƽLED�� */
#define LED_OUT_INIT( )        { R32_PA_OUT |= ELINK; R32_PA_DIR |= ELINK; }    /* ELINK �ߵ�ƽΪ������� */
#define LED_OUT_ACT( )        { R32_PA_CLR |= ELINK; }                          /* ELINK �͵�ƽ����LED��ʾ */
#define LED_OUT_INACT( )    { R32_PA_OUT |= ELINK; }                            /* ELINK �ߵ�ƽ�ر�LED��ʾ */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    while( 1 );
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ�жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{
    while( 1 );
}

/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : Ϊprintf��getkey���������ʼ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void mInitSTDIO( void )
{
    UINT32    x, x2;

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        /* 115200bps */
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        /* �������� */
    R8_UART0_LCR = 0x80;                                                        /* DLABλ��1 */
    R8_UART0_DIV = 1;                                                           /* Ԥ��Ƶ */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* �����ֽڳ���Ϊ8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* ����FIFO������Ϊ14���巢�ͺͽ���FIFO��FIFOʹ�� */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : ͨ��������������Ϣ
* Input          : c-- writes the character specified by c 
*                  *f--the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/

int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* �������� */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* �ȴ����ݷ��� */
    return( c );
}

/*******************************************************************************
* Function Name  : mStopIfError
* Description    : ������״̬
*                  �����������ʾ������벢ͣ��,Ӧ���滻Ϊʵ�ʵĴ�����ʩ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError( UINT8 iError )
{
    if( iError == ERR_SUCCESS ) 
    {    
        return;                                                                 /* �����ɹ� */
    }
    printf( "Error: %02X\n", (UINT16)iError );                                  /* ��ʾ���� */
    
    /* ���������,Ӧ�÷����������Լ�CH56xDiskStatus״̬,�������CH56xDiskConnect��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
       ���������Ĵ�������:
       1������һ��CH56xDiskReady,�ɹ����������,����Open,Read/Write��,��CH56xDiskReady�л��Զ�����CH56xDiskConnect�������������
       2�����CH56xDiskReady���ɹ�,��ôǿ�н�CH56xDiskStatus��ΪDISK_CONNECT״̬,Ȼ���ͷ��ʼ����(�ȴ�U������CH56xDiskConnect��CH56xDiskReady��) */
    while( 1 ) 
    {
        LED_OUT_ACT( );                                                         /* LED��˸ */
        Delay_ms( 200 );
        LED_OUT_INACT( );
        Delay_ms( 200 );
    }
}

/*******************************************************************************
���³���ΪEXAM13ö��U�����ļ�����
*******************************************************************************/
typedef struct _FILE_NAME 
{
    UINT32    DirStartClust;                                                    /* �ļ�����Ŀ¼����ʼ�غ� */
//    UINT32    Size;                                                           /* �ļ����� */
    UINT8    Name[8+3+1+1];                                                     /* �ļ���,��8+3�ֽ�,�ָ���,������,��Ϊδ����Ŀ¼�����������·�� */
    UINT8    Attr;                                                              /* �ļ����� */
    UINT8    Reserved[2];                                                       /* for 4 bytes align */
}FILE_NAME;

#define    MAX_FILE_COUNT               200
FILE_NAME    FileNameBuffer[ MAX_FILE_COUNT ];                                  /* �ļ����ṹ */
UINT16    FileCount;
UINT32    CurrentDirStartClust;                                                 /* ���浱ǰĿ¼����ʼ�غ�,���ڼӿ��ļ�ö�ٺʹ��ٶ� */

/*******************************************************************************
* Function Name  : ListFile
* Description    : �о�ָ��Ŀ¼�µ������ļ�
*                  �������mCmdParam.Open.mPathName[]ΪĿ¼���ַ���,��ʽ���ļ���
*                  ��ͬ,����б���������Ŀ¼
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 ListFile( void )
{
    UINT8  i;

    printf( "List Directory: %s\n", mCmdParam.Open.mPathName );                 /* ��ʾĿ¼�� */
    i = strlen( (PCCHAR)mCmdParam.Open.mPathName );                             /* ����·���ĳ���,��Ŀ¼���Ľ����� */
    if( i && mCmdParam.Open.mPathName[ i - 1 ] == '/' )                         /* �Ǹ�Ŀ¼,�������Ѿ���·���ָ��� */
    { 
    }  
    else 
    {
        mCmdParam.Open.mPathName[ i++ ] = '/';                                  /* �ڵ�ǰĿ¼�½���ö��,����Ŀ¼�ⶼ�����·��,���Ǹ�Ŀ¼���·���ָ��� */
    }
    mCmdParam.Open.mPathName[ i++ ] = '*';                                      /* ö��ͨ���,������·������"\*"����"\C51\*"����"\C51\CH563*"�� */
    mCmdParam.Open.mPathName[ i ] = 0;                                          /* ������ */
    CH56xvFileSize = 0xFFFFFFFF;                                                /* ��������ö��,ÿ�ҵ�һ���ļ�����һ��xFileNameEnumer�ص��ӳ���,���ֵС��0x80000000��ÿ��ֻö��һ���ļ�̫�� */
    i = CH56xFileOpen( );                                                       /* ö��,�ɻص�����xFileNameEnumer������¼���浽�ṹ�� */
    if( i == ERR_SUCCESS || i == ERR_FOUND_NAME || i == ERR_MISS_FILE ) 
    {  
        /* �����ɹ�,ͨ�����᷵��ERR_SUCCESS,����xFileNameEnumer��ǰ�˳�ʱ�Ż᷵��ERR_FOUND_NAME */
        printf( "Success, new FileCount = %d\n", FileCount );
        return( ERR_SUCCESS );
    }
    else 
    {
        printf( "Failed, new FileCount = %d\n", FileCount );
        return( i );
    }
}

/*******************************************************************************
* Function Name  : ListAll
* Description    : �Թ�����ȵ��㷨ö������U���е������ļ���Ŀ¼
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 ListAll( void )
{
    UINT8  i;
    UINT16 OldFileCount;

    OldFileCount = FileCount = 0;                                               /* ���ļ��ṹ���� */
    FileNameBuffer[ 0 ].Name[ 0 ] = '/';                                        /* ��Ŀ¼,������·����,����Ŀ¼�Ǿ���·��֮�ⶼ�����·�� */
    FileNameBuffer[ 0 ].Name[ 1 ] = 0;
    FileNameBuffer[ 0 ].DirStartClust = 0;                                      /* ��Ŀ¼��������������� */
    FileNameBuffer[ 0 ].Attr = ATTR_DIRECTORY;                                  /* ��Ŀ¼Ҳ��Ŀ¼,��Ϊ��һ����¼���� */

    for( FileCount = 1; OldFileCount < FileCount; OldFileCount ++ ) 
    {  
        /* ������ö�ٵ����ļ����ṹδ���з��� */
        if( FileNameBuffer[ OldFileCount ].Attr & ATTR_DIRECTORY ) 
        {  
            /* ��Ŀ¼���������������� */
            strcpy( (PCHAR)mCmdParam.Open.mPathName, (PCCHAR)FileNameBuffer[ OldFileCount ].Name );  /* Ŀ¼��,����Ŀ¼�ⶼ�����·�� */
            CH56xvStartCluster = FileNameBuffer[ OldFileCount ].DirStartClust;  /* ��ǰĿ¼���ϼ�Ŀ¼����ʼ�غ�,���������·����,������·�����ٶȿ� */
            i = CH56xFileOpen( );                                               /* ��Ŀ¼,��Ϊ�˻�ȡĿ¼����ʼ�غ�������ٶ� */
            if( i == ERR_SUCCESS ) 
            {
                return( ERR_MISS_DIR );                                         /* Ӧ���Ǵ���Ŀ¼,���Ƿ��ؽ���Ǵ����ļ� */
            }
            if( i != ERR_OPEN_DIR ) 
            {
                return( i );
            }
            if( OldFileCount ) 
            {
                CurrentDirStartClust = CH56xvStartCluster;                      /* ���Ǹ�Ŀ¼,��ȡĿ¼����ʼ�غ� */
            }
            else
            {  
                /* �Ǹ�Ŀ¼,��ȡ��Ŀ¼����ʼ�غ� */
                if( CH56xvDiskFat == DISK_FAT32 ) 
                {
                    CurrentDirStartClust = CH56xvDiskRoot;                      /* FAT32��Ŀ¼ */
                }
                else 
                {
                    CurrentDirStartClust = 0;                                   /* FAT12/FAT16��Ŀ¼ */
                }
            }
            CH56xFileClose( );                                                  /* ���ڸ�Ŀ¼һ��Ҫ�ر� */
//            strcpy( mCmdParam.Open.mPathName, FileNameBuffer[ OldFileCount ].Name );  /* Ŀ¼��,����mPathNameδ���޸����������ٸ��� */
            CH56xvStartCluster = FileNameBuffer[ OldFileCount ].DirStartClust;  /* ��ǰĿ¼���ϼ�Ŀ¼����ʼ�غ�,���������·����,������·�����ٶȿ� */
            i = ListFile( );                                                    /* ö��Ŀ¼,�ɻص�����xFileNameEnumer������¼���浽�ṹ�� */
            if( i != ERR_SUCCESS ) 
            {
                return( i );
            }
        }
    }

    /* U���е��ļ���Ŀ¼ȫ��ö�����,���濪ʼ���ݽṹ��¼���δ��ļ� */
    printf( "Total file&dir = %d, Open every file:\n", FileCount );
    for( OldFileCount = 0; OldFileCount < FileCount; OldFileCount ++ ) 
    {
        if( ( FileNameBuffer[ OldFileCount ].Attr & ATTR_DIRECTORY ) == 0 ) 
        {  
            /* ���ļ����,Ŀ¼������ */
            printf( "Open file: %s\n", FileNameBuffer[ OldFileCount ].Name );
            strcpy( (PCHAR)mCmdParam.Open.mPathName, (PCCHAR)FileNameBuffer[ OldFileCount ].Name );  /* ���·�� */
            CH56xvStartCluster = FileNameBuffer[ OldFileCount ].DirStartClust;  /* ��ǰ�ļ����ϼ�Ŀ¼����ʼ�غ�,���������·����,������·�����ٶȿ� */
            i = CH56xFileOpen( );                                               /* ���ļ� */
            if( i == ERR_SUCCESS ) 
            {  
                /* �ɹ������ļ� */
                mCmdParam.Read.mDataBuffer = (PUINT8)( & EHCI_Data_Buf[ 0 ] );  /* ָ���ļ����ݻ���������ʼ��ַ */
                mCmdParam.Read.mSectorCount = 1;                                /* ��ȡ������ */
                CH56xFileRead( );
//                CH56xFileClose( );                                            /* ����д������������ر� */
            }
        }
    }
    return( ERR_SUCCESS );
}

/*******************************************************************************
* Function Name  : xFileNameEnumer
* Description    : �ļ���ö�ٻص��ӳ���,�ο�CH56XUFI.H�ļ��е�����
*                  ÿ������һ���ļ�FileOpen������ñ��ص�����,xFileNameEnumer��
                   �غ�,FileOpen�ݼ�CH56xvFileSize������ö��ֱ�����������ļ���
                   ��Ŀ¼.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void xFileNameEnumer( void )        
{  
    UINT8  i, c;
    P_FAT_DIR_INFO    pFileDir;
    PUINT8            pNameBuf;

    pFileDir = (P_FAT_DIR_INFO)( pDISK_BASE_BUF + CH56xvFdtOffset );            /* ��ǰFDT����ʼ��ַ */
    if( pFileDir -> DIR_Name[ 0 ] == '.' ) 
    {
        return;                                                                 /* �Ǳ��������ϼ�Ŀ¼��,���붪�������� */
    }    
    if( ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) == 0 ) 
    {  
        /* �ж����ļ��� */
        if( pFileDir -> DIR_Name[ 8 ] == 'H' && pFileDir -> DIR_Name[ 9 ] == ' ' /* �����ļ�����չ��,��".H"�ļ�,����,���Ǽǲ����� */
         || pFileDir -> DIR_Name[ 8 ] == 'E' && pFileDir -> DIR_Name[ 9 ] == 'X' && pFileDir -> DIR_Name[10] == 'E' ) 
        {
            return;                                                             /* ��չ����".EXE"���ļ�,���� */
        }
    }
    pNameBuf = & FileNameBuffer[ FileCount ].Name[ 0 ];                         /* �ļ����ṹ�е��ļ��������� */
    for( i = 0; i < 11; i ++ ) 
    {  
        /* �����ļ���,����Ϊ11���ַ� */
        c = pFileDir -> DIR_Name[ i ];
        if ( i == 0 && c == 0x05 ) 
        {
            c = 0xE5;                                                           /* �����ַ� */
        }
        if( c != 0x20 ) 
        {  
            /* ��Ч�ַ� */
            if( i == 8 ) 
            {  
                /* ������չ�� */
                *pNameBuf = '.';                                                /* �ָ��� */
                pNameBuf ++;
            }
            *pNameBuf = c;                                                      /* �����ļ�����һ���ַ� */
            pNameBuf ++;
        }
    }
    *pNameBuf = 0;                                                              /* ��ǰ�ļ�������·���Ľ����� */
    FileNameBuffer[ FileCount ].DirStartClust = CurrentDirStartClust;           /* ��¼��ǰĿ¼����ʼ�غ�,���ڼӿ��ļ����ٶ� */
    FileNameBuffer[ FileCount ].Attr = pFileDir -> DIR_Attr;                    /* �ļ����� */
    if( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) 
    {  
        /* �ж���Ŀ¼�� */
        printf( "Dir %4d#: %s\n", FileCount, FileNameBuffer[ FileCount ].Name );
    }
    else 
    {  
        /* �ж����ļ��� */
        printf( "File%4d#: %s\n", FileCount, FileNameBuffer[ FileCount ].Name );
    }
    FileCount ++;                                                               /* �ļ����� */
    if( FileCount >= MAX_FILE_COUNT ) 
    {  
        /* �ļ����ṹ������̫С,�ṹ�������� */
        CH56xvFileSize = 1;                                                     /* ǿ����ǰ����ö��,����FileOpen�����ٻص�xFileNameEnumer������ǰ����,��ֹ��������� */
        printf( "FileName Structure Full\n" );
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int    main( void ) 
{
    UINT8  status;
    UINT8  i;

//    SysFreq( );                                                               /* ������Ҫ����PLL�����Ӧ��ʱ��Ƶ�� */
    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* ������LED��һ����ʾ���� */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */

    printf( "CH563_FAT_EXAM13_V1.20_2013.08.12...................................\xd\xa" );

#if DISK_BASE_BUF_LEN == 0
    pDISK_BASE_BUF = &my_buffer[0];                                             /* ����.H�ļ��ж���CH563��ר�û�����,�����û�����ָ��ָ������Ӧ�ó���Ļ��������ں����Խ�ԼRAM */
#endif

    /* USB������س�ʼ�� */
    USBHOST_ModeSet( );                                                         /* ����USB����ģʽ */
    USBHOST_EHCI_Init( );                                                       /* USB����EHCI��λ����ʼ�� */
    USBHOST_Structure_DeInit( );                                                /* USB�������ݽṹ��ʼ�� */
    USBHOST_DevicePara_Init( &UDisk.Device );                                   /* USB������ز�����ʼ�� */

    i = CH56xLibInit( );                                                        /* ��ʼ��CH563������CH563оƬ,�����ɹ�����0 */
    mStopIfError( i );
    /* ������·��ʼ�� */

    while ( 1 ) 
    {
        /***********************************************************************/
        /* ��ѯ�ȴ��豸���� */
        printf("Wait USB Device In:\n" );

#ifdef UNSUPPORT_USB_HUB

        /* �������Ҫ֧��USB-HUB,��ô�ȴ�U�̲���ĳ�����CH563����,����ͨ��CH56xDiskConnect��ѯ����,��������ͨ��CH56xDiskReady�ȴ�����,Ȼ���д */
        while ( CH56xDiskStatus < DISK_CONNECT ) 
        {  
            /* ��ѯCH563�жϲ������ж�״̬,�ȴ�U�̲��� */
            CH56xDiskConnect( );
            Delay_ms( 50 );                                                     /* û��ҪƵ����ѯ */
        }
        LED_OUT_ACT( );                                                         /* LED�� */
        Delay_ms( 200 );                                                        /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */
    
        /* ��ʼ���˵�0 */            
        status = USBHOST_EHCI_EP0_Init( &UDisk.Device );                        /* USB�������ƶ˵�0��ʼ�� */
        if( status != USB_OPERATE_SUCCESS )
        {            
            printf("USBHOST_EP0_Init Error\n");        
            while( 1 );
        }

        /* ���ڼ�⵽USB�豸��,���ȴ�100*50mS,��Ҫ�����ЩMP3̫��,���ڼ�⵽USB�豸��������DISK_MOUNTED��,���ȴ�5*50mS,��Ҫ���DiskReady������ */
        for( i = 0; i < 100; i ++ ) 
        {  
            /* ��ȴ�ʱ��,100*50mS */
            Delay_ms( 50 );
            printf( "Ready ?\n" );
            if( CH56xDiskReady( ) == ERR_SUCCESS ) 
            {
                break;                                                          /* ��ѯ�����Ƿ�׼���� */
            }
            if( CH56xDiskStatus < DISK_CONNECT ) 
            {
                break;                                                          /* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */
            }
            if( CH56xDiskStatus >= DISK_MOUNTED && i > 5 ) 
            {
                break;                                                          /* �е�U�����Ƿ���δ׼����,�������Ժ���,ֻҪ�佨������MOUNTED�ҳ���5*50mS */
            }
        }
        if( CH56xDiskStatus < DISK_CONNECT ) 
        {  
            /* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */
            printf( "Device gone\n" );
            continue;                                                           /* ���µȴ� */
        }
        if( CH56xDiskStatus < DISK_MOUNTED ) 
        {  
            /* δ֪USB�豸,����USB���̡���ӡ���� */
            printf( "Unknown device\n" );
            goto UnknownUsbDevice;
        }
        
#else

        /* �����Ҫ֧��USB-HUB,��ô����ο�����������ĵȴ����� */
        while ( 1 ) 
        {  
            /* ֧��USB-HUB */
            Delay_ms( 50 );                                                     /* û��ҪƵ����ѯ */
            
            if( CH56xDiskConnect( ) == ERR_SUCCESS ) 
            {  
                /* ��ѯ��ʽ: �������Ƿ����Ӳ����´���״̬,���سɹ�˵������ */
                Delay_ms( 200 );                                                /* ��ʱ,��ѡ����,�е�USB�洢����Ҫ��ʮ�������ʱ */

                /***********************************************************************/
                /* ��ʼ���˵�0 */    
                status = USBHOST_EP0_Init( );                                   /* USB�������ƶ˵�0��ʼ�� */
                if( status != USB_OPERATE_SUCCESS )
                {
#ifdef  MY_DEBUG_PRINTF            
                    printf("USBHOST_EP0_Init Error\n");
#endif            
                    mStopIfError( status );
                }
        
                /* ���ڼ�⵽USB�豸��,���ȴ�100*50mS,��Ҫ�����ЩMP3̫��,���ڼ�⵽USB�豸��������DISK_MOUNTED��,���ȴ�5*50mS,��Ҫ���DiskReady������ */
                for( i = 0; i < 100; i ++ ) 
                {  
                    /* ��ȴ�ʱ��,100*50mS */
                    Delay_ms( 50 );
                    printf( "Ready ?\n" );
                    if( CH56xDiskReady( ) == ERR_SUCCESS ) 
                    {
                        break;                                                  /* ��ѯ�����Ƿ�׼���� */
                    }
                    if( CH56xDiskStatus < DISK_CONNECT ) 
                    {  
                        /* ��⵽�Ͽ�,���¼�Ⲣ��ʱ */
                        printf( "Device gone\n" );
                        break;                                                  /* ���µȴ� */
                    }
                    if( CH56xDiskStatus >= DISK_MOUNTED && i > 5 ) 
                    {
                        break;                                                  /* �е�U�����Ƿ���δ׼����,�������Ժ���,ֻҪ�佨������MOUNTED�ҳ���5*50mS */
                    }
                    if( CH56xDiskStatus == DISK_CONNECT ) 
                    {  
                        /* ���豸���� */
                        if( CH56xvHubPortCount ) 
                        {  
                            /* ������һ��USB-HUB,������û��U�� */
                            printf( "No Udisk in USB_HUB\n" );
                            break;
                        }
                        else
                        {  
                            /* δ֪USB�豸,�п�����U�̷�Ӧ̫��,����Ҫ������ */
                        }
                    }
                }
                if( CH56xDiskStatus >= DISK_MOUNTED ) 
                {  
                    /* ��U�� */
                    break;                                                      /* ��ʼ����U�� */
                }
                if( CH56xDiskStatus == DISK_CONNECT ) 
                {  
                    /* ��γ��Ի��ǲ���,���Ʋ���U�� */
                    if ( CH56xvHubPortCount ) 
                    {  
                        /* ������һ��USB-HUB,������û��U�� */
                        /* ��while�еȴ�HUB�˿���U�� */
                    }
                    else 
                    {  
                        /* δ֪USB�豸,����USB���̡���ӡ����,�����Ѿ����˺ܶ�λ����� */
                        printf( "Unknown device\n" );
                        goto UnknownUsbDevice;
                    }
                }
            }
        }
        LED_OUT_ACT( );                                                         /* LED�� */
#endif
        
        /* ��ʾU�������Ϣ */
        printf( "TotalSize = %lu MB \n", mCmdParam.DiskReady.mDiskSizeSec >> ( 20 - CH56xvSectorSizeB ) );  /* ��ʾΪ��MBΪ��λ������ */
        printf( "Current disk sector size = %d Bytes \n", CH56xvSectorSize );   /* CH56xvSectorSize��U�̵�ʵ��������С */
#if DISK_BASE_BUF_LEN
        if( DISK_BASE_BUF_LEN < CH56xvSectorSize ) 
        {  
            /* ���������ݻ������Ƿ��㹻��,CH56xvSectorSize��U�̵�ʵ��������С */
            printf( "Too large sector size\n" );
            goto UnknownUsbDevice;
        }
#endif

        Delay_ms( 20 );
        printf( "List all file \n" );
        i = ListAll( );                                                         /* ö������U���е������ļ���Ŀ¼ */
        mStopIfError( i );

UnknownUsbDevice:
        printf( "Take out\n" );
        while( 1 ) 
        {  
            /* ֧��USB-HUB */
            Delay_ms( 10 );                                                     /* û��ҪƵ����ѯ */
            if( CH56xDiskConnect( ) != ERR_SUCCESS ) 
            {    
                break;                                                          /* ��ѯ��ʽ: �������Ƿ����Ӳ����´���״̬,���سɹ�˵������ */
            }
        }
        LED_OUT_INACT( );                                                       /* LED�� */
        
        /* ���³�ʼ��U����ز�����������Դ */
        USBHOST_DevicePara_Init( &UDisk.Device );                               /* USB������ز�����ʼ�� */
        USBHOST_Structure_DeInit( );                                            /* USB�������ݽṹ��ʼ�� */        
        Delay_ms( 200 );
    }
}

