/********************************** (C) COPYRIGHT ******************************
* File Name		: MQTT.C
* Author		: WCH
* Version		: V1.0
* Date			: 2018/4/15
* Description	:����TCP/IP��MQTTЭ��ͨѶ���̣�ʵ����MQTT��ʽ��ͨ���ٶȡ�������������������ʵ���豸��ͨ��ʹ��ǰ�������ʺź�����
*					(1)��CH563 Examples by KEIL;
*					(2)������0��������Ϣ,115200bps;
*					(3)��������������ʾ����TCP/IP��MQTTЭ��ͨѶ��
					     ��Ƭ��������̫����MQTT�������󽫻ᷢ��һ�����⣬
					     �ٶ���������⣬����������ⷢ����Ϣ��
						 �����յ��Լ����͵���Ϣ��
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ�����*/
#include <stdio.h>							 
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

#include "CH563NET.H"
#include "ISPXT56X.H"
#include "MQTTPacket.H"

#define CH563NET_DBG		1			 
#define KEEPLIVE_ENABLE		1														/* ����KEEPLIVE���� */


/* ����Ļ�������ȫ�ֱ�������Ҫ���壬���е��� */
__align(16)UINT8		CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];					/* MAC������������������16�ֽڶ��� */
__align(4) UINT8		CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];				/* MAC���ջ�������4�ֽڶ��� */
__align(4) SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];								/* Socket��Ϣ����4�ֽڶ��� */
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
/* ����ʾ�������غ� */
#define RECE_BUF_LEN	536					/* ���ջ������Ĵ�С */
/* CH563NET��TCP��MSS����Ϊ536�ֽڣ���һ��TCP��������ݲ����Ϊ536�ֽ� */
/* TCPЭ��ջ���û������ڽ������أ��������ֵΪsocket�Ľ��ջ��������ȡ����趨 */
/* RX_QUEUE_ENTRIESʱҪ����MSS�ʹ���֮��Ĺ�ϵ�����細��ֵΪ4*MSS����Զ��һ�λᷢ�� */
/* 4��TCP�������RX_QUEUE_ENTRIESС��4�����Ȼ�ᵼ�����ݰ���ʧ���Ӷ�����ͨѶЧ�ʽ��� */
/* ����RX_QUEUE_ENTRIESҪ����( ����/MSS ),������socketͬʱ���д������������ݣ��� */
/* ����RX_QUEUE_ENTRIESҪ����(( ����/MSS )*socket����) �ڶ��socketͬʱ���д��������շ�ʱ */
/* Ϊ�˽�ԼRAM���뽫���ջ������ĳ�������ΪMSS */




/*ʹ��ǰ��Ҫ���ö�Ӧ������ƽ̨���ʺź�����*/
UINT8 MACAddr[6] = {0x84,0xc2,0xe4,0x05,0x06,0x17};			/* CH563MAC��ַ */
UINT8 IPAddr[4] = {192,168,10,75};							/* CH563IP��ַ */
UINT8 GWIPAddr[4] = {192,168,10,1};							/* CH563���� */
UINT8 IPMask[4] = {255,255,255,0};							/* CH563�������� */
UINT16 aport=1000;											/* CH563Դ�˿� */

UINT16 DESPORT= 1883;										/* MQTT�������˿� */
UINT8 DESIP[4] = {163,177,150,12};							/* MQTT������IP��ַ */
char *username = "test1/device1";							/* �豸����ÿ���豸Ψһ�����á�/�����ּ� */
char *password = "password";								/* ��������½���� */
char *sub_topic = "topic1";									/* ���ĵĻỰ����Ϊ���Է����գ�Ӧ�뷢���ĻỰ����ͬ */
char *pub_topic = "topic1";									/* �����ĻỰ�� */


/* CH563��ض��� */
UINT8 SocketId;												/* ����socket���������Բ��ö��� */
UINT8 SocketRecvBuf[RECE_BUF_LEN];							/* socket���ջ����� */
UINT8 MyBuf[RECE_BUF_LEN];									/* ����һ����ʱ������ */
UINT32 a=0;

UINT8 con_flag=0;											/* ������MQTT��������־λ */
UINT8 pub_flag=1;											/* �ѷ����Ự��Ϣ��־λ */
UINT8 sub_flag=0;											/* �Ѷ��ĻỰ��־λ */
UINT8 tout_flag=0;											/* ��ʱ��־λ */
UINT16 packetid=0;											/* ��ID */


/*******************************************************************************
* Function Name	: SysTimeInit
* Description	: ϵͳ��ʱ����ʼ����CH563@100MHZ TIME0 10ms������CH563NETTIMEPERIOD
*				  ����ʼ����ʱ����
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void SysTimeInit(void)
{
		R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
		R32_TMR0_COUNT = 0x00000000; 
		R32_TMR0_CNT_END = 0x186a0 * CH563NETTIMEPERIOD;		/* ����Ϊ10MS��ʱ */
		R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;
		R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
}


/*******************************************************************************
* Function Name	: InitSysHal
* Description	: Ӳ����ʼ������������TIM0��ETH�ж�
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void InitSysHal(void)
{
		R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;			/* ����TIM0�ж� */
		R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ETH;			/* ����ETH�ж� */
		R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;		/* ����IRQȫ���ж� */
}


/*******************************************************************************
* Function Name	: mStopIfError
* Description	: ����ʹ�ã���ʾ�������
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
		if (iError == CH563NET_ERR_SUCCESS) return;			/* �����ɹ� */
#if CH563NET_DBG
		printf("Error: %02X\n", (UINT16)iError);			/* ��ʾ���� */
#endif		
}


/*******************************************************************************
* Function Name	: Transport_Open
* Description	: ����TCP����
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
UINT8 Transport_Open()
{
	UINT8 i;																														 
	SOCK_INF TmpSocketInf;									/* ������ʱsocket���� */

	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));		/* ���ڲ��Ὣ�˱������ƣ�������ý���ʱ������ȫ������ */
	memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);			/* ����Ŀ��IP��ַ */
	TmpSocketInf.DesPort = 1883;							/* ����Ŀ�Ķ˿� */
	TmpSocketInf.SourPort = aport++;						/* ����Դ�˿� */
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;				/* ����socekt���� */
	TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf;	/* ���ý��ջ������Ľ��ջ����� */
	TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;				/* ���ý��ջ������Ľ��ճ��� */
	i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);		/* ����socket�������ص�socket����������SocketId�� */
	mStopIfError(i);										/* ������ */
	
	i = CH563NET_SocketConnect(SocketId);					/* TCP���� */
	mStopIfError(i);										/* ������ */
	return SocketId;
}


/*******************************************************************************
* Function Name	: Transport_Close
* Description	: �ر�TCP����
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
UINT8 Transport_Close()
{
	UINT8 i;
	i=CH563NET_SocketClose(SocketId,TCP_CLOSE_NORMAL);
	mStopIfError(i);
	return i;
}


/*******************************************************************************
* Function Name	: Transport_SendPacket
* Description	: ��̫����������
* Input			: UINT8 *buf �������ݵ����ֽڵ�ַ
				  UINT32 len �������ݵĳ���
* Output		: None
* Return		: None
*******************************************************************************/
void Transport_SendPacket(UINT8 *buf,UINT32 len)
{
	UINT32 totallen;
	UINT8 *p=buf;
	
	totallen=len;
	while(1)
	{
		len = totallen;
		CH563NET_SocketSend(SocketId,p,&len);					/* ��MyBuf�е����ݷ��� */
		totallen -= len;					 					/* ���ܳ��ȼ�ȥ�Լ�������ϵĳ��� */
		p += len;												/* ��������ָ��ƫ��*/
		if(totallen)continue;									/* �������δ������ϣ����������*/
		break;													/* ������ϣ��˳� */
	}
}


/*******************************************************************************
* Function Name	: MQTT_Connect
* Description	: ����MQTT����
* Input			: char *username �豸��
				  char *password ��������������
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Connect(char *username,char *password)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	UINT32 len;
	UINT8 buf[200];

	data.clientID.cstring = "11";
	data.keepAliveInterval = 20;
	data.cleansession = 1;
	data.username.cstring = username;																		
	data.password.cstring = password;
	 
	len=MQTTSerialize_connect(buf,sizeof(buf),&data);											
	Transport_SendPacket(buf,len);						/*����MQTT����*/
}


/*******************************************************************************
* Function Name	: MQTT_Subscribe
* Description	: MQTT����һ������
* Input			: char *topic ���ĵ�������
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Subscribe( char *topic)
{
	MQTTString topicString = MQTTString_initializer;
	int req_qos=0;
	UINT32 len;
	UINT32 msgid=1;
	UINT8 buf[200];
	
	topicString.cstring=topic;
	len=MQTTSerialize_subscribe(buf,sizeof(buf),0,msgid,1,&topicString,&req_qos);
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Unsubscribe
* Description	: MQTTȡ������һ������
* Input			: char *topic ȡ�����ĵ�������
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Unsubscribe(char *topic)
{
	MQTTString topicString = MQTTString_initializer;
	UINT32 len;
	UINT32 msgid=1;
	UINT8 buf[200];
	
	topicString.cstring=topic;
	len=MQTTSerialize_unsubscribe(buf,sizeof(buf),0,msgid,1,&topicString);
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Subscribe
* Description	: MQTT����һ������
* Input			: char *topic ���ĵ�������
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Publish(char *topic,char *payload)
{
	MQTTString topicString = MQTTString_initializer;
	UINT32 payloadlen;
	UINT32 len;
	UINT8 buf[1024];
	
	topicString.cstring=topic; 
	payloadlen=strlen(payload);
	len= MQTTSerialize_publish(buf,sizeof(buf),0,0,0,packetid++,topicString,payload,payloadlen);	
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Pingreq
* Description	: MQTT����������
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Pingreq()
{
	UINT32 len;
	UINT8 buf[200];
	
	len=MQTTSerialize_pingreq(buf,sizeof(buf));
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Disconnect
* Description	: �Ͽ�MQTT����
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Disconnect()
{
	UINT32 len;
	UINT8 buf[50];
	len=MQTTSerialize_disconnect(buf,sizeof(buf));
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: net_initkeeplive
* Description	: keeplive��ʼ��
* Input			: None			
* Output		: None
* Return		: None
*******************************************************************************/
#ifdef	KEEPLIVE_ENABLE
void net_initkeeplive(void)
{
		struct _KEEP_CFG	klcfg;

		klcfg.KLIdle = 20000;				/* ���� */
		klcfg.KLIntvl = 10000;				/* ��� */
		klcfg.KLCount = 5;					/* ���� */
		CH563NET_ConfigKeepLive(&klcfg);
}
#endif

/*******************************************************************************
* Function Name	: CH563NET_LibInit
* Description	: ���ʼ������
* Input			: ip			ip��ַָ��
*				: gwip		����ip��ַָ��
*				: mask		����ָ��
*				: macaddr MAC��ַָ�� 
* Output		: None
* Return		: ִ��״̬
*******************************************************************************/
UINT8 CH563NET_LibInit(const UINT8 *ip,const UINT8 *gwip,const UINT8 *mask,const UINT8 *macaddr)
{
		UINT8 i;
		struct _CH563_CFG cfg;

		if(CH563NET_GetVer() != CH563NET_LIB_VER)return 0xfc;			/* ��ȡ��İ汾�ţ�����Ƿ��ͷ�ļ�һ�� */
		CH563NETConfig = LIB_CFG_VALUE;									/* ��������Ϣ���ݸ�������ñ��� */
		cfg.RxBufSize = RX_BUF_SIZE; 
		cfg.TCPMss	 = CH563NET_TCP_MSS;
		cfg.HeapSize = CH563_MEM_HEAP_SIZE;
		cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
		cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
		CH563NET_ConfigLIB(&cfg);
		i = CH563NET_Init(ip,gwip,mask,macaddr);
#ifdef	KEEPLIVE_ENABLE
		net_initkeeplive( );
#endif
		return (i);											/* ���ʼ�� */
}

/*******************************************************************************
* Function Name	: CH563NET_CreatTcpSocket
* Description	: ����TCP Client socket
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void CH563NET_CreatTcpSocket(void)
{
	 UINT8 i;																														 
	 SOCK_INF TmpSocketInf;											/* ������ʱsocket���� */

	 memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));				/* ���ڲ��Ὣ�˱������ƣ�������ý���ʱ������ȫ������ */
	 memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);					/* ����Ŀ��IP��ַ */
	 TmpSocketInf.DesPort = 1000;									/* ����Ŀ�Ķ˿� */
	 TmpSocketInf.SourPort = 2000;									/* ����Դ�˿� */
	 TmpSocketInf.ProtoType = PROTO_TYPE_TCP;						/* ����socekt���� */
	 TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf;			/* ���ý��ջ������Ľ��ջ����� */
	 TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;						/* ���ý��ջ������Ľ��ճ��� */
	 i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);				/* ����socket�������ص�socket����������SocketId�� */
	 mStopIfError(i);												/* ������ */

#ifdef	KEEPLIVE_ENABLE
	 CH563NET_SocketSetKeepLive( SocketId, 1 );						/* ����socket��KEEPLIVE���ܣ�V06�汾֧�֣� */
#endif

	 i = CH563NET_SocketConnect(SocketId);							/* TCP���� */
	 mStopIfError(i);												/* ������ */
}

/*******************************************************************************
* Function Name	: CH563NET_HandleSockInt
* Description	: Socket�жϴ�������
* Input			: sockeid	socket����
*				: initstat �ж�״̬
* Output		: None
* Return		: None
*******************************************************************************/
void CH563NET_HandleSockInt(UINT8 sockeid,UINT8 initstat)
{
	UINT32 len;
	UINT8 i;

	unsigned char* dup;
	unsigned short* packetid;

	int* qos;
	unsigned char* retained;
	MQTTString* topicName;
	unsigned char* payload;
	int payloadlen;
	unsigned char *p=payload;

	if(initstat & SINT_STAT_RECV)							/* �����ж� */
	{
		len = CH563NET_SocketRecvLen(sockeid,NULL);			/* ��ѯ���� */
		CH563NET_SocketRecv(sockeid,MyBuf,&len);			/* �����ջ����������ݶ���MyBuf��*/
		switch(MyBuf[0]>>4)
		{
			case FLAG_CONNACK:
				printf("connack\r\n");
				con_flag=1;
				break;

			case FLAG_PUBLISH:
				MQTTDeserialize_publish(dup,qos,retained,packetid,topicName,&payload,&payloadlen,MyBuf,len);
				printf("payloadlen=%d\r\n",(UINT16)payloadlen);
				p=payload;
				for(i=0;i<payloadlen;i++)
				{
					printf("%c ",(UINT16)*p);
					p++;
				}
				printf("\r\n");
				break;
				
			case FLAG_SUBACK:
				sub_flag=1;
				printf("suback\r\n");
				break;

			default:

				break;
		}
	}
	if(initstat & SINT_STAT_CONNECT)					/* TCP�����ж� */
	{
#if CH563NET_DBG										/* �������жϱ�ʾTCP�Ѿ����ӣ����Խ����շ����� */
		printf("TCP Connect Success\n");													 
#endif
		MQTT_Connect(username, password);

	}
	if(initstat & SINT_STAT_DISCONNECT)					/* TCP�Ͽ��ж� */
	{
#if CH563NET_DBG										/* �������жϣ�CH563���ڲ��Ὣ��socket�������Ϊ�ر�*/
		printf("TCP Disconnect\n");						/* Ӧ������������´������� */
#endif												 
		CH563NET_CreatTcpSocket();
	}
	if(initstat & SINT_STAT_TIM_OUT)					/* TCP��ʱ�ж� */
	{
#if CH563NET_DBG										/* �������жϣ�CH563���ڲ��Ὣ��socket�������Ϊ�ر�*/
		printf("TCP Timout\n");							/* Ӧ������������´������� */
#endif
		Transport_Open();
	}
}

/*******************************************************************************
* Function Name	: CH563NET_HandleGloableInt
* Description	: ȫ���жϴ�������
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void CH563NET_HandleGlobalInt(void)
{
	UINT8 initstat;
	UINT8 i;
	UINT8 socketinit;
	
	initstat = CH563NET_GetGlobalInt();										/* ��ȫ���ж�״̬����� */
	if(initstat & GINT_STAT_UNREACH)										/* ���ɴ��ж� */
	{
#if CH563NET_DBG
		printf("UnreachCode ��%d\n",CH563Inf.UnreachCode);					/* �鿴���ɴ���� */
		printf("UnreachProto ��%d\n",CH563Inf.UnreachProto);				/* �鿴���ɴ�Э������ */
		printf("UnreachPort ��%d\n",CH563Inf.UnreachPort);					/* ��ѯ���ɴ�˿� */
#endif			 
	}
	if(initstat & GINT_STAT_IP_CONFLI)										/* IP��ͻ�ж� */
	{
	 
	}
	if(initstat & GINT_STAT_PHY_CHANGE)										/* PHY�ı��ж� */
	{
		i = CH563NET_GetPHYStatus();										/* ��ȡPHY״̬ */
#if CH563NET_DBG
		printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif	 
	}
	if(initstat & GINT_STAT_SOCKET)											/* Socket�ж� */
	{
		for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)										 
		{
			socketinit = CH563NET_GetSocketInt(i);							/* ��socket�жϲ����� */
			if(socketinit) CH563NET_HandleSockInt(i,socketinit);			/* ������ж������� */
		}		
	}
}



/*******************************************************************************
* Function Name	: mInitSTDIO
* Description	: Ϊprintf��getkey���������ʼ������
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void mInitSTDIO( void )
{
	UINT32		x, x2;

	x = 10 * FREQ_SYS * 2 / 16 / 115200;									/* 115200bps */
	x2 = x % 10;
	x /= 10;
	if ( x2 >= 5 ) x ++;													/* �������� */
	R8_UART0_LCR = 0x80;													/* DLABλ��1 */
	R8_UART0_DIV = 1;														/* Ԥ��Ƶ */
	R8_UART0_DLM = x>>8;
	R8_UART0_DLL = x&0xff;

	R8_UART0_LCR = RB_LCR_WORD_SZ ;													/* �����ֽڳ���Ϊ8 */
	R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |		
								 RB_FCR_FIFO_EN ;									/* ����FIFO������Ϊ14���巢�ͺͽ���FIFO��FIFOʹ�� */
	R8_UART0_IER = RB_IER_TXD_EN;													/* TXD enable */
	R32_PB_SMT |= RXD0|TXD0;														/* RXD0 schmitt input, TXD0 slow rate */
	R32_PB_PD &= ~ RXD0;															/* disable pulldown for RXD0, keep pullup */
	R32_PB_DIR |= TXD0;																/* TXD0 output enable */
}

/*******************************************************************************
* Function Name	: fputc
* Description	: ͨ��������������Ϣ
* Input			: c-- writes the character specified by c 
*				  *f--the output stream pointed to by *f
* Output		: None
* Return		: None
*******************************************************************************/
int fputc( int c, FILE *f )
{
		R8_UART0_THR = c;													/* �������� */
		while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );				/* �ȴ����ݷ��� */
		return( c );
}


__irq void IRQ_Handler( void )
{
	if(R32_INT_FLAG & 0x8000)												/* ��̫���ж� */
	{																		/* ��̫���ж��жϷ����� */
			CH563NET_ETHIsr();
	}
	if(R32_INT_FLAG & RB_IF_TMR0)											/* ��ʱ���ж� */
	{
		CH563NET_TimeIsr(CH563NETTIMEPERIOD);								/* ��ʱ���жϷ����� */
		R8_TMR0_INT_FLAG |= 0xff;											/* �����ʱ���жϱ�־ */
		a++;
	 }
}

__irq void FIQ_Handler( void )
{
	while(1);
}

/*******************************************************************************
* Function Name	: main
* Description	: ������
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
int main(void) 
{
	UINT32 i = 0;
	UINT16 TimeDelay=0;
	char payload[500];
	
	for(i=0;i<500;i++)
	payload[i]='a';

	mInitSTDIO( );															/* Ϊ���ü����ͨ�����ڼ����ʾ���� */
	i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);					/* ���ʼ�� */
	mStopIfError(i);														/* ������ */
#if CH563NET_DBG
	printf("CH563NETLibInit Success\n");
#endif		
	SysTimeInit();															/* ϵͳ��ʱ����ʼ�� */
	InitSysHal();															/* ��ʼ���ж� */
	Transport_Open();	

	while(1)
	{
		CH563NET_MainTask();												/* CH563NET��������������Ҫ����ѭ���в��ϵ��� */
		if (CH563NET_QueryGlobalInt()) CH563NET_HandleGlobalInt();			/* ��ѯ�жϣ�������жϣ������ȫ���жϴ������� */
		Delay_ms(1);
		TimeDelay++;
		if (TimeDelay>500) {
			TimeDelay=0;
			if(con_flag) MQTT_Publish(pub_topic,payload);
		}
	}
}

/*********************************** endfile **********************************/