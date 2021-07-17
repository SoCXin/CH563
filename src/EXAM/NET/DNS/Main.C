/********************************** (C) COPYRIGHT ******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2015/02/27
* Description        : CH563NET����ʾ
*                    : MDK3.36@ARM966E-S,Thumb
*******************************************************************************/

/*******************************************************************************
CH563 TCP/IP Э����ӿ�
CH563NET����ʾ�ļ���������������ʾUDPͨѶ����Ƭ���յ����ݺ󣬻ش���Զ�ˡ�
ARM966,Thumb,С��
*******************************************************************************/

/* ͷ�ļ�����*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"


void ResetSys(void);
/****************************************************************************/

/* ������ */
/* MAC �շ����������� */
/* CH563NET.LIB �ڲ������ (RX_QUEUE_ENTRIES + TX_QUEUE_ENTRIES)*MAX_PKT_SIZE ���ֽڵĻ�����������̫���������շ� */
/* �û������޸�RX_QUEUE_ENTRIES��TX_QUEUE_ENTRIES�����ٻ��������ڲ�������, TX_QUEUE_ENTRIES����Ϊ2���ɣ�����RX_QUEUE_ENTRIES*/
/* ��Ҫ����ʵ��������е����������Ҫ�����������ݽ����շ���RX_QUEUE_ENTRIES������󣬷�����ܻᶪʧ���ݡ�*/
#define CH563NET_PING_ENABLE                 TRUE                    /* PING������FALSEΪ�رգ����������� */
/* �������ͷ�ļ����������������ú�����ļ�������������Ч */


#define CH563NET_MAX_SOCKET_NUM               4                                 /* Socket�ĸ������û��������ã�Ĭ��Ϊ4��Socket,���Ϊ32 */
#define RECE_BUF_LEN                          (CH563NET_TCP_MSS * 2  )                               /* ???????? */
#define CH563NET_TCP_MSS                      536                               /* tcp MSS�Ĵ�С*/
#define CH563NET_DBG                          1

#include "CH563NET.H"

/* ����Ļ�������ȫ�ֱ�������Ҫ���壬���е��� */
__align(16)UINT8  CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];                     /* MAC������������������16�ֽڶ��� */
__align(4)UINT8   CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];                  /* MAC���ջ�������4�ֽڶ��� */
__align(4)SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];                          /* Socket��Ϣ����4�ֽڶ��� */
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

/*=============================================================================*/
/* CH563��ض��� */
UINT8 MACAddr[6] = {0x02,0x03,0x04,0x05,0x06,0x07};                             /* CH563MAC��ַ */

UINT8 IPAddr[4] = {192,168,1,11};                                               /* CH563IP��ַ */
UINT8 GWIPAddr[4] = {192,168,1,1};                                              /* CH563���� */
UINT8 IPMask[4] = {255,255,255,0};                                              /* CH563�������� */
UINT8 RemoIP[4] = {192,168,1,3};                                                /* CH563�������� */
UINT8 SocketId;                                                                 /* ����socket���������Բ��ö��� */

UINT8 SocketRecvBuf[CH563NET_MAX_SOCKET_NUM][RECE_BUF_LEN];                     /* socket���ջ����� */
UINT8 MyBuf[RECE_BUF_LEN];                                                      /* ����һ����ʱ������ */
#define SEND_BUF_COUNT                               5
UINT8 UDPServerFlag = 0 ;

UINT8 DNSHostIP[4] = {218,2,135,1};

UINT16 DNSPort = 53;
UINT8 RemoteIp[4];

UINT8 DNSGetFlag = 0;
/******************************************************************************/
/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                                   /* ��̫���ж� */
    {                                                                           /* ��̫���ж��жϷ����� */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                               /* ��ʱ���ж� */
    {
         CH563NET_TimeIsr(CH563NETTIMEPERIOD);                                  /* ��ʱ���жϷ����� */
         R8_TMR0_INT_FLAG |= 0xff;                                              /* �����ʱ���жϱ�־ */
    }
}


__irq void FIQ_Handler( void )
{

}

/*******************************************************************************
* Function Name  : SysTimeInit
* Description    : ϵͳ��ʱ����ʼ����CH563@100MHZ TIME0 10ms������CH563NETTIMEPERIOD
*                ������ʼ����ʱ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTimeInit(void)
{
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R32_TMR0_COUNT = 0x00000000; 
    R32_TMR0_CNT_END = 0x186a0 * CH563NETTIMEPERIOD;                            /* ����Ϊ10MS��ʱ */
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
    printf("Error: %02X\n", (UINT16)iError);                                    /* ��ʾ���� */
}

/*******************************************************************************
* Function Name  : CH563NET_LibInit
* Description    : ���ʼ������
* Input          : ip      ip��ַָ��
*                ��gwip    ����ip��ַָ��
*                 : mask    ����ָ��
*                 : macaddr MAC��ַָ�� 
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
    cfg.TCPMss = CH563NET_TCP_MSS;
    cfg.HeapSize = CH563_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
    CH563NET_ConfigLIB(&cfg);
    i = CH563NET_Init(ip,gwip,mask,macaddr);
    return (i);                      /* ���ʼ�� */
}

/*******************************************************************************
* Function Name  : CH563NET_HandleSockInt
* Description    : Socket�жϴ�������
* Input          : sockeid  socket����
*                ��initstat �ж�״̬
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleSockInt(UINT8 sockeid,UINT8 initstat)
{
    UINT32 len;
    UINT32 totallen;
    UINT8 i,*p = MyBuf;

    if(initstat & SINT_STAT_RECV)                                               /* �����ж� */
    {
        len = CH563NET_SocketRecvLen(sockeid,NULL);                             /* �Ὣ��ǰ����ָ�봫�ݸ�precv*/
        CH563NET_SocketRecv(sockeid,MyBuf,&len);                                /* �����ջ����������ݶ���MyBuf��*/
        totallen = len;
        while(1)
        {
           len = totallen;
           i = CH563NET_SocketSend(sockeid,p,&len);                             /* ��MyBuf�е����ݷ��� */
           totallen -= len;                                                     /* ���ܳ��ȼ�ȥ�Լ�������ϵĳ��� */
           p += len;                                                            /* ��������ָ��ƫ��*/
           if(totallen)continue;                                                /* �������δ������ϣ����������*/
           break;                                                               /* ������ϣ��˳� */
        }
    
    }
    if(initstat & SINT_STAT_CONNECT)                                            /* TCP�����ж� */
    {
#if CH563NET_DBG
        printf("TCP CONNECT :SockID:%d \n",(UINT16)sockeid);
#endif
        CH563NET_ModifyRecvBuf(sockeid,(UINT32)SocketRecvBuf[sockeid],RECE_BUF_LEN);  

    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP�Ͽ��ж� */
    {
#if CH563NET_DBG
        printf("SINT_STAT_DISCONNECT\n");                           
#endif
        CH563NET_SocketClose(sockeid,0);
        
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP��ʱ�ж� */
    {
#if CH563NET_DBG
        printf("SINT_STAT_TIM_OUT\n");                           
#endif
    }
}

/*******************************************************************************
* Function Name  : CH563NET_HandleGloableInt
* Description    : ȫ���жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleGloableInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                         /* ��ȫ���ж�״̬����� */
    if(initstat & GINT_STAT_UNREACH)                                            /* ���ɴ��ж� */
    {
      
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP��ͻ�ж� */
   {
   
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY�ı��ж� */
   {
        i = CH563NET_GetPHYStatus();                                             /* ��ȡPHY״̬ */
#if CH563NET_DBG
        printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif  
        if(i&PHY_DISCONN)
        {
          
        }
        else
        {
             DNSGetFlag = 1;
        }
        
   }
   if(initstat & GINT_STAT_SOCKET)                                              /* Socket�ж� */
   {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* ��socket�жϲ����� */
           if(socketinit)CH563NET_HandleSockInt(i,socketinit);                  /* ������ж������� */
       }    
   }
}



/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : ���ڳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mInitSTDIO(void)
{
    UINT32    x, x2;
    x = 10 * FREQ_SYS * 2 / 16 / 115200;  // 115200bps
//    x = 10 * 62500000 * 2 / 16 / 115200;  // Ĭ����62.5MHz
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;  // ��������
    R8_UART0_LCR = 0x80;  // DLABλ��1
    R8_UART0_DIV = 1;  // Ԥ��Ƶ
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;
    R8_UART0_LCR = 0x03;
    R8_UART0_FCR = 0xC7;
    R8_UART0_IER = RB_IER_TXD_EN;  // TXD enable
    R32_PB_SMT |= RXD0|TXD0;  // RXD0 schmitt input, TXD0 slow rate
    R32_PB_PD &= ~ RXD0;  // disable pulldown for RXD0, keep pullup
    R32_PB_DIR |= TXD0;  // TXD0 output enable
}

/* ͨ��������������Ϣ */
int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                            // ��������
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );       // �ȴ����ݷ���
    return( c );
}

/*******************************************************************************
* Function Name  : CH563NET_DNSCallBack
* Description    : DNS�ص�����
* Input          : None
* Output         : None
* Return         : ִ��״̬
*******************************************************************************/
void  CH563NET_DNSCallBack(const  char  *name,  UINT8  *ipaddr,  void  *callback_arg) 
{ 
    if(ipaddr == NULL) 
    { 
        printf("DNS Fail\n"); 
        return; 
    } 
#if CH563NET_DBG
    printf("Host Name = %s\n",name); 
    printf("IP= %d.%d.%d.%d\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]); 
    if(callback_arg != NULL) 
    { 
        printf("callback_arg = %02x\n",(*(UINT8 *)callback_arg)); 
    } 
#endif
} 

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void) 
{
    UINT8 i = 0;

    mInitSTDIO();
    CH563NET_InitDNS(DNSHostIP,DNSPort); 
    i = CH56X_GetMac(MACAddr);
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* ���ʼ�� */
    mStopIfError(i);                                                            /* ������ */
    SysTimeInit();                                                              /* ϵͳ��ʱ����ʼ�� */
    InitSysHal();                                                               /* ��ʼ���ж� */
    while(1)
    {
        CH563NET_MainTask();                                                    /* CH563NET��������������Ҫ����ѭ���в��ϵ��� */
        if(CH563NET_QueryGlobalInt())CH563NET_HandleGloableInt();               /* ��ѯ�жϣ�������жϣ������ȫ���жϴ������� */
        if(DNSGetFlag)
        {
            DNSGetFlag = 0;
            i= CH563NET_GetHostName("www.wch.cn",RemoteIp,CH563NET_DNSCallBack,NULL);
            if(i == 0) 
            { 
    #if CH563NET_DBG
                printf("563NET_GetHostName Success\n"); 
                printf("IP= %d.%d.%d.%d\n",RemoteIp[0],RemoteIp[1],RemoteIp[2],RemoteIp[3]); 
    #endif
                while(1);
            } 
        }
    }
}


/****************************endfile@tech9*************************************/