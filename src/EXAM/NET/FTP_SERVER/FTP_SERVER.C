/********************************** (C) COPYRIGHT ******************************
* File Name          : FTP_SERVER.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563оƬFTP�ӿ�����
*                    : MDK3.36@ARM966E-S,Thumb,С��
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "FTPINC.H"
#include "FTPCMD.C"
#include "FTPFILE.C"

/*******************************************************************************
* Function Name  : CH563NET_FTPSendData
* Description    : ׼����Ҫ���͵�����
* Input          : pName-�ļ���
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPDataReady( char *pName )
{
    ftp.CmdDataS = FTP_MACH_SENDDATA;                                           /* ���ݴ���״̬��Ϊ�������� */
    CH563NET_FTPFileRead( pName );                                              /* ��Ҫ���͵�����д�뷢�ͻ����� */
}

/*******************************************************************************
* Function Name  : CH563NET_FTPHandleDatRece
* Description    : �������յ�������
* Input          : recv_buff -������Ϣ
                   sockeid   -socket����
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPHandleDatRece( char *recv_buff,UINT8 sockeid )
{
    if(sockeid == ftp.SocketDatConnect){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                                  /* �����ļ����� */
            CH563NET_FTPFileWrite(recv_buff,(UINT16)strlen(recv_buff));
        }
    }
    if(sockeid == ftp.SocketCtlConnect){                                        /* �������� */            
        CH563NET_FTPCmdRespond(recv_buff);                                      /* ������Ӧ������ */
    }    
}

/*******************************************************************************
* Function Name  : CH563NET_FTPServerCmd
* Description    : ��ѯ״̬��ִ����Ӧ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPServerCmd( void )
{
    UINT8 i;

    if(ftp.TcpCtlSta == FTP_MACH_CONNECT){                                      /* ����TCP FTP�������� */
        ftp.TcpCtlSta = FTP_MACH_KEPCONT; 
        CH563NET_FTPSendData( (char *)FTP_SERVICE_CMD[0],strlen(FTP_SERVICE_CMD[0]),ftp.SocketCtlConnect );
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){                                      /* ���ݴ������ */
        ftp.CmdDataS = 0;    
        CH563NET_FTPSendData( (char *)FTP_SERVICE_CMD[1],strlen(FTP_SERVICE_CMD[1]),ftp.SocketCtlConnect );
        if( ftp.SocketDatMonitor != 255 ){                                      /* �ر��������ӣ�TCP Server��*/
            i = CH563NET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
            mStopIfError(i); 
            ftp.SocketDatMonitor = 255;
        }
    }
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){                                      /* �������� */
        if(ftp.TcpDatSta >= FTP_MACH_CONNECT){ 
            ftp.TcpDatSta = FTP_MACH_KEPCONT;
            if(strlen(SendBuf))CH563NET_FTPSendData( SendBuf,strlen(SendBuf),ftp.SocketDatConnect );
            if(ftp.DataOver ){                                                  /* ������� */
                ftp.CmdDataS = FTP_MACH_DATAOVER; 
                i = CH563NET_SocketClose( ftp.SocketDatConnect,TCP_CLOSE_NORMAL );/* �ر��������� */
                mStopIfError(i);
            }
        }
    }
    if(ftp.CmdReceDatS == 1){                                                   /* ��Ҫ�������� */
        if(ftp.TcpDatSta == FTP_MACH_CONNECT){
            ftp.CmdReceDatS = 0;
            ftp.CmdDataS = FTP_MACH_RECEDATA;                                   /* ���ݴ���״̬��Ϊ�������� */
        }
    }
    if(ftp.TcpDatSta == FTP_MACH_DISCONT){                                      /* ����������� */
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){
            ftp.CmdDataS = FTP_MACH_DATAOVER;
            CH563NET_FTPListRenew( ftp.ListFlag );                              /* ����Ŀ¼ */
        }
    } 
    if((ftp.TimeCount>20)&&(ftp.SocketDatMonitor != 255||ftp.SocketCtlConnect != 255)){
        if(ftp.SocketDatMonitor != 255) CH563NET_SocketClose( ftp.SocketDatMonitor,TCP_CLOSE_NORMAL );
        if(ftp.SocketCtlConnect != 255) CH563NET_SocketClose( ftp.SocketCtlConnect,TCP_CLOSE_NORMAL );
    }
}

/*********************************** endfile **********************************/