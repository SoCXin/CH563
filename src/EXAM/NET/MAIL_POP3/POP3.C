/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563POP3.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563оƬ���ڽ����ʼ�
*                    : MDK3.36@ARM966E-S,Thumb,С��
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "POP3.H"
#include "SYSFREQ.H"

const char *base64_map = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnoprstuvwxyz0123456789+/";
const char base64_decode_map[256] = 
{
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255,  62, 255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
     255,   0, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255, 255,  26,  27,  28,
      29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
      49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};
/* POP3 CMD Codes */
const char  POP3_CLIENT_CMD[12][5] = 
{
    /* basic POP3 commands */
    "QUIT",                                                                     /* 0 �˳� */
    "USER",                                                                     /* 1 �û��� */
    "PASS",                                                                     /* 2 ���� */
    "STAT",                                                                     /* 3 ����ͳ������ */
    "LIST",                                                                     /* 4 ����ָ���ʼ��Ĵ�С */
    "RETR",                                                                     /* 5 �ʼ���ȫ���ı� */
    "DELE",                                                                     /* 6 ���ɾ�� */
    "RSET",                                                                     /* 7 �������е�DELE���� */
    "NOOP",                                                                     /* 8 ����һ���϶�����Ӧ */
    /* alternative POP3 commands */
    "APOP",                                                                     /* 9  ��֤һ�ְ�ȫ�������İ취��ִ�гɹ�����״̬ת�� */
    "TOP" ,                                                                     /* 10 ��������n���ʼ���ǰm�����ݣ�m��������Ȼ�� */
    "UIDL"                                                                      /* 11 �������ڸ�ָ���ʼ���Ψһ��ʶ */
};
/******************************************************************************/
POP   m_pop3;
POP   *p_pop3;
char  R_argv[3][32];
char  send_buff[512];                                                           /* �������ݻ����� */
char  EncodeHostName[32];                                                       /* �������󷢼������� */
char  MailBodyData[128]= "Demonstration test ch563 mail function wch.cn";       /* �ʼ��������ݣ�������ʾ���ظ�ʱ�����ݴ������������ݣ�*/
char  AttachmentData[attach_max_len]= "0123456789abcdefghijklmnopqrstuvwxyz";   /* �������ݣ�������ʾ���ظ�ʱ�����ݴ�����ĸ������ݣ�*/

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
        UINT32 i = 0, j = 0;
        
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
* Function Name  : Base64Decode
* Description    : base64����
* Input          : src     -��Ҫ������ַ���
                   src_len -��Ҫ�����ַ����ĳ���
                   dst     -�������ַ���
* Output         : None
* Return         : None
*******************************************************************************/
void Base64Decode(char *src, UINT16 src_len, char *dst)
{
        UINT32 i = 0, j = 0;
        
        for (; i < src_len; i += 4) {
            if(strncmp( &src[i], "\r\n", 2 ) == 0) i += 2;
            dst[j++] = base64_decode_map[src[i]] << 2 |
                    base64_decode_map[src[i + 1]] >> 4;
            dst[j++] = base64_decode_map[src[i + 1]] << 4 |
                    base64_decode_map[src[i + 2]] >> 2;
            dst[j++] = base64_decode_map[src[i + 2]] << 6 |
                    base64_decode_map[src[i + 3]];
        }
        if(src_len%4 == 3) {
            dst[strlen(dst)-1] = '\0'; 
        }
        else if(src_len%4 == 2) {
            dst[strlen((char *)dst)-1] = '\0'; 
            dst[strlen((char *)dst)-1] = '\0'; 
        }
        else dst[j] = '\0';
}

/*******************************************************************************
* Function Name  : QuotedPrintableEncode
* Description    : quotedprintable����
* Input          : pSrc��Ҫ������ַ���
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
* Function Name  : QuotedPrintableEncode
* Description    : quotedprintable����
* Input          : pSrc    -��Ҫ������ַ���
                   nSrcLen -��Ҫ�����ַ����ĳ���
                   pDst    -�������ַ���
* Output         : None
* Return         : None
*******************************************************************************/
void QuotedPrintableDecode( char *pSrc, char *pDst, UINT16 nSrcLen )
{
    UINT16 i = 0;
    UINT16 nDstLen = 0;

    while( i < nSrcLen ){
        if( strncmp( pSrc, "=\r\n", 3 ) == 0 ){
            pSrc += 3;
            i += 3;
        }
        else{
            if( *pSrc == '=' ){
                sscanf( pSrc, "=%02x",*pDst);
                pDst++;
                pSrc += 3;
                i += 3;
            }
            else{
                *pDst++ = *pSrc++;
                i++;
            }
            nDstLen++;
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
* Function Name  : ch563mail_pop3init
* Description    : �����ʼ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3Init( void )
{
    p_pop3 = &m_pop3;
    memset( p_pop3, '\0', sizeof(POP) );
    strcpy( p_pop3->pPop3Server,   p_Server );                                  /* ���������� */
    strcpy( p_pop3->pPop3UserName, p_UserName );                                /* �û��� */
    strcpy( p_pop3->pPop3PassWd,   p_PassWord );                                /* ����    */
}

/*******************************************************************************
* Function Name  : CH563NET_POP3User
* Description    : ��֤�û���  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3User( void )                          
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf(send_buff, "%s %s\r\n",POP3_CLIENT_CMD[1], p_pop3->pPop3UserName);
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_USER ,p_pop3->Socket); 
}

/*******************************************************************************
* Function Name  : CH563NET_POP3Pass
* Description    : ����  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3Pass( void )                          
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", POP3_CLIENT_CMD[2], p_pop3->pPop3PassWd );
    CH563NET_SendData(  send_buff, strlen(send_buff), POP_CHECK_PASS,p_pop3->Socket );    
}

/*******************************************************************************
* Function Name  : CH563NET_POP3Stat
* Description    : ��������ͳ������  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3Stat( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff,"%s\r\n", POP3_CLIENT_CMD[3] );
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_STAT,p_pop3->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_POP3List
* Description    : ����server����ָ���ʼ��Ĵ�С 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3List( void )
{
#if    0                                                                        /* �����ָ��ĳ���ʼ�����1 */
    char num;
    num = '1';                                                                  /* ������Ҫ�޸��ʼ��� */
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[4],num );
#else
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[4] );
#endif
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_LIST,p_pop3->Socket);  
}

/*******************************************************************************
* Function Name  : CH563NET_POP3Retr
* Description    : ����server�ʼ���ȫ���ı� 
* Input          : num-�ʼ���
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_RTER 
void CH563NET_POP3Retr( UINT8 num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[5], num );
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_RETR,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Dele
* Description    : ����server���ɾ�� 
* Input          : num-�ʼ���
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_DELE
void CH563NET_POP3Dele( UINT8 num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[6], num );
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_DELE,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Rset
* Description    : ����server����ɾ��
* Input          : num-�ʼ���
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_RSET
void CH563NET_POP3Rset( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s \r\n", POP3_CLIENT_CMD[7]);
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_RSET,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Top
* Description    : ����n���ʼ���ǰm������
* Input          : num-�ʼ���
                   m-����
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_TOP
void CH563NET_POP3Top( char num ,char m  )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c %c\r\n", POP3_CLIENT_CMD[10],num,m);
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_TOP,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Uidl
* Description    : ����server�������ڸ�ָ���ʼ���Ψһ��ʶ
* Input          : num-�ʼ���
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_UIDL
void CH563NET_POP3Uidl( char num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[11], num);
    CH563NET_SendData( send_buff, sizeof(send_buff), POP_CHECK_UIDL,p_pop3->Socket );
}
#endif
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
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[0]);
    if(index==p_pop3->Socket)    CH563NET_SendData( send_buff, strlen(send_buff),POP_CHECK_QUIT,index );
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
        case POP_RECEIVE_USER: 
            CH563NET_POP3User(  );
            break;
        case POP_RECEIVE_PASS: 
            CH563NET_POP3Pass(  );
            break;
        case POP_RECEIVE_STAT: 
            CH563NET_POP3Stat(  );
            break;
        case POP_RECEIVE_LIST: 
            CH563NET_POP3List(  );
            break;
        case POP_ERR_CHECK:
            CH563NET_Quit( p_pop3->Socket );
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case POP_RECEIVE_QUIT:
            CH563NET_Quit( p_pop3->Socket );
            break;
        case POP_CLOSE_SOCKET:
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        default:
            ReceDatFlag = 1; 
            break;
    }
}
/*******************************************************************************
* Function Name  : CH563NET_CheckResponse
* Description    : ��֤�����ź�
* Input          : recv_buff- ������Ϣ
                   check_type-��������
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 checktype )
{
    switch(checktype){
        case POP_CHECK_CNNT:                                                    /* ���� */
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_USER;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_CNNT;
        case POP_CHECK_USER:                                                    /* �û��� */
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_PASS;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_USER;
        case POP_CHECK_PASS:                                                    /* ���� */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_STAT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_PASS;
        case POP_CHECK_STAT:                                                    /* ���ʼ���Ϣ */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_LIST;
                return (CHECK_SUCCESS);    
            } 
            return POP_ERR_STAT;
        case POP_CHECK_LIST:                                                    /* �ʼ��б� */
            if((strncmp("+OK", recv_buff, 3) == 0)){
                OrderType = NOOP;
                return (CHECK_SUCCESS);
            }
            if((strncmp(".", recv_buff, 1) == 0)){
                p_pop3->RefreshTime = 1;
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            }
            if((strncmp("1", recv_buff, 1) == 0)){
                p_pop3->RefreshTime = 0;
                OrderType = POP_RECEIVE_RTER;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_LIST;
        case POP_CHECK_QUIT:                                                    /* �˳���½ */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_QUIT;
        case POP_CHECK_RETR:                                                    /* ��ȡ�ʼ����� */ 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = NOOP;    
                memset(AttachmentData,'\0',sizeof(AttachmentData));
                memset(R_argv,'\0',sizeof(R_argv));
                return (CHECK_SUCCESS);
            } 
            else if(strncmp("-ERROR", recv_buff, 6) != 0){
                if( strncmp("\r\n.\r\n", &recv_buff[ReceLen-5], 5) == 0 ){
                    p_pop3->AnalyMailDat = 1;                                   /* �����ʼ���ɱ�־ */
                    CH563NET_AnalyseMailData( recv_buff );                      /* �����ʼ����� */
                    OrderType = POP_RECEIVE_QUIT;    
               }
                else{
                     p_pop3->AnalyMailDat = 0;
                       CH563NET_AnalyseMailData( recv_buff );
                }
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_RETR;
        case POP_CHECK_DELE:                                                    /* ɾ�� */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* ִ�����ִ������������������ѡ����һ������ */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_DELE;
        case POP_CHECK_RSET:                                                    /* ����ɾ�� */ 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* ִ�����ִ������������������ѡ����һ������ */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_RSET;
        case POP_CHECK_TOP:        // 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* ִ�����ִ������������������ѡ����һ������ */
                return (CHECK_SUCCESS);
            }
            if(strncmp("Return", recv_buff, 6) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* ִ�����ִ������������������ѡ����һ������ */
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_TOP;
        case POP_CHECK_UIDL:    //
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* ִ�����ִ������������������ѡ����һ������ */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_UIDL;
        default:
            return CHECK_ERROR;
    }
}
/*******************************************************************************
* Function Name  : CH563NET_AnalyseMailData
* Description    : �������յ����ʼ�
* Input          : recv_buff-�ʼ�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_AnalyseMailData( char *recv_buff ) 
{
     UINT16    i=0,j=0;

    if(p_pop3->ReceFlag){
        i = 0;
        if(p_pop3->EncodeType == 2) goto type2;                                 /*  ��ת���������� ���뷽ʽ--quoted-printable */
        if(p_pop3->EncodeType == 1) goto type1;                                 /*  ��ת���������� ���뷽ʽ--base64 */
        if(p_pop3->EncodeType == 0) goto type0;                                 /*  ��ת���������� ���뷽ʽ--others    */
    }
    for(i=0;i<ReceLen;i++){
        /* ����������/��ַ */                 
        if( strncmp("\nFrom: ", &recv_buff[i], 7) == 0 ){
            i += 7;
            if(recv_buff[i] == '"') i++;
            if(recv_buff[i] == '='){
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;     /* �����˾������� */
                i += 3;
                j = 0;
                memset( send_buff, '\0', sizeof(send_buff) );
                /* ���������� */
                while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                memset( p_pop3->DecodeRName, '\0', sizeof(p_pop3->DecodeRName) );
                Base64Decode(send_buff,  strlen(send_buff),p_pop3->DecodeRName);
            }
            else{                                                               /* δ���� */
                j = 0;
                while((recv_buff[i] != '"')&&(i < ReceLen)){                    /* ���������� */
                    p_pop3->DecodeRName[j] = recv_buff[i];
                    j++;
                    i++;        
                }
            }                       
            while((recv_buff[i] != '<')&&(i < ReceLen)) i++;
            i++;
            j = 0;
            while((recv_buff[i] != '>')&&(i < ReceLen)){                        /* �����˵�ַ */
                R_argv[0][j] = recv_buff[i];
                j++;
                i++; 
            }
        }
        /* ���� */
        if( strncmp("\nCc: ", &recv_buff[i], 5) == 0 ){
            i += 5;
            while(recv_buff[i] != '<')    i++;
            i++;
            j = 0;
            while(recv_buff[i] != '>'&&(i < ReceLen)){                          /* �����˵�ַ */
                p_pop3->Ccname[j] = recv_buff[i];
                j++;
                i++;        
            }
        }
        /* ���� */
        if( strncmp("\nDate: ", &recv_buff[i], 7) == 0 ){
            i += 7;
            j = 0;
            while((recv_buff[i] != '\r')&&(i < ReceLen)){                       /* �����ʼ����� */
                p_pop3->sBufTime[j] = recv_buff[i];
                j++;
                i++;        
            }
        }
        /* ���� */
        if( strncmp("\nSubject: ", &recv_buff[i], 10) == 0 ){
            i += 10;
            if(recv_buff[i] == '[') {
                while(recv_buff[i] == ']'&&(i < ReceLen)) i++;
                i += 2;
            }
            if(recv_buff[i] == '='){                                            /* ���⾭������ */
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                i += 3;
                j = 0;
                memset( send_buff, '\0', sizeof(send_buff) );
                /* ���� */
                while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;      
                }
                Base64Decode(send_buff,strlen(send_buff),R_argv[1]);
            }
            else{                                                               /* δ���� */
                j = 0;
                while(recv_buff[i] != '\r'&&(i < ReceLen)){                     /* ���� */
                    R_argv[1][j] = recv_buff[i];
                    j++;
                    i++;      
                }
            }
        }
        /* �����ı��뷽ʽ�����֡����� */
        if( strncmp("name=", &recv_buff[i], 5) == 0 ){
            i += 5;
            while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)&&strncmp("filename=", &recv_buff[i], 9)&&(i < ReceLen)) i++;
            if(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27) == 0){ /* ��ѯ�������뷽ʽ */
/* ���뷽ʽ */
                i += 27;
                if(strncmp("base64", &recv_buff[i], 6) == 0 ){
                    i += 6;
                    p_pop3->EncodeType = 1;
                }
                else if(strncmp("quoted-printable", &recv_buff[i], 16) == 0 ) {
                    i += 16;
                    p_pop3->EncodeType = 2;
                }
                else p_pop3->EncodeType = 0;  
/* �������� */
                while(strncmp("filename=", &recv_buff[i], 9)) i++;
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
                if(recv_buff[i] == '='){                                        /* �������־������� */
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', sizeof(send_buff) );
                    /* �������� */
                    while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){             
                        send_buff[j] = recv_buff[i];
                        j++;
                        i++;      
                    }
                    Base64Decode(send_buff,  strlen(send_buff),R_argv[2]);
                }
                else{                                                           /* δ���� */
                    j = 0;
                    while(recv_buff[i] != '"'&&(i < ReceLen)){                  /* �������� */
                        R_argv[2][j] = recv_buff[i];
                        j++;
                        i++;        
                    }
                }
            }
/* �������� */
            else if( strncmp("filename=", &recv_buff[i], 9) == 0 ){
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
                if(recv_buff[i] == '='){
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', sizeof(send_buff) );
                    while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                        send_buff[j] = recv_buff[i];
                        j++;
                        i++;      
                    }
                    Base64Decode(send_buff,  strlen(send_buff),R_argv[2]);
                }
                else{
                    j = 0;
                    while(recv_buff[i] != '"'&&(i < ReceLen)){
                        R_argv[2][j] = recv_buff[i];
                        j++;
                        i++;        
                    }
                }
/* �������뷽ʽ */
                while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)) i++;
                i += 27;
                if(strncmp("base64", &recv_buff[i], 6) == 0 ){
                    i += 6;
                    p_pop3->EncodeType = 1;
                }
                else if(strncmp("quoted-printable", &recv_buff[i], 16) == 0 ) {
                    i += 16;
                    p_pop3->EncodeType = 2;
                }
                else p_pop3->EncodeType = 0;  
            }
/* �������� */
            while( strncmp("\n\r\n", &recv_buff[i], 3) != 0 &&(i < ReceLen)) i++;
            i += 3;
            if(p_pop3->EncodeType==1){    /* base64 */
                j = 0;
                memset(send_buff,'\0', sizeof(send_buff) );
type1:            while((recv_buff[i] != '=')&&strncmp("\r\n--", &recv_buff[i], 4)&&strncmp("\r\n\r\n", &recv_buff[i], 4)&&(i < ReceLen)&&\
                    (j<attach_max_len)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else {
                    p_pop3->ReceFlag = 0;
                    Base64Decode(send_buff,  strlen(send_buff),AttachmentData);
                }
            }
            else if(p_pop3->EncodeType==2){     /* quoted-printable */
                j = 0;
                memset(send_buff,'\0', sizeof(send_buff));
type2:            while(strncmp("\r\n.\r\n", &recv_buff[i], 5)&&strncmp("\r\n--", &recv_buff[i], 4)&&(i < ReceLen)&&(j<attach_max_len)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else {
                    p_pop3->ReceFlag = 0;
                    QuotedPrintableDecode(send_buff,(char *)AttachmentData, strlen(send_buff));
                }
            }
            else{    /* 7bit or others */ 
                j = 0;
type0:            while(recv_buff[i] != '\r'&&(i < ReceLen)&&(j<attach_max_len)){
                    AttachmentData[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else p_pop3->ReceFlag = 0; 
            }
        }
    }
    if(p_pop3->AnalyMailDat){
#if DEBUG
        printf("addr:\n %s\n",R_argv[0]);                                       /* �����˵ĵ�ַ    */ 
        printf("send name:\n %s\n",p_pop3->DecodeRName);                        /* ���������� */
        printf("subject:\n %s\n",R_argv[1]);                                    /* �ʼ����� */
        printf("attach name:\n%s\n",R_argv[2]);                                 /* �������� */
        printf("send time:\n %s\n",p_pop3->sBufTime);                           /* ����ʱ��    */
        printf("attach text:\n%s\n",AttachmentData);                            /* ��������    */
#endif
    }
}

/*******************************************************************************
* Function Name  : CH563NET_MailQuery
* Description    : ״̬��ѯ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_MailQuery( void )
{
    if( p_pop3->DiscnntFlag == 1){
        p_pop3->DiscnntFlag = 0;
#ifdef    POP_REFRESH
        if(p_pop3->RefreshTime){
            p_pop3->RefreshTime = 0;
            Delay_ms(200);
            CH563NET_CreatTcpPop3( ); 
        } 
#endif
    }
    if(ReceDatFlag){
#ifdef    POP_RTER                                                                                                               
        if(OrderType == POP_RECEIVE_RTER ){
            ReceDatFlag = 0;    
            CH563NET_POP3Retr( '1' );                                           /* ����server�ʼ���ȫ���ı�����    �������ʼ��ţ� */
        }
#endif
#ifdef    POP_DELE 
        if(OrderType == POP_RECEIVE_DELE ){
            ReceDatFlag = 0;    
            CH563NET_POP3Dele( '1' );                                           /* ����server���ɾ����QUIT����ִ��ʱ������ɾ���������ʼ��ţ� */
        }
#endif
#ifdef    POP_RSET 
        if(OrderType == POP_RECEIVE_RSET ){ 
            ReceDatFlag = 0;    
            CH563NET_POP3Rset(  );                                              /* �����������е�DELE���� */
        }
#endif
#ifdef    POP_TOP 
        if(OrderType == POP_RECEIVE_TOP ){
            ReceDatFlag = 0;    
            CH563NET_POP3Top( '1','3' );                                        /* ����n���ʼ���ǰm������(�����ʼ���,�к�(����Ϊ��Ȼ��)) */
        }
#endif
#ifdef    POP_UIDL 
        if(OrderType == POP_RECEIVE_UIDL ){
            ReceDatFlag = 0;    
            CH563NET_POP3Uidl( '1' );                                           /* ����server�������ڸ�ָ���ʼ���Ψһ��ʶ�����û��ָ�����������еġ��������ʼ��ţ�*/
        }
#endif
    }

}

/*********************************** endfile **********************************/