/********************************** (C) COPYRIGHT ******************************
* File Name          : TCP_CLIENT_X.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563NET����ʾ�ļ�
*                      (1)��CH563 Examples by KEIL;
*                      (2)������0��������Ϣ,115200bps;
*                      (3)���������ñ�����������ʾtcp��client�ڷ��ͷǸ��Ʒ�ʽ��ͨѶ����Ƭ���յ����ݺ󣬻ش���Զ�ˣ�ARM966,Thumb,С��
*******************************************************************************/


/* ͷ�ļ�����*/
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"

#define CH563NET_MAX_SOCKET_NUM              9                                  /* Socket�ĸ������û��������ã�Ĭ��Ϊ4��Socket */
#define CH563NET_NUM_TCP                     8                                  /* TCP���ӵĸ��� */ 
#define MISCE_CFG0_TCP_SEND_COPY             0                                  /* TCP���ͻ��������� */
#define SEND_BUF_COUNT                       16
#define CH563NET_NUM_PBUF                    12                                 /* PBUF�ṹ�ĸ��� */

#include "CH563NET.H"
#define CH563NET_DBG                          1

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

/******************************************************************************/
/* CH563��ض��� */
UINT8 MACAddr[6] =  {0x02,0x03,0x04,0x05,0x06,0x17};                            /* CH563MAC��ַ */
const UINT8 IPAddr[4] =   {192,168,1,2};                                        /* CH563IP��ַ */
const UINT8 GWIPAddr[4] = {192,168,1,1};                                        /* CH563���� */
const UINT8 IPMask[4] =   {255,255,255,0};                                      /* CH563�������� */
const UINT8 DESIP[4] =    {192,168,1,100};   

#define RECE_BUF_LEN                  (CH563NET_TCP_MSS)             

UINT8 SocketId;                                                                 /* ����socket���������Բ��ö��� */
__align(4) UINT8 SocketRecvBuf[CH563NET_NUM_TCP+1][RECE_BUF_LEN];               /* socket���ջ����� */
UINT8 MyBuf[RECE_BUF_LEN];                                                      /* ����һ����ʱ������ */
UINT8 SendBuf[SEND_BUF_COUNT][RECE_BUF_LEN];
UINT8 connect = 0;
UINT8 socketnum =0;
/******************************************************************************/
/******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    if(R32_INT_FLAG & 0x8000)                                        /* ��̫���ж� */
    {                                                                 /* ��̫���ж��жϷ����� */
        CH563NET_ETHIsr();
    }
    if(R32_INT_FLAG & RB_IF_TMR0)                                     /* ��ʱ���ж� */
    {
         CH563NET_TimeIsr(CH563NETTIMEPERIOD);                       /* ��ʱ���жϷ����� */
         R8_TMR0_INT_FLAG |= 0xff;                                   /* �����ʱ���жϱ�־ */
    }
}

/******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{

}


__packed struct _SEND_CTRL
{
   UINT32 BufAddr;
   UINT8  Status;                                                               /*0Ϊ���У�1Ϊæ*/
   UINT8  SocketID;                                                             
};
__packed struct _SEND_CTRL  SendCtrl[SEND_BUF_COUNT];


UINT32 AddrList[32];
void TCPSendOKCallBack(struct _SCOK_INF *socinf,UINT32 recv1,UINT16 recv2,UINT8 *buf,UINT32 len)
{
    UINT8  socketid=0xff;
    UINT8  i,count;
    UINT8  liscount;

    for(i=0;i<CH563NET_MAX_SOCKET_NUM;i++){
        if(socinf == &SocketInf[i]){
            socketid = i;
            break;
        }
    }
    liscount = CH563NET_QueryUnack(socinf,AddrList,sizeof(AddrList));
    for(count = 0; count < SEND_BUF_COUNT; count++){                            /* AddrList�����˵�ǰ��û���ͳɹ��Ļ������������Ƚ�������������״̬��Ϊ�ɹ� */
        if( SendCtrl[count].SocketID == socketid ){
            SendCtrl[count].Status = 0;                                         /* ������״̬Ϊ0����ʾ���������У����Լ���ʹ�� */
            if( len&&liscount ){
                for(i = 0; i < liscount; i++){                                  /* ѭ������δ���ͳɹ��Ļ�������ַ */
                    if(SendCtrl[count].BufAddr == AddrList[i]){                 /* �鿴�˵�ַ�Ƿ���socket���ͻ�������ַ */
                        SendCtrl[count].Status = 1;                             /* �˻������������ã���û������� */
                    }
                }
            }
        }
    }    
}

/******************************************************************************
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
    R32_TMR0_COUNT   = 0x00000000; 
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
    R8_INT_EN_FIQ_GLOB |= RB_IE_FIQ_GLOB;
}

/******************************************************************************
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
    cfg.TCPMss   = CH563NET_TCP_MSS;
    cfg.HeapSize = CH563_MEM_HEAP_SIZE;
    cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
    CH563NET_ConfigLIB(&cfg);
    i = CH563NET_Init(ip,gwip,mask,macaddr);
    return (i);                      
}


/*******************************************************************************
* Function Name  : CH563NET_CreatTcpSocket
* Description    : ����TCP Client socket
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_CreatTcpClient( UINT16 dport,UINT16 sport,UINT8 count)
{
   UINT8 i;                                                             
   SOCK_INF TmpSocketInf;                                                       /* ������ʱsocket���� */

   printf("Creat Socekt Tcp Client%-2d\n",count);
   memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));                            /* ���ڲ��Ὣ�˱������ƣ�������ý���ʱ������ȫ������ */
   memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);                                 /* ����Ŀ��IP��ַ */
   TmpSocketInf.DesPort = dport;                                                /* ����Ŀ�Ķ˿� */
   TmpSocketInf.SourPort = sport;                                               /* ����Դ�˿� */
   TmpSocketInf.ProtoType = PROTO_TYPE_TCP;                                     /* ����socekt���� */
   TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf[count];                  /* ���ý��ջ������Ľ��ջ����� */
   TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;                                     /* ���ý��ջ������Ľ��ճ��� */
   TmpSocketInf.AppCallBack = TCPSendOKCallBack;                                /* ���ͻص����� */
   i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);                           /* ����socket�������ص�socket����������SocketId�� */
   mStopIfError(i);                                                             /* ������ */
   i = CH563NET_SocketConnect(SocketId);                                        /* TCP���� */
   mStopIfError(i);                                                             /* ������ */

}

/******************************************************************************
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
    UINT8  i=0;
    UINT32 totallen=0;
    UINT8 *p;

    if(initstat & SINT_STAT_RECV)                                               /* �����ж� */
    {
//        printf("SINT_STAT_RECV \n");
        len = CH563NET_SocketRecvLen(sockeid,NULL);                    
        for(i = 0; i < SEND_BUF_COUNT; i++){
            if(SendCtrl[i].Status == 0){
                if( len>RECE_BUF_LEN ) len = RECE_BUF_LEN;
                CH563NET_SocketRecv(sockeid,(UINT8 *)SendCtrl[i].BufAddr,&len);    
                p = (UINT8 *)SendCtrl[i].BufAddr;
                SendCtrl[i].Status = 1;
                SendCtrl[i].SocketID = sockeid;
                totallen = len;
                while(totallen){
                   len = totallen;
                   i = CH563NET_SocketSend(sockeid,p,&len);                     /* ��MyBuf�е����ݷ��� */
                   totallen -= len;                                             /* ���ܳ��ȼ�ȥ�Ѿ�������ϵĳ��� */
                   p += len;                                                    /* ��������ָ��ƫ��*/
                }
                break;
            }
        }

    }
    if( initstat & SINT_STAT_CONNECT )                                          /* TCP�����ж� */
    {
        printf("SINT_STAT_CONNECT \n");
    }
    if(initstat & SINT_STAT_DISCONNECT)                                         /* TCP�Ͽ��ж� */
    {
        printf("SINT_STAT_DISCONNECT\n");                                   
    }
    if(initstat & SINT_STAT_TIM_OUT)                                            /* TCP��ʱ�ж� */
    {
        printf("SINT_STAT_TIM_OUT\n");                           
    }
}

/*******************************************************************************
* Function Name  : CH563NET_HandleGlobalInt
* Description    : ȫ���жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_HandleGlobalInt(void)
{
    UINT8 initstat;
    UINT8 i;
    UINT8 socketinit;
    
    initstat = CH563NET_GetGlobalInt();                                         /* ��ȫ���ж�״̬����� */
    if(initstat & GINT_STAT_UNREACH)                                            /* ���ɴ��ж� */
    {
        printf("UnreachCode ��%d\n",CH563Inf.UnreachCode);                      /* �鿴���ɴ���� */
        printf("UnreachProto ��%d\n",CH563Inf.UnreachProto);                    /* �鿴���ɴ�Э������ */
        printf("UnreachPort ��%d\n",CH563Inf.UnreachPort);                      /* ��ѯ���ɴ�˿� */
    }
   if(initstat & GINT_STAT_IP_CONFLI)                                           /* IP��ͻ�ж� */
   {
   
   }
   if(initstat & GINT_STAT_PHY_CHANGE)                                          /* PHY�ı��ж� */
   {
       i = CH563NET_GetPHYStatus();                                             /* ��ȡPHY״̬ */
       printf("GINT_STAT_PHY_CHANGE %02x\n",i);

   }
   if(initstat & GINT_STAT_SOCKET)                                              /* Socket�ж� */
   {
       for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)                     
       {
           socketinit = CH563NET_GetSocketInt(i);                               /* ��socket�жϲ����� */
           if(socketinit)CH563NET_HandleSockInt(i,socketinit);                  /* ������ж������� */
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
    R32_PB_PD  &= ~ RXD0;                                                       /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : ͨ��������������Ϣ
* Input          : c  -writes the character specified by c 
*                  *f -the output stream pointed to by *f
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
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int    main(void) 
{

    UINT8 i = 0;
    Delay_ms(100);
    mInitSTDIO( );
    i = CH56X_GetMac(MACAddr);
    i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                       /* ���ʼ�� */
    mStopIfError(i);                                                            /* ������ */
    printf("CH563IPLibInit Success %8lx\n",(UINT32)CH563MACRxBuf);

    for(i = 0; i < SEND_BUF_COUNT;i++)
    {
        SendCtrl[i].BufAddr = (UINT32)&SendBuf[i][0];
         SendCtrl[i].Status = 0;
    }
    SysTimeInit( );                                                             /* ϵͳ��ʱ����ʼ�� */
    InitSysHal( );                                                              /* ��ʼ���ж� */

    for(socketnum=0;socketnum<CH563NET_NUM_TCP;socketnum++){
        CH563NET_CreatTcpClient(2000,6000+socketnum,socketnum);
    }
      printf("mian task\n"); 
    while(1)
    {
        CH563NET_MainTask();                                                    /* CH563NET��������������Ҫ����ѭ���в��ϵ��� */
        if(CH563NET_QueryGlobalInt())CH563NET_HandleGlobalInt();                /* ��ѯ�жϣ�������жϣ������ȫ���жϴ������� */
    }
}


/****************************endfile@tech9*************************************/