/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.C
* Author             : WCH 
* Version            : V1.0
* Date               : 2015/03/02
* Description        : CH563 TCP/IP Э����ӿ�
*                    : MDK3.36@ARM966E-S,Thumb,С��
*******************************************************************************/


/******************************************************************************/
/* ͷ�ļ�����*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

#include "upnp.h"
#include "ISPXT56X.H"

#define CH563NET_DBG                          1
#define  CH563NET_TCP_MSS                     536
#include "CH563NET.H"
/* ����Ļ�������ȫ�ֱ�������Ҫ���壬���е��� */
__align(16)UINT8    CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                   /* MAC������������������16�ֽڶ��� */
__align(4) UINT8    CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                /* MAC���ջ�������4�ֽڶ��� */
__align(4) SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];                         /* Socket��Ϣ����4�ֽڶ��� */
const UINT16 MemNum[8] = {CH563NET_NUM_IPRAW,
                         CH563NET_NUM_UDP,
                         CH563NET_NUM_TCP,
                         CH563NET_NUM_TCP_LISTEN,
                         CH563NET_NUM_TCP_SEG,
                         CH563NET_NUM_IP_REASSDATA,
                         CH563NET_NUM_PBUF,
                         CH563NET_NUM_POOL_BUF
                         };
const UINT16 MemSize[8] = {CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_IPRAW_PCB),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_UDP_PCB),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_PCB),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_PCB_LISTEN),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_SEG),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_IP_REASSDATA),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_PBUF) + CH563NET_MEM_ALIGN_SIZE(0),
                          CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_PBUF) + CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_POOL_BUF)
                         };
__align(4)UINT8 Memp_Memory[CH563NET_MEMP_SIZE];
__align(4)UINT8 Mem_Heap_Memory[CH563NET_RAM_HEAP_SIZE];
__align(4)UINT8 Mem_ArpTable[CH563NET_RAM_ARP_TABLE_SIZE];

#define CH563NET_TIMEPERIOD                   10                                /* ��ʱ����ʱ���ڣ���λmS*/

/******************************************************************************/
/* ����ʾ�������غ� */
#define RECE_BUF_LEN                          CH563NET_TCP_MSS                  /* ���ջ������Ĵ�С */
#define READ_RECV_BUF_MODE                    0                                 /* socket���ջ�������ȡģʽ��1�����ƣ�0�������� */

UINT8 SocketRecvBuf[8192];                                                      /* socket���ջ����� */
/* CH563��ض��� */                                                                   
UINT8 MACAddr[6]  = {0x02,0x03,0x04,0x05,0x06,0x07};                            /* CH563MAC��ַ */ 
const UINT8 IPAddr[4]   = {192,168,1,101};                                      /* CH563IP��ַ */ 
const UINT8 GWIPAddr[4] = {192,168,1,1};                                        /* CH563���� */ 
const UINT8 IPMask[4]   = {255,255,255,0};                                      /* CH563�������� */ 

UINT8 SocketId;                                                                 /* ����socket���������Բ��ö��� */
UINT8 gPort; 
UINT8 Time = 0;                                                                 /* ���ڸı�˿�ֵ */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                                   /* ��̫���ж� */
    {                                                                           /* ��̫���ж��жϷ����� */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                               /* ��ʱ���ж� */
    {
         CH563NET_TimeIsr(CH563NET_TIMEPERIOD);                                 /* CH563NET_ ��ʱ���жϷ����� */
         Time++;
         if(Time > 25)
         {
             Time = 0;
             CH563NET_UPNPTimeIsr();
         }
         R8_TMR0_INT_FLAG |= 0xff;                                              /* �����ʱ���жϱ�־ */
   }
}

__irq void FIQ_Handler( void )
{
    while(1);
}

/*******************************************************************************
* Function Name  : SysTimeInit
* Description    : ϵͳ��ʱ����ʼ����CH563@100MHZ TIME0 10ms������CH563NET_TIMEPERIOD
*                ������ʼ����ʱ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTimeInit(void)
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT = 0x00000000; 
    R32_TMR0_CNT_END = 0x186a0 * CH563NET_TIMEPERIOD;                           /* ����Ϊ10MS��ʱ */
    R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
}

/*******************************************************************************
* Function Name  : InitSysHal
* Description    : Ӳ����ʼ������������TIM0��ETH�ж�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSysHal(void)
{
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                          /* ����TIM0�ж� */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ETH;                                           /* ����ETH�ж� */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* ����IRQȫ���ж� */
}

/*******************************************************************************
* Function Name  : mStopIfError
* Description    : ����ʹ�ã���ʾ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
    if (iError == CH563NET_ERR_SUCCESS) return;                                 /* �����ɹ� */
#if CH563NET_DBG
    printf("Error: %02X\n", (UINT16)iError);                                    /* ��ʾ���� */
#endif    
}

/*******************************************************************************
* Function Name  : CH563NET_LibInit
* Description    : ���ʼ������
* Input          : ip      ip��ַָ��
*                ��gwip    ����ip��ַָ��
*                : mask    ����ָ��
*                : macaddr MAC��ַFָ�� 
* Output         : None
* Return         : ִ��״̬
*******************************************************************************/
UINT8 CH563NET_LibInit(const UINT8 *ip,const UINT8 *gwip,const UINT8 *mask,const UINT8 *macaddr)
{
    UINT8 i;
    struct _CH563_CFG cfg;
    if(CH563NET_GetVer() != CH563NET_LIB_VER)return 0xfc;                       /* ��ȡ��İ汾�ţ�����Ƿ��ͷ�ļ�һ�� */
    CH563NETConfig = LIB_CFG_VALUE;                                             /* ��������Ϣ���ݸ�������ñ��� */
    cfg.RxBufSize = RX_BUF_SIZE; 
    cfg.TCPMss   = CH563NET_TCP_MSS;
    cfg.HeapSize = CH563_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
    CH563NET_ConfigLIB(&cfg);
    i = CH563NET_Init(ip,gwip,mask,macaddr);
#if CH563NET_DBG                                                         
    printf("CH563NET_Config: %x\n",CH563NETConfig);                           
#endif        
    return (i);                                                                 /* ���ʼ�� */
}

/*******************************************************************************
* Function Name  : CH563NET_HandleSokcetInt
* Description    : socket�жϴ�������
* Input          : sockeid  socket����
*                ��initstat �ж�״̬
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleSokcetInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
    UINT8   upnpflag = 0;
#if  CH563NET_DBG
     //   printf("sockeid��%02x  %02x\n",(UINT16)sockeid,initstat);
#endif

    upnpflag = CH563NET_UPNPCheckSocketAvail(sockeid);
    if(initstat & SINT_STAT_RECV)                                               /* �����ж� */
    {
        if(upnpflag)CH563NET_UPNPReceiveDataHandle(sockeid);
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP�����ж� */
    {
#if CH563NET_DBG                                                                /* �������жϱ�ʾTCP�Ѿ����ӣ����Խ����շ����� */
        printf("TCP Connect Success\n");                           
#endif
        if(upnpflag)CH563NET_UPNPTCPConnectHandle(sockeid);
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP�Ͽ��ж� */
    {
#if CH563NET_DBG                                                                /* �������жϣ�CH563NET_���ڲ��Ὣ��socket�������Ϊ�ر�*/
        printf("TCP Disconnect\n");                                             /* Ӧ������������´������� */
#endif
        if(upnpflag)CH563NET_UPNPTCPDisConnectHandle(sockeid);
        
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP��ʱ�ж� */
    {
        if(upnpflag)CH563NET_UPNPTCPDisConnectHandle(sockeid);
#if CH563NET_DBG                                                                /* �������жϣ�CH563NET_���ڲ��Ὣ��socket�������Ϊ�ر�*/
        printf("TCP Timout\n");                                                 /* Ӧ������������´������� */
#endif
    }
}

/*******************************************************************************
* Function Name  : CH563NET_HandleGlobalInt
* Description    : ȫ���жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UpnpTest(u8 n);
u8 tmp = 2;
void CH563NET_HandleGlobalInt( void )
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                         /* ��ȫ���ж�״̬����� */
    if(initstat & GINT_STAT_UNREACH)                                            /* ���ɴ��ж� */
    {
#if CH563NET_DBG
        printf("UnreachCode ��%d\n",CH563Inf.UnreachCode);                      /* �鿴���ɴ���� */
        printf("UnreachProto ��%d\n",CH563Inf.UnreachProto);                    /* �鿴���ɴ�Э������ */
        printf("UnreachPort ��%d\n",CH563Inf.UnreachPort);                      /* ��ѯ���ɴ�˿� */
#endif       
    }
    if(initstat & GINT_STAT_IP_CONFLI)                                          /* IP��ͻ�ж� */
    {
#if CH563NET_DBG
       printf("IP interrupt\n");
#endif   
    }
    if(initstat & GINT_STAT_PHY_CHANGE)                                         /* PHY�ı��ж� */
    {
       i = CH563NET_GetPHYStatus();
       UpnpTest(tmp);                                                           /* ��ȡPHY״̬ */
       tmp++;
#if CH563NET_DBG
       printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif   
    }
    if(initstat & GINT_STAT_SOCKET)                                             /* Socket�ж� */
    {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* ��socket�жϲ����� */
           if(socketinit)CH563NET_HandleSokcetInt(i,socketinit);                /* ������ж������� */
       }    
    }
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

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        // default 9600
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        // ��������
    R8_UART0_LCR = 0x80;                                                        // DLABλ��1
    R8_UART0_DIV = 1;                                                           // Ԥ��Ƶ
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = 0x03;
    R8_UART0_FCR = 0xC7;
    R8_UART0_IER = RB_IER_TXD_EN;                                               // TXD enable

    R32_PB_SMT |= RXD0|TXD0;                                                    // RXD0 schmitt input, TXD0 slow rate
    R32_PB_PD &= ~ RXD0;                                                        // disable pulldown for RXD0, keep pullup
    R32_PB_DIR |= TXD0;                                                         // TXD0 output enable

//    R8_UART1_IER |= RB_IER_RECV_RDY;                                          /* ʹ��UART1�����ж� */
    R8_UART0_MCR |= RB_MCR_OUT2;                                                /* ���������ж�������� */
    
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_UART0;                                         /* ʹ��IRQ�е�UART1�ж� */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* ȫ���ж�ʹ�� */}

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
* Function Name  : UpnpTest
* Description    : 
* Input          : n 
* Output         : None
* Return         : None
*******************************************************************************/
u8 buf[2];
void UpnpTest(u8 n)
{
   UPNP_MAP upnpmap;

    upnpmap.InternelPort = 9999 + n;
    upnpmap.ExternalPort = 9999 + n;
    upnpmap.UpnpProType = MAP_TYPE_TCP;
    buf[0] = n + 0x30;
    buf[1] = 0;
    upnpmap.MapPortDes = buf;
    upnpmap.IPAddr[0] = 192;
    upnpmap.IPAddr[1] = 168;
    upnpmap.IPAddr[2] = 1;
    upnpmap.IPAddr[3] = 122;
    CH563NET_UPNPStart(&upnpmap);
    if(n > 10)while(1);
}

void UpnpCallBack(u8 errcode)
{
   UpnpTest(tmp);
   tmp++;
   printf("UpnpCallBack = %02x\n",errcode);
   printf("UpnpCallBack = %02x\n",errcode);

}
/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define CH563MAC_BASE_ADDR        0x00406000
#define R32_ETHE_MACCR           (*((PUINT32)(0x88 + CH563MAC_BASE_ADDR)))
#define RB_RCV_ALL                 (1 << 12)
int main( void )
{
    UINT8 i;
    UPNP_CFG upnpsfg;

    mInitSTDIO( );                                                              /* Ϊ���ü����ͨ�����ڼ����ʾ���� */
    i = CH56X_GetMac(MACAddr);    
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* ���ʼ�� */
    mStopIfError( i );                                                          /* ������ */
#if CH563NET_DBG
    printf("CH563NET_LibInit Success\n");
#endif    
    R32_ETHE_MACCR |= RB_RCV_ALL;
    SysTimeInit( );                                                             /* ϵͳ��ʱ����ʼ�� */
    InitSysHal( );
    upnpsfg.TcpMss = CH563NET_TCP_MSS;
    upnpsfg.MemoryLen = sizeof(SocketRecvBuf);
    upnpsfg.UpnpMemory = SocketRecvBuf;
    upnpsfg.GateIPAddr[0] = GWIPAddr[0];
    upnpsfg.GateIPAddr[1] = GWIPAddr[1];
    upnpsfg.GateIPAddr[2] = GWIPAddr[2];
    upnpsfg.GateIPAddr[3] = GWIPAddr[3];
    upnpsfg.AppUpnpCallBack = UpnpCallBack;
    CH563NET_UPNPInit(&upnpsfg);
    while(1)
    {
        CH563NET_MainTask( );                                                   /* CH563IP��������������Ҫ����ѭ���в��ϵ��� */
        if(CH563NET_QueryGlobalInt( ))CH563NET_HandleGlobalInt();               /* ��ѯ�жϣ�������жϣ������ȫ���жϴ������� */
        CH563NET_UPNPMainTask();
    }
}

/*********************************** endfile **********************************/