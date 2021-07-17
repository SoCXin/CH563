/********************************** (C) COPYRIGHT ******************************
* File Name          : CHINESE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : ������ʾ
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include "SYSFREQ.H"
#include "CHINESE.H"
#include "CHFONT.H"
#include "..\PARA\PARA_TFT\LCD\LCD.H"       
            
/*******************************************************************************
* Function Name  : LCD_Draw_Point
* Description    : ��LCD�ϵģ�x��y��������
* Input          : x,y:����
*                  color:�����ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Draw_Point(UINT16 x,UINT16 y,UINT16 color)
{
    UINT16 temp;

    temp=POINT_COLOR;
    POINT_COLOR=color;
    LCD_DrawPoint(x,y);
    POINT_COLOR=temp;
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font16
* Description    : ��ָ��λ�� ��ʾ1��16*16�ĺ���
* Input          : x,y:������ʾ��λ��
*                  s  :�����ַ����׵�ַ
*                  color:���ֵ���ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font16(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);    
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1))){
                for(t=0;t<32;t++){                                              /* ÿ��16*16�ĺ��ֵ��� ��32���ֽ� */
                    temp=tfont16[k].Msk[t];                    
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* ��ʵ�ĵ� */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* ���հ׵㣨ʹ�ñ���ɫ�� */
                        temp<<=1;
                        y++;
                        if((y-y0)==16){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                }   
            }
        }
        s+=2;
    }            
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font24
* Description    : ��ָ��λ�� ��ʾ1��24*24�ĺ���
* Input          : x,y:������ʾ��λ��
*                  s  :�����ַ����׵�ַ
*                  color:���ֵ���ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font24(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1))){
                for(t=0;t<72;t++){                                              /* ÿ��24*24�ĺ��ֵ��� ��72���ֽ� */
                    temp=tfont24[k].Msk[t];                                     /* ǰ24���ֽ� */
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* ��ʵ�ĵ� */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* ���հ׵㣨ʹ�ñ���ɫ�� */
                        temp<<=1;
                        y++;
                        if((y-y0)==24){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font32
* Description    : ��ָ��λ�� ��ʾ1��32*32�ĺ���
* Input          : x,y:������ʾ��λ��
*                  s  :�����ַ����׵�ַ
*                  color:���ֵ���ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font32(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1))){
                for(t=0;t<128;t++){                                             /* ÿ��32*32�ĺ��ֵ��� ��128���ֽ� */
                    temp=tfont32[k].Msk[t];           
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* ��ʵ�ĵ� */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* ���հ׵㣨ʹ�ñ���ɫ�� */
                        temp<<=1;
                        y++;
                        if((y-y0)==32){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font48
* Description    : ��ָ��λ�� ��ʾ1��48*48�ĺ���
* Input          : x,y:������ʾ��λ��
*                  s  :�����ַ����׵�ַ
*                  color:���ֵ���ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font48(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont48)/sizeof(typFNT_GB48);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont48[k].Index[0]==*(s))&&(tfont48[k].Index[1]==*(s+1))){
                for(t=0;t<228;t++){                                             /* ÿ��48*48�ĺ��ֵ��� ��228���ֽ� */
                    temp=tfont48[k].Msk[t];           
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* ��ʵ�ĵ� */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* ���հ׵㣨ʹ�ñ���ɫ�� */
                        temp<<=1;
                        y++;
                        if((y-y0)==48){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font64
* Description    : ��ָ��λ�� ��ʾ1��64*64�ĺ���
* Input          : x,y:������ʾ��λ��
*                  s  :�����ַ����׵�ַ
*                  color:���ֵ���ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font64(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;

    HZnum=sizeof(tfont64)/sizeof(typFNT_GB64);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont64[k].Index[0]==*(s))&&(tfont64[k].Index[1]==*(s+1))){
                for(t=0;t<512;t++){                                             /* ÿ��64*64�ĺ��ֵ��� ��512���ֽ� */
                    temp=tfont64[k].Msk[t];           
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* ��ʵ�ĵ� */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* ���հ׵㣨ʹ�ñ���ɫ��*/
                        temp<<=1;
                        y++;
                        if((y-y0)==64){
                             y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font_EX
* Description    : ��ָ��λ�� ��ʾ1��32*32�ĺ���
* Input          : x,y:������ʾ��λ��
*                  s  :�����ַ����׵�ַ
*                  color:���ֵ���ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font_EX(UINT16 x,UINT16 y,UINT8 num,UINT16 color)
{                   
    UINT8 temp,t,t1;
    UINT16 y0=y;    

    for(t=0;t<128;t++){                                                        /* ÿ��32*32�ĺ��ֵ��� ��128���ֽ� */
        temp=tfont_ex[num].Msk[t];           
        for(t1=0;t1<8;t1++){
            if(temp&0x80)LCD_Draw_Point(x,y,color);                            /* ��ʵ�ĵ� */
            else LCD_Draw_Point(x,y,BACK_COLOR);                               /* ���հ׵㣨ʹ�ñ���ɫ��*/
            temp<<=1;
            y++;
            if((y-y0)==32){
                y=y0;
                x++;
                break;
            }
        }    
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font64
* Description    : ���Ժ�����ʾ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TEST_FONT(void)
{
    BACK_COLOR=WHITE;
    POINT_COLOR = BLUE; 
    LCD_ShowString(50,20,200,16,16,"WCH-CH563...");    
    POINT_COLOR = BLACK; 
    LCD_ShowString(50,40,200,16,16,"WCH-CH563...");    
    Delay_ms(500);
    Test_Show_CH_Font24(50,70,"���ֲ���",BLUE);
    Test_Show_CH_Font24(50,100,"���ֲ���",BLACK);
    Delay_ms(500);
    Test_Show_CH_Font32(10,140,"����Һ������ʾ",BLUE);
    Test_Show_CH_Font32(10,180,"����Һ������ʾ",BLACK);
    Delay_ms(500);
    Test_Show_CH_Font_EX(260,30,0,RED);
    Delay_ms(500);
    Test_Show_CH_Font_EX(260,70,1,GREEN);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,110,2,BLACK);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,150,3,GRAY);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,190,4,GRAY75);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,230,5,GRAY50);
    Delay_ms(50);
}

/*********************************** endfile **********************************/