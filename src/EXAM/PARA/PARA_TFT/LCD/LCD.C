/********************************** (C) COPYRIGHT ******************************
* File Name          : LCD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : LCD��ʼ������ʾ
*******************************************************************************/



/******************************************************************************/
/* ͷ�ļ����� */
#include <stdio.h>
#include "LCD.H"
#include "FONT.H" 
#include "SYSFREQ.H"
#include "..\PARA\PARA_TFT\CH56X\HAL.H"
                 
/* LCD�Ļ�����ɫ�ͱ���ɫ */       
UINT16 POINT_COLOR=0x0000;                                                      /* ������ɫ */
UINT16 BACK_COLOR=0xFFFF;                                                       /* ����ɫ */ 
_lcd_dev lcddev;                                                                /* ����LCD��Ҫ���� */
    
/*******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : ��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ,ͨ���ú���ת��
* Input          : c- GBR��ʽ����ɫֵ 
* Output         : None
* Return         : RGB��ʽ����ɫֵ
*******************************************************************************/
UINT16 LCD_BGR2RGB(UINT16 c)
{
    UINT16  r,g,b,rgb;   
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;     
    rgb=(b<<11)+(g<<5)+(r<<0);         
    return(rgb);
}

/*******************************************************************************
* Function Name  : LCD_ReadPoint
* Description    : ��ȡ��ĳ�����ɫֵ
* Input          : x,y:���� 
* Output         : None
* Return         : �˵����ɫ
*******************************************************************************/
UINT16 LCD_ReadPoint(UINT16 x,UINT16 y)
{
    UINT16 r=0,g=0,b=0;
    if(x>=lcddev.width||y>=lcddev.height)return 0;                              /* �����˷�Χ,ֱ�ӷ��� */           
    LCD_SetCursor(x,y);        
    if(lcddev.id==0X9341||lcddev.id==0X6804)LCD_WR_REG(0X2E);                   /* 9341/6804 ���Ͷ�GRAMָ�� */
    else LCD_WR_REG(R34);                                                       /* ����IC���Ͷ�GRAMָ�� */
    if(lcddev.id==0X9320)Delay_us(2);                                           /* FOR 9320,��ʱ2us */        
    if( LCD_RD_DATA( ) )r=0;                                                    /* dummy Read */       
    Delay_us(2);      
    r=LCD_RD_DATA( );                                                           /* ʵ��������ɫ */
    if(lcddev.id==0X9341){                                                      /* 9341Ҫ��2�ζ��� */
        Delay_us(2);      
        b=LCD_RD_DATA( ); 
        g=r&0XFF;                                                               /* ����9341,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ */
        g<<=8;
    }
    else if(lcddev.id==0X6804)r=LCD_RD_DATA( );                                 /* 6804�ڶ��ζ�ȡ�Ĳ�����ʵֵ */
    if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505){
        return r;                                                               /* �⼸��ICֱ�ӷ�����ɫֵ */
    }
    else if(lcddev.id==0X9341)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));      /* ILI9341��Ҫ��ʽת��һ�� */
    else return LCD_BGR2RGB(r);                                                 /* ����IC */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : LCD������ʾ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn( void )
{                       
    if(lcddev.id==0X9341||lcddev.id==0X6804)LCD_WR_REG(0X29);                   /* ������ʾ */
    else LCD_WriteReg(R7,0x0173);                                               /* ������ʾ */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : LCD�ر���ʾ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{       
    if(lcddev.id==0X9341||lcddev.id==0X6804)LCD_WR_REG(0X28);                   /* �ر���ʾ */
    else LCD_WriteReg(R7,0x0);//�ر���ʾ 
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : ���ù��λ��
* Input          : Xpos:������
*                   Ypos:������
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(UINT16 Xpos, UINT16 Ypos)
{              
    LCD_WR_REG(lcddev.setxcmd); 
    LCD_WR_DATA(Xpos>>8); 
    LCD_WR_DATA(Xpos&0XFF);     
    LCD_WR_REG(lcddev.setycmd); 
    LCD_WR_DATA(Ypos>>8); 
    LCD_WR_DATA(Ypos&0XFF);
} 

/*******************************************************************************
* Function Name  : LCD_SetWindows
* Description    : ����lcd��ʾ����
* Input          : xStar- ��ʼxλ��
*                  yStar- ��ʼyλ��
*                  xEnd - x����λ��
*                  yEnd - y����λ��
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetWindows(UINT16 xStar, UINT16 yStar,UINT16 xEnd,UINT16 yEnd)
{              
    LCD_WR_REG(lcddev.setxcmd); 
 
    LCD_WR_DATA( xStar>>8);
    LCD_WR_DATA( xStar&0xFF);
    LCD_WR_DATA(xEnd>>8); 
    LCD_WR_DATA(xEnd&0XFF);     

    LCD_WR_REG(lcddev.setycmd); 
 
    LCD_WR_DATA( yStar>>8 );
    LCD_WR_DATA( yStar&0xFF );
    LCD_WR_DATA( yEnd>>8 ); 
    LCD_WR_DATA( yEnd&0XFF );
#if 1
    LCD_WR_REG( 0x3A );    // 16/18 bits
    LCD_WR_DATA( 0x55 );
#endif
    LCD_WR_REG( lcddev.wramcmd );
}

/*******************************************************************************
* Function Name  : LCD_DrawPoint
* Description    : ����
* Input          : x,y:����
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawPoint(UINT16 x,UINT16 y)
{
    LCD_SetCursor(x,y);                                                         /* ���ù��λ�� */ 
    LCD_WriteRAM_Prepare();                                                     /* ��ʼд��GRAM */
    LCD_WR_DATA( POINT_COLOR ); 
}

/*******************************************************************************
* Function Name  : LCD_Fast_DrawPoint
* Description    : ���ٻ���
* Input          : x,y:����
*                   color:��ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Fast_DrawPoint(UINT16 x,UINT16 y,UINT16 color)
{       
    if(lcddev.id==0X9341||lcddev.id==0X6804){
        LCD_WR_REG(lcddev.setxcmd); 
        LCD_WR_DATA(x>>8); 
        LCD_WR_DATA(x&0XFF);     
        LCD_WR_REG(lcddev.setycmd); 
        LCD_WR_DATA(y>>8); 
        LCD_WR_DATA(y&0XFF);
    }
    else{
        if(lcddev.dir==1)x=lcddev.width-1-x;                                   /* ������ʵ���ǵ�תx,y���� */
        LCD_WriteReg(lcddev.setxcmd,x);
        LCD_WriteReg(lcddev.setycmd,y);
    }             
    LCD_WR_REG( lcddev.wramcmd ); 
    LCD_WR_DATA( color );
}     

/*******************************************************************************
* Function Name  : LCD_Display_Dir
* Description    : ����LCD��ʾ����
* Input          : dir:0,������1,����
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Display_Dir(UINT8 dir)
{
    if(dir==0){     /* ���� */
        lcddev.width=320;
        lcddev.height=480;
        lcddev.wramcmd=0X2C;
        lcddev.setxcmd=0X2A;
        lcddev.setycmd=0X2B; 
        LCD_WriteReg(0x36,0x02); //������,���ϵ���
    }
    else{          /* ���� */		  
        lcddev.width=480;
        lcddev.height=320;
        lcddev.wramcmd=0X2C;
        lcddev.setxcmd=0X2A;
        lcddev.setycmd=0X2B;     
        LCD_WriteReg(0x36,0x23); //���µ���,���ҵ���
    } 
}

/*******************************************************************************
* Function Name  : LCD_RESET
* Description    : ���ڸ�λlcd
* Input          : s-(0:�͵�ƽ,1:�ߵ�ƽ)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_RESET( UINT8 s )
{
#if 1
    R32_PB_DIR |= (1<<2); 
    R32_PB_PU |= (1<<2); 
    if(s) R32_PB_OUT |= (1<<2);
    else  R32_PB_CLR |= (1<<2); 
    R32_PB_DIR &= ~(1<<2); 

#else
    R32_PD_DIR |= led_control; 
    R32_PD_PU |= led_control; 
    if(s) R32_PD_OUT |= led_control;
    else  R32_PD_CLR |= led_control; 
    R32_PD_DIR &= ~led_control; 
#endif
}

/*******************************************************************************
* Function Name  : LCD_Init
* Description    : ��ʼ��lcd
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Init(void)
{                                       
    Delay_ms(50);     
    LCD_RESET( 0 );
    Delay_ms(200);                                                              /* reset */
    LCD_RESET( 1 );
    Delay_ms(800);  
    PRINT("LCD INIT...\r\n");
 
    lcddev.id=0x6804;             
    PRINT(" LCD ID:%x\r\n",lcddev.id);                                          /* ��ӡLCD ID */

    LCD_WR_REG(0x0011);
    Delay_ms(20);
    LCD_WR_REG(0x00D0);    
    LCD_WR_DATA(0x0007); 
    LCD_WR_DATA(0x0041); 
    LCD_WR_DATA(0x001C); 
     
    LCD_WR_REG(0x00D1);    
    LCD_WR_DATA(0x0000); 
    LCD_WR_DATA(0x0036);   // 30 vcm
    LCD_WR_DATA(0x001B);   // 15 vdv
                                                                     
    LCD_WR_REG(0x00D2);
    LCD_WR_DATA(0x0001);   // AP0[2:0]
    LCD_WR_DATA(0x0011);   // DC10[2:0],DC00[2:0]
      
    LCD_WR_REG(0x00C0);    //*************
    LCD_WR_DATA(0x0010);   // REV & SM & GS
    LCD_WR_DATA(0x003B);   // NL[5:0]
    LCD_WR_DATA(0x0000);   // SCN[6:0]
    LCD_WR_DATA(0x0012);   // 02 NDL , PTS[2:0]
    LCD_WR_DATA(0x0001);   // 11 PTG , ISC[3:0]  
      
    LCD_WR_REG(0x00C5);    
    LCD_WR_DATA(0x0003);
    
    LCD_WR_REG(0x00C8);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0057);
    LCD_WR_DATA(0x0033);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0044);
    LCD_WR_DATA(0x0002);
    LCD_WR_DATA(0x0077);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
      
    LCD_WR_REG(0x00F8);
    LCD_WR_DATA(0x0001);
      
    LCD_WR_REG(0x00FE);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0002);
      
    LCD_WR_REG(0x0036); 
    LCD_WR_DATA(0x000A); 
    Delay_ms(20);
    LCD_WR_REG(0x003a);  
    LCD_WR_DATA(0x0005);
    Delay_ms(20);  
    
    LCD_WR_REG(0x0029); 
    LCD_WR_REG(0x002c);

    lcddev.dir=0;                                                               /* ���� */
    LCD_Display_Dir( lcddev.dir );
    LCD_Clear(BLACK);
    Delay_ms(800);  
} 

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : ��������
* Input          : color-Ҫ���������ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(UINT16 color)
{
    UINT32 index=0;      
    UINT32 totalpoint=lcddev.width;    
    totalpoint*=lcddev.height;                                                  /* �õ��ܵ��� */
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
    LCD_WriteRAM_Prepare( );                                                    /* ��ʼд��GRAM */
    for(index=0;index<totalpoint;index++)
    {
        LCD_WR_DATA( color );
    }
}

/*******************************************************************************
* Function Name  : LCD_Fill
* Description    : ��ָ����������䵥����ɫ
* Input          : (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1) 
*                  color:Ҫ������ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Fill(UINT16 sx,UINT16 sy,UINT16 ex,UINT16 ey,UINT16 color)
{          
    UINT16 i,j;
    UINT16 xlen=0;
    xlen=ex-sx+1;       
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);                                                    /* ���ù��λ�� */ 
        LCD_WriteRAM_Prepare();                                                 /* ��ʼд��GRAM    */  
        for(j=0;j<xlen;j++) LCD_WR_DATA(color);                                 /* ���ù��λ�� */         
    }
}

/*******************************************************************************
* Function Name  : LCD_Color_Bulk_Fill
* Description    : ��ָ�����������ָ����ɫ��
* Input          : (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
                   color:Ҫ������ɫ
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Color_Bulk_Fill(UINT16 sx,UINT16 sy,UINT16 ex,UINT16 ey,UINT16 *color)
{  
    UINT16 height,width;
    UINT16 i,j;

    width=ex-sx+1;                                                              /* �õ����Ŀ��� */
    height=ey-sy+1;                                                             /* �߶� */
    for(i=0;i<height;i++){
        LCD_SetCursor(sx,sy+i);                                                 /* ���ù��λ�� */ 
        LCD_WriteRAM_Prepare();                                                 /* ��ʼд��GRAM */
        for(j=0;j<width;j++) LCD_WR_DATA( color[i*height+j] );                  /* д������ */
    }      
}

/*******************************************************************************
* Function Name  : LCD_Color_Fill
* Description    : ��ָ����������������ɫ
* Input          : (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Color_Fill(UINT16 sx,UINT16 sy,UINT16 ex,UINT16 ey)
{  
    UINT16 height,width;
    UINT16 i,j;

    width=ex-sx+1;                                                              /* �õ����Ŀ��� */
    height=ey-sy+1;                                                             /* �߶� */
    for(i=0;i<height;i++)
    {
        LCD_SetCursor(sx,sy+i);                                                 /* ���ù��λ�� */ 
        LCD_WriteRAM_Prepare();                                                 /* ��ʼд��GRAM */
        for(j=0;j<width;j++) LCD_WR_DATA( i*height+j*width );                   /* д������ */ 
    }      
}

/*******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : ����
* Input          : x1,y1:�������
*                  x2,y2:�յ����� 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawLine(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2)
{
    UINT16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance; 
    int incx,incy,uRow,uCol; 

    delta_x=x2-x1;                                                              /* ������������ */ 
    delta_y=y2-y1; 
    uRow=x1; 
    uCol=y1; 
    if(delta_x>0)incx=1;                                                        /* ���õ������� */
    else if(delta_x==0)incx=0;                                                  /* ��ֱ�� */ 
    else {incx=-1;delta_x=-delta_x;} 
    if(delta_y>0)incy=1; 
    else if(delta_y==0)incy=0;                                                  /* ˮƽ�� */
    else{incy=-1;delta_y=-delta_y;} 
    if( delta_x>delta_y)distance=delta_x;                                       /* ѡȡ�������������� */ 
    else distance=delta_y; 
    for(t=0;t<=distance+1;t++ ){                                                /* ������� */ 
        LCD_DrawPoint(uRow,uCol);                                               /* ���� */ 
        xerr+=delta_x ; 
        yerr+=delta_y ; 
        if(xerr>distance){ 
            xerr-=distance; 
            uRow+=incx; 
        } 
        if(yerr>distance){ 
            yerr-=distance; 
            uCol+=incy; 
        } 
    }  
}

/*******************************************************************************
* Function Name  : LCD_DrawRectangle
* Description    : ������
* Input          : x1,y1:�������
*                  x2,y2:�յ����� 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawRectangle(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2)
{
    LCD_DrawLine(x1,y1,x2,y1);
    LCD_DrawLine(x1,y1,x1,y2);
    LCD_DrawLine(x1,y2,x2,y2);
    LCD_DrawLine(x2,y1,x2,y2);
}

/*******************************************************************************
* Function Name  : Draw_Circle
* Description    : ��ָ��λ�û�һ��ָ����С��Բ
* Input          : (x,y):���ĵ�
*                  r    :�뾶
* Output         : None
* Return         : None
*******************************************************************************/
void Draw_Circle(UINT16 x0,UINT16 y0,UINT8 r)
{
    int a,b;
    int di;
    a=0;b=r;      
    di=3-(r<<1);                                                                /* �ж��¸���λ�õı�־ */
    while(a<=b)
    {
        LCD_DrawPoint(x0+a,y0-b);             //5
        LCD_DrawPoint(x0+b,y0-a);             //0           
        LCD_DrawPoint(x0+b,y0+a);             //4               
        LCD_DrawPoint(x0+a,y0+b);             //6 
        LCD_DrawPoint(x0-a,y0+b);             //1       
        LCD_DrawPoint(x0-b,y0+a);             
        LCD_DrawPoint(x0-a,y0-b);             //2             
        LCD_DrawPoint(x0-b,y0-a);             //7                  
        a++;
        /* ʹ��Bresenham�㷨��Բ */     
        if(di<0) di +=4*a+6;      
        else{
            di+=10+4*(a-b);   
            b--;
        }                             
    }
}

/*******************************************************************************
* Function Name  : LCD_ShowChar
* Description    : ��ָ��λ����ʾһ���ַ�
* Input          : x,y:��ʼ����
*                  num:Ҫ��ʾ���ַ�:" "--->"~"
*                  size:�����С 12/16
*                  mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ShowChar(UINT16 x,UINT16 y,UINT8 num,UINT8 size,UINT8 mode)
{                                
    UINT8 temp,t1,t;
    UINT16 y0=y;
    UINT16 colortemp=POINT_COLOR;                       
    /* ���ô��� */           
    num=num-' ';                                                                /* �õ�ƫ�ƺ��ֵ */
    if(!mode){                                                                   /* �ǵ��ӷ�ʽ */
        for(t=0;t<size;t++){
            if(size==12)temp=asc2_1206[num][t];                                 /* ����1206���� */
            else temp=asc2_1608[num][t];                                        /* ����1608���� */                              
            for(t1=0;t1<8;t1++){
                if(temp&0x80)POINT_COLOR=colortemp;
                else POINT_COLOR=BACK_COLOR;
                LCD_DrawPoint(x,y);    
                temp<<=1;
                y++;
                if(x>=lcddev.width){                                            /* �������� */
                    POINT_COLOR=colortemp;
                    return;
                }              
                if((y-y0)==size){
                    y=y0;
                    x++;
                    if(x>=lcddev.width){                                         /* �������� */
                        POINT_COLOR=colortemp;
                        return;
                    }         
                    break;
                }
            }
                   
        }    
    }
    else{                                                                       /* ���ӷ�ʽ */
        for(t=0;t<size;t++){
            if(size==12)temp=asc2_1206[num][t];                                 /* ����1206���� */
            else temp=asc2_1608[num][t];                                        /* ����1608���� */                               
            for(t1=0;t1<8;t1++){
                if(temp&0x80)LCD_DrawPoint(x,y); 
                temp<<=1;
                y++;
                if(x>=lcddev.height){                                           /* �������� */
                    POINT_COLOR=colortemp;
                    return;
                }
                if((y-y0)==size){
                    y=y0;
                    x++;
                    if(x>=lcddev.width){                                        /* �������� */
                        POINT_COLOR=colortemp;
                        return;
                    }
                    break;
                }
            }       
        }     
    }
    POINT_COLOR=colortemp;                          
}

/*******************************************************************************
* Function Name  : LCD_Pow
* Description    : ��������
* Input          : m^n����
* Output         : None
* Return         : ����ֵ:m^n�η�.
*******************************************************************************/
UINT32 LCD_Pow(UINT8 m,UINT8 n)
{
    UINT32 result=1;     
    while(n--)result*=m;    
    return result;
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : ��ʾ����,��λΪ0,����ʾ
* Input          : x,y :�������     
*                  len :���ֵ�λ��
*                  size:�����С
*                  color:��ɫ 
*                  num:��ֵ(0~4294967295);     
* Output         : None
* Return         : ����ֵ:m^n�η�.
*******************************************************************************/
void LCD_ShowNum(UINT16 x,UINT16 y,UINT32 num,UINT8 len,UINT8 size)
{             
    UINT8 t,temp;
    UINT8 enshow=0;                           
    for(t=0;t<len;t++){
        temp=(num/LCD_Pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1)){
            if(temp==0){
                LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
                continue;
            }
            else 
                enshow=1; 
        }
        LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
    }
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : ��ʾ����,��λΪ0,������ʾ
* Input          : x,y :�������     
*                  len :���ֵ�λ��
*                  size:�����С
*                  color:��ɫ 
*                  num:��ֵ(0~999999999)
*                  mode:[7]:0,�����;1,���0.
                        [6:1]:����
                        [0]:0,�ǵ�����ʾ;1,������ʾ.
* Output         : None
* Return         : ����ֵ:m^n�η�.
*******************************************************************************/
void LCD_ShowxNum(UINT16 x,UINT16 y,UINT32 num,UINT8 len,UINT8 size,UINT8 mode)
{  
    UINT8 t,temp;
    UINT8 enshow=0;                           
    for(t=0;t<len;t++){
        temp=(num/LCD_Pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1)){
            if(temp==0){
                if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
                else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
                continue;
            }
            else 
                enshow=1; 
        }
        LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
    }
} 

/*********************************** endfile **********************************/