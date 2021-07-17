/********************************** (C) COPYRIGHT *********************************
* File Name          : FTPCMD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563оƬFTP�������
*                    : MDK3.36@ARM966E-S,Thumb,С��
**********************************************************************************/



/******************************************************************************/
FTP   ftp;
char  send_buff[536];    // �������ݻ�����
UINT32 ListNum = 0;

/*******************************************************************************
* Function Name  : CH563NET_FTPLoginUser
* Description    : ��֤�û���   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPLoginUser( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "USER %s\r\n", pUserName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPLoginPass
* Description    : ��֤����   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPLoginPass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "PASS %s\r\n", pPassword);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPSyst
* Description    : ��ѯ������ϵͳ����   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPSyst( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "SYST\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPList
* Description    : ��ȡ�ļ��б����ļ�����ʱ��   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPList( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "LIST\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPNlst
* Description    : ��ȡ�ļ��б�   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPNlst( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "NLST\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPType
* Description    : �����ļ���������   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPType( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "TYPE A\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPSize
* Description    : ��ȡ�ļ���Ϣ   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPSize( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "SIZE %s\r\n",FileName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPRetr
* Description    : �����ļ�   
* Input          : None
* Output         : None                      
* Return         : None
*******************************************************************************/
void CH563NET_FTPRetr( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "RETR %s\r\n",FileName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCdup
* Description    : ������һ��Ŀ¼   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPCdup( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "CDUP\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPMkd
* Description    : ����Ŀ¼   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPMkd(UINT8 ReName )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    if(ReName){
        ListNum++;
        sprintf( send_buff, "MKD %s%d\r\n",ListName,ListNum);
    }
    else sprintf( send_buff, "MKD %s\r\n",ListName);
    sprintf( ListMkd,&send_buff[4] );                                            /* ����Ŀ¼�� */
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPRmd
* Description    : ɾ��Ŀ¼   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPRmd( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "RMD %s\r\n",ListName); 
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPAppe
* Description    : �ϴ��ļ�   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPAppe( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "APPE %s\r\n",FileName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPAllo
* Description    : �����ļ�   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPAllo( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "ALLO 500000\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPNoop
* Description    : ���Ϳ�ָ��   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPNoop( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "NOOP\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPPasv
* Description    : ������ȡ���Ӷ˿ں�   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPPasv( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "PASV\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl);
}

/*******************************************************************************
* Function Name  : CH563NET_FTPPort
* Description    : �趨���Ӷ˿ں�   
* Input          : port - �˿ں�
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPPort( UINT16 port)
{
    memset( send_buff, '\0', sizeof(send_buff));
    sprintf( send_buff, "PORT %s%d,%d\r\n",CharIP,(UINT8)(port/256),(UINT8)(port%256));
#if DEBUG
    printf("PORT :%s\n", send_buff);
#endif
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCwd
* Description    : ��Ŀ¼   
* Input          : index -0Ϊ��ǰĿ¼��1Ϊ��һ��Ŀ¼
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPCwd( UINT8 index )
{                                                     
    memset( send_buff, '\0', sizeof(send_buff) );
    if(index==1) sprintf( send_buff, "CWD /%s\r\n",ListMkd);
    else sprintf( send_buff, "CWD /%s\r\n",ListName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPQuit
* Description    : �˳���¼   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPQuit( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "QUIT\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckLogin
* Description    : ��½�����ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPCheckLogin( char *recv_buff )
{
    if( strncmp("220", recv_buff, 3) == 0 ){                                    /* ���ӳɹ� */
        CH563NET_FTPLoginUser( );
    }
    else if( strncmp("331", recv_buff, 3) == 0 ){                               /* �û�����ȷ */
        CH563NET_FTPLoginPass( );                   
    }
    else if( strncmp("230", recv_buff, 3) == 0 ){                               /* ��¼�ɹ� */
        ftp.CmdStatus = FTP_MACH_LOGINSUC;
    }
    else if( strncmp("530", recv_buff, 3) == 0 ){                               /* ��½ʧ�� */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* �˳���½ */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* �ر����� */
    }
    else return FTP_ERR_LOGIN;
    return FTP_CHECK_SUCCESS; 
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckSearch
* Description    : �Ѳ��ļ������ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPCheckSearch( char *recv_buff )
{
    if( strncmp("200 Type", recv_buff, 8) == 0 ){                               /* ��ʽ���� */ 
        CH563NET_FTPDatServer( &ftp.SouPort );
        CH563NET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 PORT", recv_buff, 8 ) == 0){                          /* Ŀ��IP��ַ���˿ں� */ 
        CH563NET_FTPList( );
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               /* ��ȡ�б��ɹ� */
        ftp.CmdDataS = FTP_MACH_RECEDATA;
    }
    else if( strncmp("226", recv_buff, 3) == 0 ){                               /* �б���Ϣ���� */
        if(ftp.InterCmdS == FTP_MACH_FINDFILE) ftp.CmdStatus = FTP_MACH_SEARCHSUC;
        ftp.InterCmdS = 0;
    }
    else if( strncmp("425", recv_buff, 3) == 0 ){                               /* �޷������������� */
        CH563NET_FTPQuit( );
    }                                                            
    else if( strncmp("250", recv_buff, 3) == 0 ){                               /* ��Ŀ¼ */
        ftp.InterCmdS = FTP_MACH_FINDFILE;
        CH563NET_FTPType( );
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* �˳���½ */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* �ر����� */
    }
    else return FTP_ERR_SEARCH;
    return FTP_CHECK_SUCCESS;
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckGetfile
* Description    : �����ļ������ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPCheckGetfile( char *recv_buff )
{            
    if( strncmp("200 Type", recv_buff, 8) == 0 ){                               /* ��ʽ���� */ 
        CH563NET_FTPSize( );
    }
    else if( strncmp("213", recv_buff, 3) == 0 ){                               /* �ļ���Ϣ */ 
        CH563NET_FTPDatServer( &ftp.SouPort );
        CH563NET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 PORT", recv_buff, 8) == 0 ){                          /* Ŀ��IP��ַ���˿ں� */ 
        CH563NET_FTPRetr( );
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               /* ��ȡ�ļ�����ɹ� */ 
        ftp.CmdDataS = FTP_MACH_RECEDATA;
        memset( send_buff, '\0', sizeof(send_buff) );
    }
    else if(strncmp("550", recv_buff, 3) == 0 ){                                /* û���ҵ��ļ� */
        CH563NET_FTPQuit( );
    }
    else if(strncmp("226", recv_buff,3) == 0 ){                                 /* ������� */
        ftp.CmdStatus = FTP_MACH_GETSUC;                     
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* �˳���½ */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* �ر����� */
    }
    else return FTP_ERR_GETFILE;
    return (FTP_CHECK_SUCCESS);

}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckPutfile
* Description    : ��֤�ϴ��ļ������ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPCheckPutfile( char *recv_buff )
{
    if( strncmp("250", recv_buff, 3) == 0 ){                                    /* ������һ��Ŀ¼ */
        if(ftp.InterCmdS == FTP_MACH_MKDLIST){
            CH563NET_FTPMkd( 0 );
        }
        else if(ftp.InterCmdS == FTP_MACH_PUTFILE){
            CH563NET_FTPType( );
        }
    }
    else if( strncmp("257", recv_buff, 3) == 0 ){                               /* �����ɹ� */
        ftp.InterCmdS = FTP_MACH_PUTFILE;                                       /* �ϴ��ļ� */
        CH563NET_FTPCwd( 1 );                                                   /* ��Ŀ¼ */
    }
    else if( strncmp("550", recv_buff, 3) == 0 ){                               /* ������Ŀ¼������ */
#if    1
        CH563NET_FTPMkd( 1 );                                                   /* ��Ŀ¼�������򴴽�����һ��Ŀ¼������ԭ��Ŀ¼�����ۼ������磺TXET1��*/
#else
        CH563NET_FTPCwd( 0 );                                                   /* ��Ŀ¼��������ֱ�Ӵ� */
#endif                                          
    }
    if( strncmp("200 Type", recv_buff, 8) == 0 ){                               /* ��ʽ���� */ 
        CH563NET_FTPDatServer( &ftp.SouPort );
        CH563NET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 PORT", recv_buff, 8 ) == 0){                          /* Ŀ��IP��ַ���˿ں� */
        CH563NET_FTPAppe( );                                    
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               /* �����ϴ��ļ�����ɹ� */
        ftp.CmdDataS = FTP_MACH_SENDDATA;                                       /* ��Ҫ�������� */
    }
    else if( strncmp("226", recv_buff, 3) == 0 ){                               /* �ϴ����� */
        ftp.CmdStatus = FTP_MACH_PUTSUC;             
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* �˳���½ */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* �ر����� */
    }
    else return FTP_ERR_PUTFILE;
    return (FTP_CHECK_SUCCESS);
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckRespond
* Description    : ��֤�����ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPCheckRespond( char *recv_buff,UINT8 check_type )
{
    UINT8 s;
    
    if( strncmp("421", recv_buff, 3) == 0 ){                                    /* ����������Ͽ� */
        return (FTP_CHECK_SUCCESS);
    }
    switch(check_type){
        case FTP_CMD_LOGIN:                                                     /* �˶Ե�½��������еķ�����Ϣ */
            s = CH563NET_FTPCheckLogin( recv_buff );
            return s;
        case FTP_CMD_SEARCH:                                                    /* �˶��Ѳ��ļ������еķ�����Ϣ */
            s = CH563NET_FTPCheckSearch( recv_buff );
            return s;
        case FTP_CMD_GETFILE:                                                    /* �˶������ļ������еķ�����Ϣ */
            s = CH563NET_FTPCheckGetfile( recv_buff );
            return s;
        case FTP_CMD_PUTFILE:                                                    /* �˶��ϴ��ļ������еķ�����Ϣ */
            s = CH563NET_FTPCheckPutfile( recv_buff );
            return s;
        default:
            return FTP_ERR_UNKW;
    }
}

/*******************************************************************************
* Function Name  : CH563NET_FTPFindList
* Description    : �˶�Ŀ¼����
* Input          : None
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPFindList( char *pReceiveData ) 
{
    UINT32 len,i,j;

    len = ftp.RecDatLen;
    for(i=0;i<len;i++){
        if(strncmp(ListName,&pReceiveData[i],strlen(ListName))==0){
            j = i;
            while(strncmp("<DIR>",&pReceiveData[j],strlen("<DIR>"))){
                j--;
                if( pReceiveData[j]== 'M' ) return FALSE;
            }
#if DEBUG
    printf("*********\nfind list\n*********\n");
#endif
            return FTP_CHECK_SUCCESS;                                            /* �ҵ���ָ�����ļ��� */
        } 
    }    
    return FTP_ERR_UNLIST;
}

/*******************************************************************************
* Function Name  : CH563NET_FTPFindFile
* Description    : �˶��ļ�����
* Input          : None
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPFindFile( char *pReceiveData ) 
{
    UINT32 len,i;
    len = ftp.RecDatLen;
    for(i=0;i<len;i++){
        if(strncmp(FileName,&pReceiveData[i],strlen(FileName))==0){
#if DEBUG
    printf("*********\nfind file\n*********\n");
#endif
            return FTP_CHECK_SUCCESS;                                            /* �ҵ���ָ�����ļ��� */
        } 
    }    
    return FTP_ERR_UNFILE;
}

/*******************************************************************************
* Function Name  : ch563ftp_loginstart
* Description    : ��½
* Input          : None
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPLogin( void )
{
    if(ftp.CmdFirtS == 0){
        ftp.CmdFirtS  = 1;
        ftp.FileCmd = FTP_CMD_LOGIN;                                             /* �����Ѳ��ļ�״̬ */
        CH563NET_FTPCtlClient( );
    }
    if( ftp.CmdStatus == FTP_MACH_LOGINSUC ){                                    /* ��½�ɹ� */
#if DEBUG
    printf("************\nlogin success\n*********\n");
#endif
        ftp.CmdFirtS = 0;
        ftp.CmdStatus = 0;
        ftp.FileCmd = 0;      
        return FTP_COMMAND_SUCCESS;    
    }    
    return FTP_COMMAND_CONTINUE;    
}

/*******************************************************************************
* Function Name  : ch563ftp_searchfile
* Description    : �Ѳ��ļ�
* Input          : pListNAme - ��Ҫ�Ѳ���ļ�����Ŀ¼��
                   pFileName - ��Ŀ¼�µ��ļ���
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPSearch(char *pListNAme,char *pFileName )
{
    if( ftp.CmdFirtS == 0 ){
        ftp.CmdFirtS = 1;
        ftp.FileCmd = FTP_CMD_SEARCH;                                           /* �����Ѳ��ļ�״̬ */
        ftp.InterCmdS = FTP_MACH_FINDLIST;                                      /* ����Ŀ¼ */
        memset((void *)ListName,0,sizeof(ListName));        
        sprintf( ListName,pListNAme );                                          /* �����ļ������� */
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                          /* �����Ѳ��ļ������� */
        CH563NET_FTPType( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_SEARCHSUC){                                /* �Ѳ��ļ���� */
#if DEBUG
    printf("**********\nsearch success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;     
            return FTP_COMMAND_SUCCESS;
        }
    }
    return FTP_COMMAND_CONTINUE; 
}

/*******************************************************************************
* Function Name  : ch563ftp_getfile
* Description    : �����ļ�
* Input          : pFileName - ���ص��ļ���
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH563NET_FTPGetFile( char *pFileName )
{
    if(ftp.CmdFirtS == 0){
        ftp.CmdFirtS  = 1;
        ftp.FileCmd = FTP_CMD_GETFILE;                                          /* ���������ļ�״̬ */
        ftp.InterCmdS = FTP_MACH_GETFILE;                                       /* �����ļ� */
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                          /* ���������ļ������� */
        CH563NET_FTPType( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_GETSUC){                                   /* �����ļ��ɹ� */
#if DEBUG
    printf("*********\ngetfile success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;       
            return FTP_COMMAND_SUCCESS;
        }
    }    
    return FTP_COMMAND_CONTINUE; 
}

/*******************************************************************************
* Function Name  : ch563ftp_getfile
* Description    : �ϴ��ļ�
* Input          : pListNAme - �ϴ��ļ����豣���Ŀ¼��
                   pFileName - �ϴ��ļ����豣����ļ���
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_FTPPutFile(char *pListNAme, char *pFileName)
{
    if(ftp.CmdFirtS ==0){
        ftp.CmdFirtS = 1;
        ftp.FileCmd = FTP_CMD_PUTFILE;                                          /* �����ϴ��ļ�״̬ */
        ftp.InterCmdS = FTP_MACH_MKDLIST;                                       /* ����Ŀ¼ */
        memset((void *)ListName,'\0',sizeof(ListName));               
        sprintf( ListName,pListNAme );                                          /* �����ϴ��ļ������� */
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                          /* �����ϴ��ļ������� */
        CH563NET_FTPCdup( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_PUTSUC){                                   /* �ϴ��ļ��ɹ� */
#if DEBUG
    printf("*********\nputfile success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;
            return FTP_COMMAND_SUCCESS;
        }
    }    
    return FTP_COMMAND_CONTINUE;     
}

/*********************************** endfile **********************************/