/********************************** (C) COPYRIGHT *********************************
* File Name          : SMTP.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : Define for CH563SMTP.C
**********************************************************************************/
#ifndef __SMTP_H__
#define __SMTP_H__
//================================================================================
extern  UINT8 OrderType;                          // ��������
extern  UINT8 CheckType;        
extern  UINT8 ReceDatFlag;                        // �յ����ݱ�־λ
extern  UINT16 ReceLen;    
//=================================================================================
#define attach_max_len              512           // ���͸�����󳤶ȣ�Ĭ��Ϊ512   
#define POP3_SERVER_PORT            110           // ���ʼ�Ŀ�Ķ˵��
#define SMTP_SERVER_PORT            25            // ���ʼ�Ŀ�Ķ˵��
#define DEBUG                        1            // ����    -- ������115200
typedef struct
{
    UINT8 Socket;                
    UINT8 g_MIME;                                 // ���޸�����־λ
    char  m_strFile[48];                          // ��������
    char  m_strSendFrom[48];                      // �����˵�ַ
    char  m_strSendTo[48];                        // �ռ��˵�ַ
    char  m_strSMTPServer[32];                    // ����������
    char  m_strUSERID[32];                        // �û���
    char  m_strPASSWD[32];                        // ����
    char  m_strSubject[32];                       // ����
    char  m_strSenderName[32];                    // ����������
}SMTP;
/* smtp�����ʼ���ز��� */
extern const char *m_Server;                      // ����������
extern const char *m_UserName;                    // �û���
extern const char *m_PassWord;                    // ����
extern const char *m_SendFrom;                    // �����˵�ַ
extern const char *m_SendName;                    // ����������
extern const char *m_SendTo;                      // �ռ��˵�ַ
extern const char *m_Subject;                     // ����
extern const char *m_FileName;                    // ��������(��������͸���,����Ϊ"\0")
/* CH563��ض��� */                                                                   
extern UINT8 MACAddr[6];                          // CH563MAC��ַ 
extern const UINT8 IPAddr[4];                     // CH563IP��ַ 
extern const UINT8 GWIPAddr[4];                   // CH563���� 
extern const UINT8 IPMask[4];                     // CH563�������� 
/* socketsmtp ��ض���,SMTP�����ʼ�    */                                          
extern const UINT8  SocketsmtpIP[4];              // Socket smtpĿ��IP��ַ 
extern const UINT16  SmtpSourPrt;                 // Socket smtpԴ�˿ں�
/************************************************************************************************************************
* ������� 
* 0x01-0x0f Ϊ�����ʼ�������
****************** 0x01-0x07 Ϊ��������
****************** 0x09-0x0e ΪP��ѡ����
****************** 0x01-0x02-0x03-0x06 Ϊȷ��״̬
****************** 0x04-0x05 0x09-0x0e Ϊ����״̬
* 0x10-0x1f Ϊ�����ʼ�������
****************** 0x10-0x17 �谴˳��ִ��
****************** �����Ϊ��ѡ�����
* 0x00         �����κβ���
************************************************************************************************************************/
#define NOOP                    0x00
//SMTP SEND CODE
#define SMTP_SEND_HELO            0x10    // ����hello����
#define SMTP_SEND_AUTH            0x11    // ���͵�½����
#define SMTP_SEND_USER            0x12    // �����û���
#define SMTP_SEND_PASS            0x13    // ��������
#define SMTP_SEND_MAIL            0x14    // ���ͷ����ߵ�ַ
#define SMTP_SEND_RCPT            0x15    // ���ͽ����ߵ�ַ
#define SMTP_SEND_DATA            0x16    // ���ͷ�����������
#define SMTP_DATA_OVER            0x17    // �����ʼ�����
#define SMTP_SEND_QUIT            0x18    // �˳���½
#define SMTP_CLOSE_SOCKET         0x19    // �ر�smtp��socket
#define SMTP_SEND_START           0x1E    // ���������ʼ�
#define SMTP_ERR_CHECK            0x1F    // smtp���ֳ����˳���½���ر�socket
/* *********************************************************************************************************************
* �����źź˶Դ������
************************************************************************************************************************/
//SMTP ERR CHECK CODE
#define SMTP_ERR_CNNT            0x31     // smtp���Ӵ���-------------�����Ƕ˿ںţ�������IP�����ô���
#define SMTP_ERR_HELO            0x32     // ����helo����ʧ��---------HELO��������󣬻�����������ʽ����
#define SMTP_ERR_AUTH            0x33     // �����½����ʧ��---------AUTH���������
#define SMTP_ERR_USER            0x34     // �����û���δʶ��---------�û�����ʽ���ԣ���USER���������
#define SMTP_ERR_PASS            0x35     // ��½ʧ��-----------------�û�����������󣬻�PASS���������
#define SMTP_ERR_MAIL            0x36     // �����˵�ַ����ʧ��-------�����˵�ַ����ȷ����MAIL���������
#define SMTP_ERR_RCPT            0x37     // ���ͽ��յ�ַ����---------�ռ��˵�ַ����ȷ����RCPT���������
#define SMTP_ERR_DATA            0x38     // ����������-------------DATA���������
#define SMTP_ERR_DATA_END        0x39     // �������ݹ��̳���
#define SMTP_ERR_QUIT            0x3A     //    �˳���½ʧ��
#define SMTP_ERR_UNKNOW          0x3F     // smtp����δ֪
//
#define send_data_timeout        0x10     // �������ݳ�ʱ
#define send_data_success        0x14     // �������ݳ�ʱ
/***********************************************************************************************************************
* �˶�������Ϣ�Ĵ������� 
* �ɹ�����CHECK_SUCCESS���������������Ϊ��һ�������������
* ���󷵻ض�Ӧ�Ĵ������ 
* ���������������źţ�����Ҫ�˶�
***********************************************************************************************************************/
#define CHECK_SUCCESS            0x00
#define uncheck                    0x00
//SMTP CHECK CODE
#define SMTP_CHECK_CNNT            0x60
#define SMTP_CHECK_HELO            0x61
#define SMTP_CHECK_AUTH            0x62
#define SMTP_CHECK_USER            0x63
#define SMTP_CHECK_PASS            0x64
#define SMTP_CHECK_MAIL            0x65
#define SMTP_CHECK_RCPT            0x66
#define SMTP_CHECK_DATA            0x67
#define SMTP_CHECK_DATA_END        0x68
#define SMTP_CHECK_QUIT            0x69

#define g_strBoundary    "18ac0781-9ae4-4a2a-b5f7-5479635efb6b"                      /* �߽� */
#define g_strEncode      "base64"                                                    /* ���뷽ʽ */
#define g_strcharset     "gb2312"                                                    /* windows��ʽ (linux��ʽ"utf-8") */
#define g_xMailer        "X-Mailer: X-WCH-Mail Client Sender\r\n"                    /* X-Mailer����     */
#define g_Encoding       "Content-Transfer-Encoding: quoted-printable\r\nReply-To: " /* Encoding ���� */
#define g_Custom         "X-Program: CSMTPMessageTester"                             /* X-Program��ͬ�����޸� */
#define g_FormatMail     "This is a multi-part message in MIME format."              /* �ʼ��ж������ݣ�һ�����и��� */
#define g_AttachHead     "\r\nContent-Transfer-Encoding: quoted-printable\r\n" 
#define g_AttachHedType  "text/plain"
#define g_MailHedType    "multipart/mixed"

//================================================================================
void CH563NET_SendData( char *PSend, UINT32 Len,UINT8 type,UINT8 index  );        // ��������

void CH563NET_MAILInit(void);                                                     // 563��ʼ��

void CH563NET_SMTPInit( void );                                                   // ���ʼ���ʼ��

void CH563NET_MailCmd( UINT8 choiceorder);                                        // ������������ִ�ж�Ӧ������Զ���������շ�����

UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 check_type );                 // ���Ӧ����Ϣ

void CH563NET_Quit(UINT8 index );                                                 // �˳������½

void CH563NET_CreatTcpSmtp( void );                                               // ����smtp����

void CH563NET_ReplyMailBody( void );                                              // �ظ��ʼ�����������

void CH563NET_MailQuery( void );

void mStopIfError(UINT8 iError);

void CH563NET_HandleGloableInt( void );

//================================================================================
#endif