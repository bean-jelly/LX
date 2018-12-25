#pragma once 
#ifndef __MJFANCOUNT_H_
#define	__MJFANCOUNT_H_
//�����жϷ��ͺͷ���

#include "mjdata.h"
#include "mjlogic.h"
 
class CMJFanCount
{
public:
	CMJFanCount();
	virtual~CMJFanCount();
	  
	//��ʼ������
	void InitData();
	//��ʼ����������
	void InitPartData(); 
	//��ʼ���߼�
	void InitLogicParam(game_mode param, unsigned char normal_card[], unsigned int normal_len);

	//���ú��Ʒ�ʽ
	void SetWinType(unsigned char wintype, unsigned char wincard);
	//�������ƺ�����
	void SetCardData(CMJHandCard  handcard,CMJBlockCard	blockcard);
	//������ǽ
	void SetHaiDi(bool hai_di);
	//������Ҫ���ķ���
	void SetCheckFanType(unsigned int checktype[], unsigned int checkvalue[],unsigned int checkcount);

	//��ⷬ��
	int CheckFanType();
	//��ȡ����
	int	GetFanCount();
	//��ȡ�����
	unsigned int GetMaxFan(int &maxType);
	//��ȡ�����
	unsigned int GetMaxFanType();
	//��ȡ��������
	void GetFanType(bool fantype[], unsigned int fancount);
	
	//��ȡ��ɫ
	int	GetCardColor(unsigned char cardvalue); 
	//�ж��Ƿ���1��9
	bool IsOneOrNine(unsigned char cardvalue);
	//�ж��Ƿ�������
	bool IsZiCard(unsigned char cardvalue);

	virtual bool CheckWinType(unsigned int type);
	virtual bool Check00();
	virtual bool Check01();
	virtual bool Check02();
	virtual bool Check03();
	virtual bool Check04();
	virtual bool Check05();
	virtual bool Check06();
	virtual bool Check07();
	virtual bool Check08();
	virtual bool Check09();
	virtual bool Check10();
	virtual bool Check11();
	virtual bool Check12();
	virtual bool Check13();
	virtual bool Check14();
	virtual bool Check15();
	virtual bool Check16();
	virtual bool Check17();
	virtual bool Check18();
	virtual bool Check19();  
	virtual bool Check20();
	virtual bool Check21();
	virtual bool Check22();
	virtual bool Check23();
	virtual bool Check24();
	virtual bool Check25();
	virtual bool Check26();
	virtual bool Check27();
	virtual bool Check28();
	virtual bool Check29();
	virtual bool Check30();
	virtual bool Check31();
	virtual bool Check32();
	virtual bool Check33();
	virtual bool Check34();
	virtual bool Check35();
	virtual bool Check36();
	virtual bool Check37();
	virtual bool Check38();
	virtual bool Check39(); 

public: 
	CMJlogic					m_mj_logic;						//�齫�߼�
	CMJHandCard					m_handcard;						//�������
	CMJBlockCard				m_blockcard;					//�������
	unsigned char				m_wintype;						//���Ʒ�ʽ  0����ׯ   1������ 2������  3������ 4:�ܿ�
	unsigned char				m_wincard;						//������
	bool						m_hai_di;						//�Ƿ��Ǻ���
	unsigned int				m_checktype[FAN_TYPE_COUNT];	//��Ҫ��ⷬ��
	unsigned int				m_fanvalue[FAN_TYPE_COUNT];		//���ͷ���
	unsigned int				m_checkcount;					//��Ҫ���ĸ���
	bool						m_bfan[FAN_TYPE_COUNT];			//�����Ƿ����  

};

 
#endif // !__MJFANCOUNT_H_
