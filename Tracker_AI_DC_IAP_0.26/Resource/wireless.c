#include "stm32f10x_it.h"
#include "wireless.h"
#include "rtc.h"
#include "bsp.h"
#include "iap.h"
#include "ucos_ii.h"
#include "lora.h"
#include "user485.h"




#define sw16(x)\
((short)(\
(((short)(x)&(short)0x00ffU)<<8)|\
(((short)(x)&(short)0xff00U)>>8)))

extern Tracker tracker;
extern TIME tempTime;
extern Wireless wireless;
extern UFlag flag;
extern unsigned char *pOut[256];
  			
extern char TrVersion[];	//mian.c

unsigned short DeviceType_YG = 0x105D;//�������⳧��ʶ���豸����
unsigned int TrType_YG = 0x00200100;	//ϵͳ���ͣ����� ���� ֱ�� 1ͨ�� 0��

unsigned short TrType = 1;
//������· //����Ϊһ·��������·���ݴ�����Ϊ����
unsigned short TrBracketNum = 3;    //1;	//3;
unsigned short reserved = 0;


extern uint8_t set_buf[50];
extern uint8_t ret_buf[300];
extern uint8_t trans_flag;
extern uint8_t trans_len;
extern uint8_t ret_len;
extern uint8_t trans_ret_flag;

extern U485 r485;
extern uint8_t set_buf[50];


uint16_t *ExComAddList[28]=
{   
    (uint16_t *)&TrVersion,								//40000  ����汾�ţ�V1.02�������˶Ի�Ϊ������ļ���
    ((uint16_t *)&TrVersion)+1,							//40001	
    (uint16_t *)&TrType,								//40002  ���������� 0��б�� 1��������ƽ�� 2���ϱ���ƽ�� 3��˫��
    (uint16_t *)&TrBracketNum,							//40003  �������������Ƶ�֧������
    &reserved,
    &reserved,
    (uint16_t *)&tracker.ExInterface.Mode,    	 		//40006  ����ģʽ 0��ά��ģʽ 1���Զ�ģʽ 2���ֶ�ģʽ 3���ܷ�ģʽ 4����ѩģʽ 5������ģʽ
    (uint16_t *)&tracker.ExInterface.Wspeed,			//40007  ���� ��С����1m/s
                
    (uint16_t *)&tracker.ExInterface.Wdirect,			//40008  ����  ��С����1��
    &reserved,
    (uint16_t *)&tracker.SysTime.Year,	      			//40010  �� ��Χ2000~2069
    (uint16_t *)&tracker.SysTime.Month,       			//40011  ��     1~12
    (uint16_t *)&tracker.SysTime.Day,	        		//40012	 ��     1~31
            
    (uint16_t *)&tracker.SysTime.Hour,   				//40013  ʱ     0~23	 
    (uint16_t *)&tracker.SysTime.Minute,      			//40014  ��     0~59
    (uint16_t *)&tracker.SysTime.Second,      			//40015  ��     0~59
    &reserved,
    (uint16_t *)&tracker.ExInterface.TargetAngle,		//40017 Ŀ��ת�� ���� 0.01��
    (uint16_t *)&tracker.ExInterface.Tswtich,			//40018  ��ͣ���� 0����������  1��ֹͣ	
    &reserved,                                          //40019
    ((uint16_t *)&tracker.ExInterface.Tstatus)+1,		//40020  ������״̬
    (uint16_t *)&tracker.ExInterface.Tstatus,			//40021
    
    
    &reserved,   
    (uint16_t *)&tracker.ExInterface.Cangle1,		 	//40023 ͨ��1ת��    ��Χ0~180 ����0.01��
    &reserved,		
    (uint16_t *)&tracker.ExInterface.Cangle2,	 	 	//40025 ͨ��2ת��    ��Χ0~180 ����0.01��
    &reserved,	
    (uint16_t *)&tracker.ExInterface.Cangle3,		 	//40027 ͨ��3ת��    ��Χ0~180 ����0.01��
};

uint16_t *ExComAddList_YG[30]=
{   
    (uint16_t *)&DeviceType_YG,                         //28999  �豸����,�̶�ֵ0x105D,�������⳧��ʶ���豸����
    (uint16_t *)&tracker.DeviceID,                      //29000  �豸ID 1~255
    (uint16_t *)&TrVersion,                             //29001  �汾��Ϣ������汾�ţ����ȷ�������ֽ�
    ((uint16_t *)&TrVersion)+1,                         //29002
    ((uint16_t *)&TrType_YG)+1,                         //29003  ϵͳ����
    (uint16_t *)&TrType_YG,                             //29004
    (uint16_t *)&TrBracketNum,                          //29005  ֧��������
    &reserved,                                          //29006  reserved
    &reserved,                                          //29007  reserved
    (uint16_t *)&tracker.ExInterface.Mode,              //29008  ����ģʽ 0��ά��ģʽ 1���Զ�ģʽ 2���ֶ�ģʽ 3���ܷ�ģʽ 4����ѩģʽ 5������ģʽ 6��ֹͣģʽ
    (uint16_t *)&tracker.ExInterface.Wspeed,            //29009  ���� ��С����1m/s
    (uint16_t *)&tracker.ExInterface.Wdirect,           //29010  ���� ��С����1��
    &reserved,                                          //29011  reserved
    (uint16_t *)&tracker.SysTime.Year,                  //29012  �� ��Χ2000~2069
    (uint16_t *)&tracker.SysTime.Month,                 //29013  ��     1~12
    (uint16_t *)&tracker.SysTime.Day,                   //29014	 ��     1~31
    (uint16_t *)&tracker.SysTime.Hour,                  //29015  ʱ     0~23	 
    (uint16_t *)&tracker.SysTime.Minute,                //29016  ��     0~59
    (uint16_t *)&tracker.SysTime.Second,                //29017  ��     0~59
    &reserved,                                          //29018  Ŀ����� int16 ����0.01��
    (uint16_t *)&tracker.ExInterface.TargetAngle,       //29019  Ŀ��ת�� int16 ����0.01��
    &reserved,                                          //29020  reserved
    ((uint16_t *)&tracker.SysErrMessage1.ErrMessage)+1, //29021  ������״̬(������Ϣ)
    (uint16_t *)&tracker.SysErrMessage1.ErrMessage,     //29022
    &reserved,                                          //29023  ֧��1��� int16 ��Χ[0,90]   ����0.01��
    (uint16_t *)&tracker.ExInterface.CAngle1_YG,        //29024  ֧��1ת�� int16 ��Χ[0,180]  ����0.01��
    &reserved,                                          //29025  ֧��2��� int16 ��Χ[0,90]   ����0.01��
    (uint16_t *)&tracker.ExInterface.CAngle2_YG,        //29026  ֧��2ת�� int16 ��Χ[0,180]  ����0.01��
    &reserved,                                          //29027  ֧��3��� int16 ��Χ[0,90]   ����0.01��
    (uint16_t *)&tracker.ExInterface.CAngle3_YG,        //29028  ֧��4ת�� int16 ��Χ[0,180]  ����0.01��
};

//**************************************************************************************************
// Function Name  : Data_Analyze
// Description    : 
// Input          : None
// Output         : None
// Return         : None
//**************************************************************************************************
unsigned char Data_Analyze(COMMU *com, unsigned char ID)
{
    unsigned short i = 0;
    unsigned char value = 0;
    unsigned short crcValue = 0;
    unsigned short startAdd = 0;
    unsigned short dataLen = 0;
    unsigned short endAdd = 0;
    unsigned short temp16Value = 0;
    unsigned short tempPos = 0;
    short tempvalue = 0;
    //TIME tempTime;
    
    int32_t secdiff = 0;
    
    if(com->RxDataCnt>=1)
    {
        if(com->TimeOutCnt++>=30)
        {
            //lora
            USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
            Reserved485_TX_EN();
            for(i=0;i<com->RxDataCnt;i++)
            {
                USART_SendData(USART2,com->RxBuf[i]);
                while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!=SET);
            }
            while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET); 
            Reserved485_RX_EN();
            USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
            
            LORA_MD0(0);
            LORA_MD1(0);
            //lora
            
            com->TimeOutCnt = 0;
            value = LONG_NO_CONNECT;
        }

		//if(com->RxDataCnt>=8) // && (com->RxBuf[0]==tracker.DeviceID || com->RxBuf[0]==0))
		if(com->RxDataCnt>=6)
        {
			com->TxBuf[0] = com->RxBuf[0];  // �豸��ַ
			com->TxBuf[1] = com->RxBuf[1];  // ���� 
	
	        // �������		       
            switch(com->RxBuf[1])    
	        {
				case READ_REG:		// ���Ĵ���
					if(com->RxBuf[0]==tracker.DeviceID)
					{
						crcValue = GetCRC16CheckCode(com->RxBuf, 6);
						if((crcValue&0xFF)==com->RxBuf[6] && ((crcValue>>8)&0xFF)==com->RxBuf[7])  // CRCУ����ȷ
						{
 							com->TxBuf[2] = com->RxBuf[5] << 1; // �������ݳ���
	
							crcValue = com->RxBuf[2];
							crcValue = ((crcValue << 8) | com->RxBuf[3]) * 2;
							
							startAdd =  com->RxBuf[2];
							startAdd = (((startAdd << 8) | com->RxBuf[3]));
							
							dataLen = com->RxBuf[4];
							dataLen <<= 8;
							dataLen |= com->RxBuf[5];
							
							endAdd = startAdd + dataLen-1;
							
							if(startAdd<28999)
							{
								for(i=0;i<com->TxBuf[2];i++)
								{
									com->TxBuf[3 + i] = *pOut[crcValue + i];
								}
							}
							else if(startAdd >= 28999 && endAdd <= 29028)
							{
								startAdd-=28999;
								for(i=0;i<com->RxBuf[5];i++)
								{
//									if(startAdd+i == 0)//�豸���ͣ�����ʶ���룬���ֽ��ȷ�
//									{
//										com->TxBuf[2*i+3] = *ExComAddList_YG[startAdd+i];
//										com->TxBuf[2*i+4] = (*ExComAddList_YG[startAdd+i])>>8;
//									}
									//else if(startAdd+i == 2 || startAdd+i == 3)//����汾�ţ����ȷ��� �ַ�������ֽ�
									if(startAdd+i == 2 || startAdd+i == 3)//����汾�ţ����ȷ��� �ַ�������ֽ�
									{
										com->TxBuf[2*i+3] = *ExComAddList_YG[startAdd+i];
										com->TxBuf[2*i+4] = (*ExComAddList_YG[startAdd+i])>>8;
									}
									else
									{
										com->TxBuf[2*i+3] = (*ExComAddList_YG[startAdd+i])>>8;
										com->TxBuf[2*i+4] = *ExComAddList_YG[startAdd+i];
									}
								}
							}
							
							else if(startAdd >= 40000 && endAdd <= 40027)
							{//���Ÿ���������
								//���ݸ���
								//tracker.ExInterface.Mode = 0;
								
//								tracker.ExInterface.Wspeed = 0;
//								tracker.ExInterface.Wdirect = 0;
//								tracker.ExInterface.Tswtich = 0;
//								tracker.ExInterface.TargetAngle = 0;
//								tracker.ExInterface.Tstatus = 0;
//								tracker.ExInterface.Cangle1 = 0;
//								tracker.ExInterface.Cangle2 = 0;
//								tracker.ExInterface.Cangle3 = 0;


/*2016-05-30 ע��*/
//								if(tracker.SysErrMessage1.BitEM.QJ2_NoCom==1 || flag.BitFlag.M1OverCurrent==1)
//								{//������״̬����
//									tracker.ExInterface.Tstatus = 2;
//								}
/*2016-05-30 ע�� end*/
/*2016-05-30 ����*/	
								//M1 QJ1 -Сƽ����Ŀ�õ���·����һ·������
//								if(tracker.SysErrMessage1.BitEM.QJ1_NoCom==1 || flag.BitFlag.M0OverCurrent==1)
//								{//������״̬����
//									tracker.ExInterface.Tstatus |= 1<<0;
//								}
//								else
//								{
//									tracker.ExInterface.Tstatus &= ~(1<<0);
//								}
								//M2 QJ2 -Сƽ����Ŀ�õ���·���ڶ�·������
//								if(tracker.SysErrMessage1.BitEM.QJ2_NoCom==1 || flag.BitFlag.M1OverCurrent==1)
//								{//������״̬����
//									tracker.ExInterface.Tstatus |= 1<<1;
//								}
//								else
//								{
//									tracker.ExInterface.Tstatus &= ~(1<<1);
//								}
								// ����һ·������Ϊ��һ·������Ϊ����
								if(tracker.SysErrMessage1.BitEM.QJ0_NoCom == 1 || flag.BitFlag.M0OverCurrent == 1 || tracker.SysErrMessage1.BitEM.RemoteTimeError == 1)
								{//������״̬����
									tracker.ExInterface.Tstatus |= 1<<0; //1<<2;
								}
								else
								{
									tracker.ExInterface.Tstatus &= ~(1<<0); //~(1<<2);
								}
/*2016-05-30 ���� end*/									
								//�������
								startAdd-=40000;
								for(i=0;i<com->RxBuf[5];i++)
								{
									if(startAdd+i == 0 || startAdd+i == 1)//����汾�ţ����ȷ������� �͵�ַ
									{
										com->TxBuf[2*i+3] = *ExComAddList[startAdd+i];
										com->TxBuf[2*i+4] = (*ExComAddList[startAdd+i])>>8;
									}
									else
									{
										//com->TxBuf[3 + i] = *pOut[crcValue + i];
										com->TxBuf[2*i+3] = (*ExComAddList[startAdd+i])>>8;
										com->TxBuf[2*i+4] = *ExComAddList[startAdd+i];
									}
								}
							}
							else
							{//��ַ����Χ
								com->TxBuf[0] = tracker.DeviceID;
								com->TxBuf[1] = READ_REG+0x80;
								com->TxBuf[2] = 2;
								crcValue = GetCRC16CheckCode(com->TxBuf, 3);
								com->TxBuf[3] = crcValue & 0xFF;
								com->TxBuf[4] = (crcValue >> 8) & 0xFF;
		
								com->TxDataCnt = 5;
								
								value = TRACKER_RD_FINISHED;
								if(value!=NO_CONNECT)
								{
									com->TimeOutCnt = 0;
									com->RxDataCnt = 0;

									if(ID==1)
									{
										DMA_Cmd(DMA1_Channel5, DISABLE);
										DMA1_Channel5->CNDTR = BUFFERSIZE;
										DMA_Cmd(DMA1_Channel5, ENABLE);
									}
								}
								return value;
							}
	
							crcValue = GetCRC16CheckCode(com->TxBuf, (com->TxBuf[2] + 3));
							com->TxBuf[com->TxBuf[2] + 3] = crcValue & 0xFF;
							com->TxBuf[com->TxBuf[2] + 4] = (crcValue >> 8) & 0xFF;
	
							com->TxDataCnt = com->TxBuf[2] + 5;
							
							value = TRACKER_RD_FINISHED;
						}
						else
						{
							value = TRACKER_RD_ERROR;
						}
					}
                    break;
				case WRITE_REG:		// д�Ĵ���
					if(com->RxDataCnt>=(com->RxBuf[6]+9) && (com->RxBuf[0]==tracker.DeviceID || com->RxBuf[0]==0))
                    {
                        crcValue = GetCRC16CheckCode(com->RxBuf,(com->RxBuf[6]+7));
						if((crcValue&0xFF)==com->RxBuf[(com->RxBuf[6]+7)] && ((crcValue>>8)&0xFF)==com->RxBuf[(com->RxBuf[6]+8)])  // CRCУ����ȷ
						{
							com->TxBuf[2] = com->RxBuf[2];  // �Ĵ����׵�ַ���ֽ�
							com->TxBuf[3] = com->RxBuf[3];  // �Ĵ����׵�ַ���ֽ�
							com->TxBuf[4] = com->RxBuf[4];  // �Ĵ����������ֽ�
							com->TxBuf[5] = com->RxBuf[5];  // �Ĵ����������ֽ�
                            
							crcValue = com->RxBuf[2];
							crcValue <<= 8;
							crcValue |= com->RxBuf[3];
							
							dataLen = com->RxBuf[4];
							dataLen <<= 8;
							dataLen |= com->RxBuf[5];
							endAdd = crcValue + dataLen-1;
							if(crcValue<28999)
							{
								switch(crcValue)                 // ���ݵ�ַд��
								{
								case DEVICE_EDITION_REG_ADDR_BASE: // �豸�汾д��
									for(i=0;i<2;i++)
									{
										*pOut[2*DEVICE_EDITION_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.EditionUpdate = 1;
									break;
								case SYSWORKMODE_REG_ADDR_BASE: 
									for(i=0;i<4;i++)
									{
										*pOut[2*SYSWORKMODE_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									break;/*
								case SYSWORKMODE_REG_ADDR_BASE:   // ����ģʽд��
									switch(com->RxBuf[8])
									{
										case 0x00:   // ֹͣ
										case 0x40:   // ά��ģʽ
										case 0x20:   // �Զ�ģʽ
										case 0x10:   // �ֶ�ģʽ
										case 0x18:   // �ֶ���
										//case 0x1A:   // �ֶ��򶫡�����
										//case 0x19:   // �ֶ��򶫡���
										case 0x14:   // �ֶ�����
										//case 0x16:   // �ֶ�����������
										//case 0x15:   // �ֶ���������
										//case 0x12:   // �ֶ�����
										//case 0x11:   // �ֶ���
											for(i=0;i<2;i++)
												{
													*pOut[2*SYSWORKMODE_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
												}
												flag.BitFlag.SysWorkModeUpdate = 1;
											break;
										default:
											break;
									}
									break;*/
								case TIME_REG_ADDR_BASE:          // ϵͳʱ�����
									tempTime.Year = com->RxBuf[7];
									tempTime.Year = (tempTime.Year << 8) | com->RxBuf[8];
									tempTime.Month = com->RxBuf[10];
									tempTime.Day = com->RxBuf[12];
									tempTime.Hour = com->RxBuf[14];
									tempTime.Minute = com->RxBuf[16];
									tempTime.Second = com->RxBuf[18];
									flag.BitFlag.TimeUpdate = 1;
									
									//�����ʼ�ϵ�״̬
									BKP_WriteBackupRegister(BKP_DR9, 0xAAAA);
									flag.BitFlag.PowerONTimeUpdate =  0;
									
									//��λ������ʱ�䣬���Զ�̴����־��������Զ��ʱ�����
									tracker.SysErrMessage1.BitEM.RemoteTimeError = 0;	
									flag.BitFlag.DailyTimeUpdate = 1;
								
									break;/*
								case REMOTE_WINDSPEED_REG_ADDR_BASE: // ������ѡ���ַ
									for(i=0;i<2;i++)
									{
										*pOut[2*REMOTE_WINDSPEED_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									//flag.BitFlag.SensorSelectUpdate = 1;
									break;*/
								case MOTOR_PULSE_REG_ADDR_BASE:   // ����������
									//for(i=0;i<8;i++)
									//{
										//*pOut[EW_PULSE_ADDR_BASE + i] = com->RxBuf[7 + i];
							
									//}
									//pulseBuffer[0] = com->RxBuf[7];
									//pulseBuffer[1] = com->RxBuf[11];
									//flag.BitFlag.PulseUpdate = 1;
									break;
								
								case MOTOR_RUN_CURRENT_ADDR_BASE: //�������������������
									for(i=0;i<4;i++)
									{
										*pOut[2*MOTOR_RUN_CURRENT_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.MotorRunCurUpdate = 1;
									break;

								case LONGITUDE_REG_ADDR_BASE:     // ���ĸ���
									for(i=0;i<12;i++)
									{
										*pOut[2*LONGITUDE_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.SolarUpdate = 1;
									break;
								case TWTRACKER_EW_REG_ADDR_BASE:  // �����㷨���پ���
									for(i=0;i<8;i++)
									{
										*pOut[2*TWTRACKER_EW_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.TWJDUpdate = 1;
									break;
								case MOTOR_FUNCA_REG_ADDR_BASE:   // ����A����
									for(i=0;i<16;i++)
									{
										*pOut[2*MOTOR_FUNCA_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.FuncAUpdate = 1;
									break;
								case MOTOR_FUNCB_REG_ADDR_BASE:   // ����B����
									for(i=0;i<32;i++)
									{
										*pOut[2*MOTOR_FUNCB_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.FuncBUpdate = 1;
									break;
								case ID_REG_ADDR_BASE:            // �豸��ַ����
									*pOut[2*ID_REG_ADDR_BASE + 0] = com->RxBuf[7];
									*pOut[2*ID_REG_ADDR_BASE + 1] = com->RxBuf[8];
									flag.BitFlag.IDUpdate = 1;
									break;
								case FUNC_AUTO_UPDATE_REG_ADDR_BASE:  // �����Ը���  ����Ը���
									*pOut[2*FUNC_AUTO_UPDATE_REG_ADDR_BASE + 0] = com->RxBuf[7];
									*pOut[2*FUNC_AUTO_UPDATE_REG_ADDR_BASE + 1] = com->RxBuf[8];
									flag.BitFlag.FuncAutoUpdate = 1;
									flag.BitFlag.QJAutoUpdate = 1;
									break;
								case QJ_AUTO_UPDATE_REG_ADDR_BASE:    // ����Ը���
									if(com->RxBuf[8]!=0)
									{
										flag.BitFlag.QJAutoUpdate = 1;
									}
									break;
//								case SUNSENSOR_ADJUST_EW_BALANCE_REG_ADDR_BASE: // ���У��
//									for(i=0;i<8;i++)
//									{
//										*pOut[2*SUNSENSOR_ADJUST_EW_BALANCE_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
//									}
//									flag.BitFlag.SunSensorUpdate = 1;
//									break;
                                    

                                
								case ARRAY_REG_ADDR_BASE:   // �������
									for(i=0;i<16;i++)
									{
										*pOut[2*ARRAY_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.ArrayUpdate = 1;
									break;
								case WIND_PROTECT_REG_ADDR_BASE:  // ��籣������
									for(i=0;i<8;i++)
									{
										*pOut[2*WIND_PROTECT_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.WindProtectUpdate = 1;
									break;
								case SOFT_LIMIT_REG_ADDR_BASE:    // ����λ�Ƕ�����
									for(i=0;i<8;i++)
									{
										*pOut[2*SOFT_LIMIT_REG_ADDR_BASE + i] = com->RxBuf[7 + i];
									}
									flag.BitFlag.SoftLimitUpdate = 1;
									break;
								case REMOTE_WINDSPEED_REG_ADDR_BASE: // Զ�̷���
									*pOut[2*REMOTE_WINDSPEED_REG_ADDR_BASE + 0] = com->RxBuf[7];
									*pOut[2*REMOTE_WINDSPEED_REG_ADDR_BASE + 1] = com->RxBuf[8];
									break;
                                
                                case STAGGERING_PEAK_BOOT_NUM_REG_ADDR_BASE:    // �����������
                                    *pOut[2*STAGGERING_PEAK_BOOT_NUM_REG_ADDR_BASE + 0] = com->RxBuf[7];
									*pOut[2*STAGGERING_PEAK_BOOT_NUM_REG_ADDR_BASE + 1] = com->RxBuf[8];
									flag.BitFlag.StaggerNumUpdate = 1;
									break;
                                
                                case BACK_ANGLE_REG_ADDR_BASE:  // ҹ��ͣ���Ƕ�����
                                    *pOut[2*BACK_ANGLE_REG_ADDR_BASE + 0] = com->RxBuf[7];
									*pOut[2*BACK_ANGLE_REG_ADDR_BASE + 1] = com->RxBuf[8];
                                    flag.BitFlag.BackAngleUpdate = 1;
                                    break;
                                
                                case WIND_PROTECT_ANGLE_EAST_REG_ADDR_BASE: //���ͣ���Ƕ�
                                    // ��
                                    *pOut[2*WIND_PROTECT_ANGLE_EAST_REG_ADDR_BASE + 0] = com->RxBuf[7];
									*pOut[2*WIND_PROTECT_ANGLE_EAST_REG_ADDR_BASE + 1] = com->RxBuf[8];
                                    // ��
                                    *pOut[2*WIND_PROTECT_ANGLE_EAST_REG_ADDR_BASE + 2] = com->RxBuf[9];
									*pOut[2*WIND_PROTECT_ANGLE_EAST_REG_ADDR_BASE + 3] = com->RxBuf[10];
                                    flag.BitFlag.WindProAngleUpdate = 1;
                                
								default:
									break;
								}
							}
							else if(crcValue >= 29008 && endAdd <= 29019)//����ӿڵ�ַ(д)
							{
								if(crcValue<=29008 && endAdd>=29008)
								{//����ģʽ
									//��������λ��
									tempPos = 29008 - crcValue;
									temp16Value = com->RxBuf[7+2*tempPos];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[8+2*tempPos];
									
									if(temp16Value==0)
									{//ά��ģʽ
										tracker.ExInterface.Mode = 0;
										tracker.SysWorkMode = 0x00000040;
									}
									else if(temp16Value==1)
									{//�Զ�ģʽ
										tracker.ExInterface.Mode = 1;
										tracker.SysWorkMode = 0x00000020;
									}
									else if(temp16Value==2)
									{//�ֶ�ģʽ
										tracker.ExInterface.Mode = 2;
										tracker.SysWorkMode = 0x00000010;
									}
									else if(temp16Value==3)
									{//���
										tracker.ExInterface.Mode = 3;
										tracker.SysWorkMode = 0x00000024;
									}
									else if(temp16Value==4)
									{//��ѩ
										tracker.ExInterface.Mode = 4;
										tracker.SysWorkMode = 0x00000022;
									}
									else if(temp16Value==5)
									{//����
										tracker.ExInterface.Mode = 5;
										tracker.SysWorkMode = 0x00000021;
									}
									else if(temp16Value==6)
									{//ֹͣ
										tracker.ExInterface.Mode = 6;
										tracker.SysWorkMode = 0;
									}
									else
									{//���ݳ���Χ
										
									}
								}

								if(crcValue<=29009 && endAdd>=29009)
								{//����
									tempPos = 29009 - crcValue;
									temp16Value = com->RxBuf[7+2*tempPos];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[8+2*tempPos];
									tracker.ExInterface.Wspeed = temp16Value;
									
									tracker.Sensor.RemoteWindSpeed = temp16Value;
								}
								
								if(crcValue<=29010 && endAdd>=29010)
								{//���� Ԥ��
								}
								
								//if((crcValue<=29012 && endAdd>=29012) || (crcValue<=29017 && endAdd>=29017))
								if((crcValue >= 29012 && crcValue <= 29017)
								   || (endAdd >= 29012 && endAdd <= 29017)
								   || (crcValue < 29012 && endAdd > 29017))
								{//ϵͳʱ��
									tempTime.Year   = tracker.SysTime.Year;
									tempTime.Month  = tracker.SysTime.Month;
									tempTime.Day    = tracker.SysTime.Day;
									tempTime.Hour   = tracker.SysTime.Hour;
									tempTime.Minute = tracker.SysTime.Minute;
									tempTime.Second = tracker.SysTime.Second;
									
									tempvalue = 29012-crcValue;
									if(tempvalue>=0 && endAdd>=29012)
									{//��
										tempTime.Year = com->RxBuf[7+2*tempvalue];
										tempTime.Year <<= 8;
										tempTime.Year |= com->RxBuf[8+2*tempvalue];
									}
									tempvalue = 29013-crcValue;
									if(tempvalue>=0 && endAdd>=29013)
									{//��
										tempTime.Month = com->RxBuf[7+2*tempvalue];
										tempTime.Month <<= 8;
										tempTime.Month |= com->RxBuf[8+2*tempvalue];
									}
									tempvalue = 29014-crcValue;
									if(tempvalue>=0 && endAdd>=29014)
									{//��
										tempTime.Day = com->RxBuf[7+2*tempvalue];
										tempTime.Day <<= 8;
										tempTime.Day |= com->RxBuf[8+2*tempvalue];
									}
									tempvalue = 29015-crcValue;
									if(tempvalue>=0 && endAdd>=29015)
									{//ʱ
										tempTime.Hour = com->RxBuf[7+2*tempvalue];
										tempTime.Hour <<= 8;
										tempTime.Hour |= com->RxBuf[8+2*tempvalue];
									}
									tempvalue = 29016-crcValue;
									if(tempvalue>=0 && endAdd>=29016)
									{//��
										tempTime.Minute = com->RxBuf[7+2*tempvalue];
										tempTime.Minute <<= 8;
										tempTime.Minute |= com->RxBuf[8+2*tempvalue];
									}
									tempvalue = 29017-crcValue;
									if(tempvalue>=0 && endAdd>=29017)
									{//��
										tempTime.Second = com->RxBuf[7+2*tempvalue];
										tempTime.Second <<= 8;
										tempTime.Second |= com->RxBuf[8+2*tempvalue];
									}
									
									if(flag.BitFlag.PowerONTimeUpdate == 1)	//�ϵ�����ϵ�
									{
										flag.BitFlag.PowerONTimeUpdate = 0;
										flag.BitFlag.TimeUpdate = 1;
										BKP_WriteBackupRegister(BKP_DR9, 0xAAAA);
									}
									else
									{
										if(flag.BitFlag.DailyTimeUpdate == 1)	//ÿ�ո��±�־λ
										{
											flag.BitFlag.DailyTimeUpdate = 0;
											secdiff = get_seccount(&tempTime) - get_seccount(&tracker.SysTime);
											
											if(secdiff >= -300 && secdiff <= 300)//ʱ��һ��,���5������
											{
												flag.BitFlag.TimeUpdate = 1;//
												tracker.SysErrMessage1.BitEM.RemoteTimeError = 0;//���remote����ʱ������־λ
												
											}
											else//ʱ�䲻һ��
											{
												tracker.SysErrMessage1.BitEM.RemoteTimeError = 1;//��λremote����ʱ������־λ
											}
										}
									}
								}
								//Ŀ�����  Ŀ��ת��  29018  29019
								{
								}								
							}
							else if(crcValue>= 40004 && endAdd<=40015)//��Ϊ�ӿڵ�ַ��д��
							{//��ַ40004�Ժ�Ϊ�ⲿ�ӿ�
//								unsigned short crcValue = 0;
//								unsigned short dataLen = 0;
//								unsigned short endAdd = 0;
 

								
								//����γ�ȳ���Χ   //40004  40005
								{
								}
																
								
								if(crcValue<=40006 && endAdd>=40006)
								{//����ģʽ
									//��������λ��
									//crcValue -=40006;
									tempPos = 40006 - crcValue;
									temp16Value = com->RxBuf[7+2*tempPos];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[8+2*tempPos];
									
									if(temp16Value==0)
									{//ά��ģʽ
										tracker.ExInterface.Mode = 0;
										tracker.SysWorkMode = 0x00000040;
									}
									else if(temp16Value==1)
									{//�Զ�ģʽ
										tracker.ExInterface.Mode = 1;
										tracker.SysWorkMode = 0x00000020;										
									}
									else if(temp16Value==2)
									{//�ֶ�ģʽ
										tracker.ExInterface.Mode = 2;
										tracker.SysWorkMode = 0x00000010;										
									}
									else if(temp16Value==3)
									{//���
										tracker.ExInterface.Mode = 3;
										tracker.SysWorkMode = 0x00000024;										
									}
									else if(temp16Value==4)
									{//��ѩ
										tracker.ExInterface.Mode = 4;
										tracker.SysWorkMode = 0x00000022;										
									}
									else if(temp16Value==5)
									{//����
										tracker.ExInterface.Mode = 5;
										tracker.SysWorkMode = 0x00000021;										
									}
									else if(temp16Value==6)
									{//ֹͣ
										tracker.ExInterface.Mode = 6;
										tracker.SysWorkMode = 0;
									}
									else if(temp16Value == 7)
									{//���꣨ʵ������ά��ģʽ��
										tracker.ExInterface.Mode = 7;
										tracker.SysWorkMode = 0x00000040;
									}
									else
									{//���ݳ���Χ
										
									}
								}

								if(crcValue<=40007 && endAdd>=40007)
								{//����
									tempPos = 40007 - crcValue;
									temp16Value = com->RxBuf[7+2*tempPos];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[8+2*tempPos];
									tracker.ExInterface.Wspeed = temp16Value;
									
									tracker.Sensor.RemoteWindSpeed = temp16Value;
								}
								
								if(crcValue<=40008 && endAdd>=40008)
								{//���� Ԥ��
								}
								
								//if((crcValue<=40010&&endAdd>=40010) || (crcValue<=40015&&endAdd>=40015))
								if((crcValue >= 40010 && crcValue <= 40015)
								   || (endAdd >= 40010 && endAdd <= 40015)
								   || (crcValue < 40010 && endAdd > 40015))
								{//ϵͳʱ��
									tempTime.Year = tracker.SysTime.Year;
									tempTime.Month = tracker.SysTime.Month;
									tempTime.Day  =  tracker.SysTime.Day;
									tempTime.Hour = tracker.SysTime.Hour;
									tempTime.Minute = tracker.SysTime.Minute;
									tempTime.Second = tracker.SysTime.Second;
									
									tempvalue = 40010-crcValue;
									if(tempvalue>=0 && endAdd>=40010)
									{//��
										tempTime.Year = com->RxBuf[7+2*tempvalue];
										tempTime.Year <<= 8;
										tempTime.Year |= com->RxBuf[8+2*tempvalue];										 
									}
									tempvalue = 40011-crcValue;
									if(tempvalue>=0 && endAdd>=40011)
									{//��
										tempTime.Month = com->RxBuf[7+2*tempvalue];
										tempTime.Month <<= 8;
										tempTime.Month |= com->RxBuf[8+2*tempvalue];										 
									}
									tempvalue = 40012-crcValue;
									if(tempvalue>=0 && endAdd>=40012)
									{//��
										tempTime.Day = com->RxBuf[7+2*tempvalue];
										tempTime.Day <<= 8;
										tempTime.Day |= com->RxBuf[8+2*tempvalue];										 
									}
									tempvalue = 40013-crcValue;
									if(tempvalue>=0 && endAdd>=40013)
									{//ʱ
										tempTime.Hour = com->RxBuf[7+2*tempvalue];
										tempTime.Hour <<= 8;
										tempTime.Hour |= com->RxBuf[8+2*tempvalue];										 
									}
									tempvalue = 40014-crcValue;
									if(tempvalue>=0 && endAdd>=40014)
									{//��
										tempTime.Minute = com->RxBuf[7+2*tempvalue];
										tempTime.Minute <<= 8;
										tempTime.Minute |= com->RxBuf[8+2*tempvalue];										 
									}
									tempvalue = 40015-crcValue;
									if(tempvalue>=0 && endAdd>=40015)
									{//��
										tempTime.Second = com->RxBuf[7+2*tempvalue];
										tempTime.Second <<= 8;
										tempTime.Second |= com->RxBuf[8+2*tempvalue];										 
									}
									
									if(flag.BitFlag.PowerONTimeUpdate == 1)	//�ϵ�����ϵ�
									{
										flag.BitFlag.PowerONTimeUpdate = 0;
										flag.BitFlag.TimeUpdate = 1;
										BKP_WriteBackupRegister(BKP_DR9, 0xAAAA);
									}
									else
									{
										if(flag.BitFlag.DailyTimeUpdate == 1)	//ÿ�ո��±�־λ
										{
											flag.BitFlag.DailyTimeUpdate = 0;
											secdiff = get_seccount(&tempTime) - get_seccount(&tracker.SysTime);
											
											if(secdiff >= -300 && secdiff <= 300)//ʱ��һ��,���5������
											{
												flag.BitFlag.TimeUpdate = 1;//
												tracker.SysErrMessage1.BitEM.RemoteTimeError = 0;//���remote����ʱ������־λ
												
											}
											else//ʱ�䲻һ��
											{
												tracker.SysErrMessage1.BitEM.RemoteTimeError = 1;//��λremote����ʱ������־λ
											}
										}
									}
								}
								//Ŀ�����  Ŀ��ת��  40016  40017 
								{
								}
								//��ͣ����  40018
//								if(crcValue<=40016 && endAdd>=40016)
//								{//��ͣ
//									tempPos = 40016 - crcValue;
//									temp16Value = com->RxBuf[7+2*tempPos];
//									temp16Value <<= 8;
//									temp16Value |= com->RxBuf[8+2*tempPos];
//									tracker.ExInterface.Tswtich = temp16Value;									
//									if(temp16Value==0)
//									{
//										//tracker.ExInterface.Tswtich = temp16Value;
//										tracker.ExInterface.Mode = 1;
//										tracker.SysWorkMode = 0x00000020;											
//									}
//									else
//									{
//										tracker.ExInterface.Mode = 6;
//										tracker.SysWorkMode = 0;
//									}
//								}
							}
							else
							{//��ַ����Χ
								com->TxBuf[0] = tracker.DeviceID;
								com->TxBuf[1] = WRITE_REG+0x80;
								com->TxBuf[2] = 2;
								crcValue = GetCRC16CheckCode(com->TxBuf, 3);
								com->TxBuf[3] = crcValue & 0xFF;
								com->TxBuf[4] = (crcValue >> 8) & 0xFF;
								
								com->TxDataCnt = 5;
								
								value = TRACKER_RD_FINISHED;
								if(value!=NO_CONNECT)
								{
									com->TimeOutCnt = 0;
									com->RxDataCnt = 0;

									if(ID==1)
									{
										DMA_Cmd(DMA1_Channel5, DISABLE);
										DMA1_Channel5->CNDTR = BUFFERSIZE;
										DMA_Cmd(DMA1_Channel5, ENABLE);
									}
								}								
								return value;								
							}


							// ����CRCУ����
							crcValue = GetCRC16CheckCode(com->TxBuf, 6);
							com->TxBuf[6] = crcValue & 0xFF;
							com->TxBuf[7] = crcValue >> 8;
							
							com->TxDataCnt = 8;
							if(com->RxBuf[0] == 0x00)
							{
								com->TxDataCnt = 0;
							}
							
							value = TRACKER_WR_FINISHED;
						}
						else
						{
							value = TRACKER_WR_ERROR;
						}
					}
					break;
					
				case WRITE_S_REG:	// д�����Ĵ���
					
					if(com->RxDataCnt>=8 && (com->RxBuf[0]==tracker.DeviceID || com->RxBuf[0]==0))
					{
						crcValue = GetCRC16CheckCode(com->RxBuf,6);
						if((crcValue&0xFF)==com->RxBuf[6] && ((crcValue>>8)&0xFF)==com->RxBuf[7])  // CRCУ����ȷ
						{						
							//�����������
							com->TxBuf[0] = com->RxBuf[0];  
							com->TxBuf[1] = com->RxBuf[1];
							com->TxBuf[2] = com->RxBuf[2];
							com->TxBuf[3] = com->RxBuf[3];  
							com->TxBuf[4] = com->RxBuf[4];  
							com->TxBuf[5] = com->RxBuf[5]; 
							com->TxBuf[6] = com->RxBuf[6];  
							com->TxBuf[7] = com->RxBuf[7];  
						
							crcValue = com->RxBuf[2];
							crcValue <<= 8;
							crcValue |= com->RxBuf[3];
							
							if(crcValue >= 29008 && crcValue <= 29019)//����ӿڵ�ַ��д��
							{
								if(crcValue==29008)
								{
									temp16Value = com->RxBuf[4];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[5];
									
									if(temp16Value==0)
									{//ά��ģʽ
										tracker.ExInterface.Mode = 0;
										tracker.SysWorkMode = 0x00000040;
									}
									else if(temp16Value==1)
									{//�Զ�ģʽ
										tracker.ExInterface.Mode = 1;
										tracker.SysWorkMode = 0x00000020;										
									}
									else if(temp16Value==2)
									{//�ֶ�ģʽ
										tracker.ExInterface.Mode = 2;
										tracker.SysWorkMode = 0x00000010;										
									}
									else if(temp16Value==3)
									{//���
										tracker.ExInterface.Mode = 3;
										tracker.SysWorkMode = 0x00000024;										
									}
									else if(temp16Value==4)
									{//��ѩ
										tracker.ExInterface.Mode = 4;
										tracker.SysWorkMode = 0x00000022;										
									}
									else if(temp16Value==5)
									{//����
										tracker.ExInterface.Mode = 5;
										tracker.SysWorkMode = 0x00000021;										
									}
									else if(temp16Value==6)
									{//ֹͣ
										tracker.ExInterface.Mode = 6;
										tracker.SysWorkMode = 0;
									}
									else
									{//���ݳ���Χ
										
									}									
								}
								else if(crcValue==29009)
								{//����
									temp16Value = com->RxBuf[4];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[5];
									tracker.ExInterface.Wspeed = temp16Value;
									
									tracker.Sensor.RemoteWindSpeed = temp16Value;									
								}
//								else if(crcValue==29010)
//								{//����Ԥ��
//									
//								}
								else if(crcValue>=29012 && crcValue<=29017)
								{
									if(flag.BitFlag.RemoteTimeUpdate_S == 0)
									{
										tempTime.Year   = tracker.SysTime.Year;
										tempTime.Month  = tracker.SysTime.Month;
										tempTime.Day    = tracker.SysTime.Day;
										tempTime.Hour   = tracker.SysTime.Hour;
										tempTime.Minute = tracker.SysTime.Minute;
										tempTime.Second = tracker.SysTime.Second;
									}										
									if(crcValue==29012)
									{//��
									   tempTime.Year = com->RxBuf[4];
									   tempTime.Year <<= 8;
									   tempTime.Year |= com->RxBuf[5];										
									}
									else if(crcValue==29013)
									{//��
									   tempTime.Month = com->RxBuf[4];
									   tempTime.Month <<= 8;
									   tempTime.Month |= com->RxBuf[5];											
									}
									else if(crcValue==29014)
									{//��
									   tempTime.Day = com->RxBuf[4];
									   tempTime.Day <<= 8;
									   tempTime.Day |= com->RxBuf[5];	
									}
									else if(crcValue==29015)
									{//ʱ
									   tempTime.Hour = com->RxBuf[4];
									   tempTime.Hour <<= 8;
									   tempTime.Hour |= com->RxBuf[5];	
									}
									else if(crcValue==29016)
									{//��
									   tempTime.Minute = com->RxBuf[4];
									   tempTime.Minute <<= 8;
									   tempTime.Minute |= com->RxBuf[5];											
									}	
									else if(crcValue==29017)
									{//��
									   tempTime.Second = com->RxBuf[4];
									   tempTime.Second <<= 8;
									   tempTime.Second |= com->RxBuf[5];											
									}
									
									flag.BitFlag.RemoteTimeUpdate_S = 1;
									//flag.BitFlag.TimeUpdate = 1;									
								}
								else if(crcValue == 29018)
								{//Ŀ��ת��
									
								}
								else if(crcValue == 29019)
								{//Ŀ�����
									
								}
							}
						  
							else if(crcValue>=40006 && crcValue<=40015)//��Ϊ�ӿڵ�ַ��д��
							{
								if(crcValue==40006)
								{
									temp16Value = com->RxBuf[4];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[5];
									
									if(temp16Value==0)
									{//ά��ģʽ
										tracker.ExInterface.Mode = 0;
										tracker.SysWorkMode = 0x00000040;
									}
									else if(temp16Value==1)
									{//�Զ�ģʽ
										tracker.ExInterface.Mode = 1;
										tracker.SysWorkMode = 0x00000020;										
									}
									else if(temp16Value==2)
									{//�ֶ�ģʽ
										tracker.ExInterface.Mode = 2;
										tracker.SysWorkMode = 0x00000010;										
									}
									else if(temp16Value==3)
									{//���
										tracker.ExInterface.Mode = 3;
										tracker.SysWorkMode = 0x00000024;										
									}
									else if(temp16Value==4)
									{//��ѩ
										tracker.ExInterface.Mode = 4;
										tracker.SysWorkMode = 0x00000022;										
									}
									else if(temp16Value==5)
									{//����
										tracker.ExInterface.Mode = 5;
										tracker.SysWorkMode = 0x00000021;										
									}
									else if(temp16Value==6)
									{//ֹͣ
										tracker.ExInterface.Mode = 6;
										tracker.SysWorkMode = 0;
									}
									else if(temp16Value == 7)
									{//���꣨�л���ά��ģʽ��
										tracker.ExInterface.Mode = 7;
										tracker.SysWorkMode = 0x00000040;
									}
									else
									{//���ݳ���Χ
										
									}									
								}
								else if(crcValue==40007)
								{//����
									temp16Value = com->RxBuf[4];
									temp16Value <<= 8;
									temp16Value |= com->RxBuf[5];
									tracker.ExInterface.Wspeed = temp16Value;
									
									tracker.Sensor.RemoteWindSpeed = temp16Value;									
								}
//								else if(crcValue==40008)
//								{//����Ԥ��
//									
//								}
								else if(crcValue>=40010 && crcValue<=40015)
								{
									if(flag.BitFlag.RemoteTimeUpdate_S == 0)
									{
										tempTime.Year = tracker.SysTime.Year;
										tempTime.Month = tracker.SysTime.Month;
										tempTime.Day  =  tracker.SysTime.Day;
										tempTime.Hour = tracker.SysTime.Hour;
										tempTime.Minute = tracker.SysTime.Minute;
										tempTime.Second = tracker.SysTime.Second;
									}										
									if(crcValue==40010)
									{//��
									   tempTime.Year = com->RxBuf[4];
									   tempTime.Year <<= 8;
									   tempTime.Year |= com->RxBuf[5];										
									}
									else if(crcValue==40011)
									{//��
									   tempTime.Month = com->RxBuf[4];
									   tempTime.Month <<= 8;
									   tempTime.Month |= com->RxBuf[5];											
									}
									else if(crcValue==40012)
									{//��
									   tempTime.Day = com->RxBuf[4];
									   tempTime.Day <<= 8;
									   tempTime.Day |= com->RxBuf[5];	
									}
									else if(crcValue==40013)
									{//ʱ
									   tempTime.Hour = com->RxBuf[4];
									   tempTime.Hour <<= 8;
									   tempTime.Hour |= com->RxBuf[5];	
									}
									else if(crcValue==40014)
									{//��
									   tempTime.Minute = com->RxBuf[4];
									   tempTime.Minute <<= 8;
									   tempTime.Minute |= com->RxBuf[5];											
									}	
									else if(crcValue==40015)
									{//��
									   tempTime.Second = com->RxBuf[4];
									   tempTime.Second <<= 8;
									   tempTime.Second |= com->RxBuf[5];											
									}
									
									flag.BitFlag.RemoteTimeUpdate_S = 1;
									
									//flag.BitFlag.TimeUpdate = 1;									
								}
//								else if(crcValue==50013)
//								{//��ͣ
//									temp16Value = com->RxBuf[4];
//									temp16Value <<= 8;
//									temp16Value |= com->RxBuf[5];
//									tracker.ExInterface.Tswtich = temp16Value;									
//									if(temp16Value==0)
//									{
//										//tracker.ExInterface.Tswtich = temp16Value;
//										tracker.ExInterface.Mode = 1;
//										tracker.SysWorkMode = 0x00000020;											
//									}
//									else
//									{
//										tracker.ExInterface.Mode = 6;
//										tracker.SysWorkMode = 0;
//									}									
//									
//								}
								else
								{//��Ч��ַ
									com->TxBuf[0] = tracker.DeviceID;
									com->TxBuf[1] = WRITE_S_REG+0x80;
									com->TxBuf[2] = 2;
									crcValue = GetCRC16CheckCode(com->TxBuf, 3);
									com->TxBuf[3] = crcValue & 0xFF;
									com->TxBuf[4] = (crcValue >> 8) & 0xFF;
									
									com->TxDataCnt = 5;
									
									value = TRACKER_RD_FINISHED;
									if(value!=NO_CONNECT)
									{
										com->TimeOutCnt = 0;
										com->RxDataCnt = 0;
									
										if(ID==1)
										{
											DMA_Cmd(DMA1_Channel5, DISABLE);
											DMA1_Channel5->CNDTR = BUFFERSIZE;
											DMA_Cmd(DMA1_Channel5, ENABLE);
										}
									}								
									return value;									 
								}
							}
							else
							{//��Ч��ַ
								com->TxBuf[0] = tracker.DeviceID;
								com->TxBuf[1] = WRITE_S_REG+0x80;
								com->TxBuf[2] = 2;
								crcValue = GetCRC16CheckCode(com->TxBuf, 3);
								com->TxBuf[3] = crcValue & 0xFF;
								com->TxBuf[4] = (crcValue >> 8) & 0xFF;
				
								com->TxDataCnt = 5;
								
								value = TRACKER_RD_FINISHED;
								if(value!=NO_CONNECT)
								{
									com->TimeOutCnt = 0;
									com->RxDataCnt = 0;

									if(ID==1)
									{
										DMA_Cmd(DMA1_Channel5, DISABLE);
										DMA1_Channel5->CNDTR = BUFFERSIZE;
										DMA_Cmd(DMA1_Channel5, ENABLE);
									}
								}								
								return value;										
							}	
							com->TxDataCnt = 8;
							if(com->RxBuf[0] == 0x00)
							{
								com->TxDataCnt = 0;
							}
							
							value = TRACKER_WR_FINISHED;
						}
						else
						{
							value = TRACKER_WR_ERROR;							
						}
					}
					break;
                case START_UPDATE:
                    if(com->RxDataCnt==6 && (com->RxBuf[0]==tracker.DeviceID || com->RxBuf[0]==0))
					{
						crcValue = GetCRC16CheckCode(com->RxBuf,4);
						if((crcValue&0xFF)==com->RxBuf[4] && ((crcValue>>8)&0xFF)==com->RxBuf[5])  // CRCУ����ȷ
                        {
                            if((IAP_TZ_DC&0xFF)==com->RxBuf[3] && ((IAP_TZ_DC>>8)&0xFF)==com->RxBuf[2])//�豸������ȷ
                            {
                                IAP_Enter_Process();
                            
                                com->TxDataCnt = 0;
                            
                                value = IAP_PACKAGE_RECEIVED;
                            }
                            else
                            {
                                value = IAP_DEVICE_TYPE_ERROR;
                            }
                        }
                        else
                        {
                            value = IAP_PACKAGE_ERROR;
                        }
                    }
                    break;
                    
                //lora
                case 0xA5:
                    for(i=0;i<6;i++)
                    {
                        com->TxBuf[i] = com->RxBuf[2+i];
                        com->TxDataCnt = 6;
                        value = TRACKER_RD_FINISHED;
                        com->TimeOutCnt = 0;
                        com->RxDataCnt = 0;
                        if(value!= NO_CONNECT)
                        {
                            com->TimeOutCnt = 0;
                            com->RxDataCnt = 0;
                        }
                    }
                    break;
                    
            }
        }
    }

    //==========================================================================
    if(value!=NO_CONNECT)
    {
        com->TimeOutCnt = 0;
        com->RxDataCnt = 0;

        if(ID==1)
        {
            DMA_Cmd(DMA1_Channel5, DISABLE);
            DMA1_Channel5->CNDTR = BUFFERSIZE;
            DMA_Cmd(DMA1_Channel5, ENABLE);
        }
    }

    return value;
}





unsigned char Data_transparent(void)
{
    unsigned short i = 0;
    unsigned char value;
    
    
    if(r485.Uart.RxDataCnt>=1)
    {
        if(r485.Uart.TimeOutCnt++>30)
        {
            r485.Uart.TimeOutCnt = 0;
            value = LONG_NO_CONNECT;
        }
        
        if(r485.Uart.RxDataCnt>=3)
        {
            if(r485.Uart.RxBuf[0]==0xAF && r485.Uart.RxBuf[1]==0xAF)
            {
                for(i=0;i<=r485.Uart.RxDataCnt;i++)
                    set_buf[i] = r485.Uart.RxBuf[i];
            
                trans_len = r485.Uart.RxDataCnt;
                r485.Uart.RxDataCnt = 0;
                trans_flag = 1;
            }
            else
            {
//                LORA_MD0(1);
//                LORA_MD1(1);
//                OSTimeDly(1);
                value = Data_Analyze(&r485.Uart, 2);
                return value;
            }
        }
    }
    
    if(trans_flag == 1)
    {
        trans_flag = 0;
        trans_ret_flag = 1;
        if(trans_len> 0)
        {
            if(set_buf[0] >= 0xc0 && set_buf[0]<= 0xc5)
            {
                for(i=0;i<trans_len;i++)
                {
                    USART_SendData(USART3, set_buf[i]);
                    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE)!=SET);
                }
                while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
                // COM_SentData(&r485,set_buf,trans_len);			
                // COM_SentData(&r485,com->RxBuf,com->RxDataCnt);							 
            }
        }
    }
    return NO_CONNECT;
}


