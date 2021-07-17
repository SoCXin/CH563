/********************************** (C) COPYRIGHT *******************************
* File Name          : UDISK.C
* Author             : WCH
* Version            : V1.20
* Date               : 2013/11/15
* Description        : CH563 USB Host Controller Mass Storage Device Operate
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "EHCI_HCD.H"                                                           /* �����������Ͳ�������ͷ�ļ� */
#include "CH563SFR.H"                                                           /* CH563�Ĵ����������ͷ�ļ� */
#include "CH563USBSFR.H"                                                        /* оƬUSB�Ĵ������ͷ�ļ� */    
#include "CH563USB.H"                                                           /* USB���ö������ͷ�ļ� */    
#include "Udisk.H"                                                              /* Udisk�������ͷ�ļ� */
#include "PRINTF.H"                                                             /* ���ڴ�ӡ�������ͷ�ļ� */

/*******************************************************************************
* Function Name  : MS_CofDescrAnalyse
* Description    : ����Mass Storage Device����������
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *pbuf--------��ǰ��Ҫ����������������������
* Output         : None
* Return         : USB_OPERATE_SUCCESS----Mass Storage Device; 
*                  USB_OPERATE_ERROR---�����豸
*******************************************************************************/
UINT8 MS_CofDescrAnalyse( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    UINT16 i;
    UINT8  status;
    
    /* ��������������,��ȡ�˵�����(���˵��ַ/���˵��С��) */
    if ( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != 0x08 ) 
    {
        return( USB_OPERATE_ERROR );                                            /* ����USB�洢���豸,ֱ�ӷ���  */
    }    
    Diskinfo->InterfNumber = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceNumber;  /* ����ӿں� */
    Diskinfo->BulkInEp  = 0;
    Diskinfo->BulkOutEp = 0;
    for( i = 0; i < 2; i ++ )                                                   /* ֻ����ǰ�����˵� */
    {
        if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bLength == 0x07
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bDescriptorType == 0x05  
            && ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bmAttributes == 2 ) 
        {      
            if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x80 )
            {    
                /* ����IN�˵� */
                Diskinfo->BulkInEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;   /* IN�˵� */
                Diskinfo->BulkInEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
                Diskinfo->BulkInEpSize = Diskinfo->BulkInEpSize << 8;
                Diskinfo->BulkInEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;                
            }
            else 
            {
                Diskinfo->BulkOutEp = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].bEndpointAddress & 0x0F;  /* OUT�˵� */
                Diskinfo->BulkOutEpSize = ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeH;
                Diskinfo->BulkOutEpSize = Diskinfo->BulkOutEpSize << 8;
                Diskinfo->BulkOutEpSize += ( (PUSB_CFG_DESCR_LONG)pbuf ) -> endp_descr[ i ].wMaxPacketSizeL;                
            }
        }
    }
    if( ( (PUSB_CFG_DESCR_LONG)pbuf ) -> itf_descr.bInterfaceClass != 0x08 || Diskinfo->BulkInEp == 0 || Diskinfo->BulkOutEp == 0 )
    {                                                                           /* ����USB�洢���豸,��֧�� */
        return( USB_OPERATE_ERROR );                                            /* ��ֹ����,ֱ�ӷ��� */
    }

#ifdef  MY_DEBUG_PRINTF 
    printf("Diskinfo->BulkInEp:%02x\n ",Diskinfo->BulkInEp);
    printf("Diskinfo->BulkOutEp:%02x\n ",Diskinfo->BulkOutEp);
    printf("Diskinfo->BulkInEpSize:%02x\n ",Diskinfo->BulkInEpSize);
    printf("Diskinfo->BulkOutEpSize:%02x\n ",Diskinfo->BulkOutEpSize);
#endif

    /*******************************����������洢�豸����ڴ�************************/
    status = MS_Bluk_QH_Allocate( Diskinfo );                                   /* USB���������������QH��TD���� */
    if ( status != USB_OPERATE_SUCCESS ) 
    {
        return( status );                                                       /* ��ֹ����,ֱ�ӷ��� */    
    }    
    return( USB_OPERATE_SUCCESS );
}

/*******************************************************************************
* Function Name  : MS_Bluk_QH_Allocate
* Description    : ���������������QH
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 MS_Bluk_QH_Allocate( DISK_INFO *Diskinfo )
{
    UINT32 temp;

    /* ���������������QH(qHD_Bulk_In) */
    if( Diskinfo->BulkInEp )
    {
        temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
        if( temp == USB_MEM_ALLOCATE_ERROR )
        {
            return( USB_MEM_ALLOCATE_ERROR );
        }    
        Diskinfo->qHD_Bulk_In = ( qHD_Structure *)temp;                         /* ����qHD_Structure */
    
        /* ��ʼ�������IN�˵� */
        USBHOST_Allocate_QHD( &Diskinfo->Device, 
                              Diskinfo->qHD_Bulk_In, 
                              EHCI_HD_TYPE_QH, 
                              EHCI_EP_TYPE_NO_CONTROL, 
                              0, 
                              Diskinfo->BulkInEp, 
                              Diskinfo->BulkInEpSize );    
    
        /* ָ����� */    
        USBHOST_qHD_Link_Insert( Diskinfo->qHD_Bulk_In );                       /* ��qHD_Bulk_In���ӵ�QH������ */
    }

    /* ���������������QH(qHD_Bulk_Out) */
    if( Diskinfo->BulkOutEp )
    {
        temp = USBHOST_GetStructure( EHCI_MEM_TYPE_QHD );
        if( temp == USB_MEM_ALLOCATE_ERROR )
        {
            return( USB_MEM_ALLOCATE_ERROR );
        }    
        Diskinfo->qHD_Bulk_Out = ( qHD_Structure *)temp;                        /* ����qHD_Structure */
    
        /* ��ʼ�������OUT�˵� */
        USBHOST_Allocate_QHD( &Diskinfo->Device, 
                              Diskinfo->qHD_Bulk_Out, 
                               EHCI_HD_TYPE_QH, 
                              EHCI_EP_TYPE_NO_CONTROL, 
                              0, 
                              Diskinfo->BulkOutEp, 
                              Diskinfo->BulkOutEpSize );
                
        /* ָ����� */    
        USBHOST_qHD_Link_Insert( Diskinfo->qHD_Bulk_Out );                      /* ��qHD_Bulk_Out���ӵ�QH������ */
    }
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : MS_GetMaxLun_EX
* Description    : USB������ȡ����߼���Ԫ��
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 MS_GetMaxLun_EX( DISK_INFO *Diskinfo ) 
{
    UINT8  status;
    UINT8  buf[ 1 ];  

    /* ���SETUP����� */
    Ctl_Setup->bReqType = 0xA1;
    Ctl_Setup->bRequest = 0xFE;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = 0x00;
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x01;
    Ctl_Setup->wLengthH = 0x00;

    /* ��ȡ��ǰU������߼���Ԫ�� */        
    Diskinfo->MaxLun = 0x00;
    status = USBHOST_Issue_Control( &Diskinfo->Device, Ctl_Setup, buf, NULL );  /* ִ�п��ƴ��� */
    if( status == USB_OPERATE_SUCCESS )
    {
        Diskinfo->MaxLun = buf[ 0 ];                                            /* ���浱ǰU�̵�����߼���Ԫ�� */
    }
    else
    {
        if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                          /* ����STALL */     
        {    
            status = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x00 );    /* ����˵�0 */    
        }                        
    }            
    return( status );
}

/*******************************************************************************
* Function Name  : MS_ResetErrorBOC_EX
* Description    : USB������λU��
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 MS_ResetErrorBOC_EX( DISK_INFO *Diskinfo )
{
    UINT8  status;

    /* ���SETUP����� */
    Ctl_Setup->bReqType = 0x21;
    Ctl_Setup->bRequest = 0xFF;
    Ctl_Setup->wValueL  = 0x00;
    Ctl_Setup->wValueH  = 0x00;
    Ctl_Setup->wIndexL  = Diskinfo->InterfNumber;                               /* �ӿں� */
    Ctl_Setup->wIndexH  = 0x00;
    Ctl_Setup->wLengthL = 0x00;
    Ctl_Setup->wLengthH = 0x00;

    /* USB������λU�� */        
    status = USBHOST_Issue_Control( &Diskinfo->Device, Ctl_Setup, NULL, NULL  );/* ִ�п��ƴ��� */
    if( status == USB_OPERATE_SUCCESS )    
    {
        /* ��������ϡ��´��˵� */
        status = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x80 | Diskinfo->BulkInEp ); /* ��������ϴ��˵� */
        if( status != USB_OPERATE_SUCCESS )    
        {
            return( status );
        }
        status = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, Diskinfo->BulkOutEp );    /* ��������´��˵� */
    }        
    return( status );                                                      
}

/*******************************************************************************
* Function Name  : MS_ScsiCmd_Process_EX
* Description    : ִ�л���BulkOnlyЭ��������
*                  ����һ�ο��Դ������20K����,��δ����ѭ������
*                  ע��: ���ݻ����������4K�߽�����,���δ����,�򻺳���������С����
*                        ��ʵ��Ҫ��д�Ĵ�СҪ��4K�� 
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *DataBuf-----���롢������ݻ�����
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 MS_ScsiCmd_Process_EX( DISK_INFO *Diskinfo, UINT8 *DataBuf )
{
    UINT8  status, s;
    UINT8  *p;
    UINT16 len;
    
#ifdef  MY_DEBUG_PRINTF     
//    printf("MS_ScsiCmd_Process...\n");
#endif

    /* 1�����SCSI�������Ϣ */
    p = DataBuf;
    mBOC.mCBW.mCBW_Sig = USB_BO_CBW_SIG;
    mBOC.mCBW.mCBW_Tag = 0x05630563;
    mBOC.mCBW.mCBW_LUN = Diskinfo->CurLun;                                      /* ������ǰ�߼���Ԫ�� */

    /* 2������CSW�� */
#ifdef  MY_DEBUG_PRINTF     
//    printf("CBW\n");
#endif
    len = USB_BO_CBW_SIZE;
    status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_Out, (UINT8 *)&mBOC.mCBW, &len, EHCI_QTD_PID_OUT );
    if( status == USB_INT_DISCONNECT )                                          /* ����豸�Ͽ�,ֱ�ӷ��� */
    {
        return( status );
    }
    if( status != USB_OPERATE_SUCCESS )
    {
        status = MS_ResetErrorBOC_EX( Diskinfo );
        if( status == USB_INT_DISCONNECT )                                      /* ����豸�Ͽ�,ֱ�ӷ��� */
        {
            return( status );
        }
                        
        /* �ٴη���CBW�� */
        len = USB_BO_CBW_SIZE;
        status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_Out, (UINT8 *)&mBOC.mCBW, &len, EHCI_QTD_PID_OUT );            
        if( status == USB_INT_DISCONNECT )                                      /* ����豸�Ͽ�,ֱ�ӷ��� */
        {
            return( status );
        }
        if( status != USB_OPERATE_SUCCESS )
        {
            return( MS_ResetErrorBOC_EX( Diskinfo ) );
        }    
    }

    /* 3���������ݰ��������ݰ� */    
    if( ( mBOC.mCBW.mCBW_DataLen > 0 ) && ( mBOC.mCBW.mCBW_Flag == USB_BO_DATA_IN ) )
    {        
#ifdef  MY_DEBUG_PRINTF     
//        printf("In\n");
#endif        
        /* �����������Ҫ�ϴ�,����IN���ƽ������ݶ�ȡ */    
        if( mBOC.mCBW.mCBW_DataLen > DEFAULT_MAX_OPERATE_SIZE )
        {
            return( USB_PARAMETER_ERROR );                                      /* �������� */
        }    
        
        /* ���������ϴ�IN */
        len = mBOC.mCBW.mCBW_DataLen;
        status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_In, p, &len, EHCI_QTD_PID_IN );                
        if( status == USB_INT_DISCONNECT )                                      /* ����豸�Ͽ�,ֱ�ӷ��� */
        {
            return( status );
        }
        if( status != USB_OPERATE_SUCCESS )
        {    
            if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                      /* ����STALL���� */
            {
                s = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x80 | Diskinfo->BulkInEp );/* ����ϴ��˵� */
                if( s == USB_INT_DISCONNECT )                                   /* ����豸�Ͽ�,ֱ�ӷ��� */
                {
                    return( s );
                }                                    
            }    
        }    
    }
    else if( ( mBOC.mCBW.mCBW_DataLen > 0 ) && ( mBOC.mCBW.mCBW_Flag == USB_BO_DATA_OUT ) )
    {
#ifdef  MY_DEBUG_PRINTF     
//        printf("Out\n");
#endif                
        /* �����������Ҫ�´�,����OUT���ƽ������ݶ�ȡ */    
        if( mBOC.mCBW.mCBW_DataLen > DEFAULT_MAX_OPERATE_SIZE )
        {
            return( USB_PARAMETER_ERROR );                                      /* �������� */
        }    

        /* ���������´�OUT */
        len = mBOC.mCBW.mCBW_DataLen;
        status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_Out, p, &len, EHCI_QTD_PID_OUT );        
        if( status == USB_INT_DISCONNECT )                                      /* ����豸�Ͽ�,ֱ�ӷ��� */
        {
            return( status );
        }
        if( status != USB_OPERATE_SUCCESS )
        {    
            if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                      /* ����STALL���� */
            {
                s = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, Diskinfo->BulkOutEp );/* ����´��˵� */
                if( s == USB_INT_DISCONNECT )                                   /* ����豸�Ͽ�,ֱ�ӷ��� */
                {
                    return( s );
                }
            }
        }
    }    

#ifdef  MY_DEBUG_PRINTF     
//    printf("CSW\n");
#endif
    /* 4������CSW�� */
    len = USB_BO_CSW_SIZE;
    status = USBHOST_Issue_Bulk( &Diskinfo->Device, Diskinfo->qHD_Bulk_In, (UINT8 *)&mBOC.mCSW, &len, EHCI_QTD_PID_IN );    
    if( status == USB_OPERATE_SUCCESS )
    {
        if( len != 0x0D )                                                       /* �жϳ����Ƿ�Ϊ13���ֽ� */
//        if( ( len != 0x00 ) || ( mBOC.mCSW.mCSW_Sig != USB_BO_CSW_SIG ) )     /* ��ЩU�̿��ܲ��������־���� */ 
        {    
            return( USB_INT_DISK_ERR );
        }
        if( mBOC.mCSW.mCSW_Status == 0 ) 
        {
            return( USB_OPERATE_SUCCESS );
        }
        else if( mBOC.mCSW.mCSW_Status >= 2 ) 
        {    
            return( MS_ResetErrorBOC_EX( Diskinfo ) );
        }
        else 
        {
            return( USB_INT_DISK_ERR );                                         /* ���̲������� */
        }            
    }    
    else if( status == USB_INT_DISCONNECT )                                     /* ����豸�Ͽ�,ֱ�ӷ��� */
    {
        return( status );
    }
    else
    {
        /* �ж���һ�����Ĵ��� */
        /* λ6:Halted; λ5:Data Buffer Error; λ4: Babble Detectd; λ3: Transaction Error; λ2��Missed Micri-fRAME */
        if( gEHCILastStatus & EHCI_QTD_STATUS_HALTED )                          /* ����STALL���� */
        {
            s = USBHOST_EHCI_ClearEndpStall( &Diskinfo->Device, 0x80 | Diskinfo->BulkInEp );    
            if( s == USB_INT_DISCONNECT )                                       /* ����豸�Ͽ�,ֱ�ӷ��� */
            {
                return( s );
            }
        }        
    }    
    return( USB_INT_DISK_ERR ); 
}

/*******************************************************************************
* Function Name  : MS_RequestSense_EX
* Description    : USB���������̴���״̬
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *pbuf------���ݻ�����
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 MS_RequestSense_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    UINT8  status;
    
#ifdef  MY_DEBUG_PRINTF
    printf( "MS_RequestSense:...\n" );
#endif
    
    Delay_ms( 10 );                                                             /* ��ʱ10���� */    
    status = USBHOST_EHCI_Detect_Connect( &Diskinfo->Device );    
    if( ( status == USB_INT_CONNECT ) || ( status == USB_INT_DISCONNECT ) || ( status == 0 ) )
    {
        return( status );
    }    
    mBOC.mCBW.mCBW_DataLen        = 0x00000012;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len        = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x03;                                          /* ������ */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x12;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* ִ�л���BulkOnlyЭ������� */
}

/*******************************************************************************
* Function Name  : MS_DiskInquiry_EX
* Description    : USB������ȡ��������
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *pbuf------���ݻ�����
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 MS_DiskInquiry_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    mBOC.mCBW.mCBW_DataLen        = 0x00000024;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len        = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x12;                                          /* ������ */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x24;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* ִ�л���BulkOnlyЭ������� */
}

/*******************************************************************************
* Function Name  : MS_DiskCapacity_EX
* Description    : USB������ȡ��������
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *pbuf------���ݻ�����
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 MS_DiskCapacity_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    mBOC.mCBW.mCBW_DataLen     = 0x00000008;
    mBOC.mCBW.mCBW_Flag        = 0x80;
    mBOC.mCBW.mCBW_CB_Len      = 10;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x25;                                          /* ������ */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 8 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* ִ�л���BulkOnlyЭ������� */
}

/*******************************************************************************
* Function Name  : MS_DiskTestReady_EX
* Description    : USB�������Դ����Ƿ����
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *pbuf------���ݻ����� 
* Output         : None
* Return         : ���ص�ǰ����ִ��״̬
*******************************************************************************/
UINT8 MS_DiskTestReady_EX( DISK_INFO *Diskinfo, UINT8 *pbuf )
{
    mBOC.mCBW.mCBW_DataLen        = 0x00;
    mBOC.mCBW.mCBW_Flag        = 0x00;
    mBOC.mCBW.mCBW_CB_Len      = 0x06;
    mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x00;                                          /* ������ */
    mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 2 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 3 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 4 ] = 0x00;
    mBOC.mCBW.mCBW_CB_Buf[ 5 ] = 0x00;
    return( MS_ScsiCmd_Process_EX( Diskinfo, pbuf ) );                          /* ִ�л���BulkOnlyЭ������� */
}

/*******************************************************************************
* Function Name  : MS_Init_EX
* Description    : USB�������洢�豸��ʼ�� 
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  *pbuf------���ݻ����� 
* Output         : None
* Return         : USB_OPERATE_SUCCESS---��ʼ���ɹ�;
*                  USB_OPERATE_ERROR--��ʼ��ʧ��;
*******************************************************************************/
UINT8 MS_Init_EX( DISK_INFO *Diskinfo, UINT8 *pbuf ) 
{
    UINT8  count, status;        
    UINT16 i;
    
    /*******************************��ȡ����߼���Ԫ��*************************/
#ifdef  MY_DEBUG_PRINTF
    printf( "MS_GetMaxLun...\n" );
#endif
    status = MS_GetMaxLun_EX( Diskinfo );
    if ( status != USB_OPERATE_SUCCESS ) 
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "ERROR = %02X\n", (UINT16)status );
#endif
        Diskinfo->MaxLun = 0x00;                                                /* ��ЩU�̿��ܲ�֧��,����֮��,ȡĬ��ֵ */
    }
    Delay_ms( 3 );

    /*******************************��ȡU����Ϣ(INQUIRY)***********************/
    /* �жϵ�ǰ�߼���Ԫ�Ƿ���CD-ROM */
    for( count = 0; count < Diskinfo->MaxLun + 1; count++ )
    {
#ifdef  MY_DEBUG_PRINTF
        printf( "Disk Inquiry:...\n" );
#endif
        status = MS_DiskInquiry_EX( Diskinfo, pbuf );                           /* ��ȡ�������� */
        if ( status != USB_OPERATE_SUCCESS ) 
        {
#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02X\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                                  /* ����豸�Ͽ�,ֱ�ӷ��� */
            {
                return( status );
            }
            return( USB_OPERATE_ERROR );                                        /* ��ֹ����,ֱ�ӷ��� */
        }
    
#ifdef  MY_DEBUG_PRINTF
        for ( i = 8; i < 36; i ++ ) 
        {
            printf( "%c", *( pbuf + i ) );
        }
        printf( "\n" );
#endif
        if( ( Diskinfo->MaxLun == 0x00 ) && ( *( pbuf + 0 ) == 0x05 ) )        
        {
            /* ��ǰU��ֻ��1���߼���,����CD-ROM ,����ֹ����,ֱ�ӷ��� */
            return( USB_OPERATE_ERROR );                                                      
        }
        if( *( pbuf + 0 ) == 0x05 ) 
        {
            Diskinfo->CurLun++;
            Delay_ms( 3 );
            continue;                                                           /* ���� */
        }
        else
        {
            break;
        }        
    }

    /*******************************��ȡU������********************************/
    for( count = 0; count < 5; count++ )
    {
        /* ע�⣺������ЩU��,��һ�γ���֮��,�ڶ��β�������ȴ��㹻��ʱ��֮���ٲ���,�����һֱ����NAK */
        Delay_ms( 200 );
        Delay_ms( 100 * count );                                               
#ifdef  MY_DEBUG_PRINTF
        printf( "Disk Capacity:...\n" );
#endif
        status = MS_DiskCapacity_EX( Diskinfo, pbuf );                          /* ��ȡ�������� */
        if ( status != USB_OPERATE_SUCCESS ) 
        {
#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02x\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                                  /* ����豸�Ͽ�,ֱ�ӷ��� */
            {
                return( status );
            }
            MS_RequestSense_EX( Diskinfo, pbuf );                               /* �������,����еͲ�������� */
        }
        else 
        {            
            /* ���浱ǰ������С(�ֿ������ֹ����������) */            
            Diskinfo->PerSecSize = ( ( (UINT32)( *( pbuf + 4 ) ) ) << 24 );
            Diskinfo->PerSecSize |= ( ( (UINT32)( *( pbuf + 5 ) ) ) << 16 );
            Diskinfo->PerSecSize |= ( ( (UINT32)( *( pbuf + 6 ) ) ) << 8 ) + ( *( pbuf + 7 ) );

            /* ���浱ǰ������Ŀ(�ֿ������ֹ����������) */            
            Diskinfo->Capability = ( ( (UINT32)( *( pbuf + 0 ) ) ) << 24 );
            Diskinfo->Capability |= ( ( (UINT32)( *( pbuf + 1 ) ) ) << 16 );
            Diskinfo->Capability |= ( ( (UINT32)( *( pbuf + 2 ) ) ) << 8 ) + ( *( pbuf + 3 ) );

#ifdef  MY_DEBUG_PRINTF
            printf("Diskinfo->PerSecSize: %04x\n",(UINT32)Diskinfo->PerSecSize);
            printf("Diskinfo->Capability: %08lx\n",Diskinfo->Capability);
#endif            
            break;
        }
    }

    /***************************����U���Ƿ�׼����******************************/
    for( count = 0; count < 5; count ++ )
    {
        Delay_ms( 50 );

#ifdef  MY_DEBUG_PRINTF
        printf( "Disk Ready:...\n" );
#endif
        status = MS_DiskTestReady_EX( Diskinfo, pbuf );                         /* ���Դ����Ƿ���� */
        if ( status != USB_OPERATE_SUCCESS ) 
        {

#ifdef  MY_DEBUG_PRINTF
            printf( "ERROR = %02X\n", (UINT16)status );
#endif
            if( status == USB_INT_DISCONNECT )                                  /* ����豸�Ͽ�,ֱ�ӷ��� */
            {
                return( status );
            }
            MS_RequestSense_EX( Diskinfo, pbuf );                               /* �������,����еͲ�������� */
        }
        else 
        {
            break;            
        }
    }
    return( USB_OPERATE_SUCCESS );    
}

/*******************************************************************************
* Function Name  : MS_ReadSector_EX
* Description    : ������Ϊ��λ�Ӵ��̶�ȡ����
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  StartLba----��ʼ������
*                  SectCount---��Ҫ��ȡ��������Ŀ
*                  DataBuf-----���ݻ����� 
* Output         : None
* Return         : ִ��״̬
*******************************************************************************/
UINT8 MS_ReadSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf )
{
    UINT8  err, s;
    UINT32 len;

#ifdef  MY_DEBUG_PRINTF
//    printf( "MS_ReadSector:...\n" );
#endif    
    len = SectCount * Diskinfo->PerSecSize;                                     /* �����ȡ�ܳ��� */ 
    if( len > DEFAULT_MAX_OPERATE_SIZE )
    {
        return( USB_PARAMETER_ERROR );                                          /* �������� */        
    }    
    for( err = 0; err < 3; err ++ )                                             /* �������� */
    {      
        mBOC.mCBW.mCBW_DataLen = len;    
        mBOC.mCBW.mCBW_Flag = 0x80;
        mBOC.mCBW.mCBW_CB_Len = 10;
        mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x28;                                      /* ������ */
        mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
        mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
        mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
        mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
        mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
        mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
        s = MS_ScsiCmd_Process_EX( Diskinfo, DataBuf );                          /* ִ�л���BulkOnlyЭ������� */        
        if( s == USB_OPERATE_SUCCESS ) 
        {
            return( USB_OPERATE_SUCCESS );                                      /* �����ɹ� */
        }
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  ) 
        {
            return( s );                                                        /* ��⵽USB�豸�Ͽ��¼�,�����Ѿ��Ͽ����߸ո����²��� */
        }
        s = MS_RequestSense_EX( Diskinfo, DataBuf );
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  ) 
        {
            return( s );                                                          
        }
    }
    return( s );                                                                /* ���ش��� */
}

/*******************************************************************************
* Function Name  : MS_WriteSector_EX
* Description    : ������Ϊ��λ������д�����
* Input          : *Diskinfo----��ǰ������U����ؽṹ��
*                  StartLba----��ʼ������
*                  SectCount---��Ҫд���������Ŀ
*                  DataBuf-----���ݻ�����
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 MS_WriteSector_EX( DISK_INFO *Diskinfo, UINT32 StartLba, UINT8 SectCount, PUINT8 DataBuf )
{
    UINT8  err, s;
    UINT32 len;

#ifdef  MY_DEBUG_PRINTF
//    printf( "MS_WriteSector:...\n" );
#endif        
    len = SectCount * Diskinfo->PerSecSize;                                     /* �����ȡ�ܳ��� */
    if( len > DEFAULT_MAX_OPERATE_SIZE )
    {
        return( USB_PARAMETER_ERROR );                                          /* �������� */        
    }    
    for( err = 0; err < 3; err++ )                                              /* �������� */
    {      
        mBOC.mCBW.mCBW_DataLen = len;
        mBOC.mCBW.mCBW_Flag = 0x00;
        mBOC.mCBW.mCBW_CB_Len = 10;
        mBOC.mCBW.mCBW_CB_Buf[ 0 ] = 0x2A;                                      /* ������ */
        mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
        mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
        mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
        mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
        mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
        mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
        mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
        s = MS_ScsiCmd_Process_EX( Diskinfo, DataBuf );                         /* ִ�л���BulkOnlyЭ������� */
        if( s == USB_OPERATE_SUCCESS ) 
        {
            return( USB_OPERATE_SUCCESS );                                      /* �����ɹ� */
        }
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT ) 
        {
            return( s );                                                        /* ��⵽USB�豸�Ͽ��¼�,�����Ѿ��Ͽ����߸ո����²��� */
        }
        s = MS_RequestSense_EX( Diskinfo, DataBuf );
        if( s == USB_INT_DISCONNECT || s == USB_INT_CONNECT  ) 
        {
            return( s );                                                          
        }        
    }
    return( s );                                                                /* ���ش��� */
}