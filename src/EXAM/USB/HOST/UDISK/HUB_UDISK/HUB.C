/********************************** (C) COPYRIGHT *******************************
* File Name          : HUB.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller HUB Operate
*                      ע: HUB�˿ں���СֵΪ1 
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  "CH563BAS.H"
#include  "EHCI_HCD.H"                                                          /* �����������Ͳ�������ͷ�ļ� */
#include  "CH563SFR.H"                                                          /* CH563�Ĵ����������ͷ�ļ� */
#include  "CH563USBSFR.H"                                                       /* оƬUSB�Ĵ������ͷ�ļ� */    
#include  "CH563USB.H"                                                          /* USB���ö������ͷ�ļ� */    
#include  "HUB.H"                                                              
#include  "PRINTF.H"                                                            /* ���ڴ�ӡ�������ͷ�ļ� */

/*******************************************************************************
* Function Name  : HUB_CofDescrAnalyse
* Description    : ����HUB������������
* Input          : *Hubinfo----��ǰHUB��ؽṹ��
*                  *pbuf-------��ǰҪ���������ݻ�����
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 HUB_CofDescrAnalyse( HUB_INFO *Hubinfo, UINT8 *pbuf )
{
    UINT16 i;

    /* ��������������,��ȡ�˵�����(���˵��ַ/���˵��С��) */
    if ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != USB_DEV_CLASS_HUB ) 
    {
        return( USB_OPERATE_ERROR );                                            /* ����HUB�豸,ֱ�ӷ���  */
    }    

    Hubinfo->InterfNumber = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceNumber;  /* ����ӿں� */
    Hubinfo->IntInEp = 0x00;
    for( i = 0; i < 1; i ++ )                                                   /* ֻ����1���˵� */
    {
        if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bLength == 0x07
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bDescriptorType == 0x05  
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes == 0x03 ) 
        {      
            if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
            {    
                /* �ж�IN�˵� */
                Hubinfo->IntInEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;   /* IN�˵� */
                Hubinfo->IntInEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
                Hubinfo->IntInEpSize = Hubinfo->IntInEpSize << 8;
                Hubinfo->IntInEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;                
            }
        }
    }
    if( ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != USB_DEV_CLASS_HUB )
      || ( Hubinfo->IntInEp == 0 ) )
    {                                                                           /* ����USB�洢���豸,��֧�� */
        return( USB_OPERATE_ERROR );                                            /* ��ֹ����,ֱ�ӷ��� */
    }

#ifdef  MY_DEBUG_PRINTF 
    printf("Hubinfo->IntInEp:%02x\n ",Hubinfo->IntInEp);
    printf("Hubinfo->IntInEpSize:%02x\n ",Hubinfo->IntInEpSize);
    printf("Hubinfo->InterfNumber:%02x\n ",Hubinfo->InterfNumber);
#endif
    return( TRUE );
}

/*******************************************************************************
* Function Name  : HUB_GetClassDevDescr
* Description    : ��ȡHUB��������
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
                   *buf-------���ݻ�����
                   *len-------���ݳ���
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_GetClassDevDescr( HUB_INFO *Hubinfo, UINT8 *buf, UINT16 *len ) 
{
    UINT16 l;
    UINT8  status;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_GET_HUB_DESCR;
    Ctl_Setup->bRequest = USB_REQ_GET_DESCR;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x29;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x02;
    Ctl_Setup->wLengthH = 0x00;
    
    /* �Ȼ�ȡHUB����������ǰ2���ֽ� */    
    l = Ctl_Setup->wLengthL;
    status = USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &l );     /* ִ�п��ƴ��� */    
    if( status == USB_OPERATE_SUCCESS )
    {        
        /* �ٻ�ȡȫ��HUB�������� */
        Ctl_Setup->wLengthL = buf[ 0 ];
        l = Ctl_Setup->wLengthL;
        status = USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &l ); /* ִ�п��ƴ��� */    
        if( status == USB_OPERATE_SUCCESS )
        {
            *len = l;
        }        
        else
        {
            *len = 0x00;
        }
        return( status );
    }
    return( status );
}

/*******************************************************************************
* Function Name  : HUB_ClearHubFeature
* Description    : ���HUB����
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                  *buf---���ݻ�����
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_ClearHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector ) 
{
    UINT16 len;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_CLEAR_HUB_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_CLR_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* ִ�п��ƴ��� */        
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* ִ�п��ƴ��� */
}

/*******************************************************************************
* Function Name  : HUB_ClearPortFeature
* Description    : ���HUB�˿�����
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                  *buf---���ݻ�����
*                  port---�˿ں�
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_ClearPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 selector ) 
{
    UINT16 len;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_CLEAR_PORT_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_CLR_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = port + 1;                                             /* ��С�˿ں�Ϊ1 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* ִ�п��ƴ��� */        
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* ִ�п��ƴ��� */
}

/*******************************************************************************
* Function Name  : HUB_GetHubStatus
* Description    : ��ѯHUB״̬
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                   *buf-------���ݻ�����(����4���ֽ�: wHubStatus��wHubChange)
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_GetHubStatus( HUB_INFO *Hubinfo, UINT8 *buf ) 
{
    UINT16 len;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_GET_PORT_STATUS;
    Ctl_Setup->bRequest = USB_REQ_GET_STATUS;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x04;
    Ctl_Setup->wLengthH = 0x00;
    
    /* ִ�п��ƴ��� */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* ִ�п��ƴ��� */
}

/*******************************************************************************
* Function Name  : HUB_GetPortStatus
* Description    : ��ѯHUB�˿�״̬
* Input          : *Hubinfo---��ǰHUB��ؽṹ��(����4���ֽ�: wPortStatus��wPortChange)
*                   *buf-------���ݻ�����
*                  port-------�˿ں�
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_GetPortStatus( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port ) 
{
    UINT16 len;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_GET_PORT_STATUS;
    Ctl_Setup->bRequest = USB_REQ_GET_STATUS;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = port + 1;                                             /* ��С�˿ں�Ϊ1 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x04;
    Ctl_Setup->wLengthH = 0x00;
    
    /* ִ�п��ƴ��� */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* ִ�п��ƴ��� */
}

/*******************************************************************************
* Function Name  : HUB_SetHubFeature
* Description    : ����HUB�˿�����
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                  *buf---���ݻ�����
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_SetHubFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 selector ) 
{
    UINT16 len;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_SET_HUB_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_SET_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;                                            
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* ִ�п��ƴ��� */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* ִ�п��ƴ��� */
}

/*******************************************************************************
* Function Name  : HUB_SetPortFeature
* Description    : ����HUB�˿�����
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                  *buf---���ݻ�����
*                  port---�˿ں�
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 HUB_SetPortFeature( HUB_INFO *Hubinfo, UINT8 *buf, UINT8 port, UINT8 selector ) 
{
    UINT16 len;
    
    /* ���SETUP����� */
    Ctl_Setup->bReqType = USB_SET_PORT_FEATURE;
    Ctl_Setup->bRequest = USB_REQ_SET_FEATURE;
    Ctl_Setup->wValueL  = selector;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = port + 1;                                             /* ��С�˿ں�Ϊ1 */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;
    
    /* ִ�п��ƴ��� */    
    len = Ctl_Setup->wLengthL;
    return( USBHOST_Issue_Control( &Hubinfo->Device, Ctl_Setup, buf, &len ) );  /* ִ�п��ƴ��� */
}

/*******************************************************************************
* Function Name  : HUB_CheckPortConnect
* Description    : ���HUB���ζ˿�����״̬
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                  *buf---���ݻ�����
*                  port---�˿ں�(��ΧΪ0---3)
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬��˿��豸״̬
*                    0x00-----------------��USB�豸
*                  0x02-----------------��USB�豸
*                  USB_HUBDEVICE_CONNECT------��⵽HUB�µ�USB�豸�����¼�
*                  USB_HUBDEVICE_DISCONNECT---��⵽HUB�µ�USB�豸�Ͽ��¼�
*******************************************************************************/
UINT8 HUB_CheckPortConnect( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port ) 
{
    UINT8  status;

    /* ��ѯ�˿�״̬ */
    status = HUB_GetPortStatus( Hubinfo, pbuf, port );                                                                    
    if( status )
    {
        return( status );                                                       /* ����ʧ��ֱ�ӷ��� */
    }
    
    /* �жϵ�ǰ�˿�����״̬ */
    if( pbuf[ 2 ] & 0x01 )                                                      /* �ö˿�����״̬�����ı� */
    {
        if( pbuf[ 0 ] & 0x01 )
        {
            return( USB_HUBDEVICE_CONNECT );                                    /* �ö˿ڣ���⵽�豸���� */    
        }
        else
        {
            return( USB_HUBDEVICE_DISCONNECT );                                 /* �ö˿ڣ���⵽�豸�Ͽ� */    
        }                    
    }
    else                                                                        /* �ö˿�����״̬δ�����ı� */
    {
        if( pbuf[ 0 ] & 0x01 )
        {
            return( 0x02 );                                                     /* �ö˿ڣ����豸 */
        }
        else
        {
            return( 0x00 );                                                     /* �ö˿ڣ����豸 */
        }                            
    }    
}

/*******************************************************************************
* Function Name  : HUB_CheckPortSpeed
* Description    : ���HUB���ζ˿������豸���ٶ�
* Input          : *Hubinfo---��ǰHUB��ؽṹ��
*                  *buf---���ݻ�����
*                  port---�˿ں�(��ΧΪ0---3)
* Output         : None
* Return         : ���ص�ǰ�˿��������豸���ٶ�
*                  0x00----ȫ���豸
*                  0x01----�����豸
*                  0x02----�����豸
*******************************************************************************/
UINT8 HUB_CheckPortSpeed( HUB_INFO *Hubinfo, UINT8 *pbuf, UINT8 port ) 
{
    UINT8  status;

    /* ��ѯ�˿�״̬ */
    status = HUB_GetPortStatus( Hubinfo, pbuf, port );                                                                    
    if( status )
    {
        return( status );                                                       /* ����ʧ��ֱ�ӷ��� */
    }
    
    /* �ٶ��ɷ��صĵ�һ��˫�ֽڵĵ�9λ�͵�10λ���� */
    if( pbuf[ 1 ] & 0x02 )
    {
        return( EHCI_DEVICE_SPEED_LS );                                         /* ��ǰΪ�����豸 */
    }    
    else
    {
        if( pbuf[ 1 ] & 0x04 )
        {
            return( EHCI_DEVICE_SPEED_HS );                                     /* ��ǰΪ�����豸 */
        }
        else
        {
            return( EHCI_DEVICE_SPEED_FS );                                     /* ��ǰΪȫ���豸 */            
        }        
    }        
}     

/*********************************** endfile **********************************/