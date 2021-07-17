/********************************** (C) COPYRIGHT *********************************
* File Name          : SD.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : DEFINE FOR SDCARD.C
**********************************************************************************/

#define  SD_INSERT              1
#define  SD_WRITE_PROTECE       1

#define  Write_Secret_Num        0x18
#define  Read_Secret_Num         0x18


#define  R1    1
#define  R1B   2
#define  R2    3
#define  R3    4
#define	 R7	   7


/* R1��R2���ֽڴ����� R1 and upper byte of R2 error code */
#define MSK_IDLE          		  0x01
#define MSK_ERASE_RST     		  0x02
#define MSK_ILL_CMD       		  0x04
#define MSK_CRC_ERR       		  0x08
#define MSK_ERASE_SEQ_ERR 		  0x10
#define MSK_ADDR_ERR      		  0x20
#define MSK_PARAM_ERR     		  0x40

/* R2���ֽڴ����� lower byte of R2 error code */
#define MSK_TOK_ERROR             0x01
#define MSK_TOK_CC_ERROR          0x02
#define MSK_TOK_ECC_FAILED        0x04
#define MSK_TOK_CC_OUTOFRANGE     0x08
#define MSK_TOK_CC_LOCKED         0x10



/* �������� Data Tokens */
#define SD_TOK_READ_STARTBLOCK    0xFE
#define SD_TOK_WRITE_STARTBLOCK   0xFE
#define SD_TOK_READ_STARTBLOCK_M  0xFE
#define SD_TOK_WRITE_STARTBLOCK_M 0xFC
#define SD_TOK_STOP_MULTI         0xFD

/* ������Ӧ���� Data Response Tokens */
#define SD_RESP_DATA_MSK		  0x0F		//������Ӧ����
#define SD_RESP_DATA_ACCETPTED	  0x05		//���ݱ�����
#define SD_RESP_DATA_REJECT_CRC	  0x0B      //����CRC��������ܾ�
#define SD_RESP_DATA_REJECT_WRITE 0x0D		//����д��������ܾ�

/* �ȴ����� Wait Type */
#define SD_WAIT_READ			  0x00		//���ȴ�
#define SD_WAIT_WRITE			  0x01		//д�ȴ�
#define SD_WAIT_ERASE		 	  0x02		//�����ȴ�


/******************************** �������  **************************/
/* ��λSD �� */
#define CMD0 0
#define CMD0_R R1

/* ��OCR�Ĵ��� */
#define CMD1 1
#define CMD1_R R1


#define CMD8 8
#define CMD8_R R7

/* ��CSD�Ĵ��� */
#define CMD9 9
#define CMD9_R R1

/* ��CID�Ĵ���  */
#define CMD10 10
#define CMD10_R R1

/* ֹͣ�����ʱ�����ݴ���  */
#define CMD12 12
#define CMD12_R R1B

/* �� Card_Status �Ĵ���  */
#define CMD13 13
#define CMD13_R R2

/***************************** ������ Block read commands **************************/

/* ���ÿ�ĳ���  */
#define CMD16 16
#define CMD16_R R1

/* ������*/
#define CMD17 17
#define CMD17_R R1

/* �����,ֱ����������CMD12Ϊֹ */
#define CMD18 18
#define CMD18_R R1

/***************************** ��д��� B *************************/
/* д����  */
#define CMD24 24
#define CMD24_R R1

/* д��� */
#define CMD25 25
#define CMD25_R R1

/* дCSD�Ĵ��� */
#define CMD27 27
#define CMD27_R R1

/***************************** д���� *****************************/
#define CMD28 28
#define CMD28_R R1B

#define CMD29 29
#define CMD29_R R1B

#define CMD30 30
#define CMD30_R R1

/***************************** �������� Erase commands *******************************/
/* ���ò��������ʼ��ַ(ֻ����SD��) */
#define CMD32 32
#define CMD32_R R1

/* ���ò��������ֹ��ַ(ֻ����SD��)  */
#define CMD33 33
#define CMD33_R R1

/* ���ò��������ʼ��ַ(ֻ����MMC��) */
#define CMD35 35
#define CMD35_R R1

/* ���ò��������ֹ��ַ(ֻ����MMC��) */
#define CMD36 36
#define CMD36_R R1

/* ������ѡ��Ŀ� */
#define CMD38 38
#define CMD38_R R1B

/***************************** �������� Lock Card commands ***************************/
/* ����/��λ���������/������ */
#define CMD42 42
#define CMD42_R	R1B

/***************************** Ӧ������  ****************/
/* ��ֹ��һ������ΪӦ������*/
#define CMD55 55
#define CMD55_R R1

/* Ӧ�������ͨ��*/
#define CMD56 56
#define CMD56_R R1

/* ��OCR�Ĵ��� */
#define CMD58 58
#define CMD58_R R3

/* ʹ�ܻ��ֹ */
#define CMD59 59
#define CMD59_R R1

/***************************** Ӧ������ ***************/
/* ��ȡ SD Status�Ĵ��� */
#define ACMD13 13
#define ACMD13_R R2

/* �õ���д�뿨�еĿ�ĸ��� */
#define ACMD22 22
#define ACMD22_R R1

/* ��д֮ǰ,����Ԥ�Ȳ����Ŀ�ĸ���  */
#define ACMD23 23
#define ACMD23_R R1

/* ��ȡOCR�Ĵ���  */
#define ACMD41 41
#define ACMD41_R R1

/* ����/�Ͽ�CD/DATA[3]�����ϵ���������  */
#define ACMD42 42
#define ACMD42_R R1

/* ��ȡSCR�Ĵ���  */
#define ACMD51 51
#define ACMD51_R R1

/****************************************Copyright (c)**************************************************
**                               Guangzhou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			sddriver.c
** Last modified Date:	2005-3-10
** Last Version:		V2.0
** Descriptions:		SD/MMC����дģ��: ������ �û�API����(ͷ�ļ�)
**						Soft Packet of SD Card: user API funciton (header file)
**
**------------------------------------------------------------------------------------------------------
** Created by:			Ming Yuan Zheng
** Created date:		2005-1-6
** Version:				V1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			Ming Yuan Zheng
** Modified date:		2005-3-10
** Version:				V2.0
** Descriptions:		�����˶�MMC����֧��,�����˶�UCOS-II��֧��,ʹ��ģ�鲻����������ǰ��̨ϵͳ,��������
**						��UCOS-II��
**------------------------------------------------------------------------------------------------------
** Modified by: 
** Modified date:
** Version:	
** Descriptions: 
**
********************************************************************************************************/

/* SD����Ϣ�ṹ�嶨�� */ 
/* the information structure variable of SD Card*/
typedef struct SD_STRUCT
{	
	UINT8 card_type;
	UINT32 block_num;				/* ���п������ */
	UINT16 block_len;				/* ���Ŀ鳤��(��λ:�ֽ�) */
	UINT16 erase_unit;				/* һ�οɲ����Ŀ���� */
	UINT32 SD_Capability ;           /*SD����������λ���ֽ�*/

	UINT8 timeout_read;			/* ���鳬ʱʱ��(��λ: 8 SPI clock) */
	UINT8 timeout_write;			/* д�鳬ʱʱ��(��λ: 8 SPI clock) */
	UINT8 timeout_erase;			/* ���鳬ʱʱ��(��λ: 8 SPI clock) */
	
}sd_struct;
		/*
		*************************************************************

				�û�API����:	��, д, �� SD��  API������������
 			
		*************************************************************
		*/
		
/* ������ error code */
#define   SD_NO_ERR			     	0x00			// ����ִ�гɹ�
#define   SD_ERR_NO_CARD		 	0x01			// ��û����ȫ���뵽������
#define   SD_ERR_USER_PARAM      	0x02			// �û�ʹ��API����ʱ����ڲ����д���
#define   SD_ERR_CARD_PARAM		 	0x03			// ���в����д����뱾ģ�鲻���ݣ�
#define	  SD_ERR_VOL_NOTSUSP        0x04			// ����֧��3.3V����
#define   SD_ERR_OVER_CARDRANGE		0x05			// ����������������Χ
#define   SD_ERR_UNKNOWN_CARD       0x06			// �޷�ʶ����
#define   SD_ERR_WP_PROTECT         0x82            //д����

#define   SD_Card_Busy              0x07            //�ڶ�ȡOCR�Ĵ���ʱ��⵽BIT31Ϊ0��ʾæ��
#define   SD_BLOCK_ERROR            0x08            //���Сһ��Ҫ���ڵ���512�������޷�����

/* SD������ܷ��صĴ����� */
#define   SD_ERR_CMD_RESPTYPE	 	0x10			// �������ʹ���
#define   SD_ERR_CMD_TIMEOUT     	0x11			// ��������Ӧ��ʱ
#define   SD_ERR_CMD_RESP		 	0x12			// ��������Ӧ����
#define   SD_Power_Up_Busy          0x13			//�ϵ�æ

/* ������������ */
#define   SD_ERR_DATA_CRC16      	0x20			// ������CRC16У�鲻ͨ��
#define   SD_ERR_DATA_START_TOK		0x21			// ���������ʱ�����ݿ�ʼ���Ʋ���ȷ
#define	  SD_ERR_DATA_RESP		 	0x22			// д�������ʱ����������Ӧ���Ʋ���ȷ

/* �ȴ������� */
#define   SD_ERR_TIMEOUT_WAIT    	0x30			// д�������ʱ��������ʱ����
#define   SD_ERR_TIMEOUT_READ    	0x31			// ��������ʱ����
#define	  SD_ERR_TIMEOUT_WRITE	 	0x32			// д������ʱ����
#define   SD_ERR_TIMEOUT_ERASE   	0x33			// ����������ʱ����
#define	  SD_ERR_TIMEOUT_WAITIDLE 	0x34			// ��ʼ����ʱ���ȴ����˳�����״̬��ʱ����

/* д�������ܷ��صĴ����� */
#define	  SD_ERR_WRITE_BLK			0x40			// д�����ݴ���
#define	  SD_ERR_WRITE_BLKNUMS      0x41			// д���ʱ����Ҫд��Ŀ�����ȷд��Ŀ�����һ��
#define   SD_ERR_WRITE_PROTECT		0x42			// ����ǵ�д�������ش���д����λ��

/* ʹ��UCOS-IIʱ���ܳ��ֵĴ��� */
#define   SD_ERR_CREATE_SEMSD		0xA0			// ����SD/MMC�������ź���ʧ��



/**************************************************
	
			�����궨��

***************************************************/
#define	  CARDTYPE_SD				0x00			// ����ΪSD  ��
#define   CARDTYPE_MMC				0x01			// ����ΪMMC ��

#define   SD_UCOSII_SMALLWAIT		256				// ������UCOS-IIʱ��С�ȴ�ʱ��
#define   SD_READREG_TIMEOUT		8				// ���Ĵ����ĳ�ʱʱ��
	

	    /*
	    *************************************************************
	    
	     			����Ϊ�������õ�����SD����صĺ궨��
	     	
	    *************************************************************
	    */
	    	    	    
/* Mask off the bits in the OCR corresponding to voltage range 3.2V to
 * 3.4V, OCR bits 20 and 21 */
#define MSK_OCR_33			 	0xC0

/* �����ڳ�ʼ���׶�,�ȴ�SD���˳�����״̬�����Դ��� */
/* Number of tries to wait for the card to go idle during initialization */
#define SD_IDLE_WAIT_MAX     	50000

/* SD�����ʱʱ��(��λ 8clock)*/
/* timeout of command */
#define SD_CMD_TIMEOUT   	 	100
	
/* 100ms �൱��SPIʱ����(��λ unit: 8 clocks) */
/* 100ms correspond to SPI clock(unit: 8 clocks)*/
#define READ_TIMEOUT_100MS      100 * SPI_CLOCK / 1000 / 8

/* 250ms �൱��SPIʱ����(��λ unit: 8 clocks) */
/* 250ms correspond to SPI clock(unit: 8 clocks)*/
#define WRITE_TIMEOUT_250MS		250 * SPI_CLOCK / 1000 / 8    


/* CSD��һЩ����ֽ�λ��(���ֽ���ǰ) */
#define TAAC_POS 			1			//TACC
#define NSAC_POS			2			//NSAC

#define READ_BL_LEN_POS		5			//READ_BL_LEN

#define C_SIZE_POS1			6			//C_SIZE upper  2-bit
#define C_SIZE_POS2			7			//C_SIZE middle 8-bit
#define C_SIZE_POS3			8			//C_SIZE lower	2-bit

#define C_SIZE_MULT_POS1	9			//C_SIZE_MULT upper 2-bit
#define C_SIZE_MULT_POS2	10			//C_SIZE_MULT lower 1-bit	

#define SECTOR_SIZE_POS1	10			//SECTOR_SIZE upper 5-bit
#define SECTOR_SIZE_POS2	11			//SECTOR_SIZE lower 2-bit

#define R2WFACTOR_POS 		12			//R2WFACTOR_POS

/**/
#define ERASE_GRP_SIZE_POS   	10		//MMC�� ERASE_GRP_SIZE 5-bit
#define ERASE_GRP_MULTI_POS1 	10		//MMC�� ERASE_GRP_MULTI 2-bit
#define ERASE_GRP_MULTI_POS2 	11		//MMC�� ERASE_GRP_MULTI 3-bit


//CSD��һЩ�������
#define TAAC_MSK			0x07		//TACC ������
#define NSAC_MSK			0x78		//NSAC ������

#define READ_BL_LEN_MSK		0x0F		//READ_BL_LEN ������

#define C_SIZE_MSK1			0x03		//C_SIZE ��2λ����
#define C_SIZE_MSK3			0xC0		//C_SIZE ��2λ����

#define C_SIZE_MULT_MSK1 	0x03		//C_SIZE_MULT �ĸ�2λ����
#define C_SIZE_MULT_MSK2 	0x80		//C_SIZE_MULT �ĵ�2λ����

#define R2WFACTOR_MSK		0x1C		//R2WFACTOR ����

#define SECTOR_SIZE_MSK1	0x3F		//SECTOR_SIZE �ĸ�5λ
#define SECTOR_SIZE_MSK2	0x80		//SECTOR_SIZE �ĵ�2λ

#define ERASE_GRP_SIZE_MSK 		0x7C		//MMC�� ERASE_GRP_SIZE ����
#define ERASE_GRP_MULTI_MSK1 	0x03		//MMC�� ERASE_GRP_MULTI ��2λ����
#define ERASE_GRP_MULTI_MSK2 	0xE0		//MMC�� ERASE_GRP_NULTI ��3λ����


#define SD_BLOCK_LEN       512

//extern sd_struct sds; 





/***************************************************************
����SD��״̬�Ķ���
****************************************************************/
struct _SD_STAU
{
 UINT8   SD_Inster; 
 UINT8   SD_Write_Pro;
 UINT8	 SD_Type;
 UINT8	 SD_Cap_Type;
};