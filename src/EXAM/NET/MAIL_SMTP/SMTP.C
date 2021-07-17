/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563SMTP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : CH563оƬ���ڷ����ʼ�
*                    : MDK3.36@ARM966E-S,Thumb,С��
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "SMTP.H"
#include "SYSFREQ.H"

const char base64_map[64] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/* SMTP CMD Codes */
const  char  SMTP_CLIENT_CMD[6][11] = 
{
    "EHLO",                                                                     /* 0 �˳� */
    "AUTH LOGIN",                                                               /* 1 ��½ */
    "MAIL FROM:",                                                               /* 2 �����˵�ַ */
    "RCPT TO:",                                                                 /* 3 �ռ��˵�ַ */
    "DATA",                                                                     /* 4 ��ʼ������������ */
    "QUIT"                                                                      /* 5 �˳� */
};
/******************************************************************************/
SMTP    m_smtp;
SMTP    *p_smtp;
char     send_buff[512];                                                        /* �������ݻ����� */
char     EncodeHostName[32];                                                    /* �������󷢼������� */
char     MailBodyData[128]= "Demonstration test ch563 mail function wch.cn";    /* �ʼ��������ݣ�������ʾ���ظ�ʱ�����ݴ������������ݣ�*/
char     AttachmentData[attach_max_len]= "0123456789abcdefghijklmnopqrstuvwxyz";/* �������ݣ�������ʾ���ظ�ʱ�����ݴ�����ĸ������ݣ�*/

/*******************************************************************************
* Function Name  : Base64Encode
* Description    : base64����
* Input          : src     -��Ҫ������ַ���
                   src_len -��Ҫ�����ַ����ĳ���
                   dst     -�������ַ���
* Output         : None
* Return         : None
*******************************************************************************/
void Base64Encode(char *src, UINT16 src_len, char *dst)
{
        UINT16 i = 0,j = 0;
        
        for (; i < src_len - src_len % 3; i += 3) {
                dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                dst[j++] = base64_map[((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f];
                dst[j++] = base64_map[((src[i + 1] << 2) | (src[i + 2] >> 6 )) & 0x3f];
                dst[j++] = base64_map[src[i + 2] & 0x3f];
        }
        
        if (src_len % 3 == 1) {
                 dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                 dst[j++] = base64_map[(src[i] << 4) & 0x3f];
                 dst[j++] = '=';
                 dst[j++] = '=';
        }
        else if (src_len % 3 == 2) {
                dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                dst[j++] = base64_map[((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f];
                dst[j++] = base64_map[(src[i + 1] << 2) & 0x3f];
                dst[j++] = '=';
        }
        dst[j] = '\0';
}

/*******************************************************************************
* Function Name  : QuotedPrintableEncode
* Description    : quotedprintable����
* Input          : pSrc    -��Ҫ������ַ���
                   pDst    -�������ַ���
                   nSrcLen -��Ҫ�����ַ����ĳ���
                   MaxLine -�������
* Output         : None
* Return         : None
*******************************************************************************/
void QuotedPrintableEncode( char *pSrc, char *pDst, UINT16 nSrcLen, UINT16 MaxLine )
{
    UINT16 nDstLen  = 0;
    UINT16 nLineLen = 0;
    UINT16 i = 0;

    for(i = 0; i < nSrcLen; i++, pSrc++ ){        
        if( (*pSrc >= '!') && (*pSrc <= '~') && (*pSrc != '=') ){
            *pDst++ = *pSrc;
            nDstLen++;
            nLineLen++;
        }
        else{
            sprintf(pDst, "=%02x", *pSrc);
            pDst += 3; 
            nDstLen += 3;
            nLineLen += 3;
        }
        if( nLineLen >= MaxLine - 3 ){    
            sprintf(pDst,"=\r\n");
            pDst += 3;
            nDstLen += 3;
            nLineLen = 0;
        }
    }
    *pDst = '\0';
}

/*******************************************************************************
* Function Name  : CH563NET_XToChar
* Description    : 16����ת�ַ���
* Input          : dat -Ҫת����ʮ����������
                   p   -ת�����Ӧ���ַ���
                   len -Ҫת���ĳ���
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_XToChar( char  *dat,char  *p,char len)
{
    char k;
    for(k=0;k<len;k++){
        *p = (((dat[k]&0xf0)>>4)/10)?(((dat[k]&0xf0)>>4)+'A'-10):(((dat[k]&0xf0)>>4)+'0');
        p++;
        *p = ((dat[k]&0x0f)/10)?((dat[k]&0x0f)+'A'-10):((dat[k]&0x0f)+'0');
        p++;
        if(k<len-1){
            *p = '.';
            p++;        
        }
    }
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPInit
* Description    : �����ʼ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPInit( void )
{
    p_smtp = &m_smtp;
    p_smtp->g_MIME = 0;
    memset( p_smtp, '\0', sizeof(SMTP) );
    strcpy( p_smtp->m_strSMTPServer, m_Server );                                /* ���������� */
    strcpy( p_smtp->m_strUSERID,m_UserName );                                   /* �û��� */
    strcpy( p_smtp->m_strPASSWD,m_PassWord );                                   /* ���� */        
    strcpy( p_smtp->m_strSendFrom,m_SendFrom );                                 /* �����˵�ַ */
    strcpy( p_smtp->m_strSenderName, m_SendName );                              /* ���������� */
    strcpy( p_smtp->m_strSendTo,m_SendTo );                                     /* �ռ��˵�ַ */
    strcpy( p_smtp->m_strSubject,m_Subject );                                   /* ����    */
    strcpy( p_smtp->m_strFile,m_FileName );                                     /* ��������(��������͸������˴������ʼ��) */
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPIsMIME
* Description    : �ж����޸���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPIsMIME( void )
{
    if( strlen(p_smtp->m_strFile) <= 0 ) p_smtp->g_MIME = 0;
    else p_smtp->g_MIME = 1;
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPAttachHeader
* Description    : �����齨�����ŷ� 
* Input          : pFileName     -��������
                   pAttachHeader -�齨�õ��ŷ�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPAttachHeader(  char *pFileName, char *pAttachHeader )
{
    const char *strContentType1 = "application/octet-stream";
    sprintf(pAttachHeader, "\r\n\r\n--%s\r\nContent-Type: %s;\r\n name=\"%s\"%sContent-Disposition: \
    attachment;\r\n filename=\"%s\"\r\n\r\n", g_strBoundary, strContentType1, pFileName,g_AttachHead, pFileName ); 
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPAttachEnd
* Description    : �齨������������ 
* Input          : EndSize    -�����������ݴ�С
*                   pAttachEnd -������������
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPAttachEnd( UINT16 *EndSize, char *pAttachEnd )
{
    strcat( pAttachEnd, "\r\n--" );
    strcat( pAttachEnd, g_strBoundary );
    strcat( pAttachEnd, "--" );
    *EndSize = strlen(pAttachEnd);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPMailHeader
* Description    : �ʼ��ŷ�  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPMailHeader( void )
{
    // "FROM: "
    memset( send_buff, '\0', sizeof(send_buff) );
    strcat( send_buff, "From: \"" );
    strcat( send_buff, p_smtp->m_strSenderName );
    strcat( send_buff, "\" <" );
    strcat( send_buff, p_smtp->m_strSendFrom );
    strcat( send_buff, ">\r\n" );
    // "TO: " 
    strcat( send_buff, "To: <" );
    strcat( send_buff, p_smtp->m_strSendTo );
    strcat( send_buff, ">\r\n" );
    // "Subject: " 
    strcat( send_buff, "Subject: ");
    strcat( send_buff, p_smtp->m_strSubject );
    strcat( send_buff, "\r\n" );
    //"Date: " 
    strcat( send_buff, "Date: ");
//    strcat( send_buff, "" );     /* ʱ��    */
     strcat( send_buff, "\r\n" );
    /* "X-Mailer: " */
    strcat( send_buff, g_xMailer );
    /* �и��� */
    if( p_smtp->g_MIME == 1 ){
        strcat( send_buff, "MIME-Version: 1.0\r\nContent-Type: " );
        strcat( send_buff, g_MailHedType );
        strcat( send_buff, ";\r\n\tboundary=\"" );
        strcat( send_buff, g_strBoundary );
        strcat( send_buff, "\"\r\n" );
    }
    /* Encoding information    */
    strcat( send_buff, g_Encoding );
    strcat( send_buff, p_smtp->m_strSenderName );
    strcat( send_buff, " <" );
    strcat( send_buff, p_smtp->m_strSendFrom );
    strcat( send_buff, ">\r\n" );
    /* add custom-tailor */
    strcat( send_buff, g_Custom );
    /* end of mail header */
    strcat( send_buff, "\r\n\r\n" );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);    
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPSendAttachData
* Description    : ���͸�������  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPSendAttachData( void )
{
    UINT16  EndSize;

    memset( send_buff, '\0', sizeof(send_buff) );
    CH563NET_SMTPAttachHeader( p_smtp->m_strFile, send_buff);
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);   /* Send attached file header */
/*******************************************************************************
*���͸������� 
*****************************************************************************/
    memset( send_buff, '\0', sizeof(send_buff) );
    QuotedPrintableEncode( AttachmentData, send_buff, strlen(AttachmentData),200 );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
    memset( send_buff, '\0', sizeof(send_buff) );
    CH563NET_SMTPAttachEnd( &EndSize, send_buff );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);   /* Send attached file end */
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPSendAttachHeader
* Description    : ���͸����ŷ�  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPSendAttachHeader( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, g_FormatMail );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck,p_smtp->Socket );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n--%s\r\nContent-Type: %s;\r\n\tcharset=\"%s\"%s\r\n",\
        g_strBoundary, g_AttachHedType, g_strcharset,g_AttachHead );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPEhlo
* Description    : ���뷢���ʼ�״̬  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPEhlo( void )
{
    memset( EncodeHostName, '\0', sizeof(EncodeHostName) );
    QuotedPrintableEncode( (char *)p_smtp->m_strSenderName, EncodeHostName, strlen(p_smtp->m_strSenderName),76 );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", SMTP_CLIENT_CMD[0],EncodeHostName );
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_HELO ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPAuth
* Description    : ���뷢���ʼ�״̬  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPAuth( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[1]);
    /* send "AUTH LOGIN" command */
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_AUTH ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPUser
* Description    : ��֤�û���   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPUser( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    Base64Encode( (char *)p_smtp->m_strUSERID, strlen(p_smtp->m_strUSERID), send_buff );
    sprintf( send_buff, "%s\r\n", send_buff);
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_USER,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPPass
* Description    : ��½����    
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPPass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    Base64Encode( (char *)p_smtp->m_strPASSWD, strlen(p_smtp->m_strPASSWD), send_buff);
    sprintf( send_buff, "%s\r\n", send_buff);
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_PASS,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPMail
* Description    : ���ͷ���������  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPMail( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[2],p_smtp->m_strSendFrom );
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_MAIL,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPRcpt
* Description    : �ռ��˵�ַ 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPRcpt( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[3],p_smtp->m_strSendTo );    
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_RCPT,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPData
* Description    : ����data���� 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPData( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[4] );
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPSendMail
* Description    : �����ʼ�����  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPSendMail( void )
{
    CH563NET_SMTPIsMIME( );
    CH563NET_SMTPMailHeader(  );                                                /* Send Mail Header */
    if( p_smtp->g_MIME ==  1 ){
        CH563NET_SMTPSendAttachHeader(  );                                      /* Send MIME Header */
    }
    else {
        CH563NET_SendData("\r\n", strlen("\r\n"),uncheck,p_smtp->Socket);
    }
    memset( send_buff, '\0', sizeof(send_buff) );
     QuotedPrintableEncode( (char *)MailBodyData, send_buff, strlen(MailBodyData),76 );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
    if( 1 == p_smtp->g_MIME ) CH563NET_SMTPSendAttachData( );                   /* Send Attached file */
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n.\r\n" );
    /* Send end flag of Mail */
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA_END ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_Quit
* Description    : �˳���½
* Input          : index-���˳���socketid
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_Quit( UINT8 index )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", SMTP_CLIENT_CMD[5]);
    if(index==p_smtp->Socket)    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_QUIT,index );
}

/*******************************************************************************
* Function Name  : CH563NET_MailCmd
* Description    : �ж���������Ӧ�ӳ���
* Input          : choiceorder-��������
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_MailCmd( UINT8 choiceorder )
{
    UINT8 i;

    switch( choiceorder ){
        case SMTP_SEND_HELO: 
            CH563NET_SMTPEhlo( );
            break;
        case SMTP_SEND_AUTH: 
            CH563NET_SMTPAuth( );
            break;
        case SMTP_SEND_USER: 
            CH563NET_SMTPUser( );
            break;
        case SMTP_SEND_PASS: 
            CH563NET_SMTPPass( );
            break;
        case SMTP_SEND_MAIL: 
            CH563NET_SMTPMail( );
            break;
        case SMTP_SEND_RCPT: 
            CH563NET_SMTPRcpt( );
            break;
        case SMTP_SEND_DATA: 
            CH563NET_SMTPData( );
            break;
        case SMTP_DATA_OVER:
            CH563NET_SMTPSendMail( );
            break;
        case SMTP_ERR_CHECK:
            CH563NET_Quit( p_smtp->Socket );
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case SMTP_SEND_QUIT:
            CH563NET_Quit( p_smtp->Socket );
            break;
        case SMTP_CLOSE_SOCKET:
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        default: 
            break;
    }
}

/*******************************************************************************
* Function Name  : CH563NET_CheckResponse
* Description    : ��֤�����ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 checktype )
{
    switch(checktype){
        case SMTP_CHECK_CNNT:    // 
            if( strncmp("220", recv_buff, 3) == 0 ){
                OrderType = SMTP_SEND_HELO;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_CNNT;
        case SMTP_CHECK_HELO:    // 
            if( strncmp("250", recv_buff, 3) == 0 ){
                OrderType = SMTP_SEND_AUTH;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_HELO;
        case SMTP_CHECK_AUTH:                                                   /* ��½���� */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = NOOP;
                return (CHECK_SUCCESS);
            }
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_USER;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_AUTH;
        case SMTP_CHECK_USER:                                                   /* �û��� */
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_PASS;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_USER;
        case SMTP_CHECK_PASS:                                                   /* ���� */
            if(strncmp("235", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_MAIL;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_PASS;
        case SMTP_CHECK_MAIL:                                                   /* ������ */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_RCPT;
                return (CHECK_SUCCESS);    
            } 
            return SMTP_ERR_MAIL;
        case SMTP_CHECK_RCPT:                                                   /* ���� */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_DATA;
                return (CHECK_SUCCESS);            
            }
            return SMTP_ERR_RCPT;
        case SMTP_CHECK_DATA:                                                   /* data ���� */
            if(strncmp("354", recv_buff, 3) == 0){
                OrderType = SMTP_DATA_OVER;
                return (CHECK_SUCCESS);
            } 
            return SMTP_ERR_DATA;
        case SMTP_CHECK_DATA_END:                                               /* ���ݷ������ */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_QUIT;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_DATA_END;
        case SMTP_CHECK_QUIT:                                                   /* �˳���½ */
            if(strncmp("220", recv_buff, 3) == 0||strncmp("221", recv_buff, 3) == 0){
                OrderType = SMTP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_QUIT;
        default:
            return SMTP_ERR_UNKNOW;
    }
}

/*********************************** endfile **********************************/