/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563���粿�ֳ�ʼ����MAC�շ��ӿں���
*******************************************************************************/



/******************************************************************************/
/* �ļ����� */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "NETSFR.H"
#include "NETMAC.H"

/******************************************************************************/
/* ȫ�ֱ������� */
ETHERHEAD    ETH;                                                               /* mac���ͷ */
ARPPACKAGE   ARP;                                                               /* arp���ݰ� */
MACCtrl      mac_cotrl;                                                         /* CH563MAC���ƽṹ */
struct  _RXBUFST    RxCtrl;                                                     /* RX���տ�����Ϣ�� */
struct  _TXBUFST    TxCtrl;                                                     /* TX���տ�����Ϣ�� */
UINT8V  SendSuc;                                                                /* �������ݰ� �ɹ���־ */
UINT8V  PHYStatus;                                                              /* PHY״̬ */

/*******************************************************************************
* Function Name  : PHY_ReadData
* Description    : ��PHY��ȡ���ݣ����ݿ���Ϊ16λ
* Input          : regad PHY�Ĵ�����ַ 
* Output         : None
* Return         : None
*******************************************************************************/
UINT16 PHY_ReadData(UINT16 regad)
{
    R32_ETHE_PHYCR = RB_PHYAD(FTPHY_RB_PHYAD) | RB_REGAD(regad) | RB_MIIRD;     /* ��PHY���ƼĴ���д��PHY��ַ��PHY�Ĵ�����ַ���Լ����Ͷ��ź�*/
    Delay_us(100);
    while( R32_ETHE_PHYCR & RB_MIIRD );                                         /* �ȴ������ */
    return( (UINT16)(R32_ETHE_PHYCR & RB_MIIRDATA) );
}

/*******************************************************************************
* Function Name  : CH563PHY_Process
* Description    : PHY��������������Э�̽����������MACCR�Ĵ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563PHY_Process(void)
{
    UINT16 value;

    value = PHY_ReadData(FTPHY_REG_STATUS);
    if( (value&RB_LINK_STATUS)==0 ){
    }
    if( (value&RB_AUTO_NEG_CMP)==0 ){
    }
    value = PHY_ReadData(FTPHY_REG_CONTROL);
    if((value & RB_DUPLEX_MODE) > 0){
        R32_ETHE_MACCR |= RB_FULLDUP;
    }
    else{
        R32_ETHE_MACCR &= ~(RB_FULLDUP);
    }
    if((value & RB_SPEED_SEL) > 0){
        R32_ETHE_MACCR |= RB_SPEED_100;
    }
    else{
        R32_ETHE_MACCR &= ~(RB_SPEED_100);
    }
}

/*******************************************************************************
* Function Name  : CH563MAC_ParamInit
* Description    : ������ʼ����������ʼ��ȫ�ֱ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563MAC_ParamInit(void)
{
    memset((void *)&TxCtrl,0,sizeof(TxCtrl));
    memset((void *)&RxCtrl,0,sizeof(RxCtrl));
    RxCtrl.RxDMAEnable = TRUE;
}

/*******************************************************************************
* Function Name  : MAC_InitTXDes
* Description    : ��ʼ��������������������������16�ֽڶ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_InitTXDes(void)
{

    UINT32 i;
    mac_txdes *tmp;

    mac_cotrl.txdes_top = (mac_txdes *)TxDesBuf;                                /* ������������ָ��������������ͷ�� */
    mac_cotrl.txdes_cur = mac_cotrl.txdes_top;                                  /* ��ǰ������ָ��������������ͷ��*/
    mac_cotrl.txdes_end = mac_cotrl.txdes_top + (TX_QUEUE_NUM - 1);             /* ���һ��������ָ�򻺳���β��*/
    memset(mac_cotrl.txdes_top, 0,sizeof(TxDesBuf));                            /* ��շ��������� */
    mac_cotrl.txdes_end->txdes1 |= TXDES1_EDOTR;                                /* ���һ����������Ϊ������־ */
    mac_cotrl.txdes_cur->txdes2 |= (UINT32)MACTxBuf[TxCtrl.SendIndex];          /* ���ͻ������Ļ���ַ */
    MACSendbuf = (UINT8 *)&MACTxBuf[TxCtrl.SendIndex][14];
    tmp = mac_cotrl.txdes_top;
    for(i = 0; i < TX_QUEUE_NUM; i++){                                          /* ���������������������� */
        tmp->txdes3 = (UINT32)(tmp + 1);
        tmp = (mac_txdes *)tmp->txdes3;
    }
    mac_cotrl.txdes_end->txdes3 = (UINT32)mac_cotrl.txdes_top;
}

/*******************************************************************************
* Function Name  : MAC_InitRXDes
* Description    : ��ʼ��������������������������16�ֽڶ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_InitRXDes(void)
{
    int i;
    mac_rxdes *tmp;

    mac_cotrl.rxdes_top = (void *)RxDesBuf;                                     /* rxdes_topָ���һ�������� */
    mac_cotrl.rxdes_cur = mac_cotrl.rxdes_top;                                  /* rxdes_curָ���һ�������� */
    mac_cotrl.rxdes_end = mac_cotrl.rxdes_top + (RX_QUEUE_NUM - 1);             /* rxdes_endָ�����һ�������� */
    memset(mac_cotrl.rxdes_top, 0, sizeof(RxDesBuf));                           /* ���������*/
    mac_cotrl.rxdes_end->rxdes1 |= RXDES1_EDORR;                                /* ���һ����������Ϊ������־ */    
    mac_cotrl.rxdes_cur->rxdes2 |= (UINT32)MACRxBuf[RxCtrl.RecvIndex];          /* ���ջ������Ļ���ַ */
    tmp = mac_cotrl.rxdes_top;                                                  /* ָ���һ�������� */
    tmp->rxdes0 |= RXDES0_RXDMA_OWN;                                            /* ��MACӵ�б�������ʹ��Ȩ�� */
    for(i = 0; i < RX_QUEUE_NUM; i++){                                          /* ���������������������� */
        tmp->rxdes1 |= RXDES1_RXBUF_SIZE( RX_BUF_SIZE );                        /* ���ջ�������С����λΪ�ֽ� */
        tmp->rxdes3 = (UINT32)(tmp + 1);                                        /* rxdes3������һ����������ַ */
        tmp = (mac_rxdes *)tmp->rxdes3;                                         /* ָ����һ����������ַ */ 
    }
    mac_cotrl.rxdes_end->rxdes3 = (UINT32)mac_cotrl.rxdes_top;                  /* rxdes3������һ����������ַ���׸���������*/
}

/*******************************************************************************
* Function Name  : CH563MAC_Open
* Description    : CH563MAC�򿪺�����������ʹ��Ĭ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563MAC_Open(void)
{

    UINT32 i;

    i = R32_ETHE_ISR;                                                           /* ����ж� */
    R32_ETHE_MACCR = RB_SW_RST;                                                 /* ��λMAC */ 
    MAC_InitRXDes( );                                                           /* ��ʼ������������ */
    MAC_InitTXDes( );                                                           /* ��ʼ������������ */

#ifdef  TX_QUEUE_NUM
    R32_ETHE_TXR_BADR = (UINT32)mac_cotrl.txdes_top;                            /* ��������������ַд��R32_ETHE_TXR_BADR */
#else
    R32_ETHE_TXR_BADR = (UINT32)mac_cotrl.txdes_point;                          /* ��������������ַд��R32_ETHE_TXR_BADR */
#endif

    R32_ETHE_RXR_BADR = (UINT32)mac_cotrl.rxdes_top;                            /* ��������������ַд��R32_ETHE_RXR_BADR */
    R32_ETHE_IMR = RB_RPKT_FINISH_EN | RB_NORXBUF_EN | RB_XPKT_OK_EN |          /* ����MAC�ж�ʹ�ܼĴ��� */
                   RB_XPKT_LOST_EN | RB_RPKT_LOST_EN | RB_PHYSTS_CHG_EN;
    i = (UINT32)(CH563MACAddr[2]<<24) + (UINT32)(CH563MACAddr[3]<<16) + (UINT32)(CH563MACAddr[4]<<8) + CH563MACAddr[5]; 
    R32_ETHE_MAC_LADR = i;                                                      /* дMAC��ַ�Ĵ����ĵ�4��*/
    i = (UINT32)(CH563MACAddr[0]<<8) + CH563MACAddr[1]; 
    R32_ETHE_MAC_MADR = i & 0xffff;                                             /* дMAC��ַ�Ĵ����ĸ�2��*/

    R32_ETHE_ITC   = RB_TXINT_THR(1);                                           /* �����ж϶�ʱ�� */
     R32_ETHE_APTC  = RB_RXINT_CNT(1);                                          /* ���ò�ѯ�Ĵ��� */
    R32_ETHE_DBLAC = RB_RXFIFO_LTHR(2) | RB_RXFIFO_HTHR(6) | RB_RX_THR_EN |     /* ����DMAͻ������ */
                     RB_INCR_SEL(2);
#if 1
    R32_ETHE_MACCR |= ( RB_RCV_ALL | RB_TDMA_EN | RB_RDMA_EN | RB_CRC_APD | 
                      RB_XMT_EN | RB_RCV_EN );                                  /* ����MACCR�Ĵ��� */
#else
    R32_ETHE_MACCR |= ( RB_TDMA_EN | RB_RDMA_EN | RB_CRC_APD | 
                      RB_XMT_EN | RB_RCV_EN );                                  /* ����MACCR�Ĵ��� */
#endif  
}

/*******************************************************************************
* Function Name  : CH563NET_Init
* Description    : ��ʼ��CH56��̫������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_Init(void)
{
    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                                      /* �����ȫ���ж� */

    R32_PA_DIR |= (1<<18);                                                      /*PA18ΪLINK��ACK�� */
    R8_MISC_CTRL_ETH |= RB_MISC_ETH_RST | RB_MISC_ETH_LED;

    R8_SAFE_ACCESS_SIG = 0x57;                                                  /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8;                                                  /* unlock step 2 */                   
    R8_SLP_WAKE_CTRL  &= 0x7f;                                                  /* ������Դ */
    R8_SAFE_ACCESS_SIG = 0x00;                                                  /* д���� */

    Delay_ms( 50 );
    R8_MISC_CTRL_ETH &= ~RB_MISC_ETH_RST;                                       /* ��λ����*/
    Delay_ms( 200 );

    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */
    R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_ETH;                                         /* ��ʱ�� */
    R8_SAFE_ACCESS_SIG = 0;                                                     /* д���� */
}

/*******************************************************************************
* Function Name  : NET_INIT
* Description    : ��̫����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NET_INIT( void )
{
    CH563MAC_ParamInit( );                                                      /* ������ʼ�� */
    CH563NET_Init( );                                                           /* ������̫�����ֵ�Դ��ʱ�ӣ�elink�� */
    CH563MAC_Open( );                                                           /* ��ʼ��MAC */
    CH563PHY_Process( );                                                        /* ��ʼ��PHY */
}

/*******************************************************************************
* Function Name  : MAC_RxSuccessHandle
* Description    : ��̫�����ճɹ��ص���������Ҫ�Ǵ�����������ʶ����ѯ����ǰ������
                   û�п��У���ֹͣDMA����ֱ�����������òſ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_RxSuccessHandle(void)
{
    RxCtrl.RxBufStau[RxCtrl.RecvIndex] = BUF_IP_LOCK;                           /* ���˻�����������Ȩ�޽����ϲ� */
    RxCtrl.RxBufAddr[RxCtrl.RecvIndex]=((UINT32)MACRxBuf[RxCtrl.RecvIndex])+2;  /* ��ַ���ƫ��2�ֽڣ�ÿ������ǰ2���ֽ���Ч */
    RxCtrl.RecvIndex++;                                                         /* MAC��������ָ����һ�����ջ����� */
    if(RxCtrl.RecvIndex >= RX_QUEUE_NUM){                                       /* ����Ƿ�β�� */
        RxCtrl.RecvIndex = 0;
    }
    /* �������ջ���������ķ����Ƕ�ʧ��������ݰ� */
    if(RxCtrl.RxBufStau[RxCtrl.RecvIndex] == BUF_IDLE){                         /* �����һ�����ջ��������У��򽫽���������ָ��˻����� */
        RxCtrl.RemainCout++;
        if(RxCtrl.RemainCout > RX_QUEUE_NUM){
            RxCtrl.RemainCout = RX_QUEUE_NUM;
        }
    }
    else{
    }
    RxCtrl.RxBufStau[RxCtrl.RecvIndex] = BUF_MAC_LOCK;                          /* �˻�������MACʹ�ã�MAC�������� */
    mac_cotrl.rxdes_cur = (mac_rxdes *)mac_cotrl.rxdes_cur->rxdes3;             /* ����ǰ������ָ����һ�� */
    /* ��ʼ����һ�������� */
    mac_cotrl.rxdes_cur->rxdes2 = (UINT32)MACRxBuf[RxCtrl.RecvIndex];    
    mac_cotrl.rxdes_cur->rxdes0 = RXDES0_RXDMA_OWN;
    RxCtrl.RxDMAEnable = TRUE;
}

/*******************************************************************************
* Function Name  : MAC_QueryInput
* Description    : ��ѯ�Ƿ��пɶ����ݰ�
* Input          : None
* Output         : None
* Return         : ����ʣ�����ݰ�������0��ʾû������
*******************************************************************************/
UINT8 MAC_QueryInput(void)
{
    return RxCtrl.RemainCout;                                                   /* ����ʣ���������Ŀ */
}

/*******************************************************************************
* Function Name  : MAC_Xmit
* Description    : MAC���ͺ�����ֻ�ܷ���һ������
* Input          : tx_addr  ���ͻ�������ַ
*                ��size     �������ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_Xmit( UINT32 size)
{
    TxCtrl.TxBufStat[TxCtrl.SendIndex] = BUF_MAC_LOCK;                         /* �޸ķ�����Ϣ�б� */
    if( mac_cotrl.txdes_cur->txdes0 & TXDES0_TXDMA_OWN ){
//        R32_ETHE_MACCR &= (~RB_TDMA_EN);
//        R32_ETHE_MACCR |= RB_TDMA_EN; 
    }
    mac_cotrl.txdes_cur->txdes1 &= TXDES1_EDOTR;
    mac_cotrl.txdes_cur->txdes1 |= ( TXDES1_TXBUF_SIZE(size) | TXDES1_LTS | 
                                   TXDES1_FTS | TXDES1_TXIC );
    mac_cotrl.txdes_cur->txdes0 = TXDES0_TXDMA_OWN;
    SendSuc = 1;
    TxCtrl.SendIndex++;
    if(TxCtrl.SendIndex >= TX_QUEUE_NUM){
        TxCtrl.SendIndex = 0;
    }
    if( (R32_ETHE_APTC & RB_TXINT_CNT(0xf)) == 0 ){
        R32_ETHE_TXPD = 0xffffffff;                                             /* д����������������MAC������������ */
    }
}

/*******************************************************************************
* Function Name  : MAC_SendOkHanld
* Description    : ��CH563MAC_Isr���ã�CH563MAC��������ж�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_SendOkHanld(void)
{                                                                                    
    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                                      /* �ر�IRQȫ���ж� */
    mac_cotrl.txdes_cur = (mac_txdes *)mac_cotrl.txdes_cur->txdes3;             /* ������������ָ����һ�� */
    mac_cotrl.txdes_cur->txdes2 |= (UINT32)MACTxBuf[TxCtrl.SendIndex];          /* ���ͻ������Ļ���ַ */
    MACSendbuf = (UINT8 *)&MACTxBuf[TxCtrl.SendIndex][14];
    SendSuc = 0;                                                                /* ���ͳɹ���־��0 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* ����IRQȫ���ж� */
}

/*******************************************************************************
* Function Name  : PHY_GetLinkStatus
* Description    : ��ȡPHY������״̬����PHY�ı��ж��е��ã���ȡ��ǰ״̬
* Input          : None
* Output         : None
* Return         : ��������״̬
*******************************************************************************/
UINT8 PHY_GetLinkStatus(void)
{
    UINT8  status;
    UINT16 value;

    value = PHY_ReadData(FTPHY_REG_STATUS);                                     /* ��ȡ״̬�Ĵ��� */
    if( (value & RB_LINK_STATUS) == 0 ){                                        /* ���Ϊ�Ͽ�״̬ */
        status =  PHY_DISCONN;  
    }
    else{
        value = PHY_ReadData(FTPHY_REG_CONTROL);                                /* ��ȡPHY���ƼĴ��� */
        if((value & RB_DUPLEX_MODE) ){                                          /* ���Ϊȫ˫��ģʽ */
            R32_ETHE_MACCR |= RB_FULLDUP;
            if((value & RB_SPEED_SEL) > 0){                                     /* ��ȡ�ٶ� */
                R32_ETHE_MACCR |= RB_SPEED_100;
                status = PHY_100M_FLL;                                          /* ȫ˫����100M */
            }
            else{ 
                R32_ETHE_MACCR &= ~RB_SPEED_100;
                status = PHY_10M_FLL;                                           /* ȫ˫����10M */
            }
        }
        else{                                                                   /* ���Ϊ��˫��ģʽ */
            R32_ETHE_MACCR &= ~(RB_FULLDUP);                                    /* ��ȡ�ٶ� */
            if((value & RB_SPEED_SEL) > 0){                                    
                R32_ETHE_MACCR |= RB_SPEED_100;
                status = PHY_100M_HALF;                                         /* ��˫����100M */
            }
            else{ 
                R32_ETHE_MACCR &= ~(RB_SPEED_100);
                status = PHY_10M_HALF;                                          /* ��˫����10M */
            }
        }
    }                   
    return status;
}

/*******************************************************************************
* Function Name  : MAC_RecvFinishHanld
* Description    : ��CH563MAC_Isr���ã���������жϴ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_RecvFinishHanld(void)
{

    if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_FRS){                               /* �Ƿ��Ƿ�Ϊ��һ�������� */
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RX_ODD_NB){                     /* ���ֽڴ��� */
#if  CH563_DEBUG
            printf("ERROR *** RX_ODD_NB\n");
#endif 
            return;
        }
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_CRC_ERR){                       /* CRCУ����� */
#if  CH563_DEBUG
            printf("ERROR *** CRC\n");
#endif
            return;
        }
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RX_ERR){                        /* ����������DES0���� */
#if  CH563_DEBUG
            printf("ERROR *** RX_ERR\n");
#endif
            return;
        }
        RxCtrl.RxBufLen[RxCtrl.RecvIndex] = mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RFL;/* ��ȡ���ȣ����������Ƿ񳬹�TX_BUF_SIZE */
        if( RxCtrl.RxBufLen[RxCtrl.RecvIndex] > TX_BUF_SIZE){
#if  CH563_DEBUG
            printf("ERROR *** len large\n");
#endif
            return;
        }
    }
    else{                                                                       /* ���ǵ�һ������ */
#if  CH563_DEBUG
        printf("ERROR *** not the first package\n");
#endif
        return;
    }
    while(1){
        while(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_RXDMA_OWN);                  /* �ȴ�������Ȩ�� */
        if(mac_cotrl.rxdes_cur->rxdes0 & RXDES0_LRS){
            MAC_RxSuccessHandle();                                              /* ���ճɹ�������������Ҫ���¿����б� */
            break;
        }
        mac_cotrl.rxdes_cur->rxdes0 = RXDES0_RXDMA_OWN;                         /* �ͷ�DMA�����ڶ����������������CH563MAC_RxSuccessHandle�Ѿ���ת */
        mac_cotrl.rxdes_cur = (mac_rxdes *)mac_cotrl.rxdes_cur->rxdes3;         /* ����ǰ������ָ����һ�� */
    }
}

/*******************************************************************************
* Function Name  : MAC_Input
* Description    : ��ȡ�������ݣ���ȡһ֡����
* Input          : buf  ������,�������Ĵ�С���벻С��RX_BUF_SIZE��
*                : len  ���ݰ���Ч����
* Output         : None
* Return         : ����ʣ�����ݰ�������0��ʾû������
*******************************************************************************/
UINT16 MAC_Input(UINT8 *buf,UINT16 *len)
{
    UINT8  *macdata;
    UINT16 mac_len;

    mac_len = RxCtrl.RxBufLen[RxCtrl.ReadIndex];                                /* ��ȡ��ǰ���ݰ����� */
    if( mac_len < ETHER_HEAD_LEN ){
        *len = 0;
        return FALSE;
    }
    if(  mac_len > RECE_BUF_LEN ){
        mac_len = RECE_BUF_LEN;
    }
    macdata = (UINT8 *)RxCtrl.RxBufAddr[RxCtrl.ReadIndex];                      /* ��ȡ��ǰ���ݰ�ָ�� */
    memcpy(ETH.buffer,macdata,ETHER_HEAD_LEN);                                  /* ����̫����ͷ���Ƴ��� */
    /// �˴����Զ�mac��ַ���ݳ�������� /////////
    mac_len -= ETHER_HEAD_LEN;
    memcpy(buf,&macdata[ETHER_HEAD_LEN],mac_len);                               /* �����ݰ����Ƶ��û�ָ���Ļ������� */
    R8_INT_EN_IRQ_GLOB &= ~RB_IE_IRQ_GLOB;                                      /* �ر��жϣ��޸Ľ�����Ϣ�б� */
    RxCtrl.RxBufStau[RxCtrl.ReadIndex] = BUF_IDLE;                              /* ����ǰ�б���Ϊ����״̬ */
    RxCtrl.ReadIndex++;                                                         /* ������+1 */
    if(RxCtrl.ReadIndex >= RX_QUEUE_NUM){                                       /* ���������Χ */
        RxCtrl.ReadIndex = 0;
    }
    if(RxCtrl.RemainCout){                                                      /* ʣ�������-1 */
        RxCtrl.RemainCout--; 
    }
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* �����ж�  */
    *len = mac_len;
    return TRUE;
}

/*******************************************************************************
* Function Name  : MAC_Output
* Description    : CH563MAC ��������
* Input          : buf  ������,�������Ĵ�С���벻�ô���TX_BUF_SIZE���Ҳ���С��60
*                : len  ���ݰ���Ч����
*                ��code mac��Э������
* Output         : None
* Return         : ���ط���״̬
*******************************************************************************/
UINT8 MAC_Output(UINT8 *buf,UINT16 *len,UINT16 code)
{
    UINT8   timecount = 0;
    UINT16  mac_len;
    
    if(buf) MACSendbuf = buf;
    mac_len = *len;
    if( mac_len > MAC_MAX_LEN ){                                                /* ��鳤�ȣ����ȴ���TX_BUF_SIZE�����ܻᵼ��ϵͳ���� */
        mac_len = MAC_MAX_LEN; 
        *len = MAC_MAX_LEN;
    }
    memcpy( ETH.Head.SourMAC,CH563MACAddr,MACADDR_LEN );                        /* ��д����mac��ַ */
    ETH.Head.PROTOCOL = code;                                                   /* ��дЭ������ */
    memcpy( MACTxBuf[TxCtrl.SendIndex],ETH.buffer,ETHER_HEAD_LEN );             /* ����̫����ͷ���Ƶ����ͻ����� */
    mac_len += ETHER_HEAD_LEN; 
    if( mac_len < 60){
        memset(&MACTxBuf[mac_len],0,60 - mac_len);                              /* ������60���ֽڵĲ��ֲ�0 */
        mac_len = 60;                                                           /* ��С��Ϊ60�ֽ� */
    }
    MAC_Xmit( mac_len);                                                         /* Ϊ�˽�ʡRAM������ֱ����buf�����ݷ��ͣ���ʡȥ����ʱ�䣬���Ч��*/
    while(SendSuc){                                                             /* ������Ϻ󣬻�������ͳɹ��ж�CH563MAC_INT_XPKT_OK */
       timecount++;
       Delay_us(10);
       if((timecount > 200))return FALSE;                                       /* ���ʱʱ��ԼΪ2MS����ʱ����FALSE(0) */
    }
    return TRUE;                                                                /* �ɹ�����TRUE(1) */
}

/*******************************************************************************
* Function Name  : GLOB_nTohs
* Description    : 16λ��С���л�
* Input          : in 
* Output         : None
* Return         : None
*******************************************************************************/
UINT16 GLOB_nTohs( UINT16 in ) 
{
    UINT16 out;
    out = (in<<8) | (in>>8);
    return out;
}

/*******************************************************************************
* Function Name  : MAC_Isr
* Description    : MAC�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAC_Isr(void)
{
    UINT32  int_status;
     
    int_status = R32_ETHE_ISR;
    if(int_status & RB_RPKT_FINISH) {                                           /* ��������ж� */  
        MAC_RecvFinishHanld( );                                     
    }
    if(int_status & RB_XPKT_FINISH){                                            /* �������(TXDMA ������д��FIFO������ʾ���ͳɹ�)*/

    }
    if(int_status & RB_XPKT_OK){                                                /* ���ݷ��ͳɹ� */
        MAC_SendOkHanld( );
    }      
    if(int_status & RB_PHYSTS_CHG){                                             /* PHY�ı��ж� */
        PHYStatus = PHY_GetLinkStatus( );                                       /* ��ȡPHY״̬ */
#if  CH563_DEBUG
        printf("PHY change,status=%x\r\n",(UINT16)PHYStatus);
#endif
    }                                                                 
}

/*******************************************************************************
* Function Name  : ARP_Output
* Description    : ����arp����
* Input          : DestIP�����͵�Ŀ��ip��ַ
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 ARP_Output( UINT8 *desip,UINT8 code )
{
    UINT16 arp_len;    

    arp_len = ARP_PACKAGE_LEN;                                               
    ARP.Package.HRDTYPE = GLOB_nTohs( ARP_HARDWARE_TYPE_ETHER );
    ARP.Package.PROTYPE = GLOB_nTohs( ARP_PROTOCOL_TYPE_IP );    
    ARP.Package.MACLEN  = MACADDR_LEN;    
    ARP.Package.IPLEN   = IPADDR_LEN;    
    ARP.Package.OPCODE  = GLOB_nTohs( code );                                   /* ���ݰ����ͣ���1 -arp����2-arpӦ��*/
    memcpy(ARP.Package.TARGIP,desip,IPADDR_LEN);                                /* arp�����Ŀ��ip��ַ */
    memset(ARP.Package.TARGMAC,0,MACADDR_LEN);                                  /* arp�����Ŀ��mac��ַ��Ϊȫ0 */
    memcpy(ARP.Package.SENDIP,CH563IPAddr,IPADDR_LEN);                          /* ����ip��ַ */
    memcpy(ARP.Package.SENDMAC,CH563MACAddr,MACADDR_LEN);                       /* ����mac��ַ */
    memcpy( MACSendbuf,ARP.buffer,arp_len );                                    /* ��arp���ݰ����Ƶ����ͻ����� */
    if( code == ARP_CODE_REPLY ){
        memcpy(ETH.Head.DestMAC,ETH.Head.SourMAC,MACADDR_LEN);                  /* ��дĿ��mac��ַ */
    }
    else memset(ETH.Head.DestMAC,0xff,MACADDR_LEN);                             /* ��дĿ��mac��ַ */
    return ( MAC_Output( MACSendbuf,&arp_len,GLOB_nTohs( ETHER_TYPE_ARP ) ));
}

/*******************************************************************************
* Function Name  : ARP_Input
* Description    : �յ�arp��
* Input          : DestIP�����͵�Ŀ��ip��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void ARP_Input( UINT8 *arpdata,UINT16 len )
{
    
    if( len < ARP_PACKAGE_LEN ) return;
    memcpy(ARP.buffer,arpdata,ARP_PACKAGE_LEN);
    if(    ARP.Package.HRDTYPE == GLOB_nTohs( ARP_HARDWARE_TYPE_ETHER ) &&
        ARP.Package.PROTYPE == GLOB_nTohs( ARP_PROTOCOL_TYPE_IP ) &&
        ARP.Package.MACLEN  == MACADDR_LEN &&    
        ARP.Package.IPLEN   == IPADDR_LEN ){
        if( ARP.Package.OPCODE  == GLOB_nTohs( ARP_CODE_QUEST )){
            if(  memcmp(ARP.Package.TARGIP,CH563IPAddr,IPADDR_LEN) == 0 ){
                ARP_Output( ARP.Package.TARGIP,ARP_CODE_REPLY );
            }
        }
        else{
            return ;
        }
    }    
}

/*********************************** endfile **********************************/