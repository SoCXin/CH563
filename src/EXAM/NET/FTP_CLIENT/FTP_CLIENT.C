/********************************** (C) COPYRIGHT ******************************
* File Name          : FTP_CLIENT.C
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
* Function Name  : CH563NET_FTPProcessReceDat
* Description    : ��ȡ��������   
* Input          : recv_buff  -563���յ�������
*                  check_type -��ǰ��������
*                  socketid   -socket����
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPProcessReceDat( char *recv_buff,UINT8 check_type,UINT8 socketid )
{
    UINT8 S;
    if( socketid == ftp.DatTransfer ){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                                  /* �����ļ����� */
            if( ftp.InterCmdS == FTP_MACH_GETFILE ){ 
                S = CH563NET_FTPFileWrite(recv_buff,strlen(recv_buff));
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDLIST ){
                S = CH563NET_FTPFindList( recv_buff );                          /* ��֤�������ݣ����ڲ���ָ����Ŀ¼�� */
                if( S == FTP_CHECK_SUCCESS ) ftp.FindList = 1;                  /* �鵽ָ����Ŀ¼�� */
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDFILE ){
                S = CH563NET_FTPFindFile( recv_buff );                          /* �����ļ� */    
                if( S == FTP_CHECK_SUCCESS ) ftp.FindFile = 1;                  /* �ҵ��ļ� */
            }
        }
    }
    else if( socketid == ftp.SocketCtl ){                                       /* ����Ϊ����Ӧ�� */            
        S = CH563NET_FTPCheckRespond(recv_buff,check_type);
    }
}

/*******************************************************************************
* Function Name  : CH395_FTPSendFile
* Description    : ��������
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPSendFile( void )
{
    UINT8 S;    
    
    S = CH563NET_FTPFileOpen( FileName );
    if(S == FTP_CHECK_SUCCESS)    CH563NET_FTPFileRead( );
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.DatTransfer );
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){
        CH563NET_SocketClose( ftp.DatTransfer,TCP_CLOSE_NORMAL );
    }    
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckRespond
* Description    : ��ѯ״̬��ִ����Ӧ����
* Input          : recv_buff  -������Ϣ
                   check_type -��������
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPClientCmd( void )
{
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){
        if(ftp.TcpStatus == FTP_MACH_CONNECT) CH563NET_FTPSendFile( );          /* ��������������� */
        return ;     
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){
        if(ftp.FindList == 1){                                                  /* �ҵ�Ŀ¼�� */
            ftp.FindList = 0;
            CH563NET_FTPCwd( 0 );
        }
    }
    if(ftp.FileCmd){
        CH563NET_FTPInterCmd( );                                                /* ִ�ж�Ӧ�Ľӿ����� */
    }
}

/*******************************************************************************
* Function Name  : CH563NET_FTPInterCmd
* Description    : ִ�ж�Ӧ����������е���ִ��˳��
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPInterCmd( void )
{
    switch(ftp.FileCmd){
        case FTP_CMD_LOGIN:                                                     /* ��½ */
            if( CH563NET_FTPLogin( ) == FTP_COMMAND_SUCCESS ){                  /* ��½�ɹ�,�ɽ����������� */
                CH563NET_FTPSearch("USER","FILELIST.txt" );                     /* ��ʼ��ѯָ����Ŀ¼ */            
            }
            break;
        case FTP_CMD_SEARCH:                                                    /* �Ѳ��ļ���������Ŀ¼�����ļ�����*/
            if( CH563NET_FTPSearch("USER","FILELIST.txt" ) == FTP_COMMAND_SUCCESS ){        
                if( ftp.FindFile )CH563NET_FTPGetFile("FILELIST.txt" );         /* �Ѳ鵽ָ����Ŀ¼�µ��ļ���ʼ�����ļ� */
                else CH563NET_FTPQuit( );                                       /* û�Ѳ鵽ָ����Ŀ¼�µ��ļ����˳���Ҳ�ɽ����������������ϴ���*/
            }
            break;
        case FTP_CMD_GETFILE:                                                   /* �����ļ����������ļ�����*/ 
            if(CH563NET_FTPGetFile("FILELIST.txt" ) == FTP_COMMAND_SUCCESS ){   /* �����ļ��ɹ����ɽ����������� */
                CH563NET_FTPPutFile("TEXT","abc.txt");                          /* �ϴ��ļ� */
            }
            break;
        case FTP_CMD_PUTFILE:                                                   /* �ϴ��ļ���������Ŀ¼�����ļ�����*/
            if( CH563NET_FTPPutFile("TEXT","abc.txt")== FTP_COMMAND_SUCCESS ){  /* �ϴ��ļ��ɹ����ɽ����������� */
                CH563NET_FTPQuit( );                                            /* �˳� */
            }
            break;
        default:
            break;
    }
}

/*********************************** endfile **********************************/