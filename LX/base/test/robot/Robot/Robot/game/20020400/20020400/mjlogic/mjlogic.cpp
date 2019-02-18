 #include "mjlogic.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//�齫���� 
/*
0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,						//��	������
0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,						//��	������
0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,						//Ͳ	������
0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,									//�����ϡ����������С�������	 ������
0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E,									//÷�������񡢾ա������ġ����  ��һ��
*/

CMJlogic::CMJlogic()
{ 
	m_normal_len = 0;
	m_king_count_limit = 255;
	m_allow_qidui = true;
	m_allow_mix_jiang = true;
	m_allow_laizi = false; 
	m_allow_chi = false;
	m_allow_peng = true;
	m_allow_gang = true;
	m_king_count = 0;
	memset(m_king_card, 0, sizeof(m_king_card));
	memset(m_normal_card, 0, sizeof(m_normal_card));
}


CMJlogic::~CMJlogic()
{
	m_king_count = 0;
	memset(m_king_card, 0, sizeof(m_king_card));
}

//��������
bool CMJlogic::clearpartdata()
{
	m_normal_len = 0;
	m_king_count_limit = 255;
	m_allow_qidui = true;
	m_allow_mix_jiang = true;
	m_allow_laizi = false;
	m_allow_chi = false;
	m_allow_peng = true;
	m_allow_gang = true;
	m_king_count = 0;
	memset(m_king_card, 0, sizeof(m_king_card));
	memset(m_normal_card, 0, sizeof(m_normal_card));

	return true;
}
//�Ƿ�����Ч��ֵ
bool CMJlogic::is_valid_card(unsigned char card_value)
{ 
	unsigned char color = (card_value&MASK_COLOR) >> 4;
	unsigned char value = (card_value&MASK_VALUE);
	return (((value >= 0x01) && (value <= 0x09) && (color <= 2)) || ((value >= 0x01) && (value <= 0x0F) && (color == 3)));
	 
}

//����齫������ֵ
void CMJlogic::add_normal_card(unsigned char card_value)
{
	if (is_valid_card(card_value) && m_normal_len < NORMAL_CARD_NUM)
	{
		//ȥ��
		for (unsigned int s = 0; s < m_normal_len; s++)
		{
			if (m_normal_card[s] == card_value)
			{
				return;
			}
		}
		 
		m_normal_card[m_normal_len] = card_value;
		m_normal_len++;
	}
}

//���������
bool CMJlogic::check_cardvalue(unsigned char card_value)
{   
	for (unsigned int i = 0; i < m_normal_len; i++)
	{
		if (m_normal_card[i] == card_value)
		{
			return true;
		}
	} 
	return false; 
}

//��ȡ��ɫ
int CMJlogic::get_card_color(unsigned char card_value)
{
	if (!check_cardvalue(card_value))
	{
		return -1;
	}
	return  (MASK_COLOR & card_value)>>4;
}

//��ȡ��ֵ
int CMJlogic::get_card_value(unsigned char card_value)
{
	if (!check_cardvalue(card_value))
	{
		return -1;
	}
	return  (MASK_VALUE & card_value);
}


/*��ֵת��-
@param: card_value  ��Ҫת������ֵ
@param: card_count	������
@param: card_color  ��Ҫת���Ļ�ɫ 
@param: card_index  ת�����ֵ
return  ����ת���ĸ���
*/
int CMJlogic::change_card_value(unsigned char card_value[], unsigned int card_count, int card_color, int card_index[])
{
	if (card_count == 0 || card_color == -1)
	{
		return 0;
	}
	int change_count = 0;
	int temp_color = -1;
	int temp_value = -1;
	for (unsigned int i = 0; i < card_count; i++)
	{
		if (check_cardvalue(card_value[i]))
		{
			temp_color = get_card_color(card_value[i]);
			if (temp_color != card_color)
			{
				continue;
			}
			temp_value = get_card_value(card_value[i]);
			card_index[temp_value]++;
			change_count++;
		}
	} 
	return change_count;
}
  
/*ת��������ֵ
@param: card_value  ��Ҫת������ֵ
@param: card_count	������ 
@param: card_index  ת�����ֵ����
return  ����ת���ĸ���
*/
int CMJlogic::change_card_index(const unsigned char card_value[], unsigned int card_count, int card_index[])
{
	if ( card_count == 0 )
	{
		return 0;
	}
	int change_count = 0; 
	int temp_value = -1;
	for (unsigned int i = 0; i < card_count; i++)
	{
		if (check_cardvalue(card_value[i]))
		{ 
			temp_value = card_value[i];
			card_index[temp_value]++;
			change_count++;
		}
	}
	return change_count;
}

int CMJlogic::change_card_index(CMJHandCard handcard, int card_index[])
{
	// ������� 
	int change_count = 0;
	for (unsigned int i = 0; i < handcard.GetHandCardLength(); i++)
	{
		unsigned char check_card =handcard.OnGetHandCard(i);
		if (check_cardvalue(check_card))
		{
			card_index[check_card] += 1;  // �ۼ�������
			change_count++;
		}
	}
	return change_count;
}
//�������ƣ���ȡĳ���Ƶĸ���
int CMJlogic::get_card_count(unsigned char hand_card_value[], unsigned int card_count, unsigned char check_card)
{
	int card_num = 0;

	if (!check_cardvalue(check_card))
	{
		return card_num;
	}
	if (card_count <= HAND_CARD_COUNT)
	{
		for (unsigned int i = 0; i < card_count; i++)
		{
			if (check_cardvalue(hand_card_value[i]) && check_card == hand_card_value[i])
			{
				card_num++;
			} 
		}
	} 
	return card_num;
}

//�����Ƶ�λ��-��һ��λ��
int CMJlogic::find_card_index(unsigned char hand_card_value[], unsigned int card_count, unsigned char check_card)
{
	int card_num = -1;

	if (!check_cardvalue(check_card))
	{
		return card_num;
	}
	if (card_count <= HAND_CARD_COUNT)
	{
		for (unsigned int i = 0; i < card_count; i++)
		{
			if (check_cardvalue(hand_card_value[i]) && check_card == hand_card_value[i])
			{
				card_num = i;
				break;
			}
		}
	}
	return card_num;
}


//ɾ����ֵ-ָ��ֵ
bool CMJlogic::del_card_value(unsigned char hand_card_value[], unsigned int card_count, unsigned char del_card, unsigned int del_count)
{
	int del_success_count = 0;

	if (card_count <= HAND_CARD_COUNT && del_count <= card_count && del_count >0)
	{
		unsigned int temp_card_count = card_count;
		unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
		memcpy(temp_hand_card, hand_card_value, temp_card_count);

		for (unsigned int i = 0; i < del_count; i++)
		{
			int key_pos = find_card_index(hand_card_value, temp_card_count, del_card);
			if (-1 != key_pos)
			{
				del_success_count++;
				hand_card_value[key_pos] = hand_card_value[temp_card_count - del_success_count];
				hand_card_value[temp_card_count - del_success_count] = 0x00;
			}
		}
		//ɾ���ĸ��������ƥ�䣬��ԭ����
		if (del_success_count != del_count)
		{
			memcpy(hand_card_value, temp_hand_card, temp_card_count);

			return false;
		} 
		return true;

	}
	return false;
}
//ɾ����ֵ--ָ������
bool CMJlogic::del_card_value(unsigned char hand_card_value[], unsigned int card_count, unsigned char del_card[], unsigned int del_count)
{
	int del_success_count = 0;

	if (card_count <= HAND_CARD_COUNT && del_count <= card_count && del_count >0)
	{
		unsigned int temp_card_count = card_count;
		unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
		memcpy(temp_hand_card, hand_card_value, temp_card_count);

		for (unsigned int i = 0; i < del_count; i++)
		{
			int key_pos = find_card_index(hand_card_value, temp_card_count, del_card[i]);
			if (-1 != key_pos)
			{ 
				del_success_count++;
				hand_card_value[key_pos] = hand_card_value[temp_card_count - del_success_count];
				hand_card_value[temp_card_count - del_success_count] = 0x00;
			}
		} 
		//ɾ���ĸ��������ƥ�䣬��ԭ����
		if (del_success_count != del_count)
		{ 
			memcpy(hand_card_value, temp_hand_card, temp_card_count);

			return false;
		} 
		return true;

	}
	return false;
}

//�����ֵ
bool CMJlogic::add_card(unsigned char hand_card_value[], unsigned int& card_count, unsigned char add_card[], unsigned int add_count)
{

	int add_success_count = 0;

	if (card_count + add_count <= HAND_CARD_COUNT && add_count > 0)
	{

		unsigned int temp_card_count = card_count;
		unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
		memcpy(temp_hand_card, hand_card_value, temp_card_count);
		  
		for (unsigned int i = 0; i < add_count; i++)
		{
			if (!check_cardvalue(add_card[i]))
			{
				break;
			} 
			hand_card_value[card_count + add_success_count] = add_card[i]; 
			add_success_count++;
		} 

		//���ӵĸ��������ƥ�䣬��ԭ����
		if (add_success_count != add_count)
		{
			memcpy(hand_card_value, temp_hand_card, temp_card_count);

			return false;
		}
		card_count = card_count + add_count;
		 
		return true;
	}
	 
	return false; 
}
 
//����
void CMJlogic::sort_card( unsigned char hand_card_value[], unsigned int card_count)
{    
	int card_index[CARD_INDEX_NUM] = { 0 };
	change_card_index(hand_card_value, card_count, card_index);

	unsigned int temp_card_count = 0;
	for (unsigned int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (card_index[i] == 0 && check_cardvalue(i))
		{
			continue;
		}
		for (int j = 0; j < card_index[i]; j++)
		{  
			hand_card_value[temp_card_count++] = i;
		}
	}
	 
}


/*����Ƿ���Գ���
@param: hand_card  �������
@param: hand_card_count  ���������
@param: check_card  �������
@param: chi_type  �������� 0x00:���ܳ�  0x01:��ͷ 0x10������ 0x11����β
return  ���� ��������
*/
unsigned char CMJlogic::check_can_chi(const unsigned char hand_card[], unsigned int hand_card_count, unsigned char check_card, unsigned char& chi_type, block_card_info blockinfo[])
{
	chi_type = BLOCK_TYPE_CHI_ERR;
	if (!check_cardvalue(check_card) || m_allow_chi == false)
	{
		return   BLOCK_TYPE_GAVE;
	}  
	if (hand_card_count > HAND_CARD_COUNT)
	{
		return   BLOCK_TYPE_GAVE; 
	}
	unsigned char  temp_hand_value[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_value, hand_card, hand_card_count); 
	int block_count = 0;
	//1�����ƣ����Ƴ� 
	if (check_card >= 0x31 && check_card <= 0x34)
	{
		bool IsHave = false;

		if (m_allow_wind_chi)
		{
			switch (check_card)
			{
			case 0x31:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_card[0] = check_card;
					blockinfo[block_count].block_card[1] = check_card + 1;
					blockinfo[block_count].block_card[2] = check_card + 2;
					block_count++;
				}
				if (get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 3) != 0)
				{
					IsHave = true; 
					chi_type |= BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_card[0] = check_card;
					blockinfo[block_count].block_card[1] = check_card + 1;
					blockinfo[block_count].block_card[2] = check_card + 3;
					block_count++;
				}

				if (get_card_count(temp_hand_value, hand_card_count, check_card + 2) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 3) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_card[0] = check_card;
					blockinfo[block_count].block_card[1] = check_card + 2;
					blockinfo[block_count].block_card[2] = check_card + 3;
					block_count++;
				}

			}
				break;
			case 0x32:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_card[0] = check_card - 1;
					blockinfo[block_count].block_card[1] = check_card;
					blockinfo[block_count].block_card[2] = check_card + 1;
					block_count++;
				}
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_card[0] = check_card - 1;
					blockinfo[block_count].block_card[1] = check_card;
					blockinfo[block_count].block_card[2] = check_card + 2;
					block_count++;
				}

				if (get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_card[0] = check_card;
					blockinfo[block_count].block_card[1] = check_card + 1;
					blockinfo[block_count].block_card[2] = check_card + 2;
					block_count++;
				}
			}
				break;
			case 0x033:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card - 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_card[0] = check_card - 2;
					blockinfo[block_count].block_card[1] = check_card - 1;
					blockinfo[block_count].block_card[2] = check_card;
					block_count++;
				}
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_card[0] = check_card - 1;
					blockinfo[block_count].block_card[1] = check_card;
					blockinfo[block_count].block_card[2] = check_card + 1;
					block_count++;
				}

				if (get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card - 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_card[0] = check_card - 2;
					blockinfo[block_count].block_card[1] = check_card;
					blockinfo[block_count].block_card[2] = check_card + 1;
					block_count++;
				}
			}
				break;
			case 0x34:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card - 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_END;

					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_card[0] = check_card - 2;
					blockinfo[block_count].block_card[1] = check_card - 1;
					blockinfo[block_count].block_card[2] = check_card;
					block_count++;
				}
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card - 3) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_END;

					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_card[0] = check_card - 3;
					blockinfo[block_count].block_card[1] = check_card - 1;
					blockinfo[block_count].block_card[2] = check_card;
					block_count++;
				}

				if (get_card_count(temp_hand_value, hand_card_count, check_card - 3) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card - 2) != 0)
				{
					IsHave = true;
					chi_type |= BLOCK_TYPE_CHI_END;

					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_card[0] = check_card - 3;
					blockinfo[block_count].block_card[1] = check_card - 2;
					blockinfo[block_count].block_card[2] = check_card;
					block_count++;
				}
			}
				break;
			default:
				break;
			}
			if (IsHave)
			{
				return BLOCK_TYPE_CHI;
			}

		}
		return   BLOCK_TYPE_GAVE;
	}
	if (check_card >= 0x35 && check_card <= 0x37)
	{
		if (m_allow_arrow_chi)
		{
			switch (check_card)
			{
			case 0x35:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 2) != 0)
				{
					chi_type |= BLOCK_TYPE_CHI_FIS;

					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
					blockinfo[block_count].block_card[0] = check_card;
					blockinfo[block_count].block_card[1] = check_card + 1;
					blockinfo[block_count].block_card[2] = check_card + 2;
					block_count++;
				}
			}
				break;
			case 0x36:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card + 1) != 0)
				{
					chi_type |= BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
					blockinfo[block_count].block_card[0] = check_card - 1;
					blockinfo[block_count].block_card[1] = check_card;
					blockinfo[block_count].block_card[2] = check_card + 1;
					block_count++;
				}
			}
				break;
			case 0x37:
			{
				if (get_card_count(temp_hand_value, hand_card_count, check_card - 1) != 0 && get_card_count(temp_hand_value, hand_card_count, check_card - 2) != 0)
				{
					chi_type |= BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
					blockinfo[block_count].block_card[0] = check_card - 2;
					blockinfo[block_count].block_card[1] = check_card - 1;
					blockinfo[block_count].block_card[2] = check_card;
					block_count++;
				}
			}
				break;
			default:
				break;
			}
			if (BLOCK_TYPE_CHI_ERR != chi_type)
			{
				return BLOCK_TYPE_CHI;
			}
		}
		return   BLOCK_TYPE_GAVE;
	}

	//��ȡ������Ƶ���ֵ
	int check_card_value = this->get_card_value(check_card);
	int check_card_color = this->get_card_color(check_card);
	int card_index[10] = { 0 };// 1-9
	this->change_card_value(temp_hand_value, hand_card_count, check_card_color, card_index);

	//��check_card_value == 1 orcheck_card_value ==9 ʱ
	if (check_card_value == 1 )
	{
		if (card_index[check_card_value + 1]>0 && card_index[check_card_value + 2]>0 )
		{
			chi_type |= BLOCK_TYPE_CHI_FIS;

			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
			blockinfo[block_count].block_card[0] = check_card;
			blockinfo[block_count].block_card[1] = check_card + 1;
			blockinfo[block_count].block_card[2] = check_card + 2;
			block_count++;
		}
	}
	else if (check_card_value == 9)
	{
		if (card_index[check_card_value - 1]>0 && card_index[check_card_value - 2]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_END;

			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
			blockinfo[block_count].block_card[0] = check_card - 2;
			blockinfo[block_count].block_card[1] = check_card - 1;
			blockinfo[block_count].block_card[2] = check_card;
			block_count++;
		}
	}
	else if (check_card_value == 2)
	{
		if (card_index[check_card_value + 1]>0 && card_index[check_card_value + 2]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_FIS;

			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
			blockinfo[block_count].block_card[0] = check_card;
			blockinfo[block_count].block_card[1] = check_card + 1;
			blockinfo[block_count].block_card[2] = check_card + 2;
			block_count++;
		}
		if (card_index[check_card_value + 1]>0 && card_index[check_card_value - 1]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_MID;
			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
			blockinfo[block_count].block_card[0] = check_card - 1;
			blockinfo[block_count].block_card[1] = check_card;
			blockinfo[block_count].block_card[2] = check_card + 1;
			block_count++;
		}
	}
	else if (check_card_value == 8)
	{ 
		if (card_index[check_card_value - 1]>0 && card_index[check_card_value +1]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_MID;
			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
			blockinfo[block_count].block_card[0] = check_card - 1;
			blockinfo[block_count].block_card[1] = check_card;
			blockinfo[block_count].block_card[2] = check_card + 1;
			block_count++;
		}
		if (card_index[check_card_value - 1]>0 && card_index[check_card_value - 2]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_END;
			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
			blockinfo[block_count].block_card[0] = check_card - 2;
			blockinfo[block_count].block_card[1] = check_card - 1;
			blockinfo[block_count].block_card[2] = check_card;
			block_count++;
		}
	}
	else
	{
		if (card_index[check_card_value + 1]>0 && card_index[check_card_value + 2]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_FIS;
			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_FIS;
			blockinfo[block_count].block_card[0] = check_card;
			blockinfo[block_count].block_card[1] = check_card + 1;
			blockinfo[block_count].block_card[2] = check_card + 2;
			block_count++;
		}
		if (card_index[check_card_value - 1]>0 && card_index[check_card_value + 1]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_MID;
			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_MID;
			blockinfo[block_count].block_card[0] = check_card - 1;
			blockinfo[block_count].block_card[1] = check_card;
			blockinfo[block_count].block_card[2] = check_card + 1;
			block_count++;
		}
		if (card_index[check_card_value - 1]>0 && card_index[check_card_value - 2]>0)
		{
			chi_type |= BLOCK_TYPE_CHI_END;
			blockinfo[block_count].block_type = BLOCK_TYPE_CHI_END;
			blockinfo[block_count].block_card[0] = check_card - 2;
			blockinfo[block_count].block_card[1] = check_card - 1;
			blockinfo[block_count].block_card[2] = check_card;
			block_count++;
		}
	} 
	if (BLOCK_TYPE_CHI_ERR != chi_type)
	{
		return BLOCK_TYPE_CHI;
	}
	return   BLOCK_TYPE_GAVE;
}

/*����Ƿ���Գ���
@param: handcard  �������
@param: check_user  ������
@param: out_user	���Ƶ���
@param: check_card  �������
@param: block_info	������Ϣ
return  �Ƿ���ڳ��ƿ���
*/
bool CMJlogic::check_chi(CMJHandCard handcard, unsigned char check_user, unsigned char out_user, unsigned char check_card, CMJBlockCard& block_info)
{
	if (!check_cardvalue(check_card) || m_allow_chi == false)
	{
		return false;
	}

	if (handcard.GetHandCardLength() > HAND_CARD_COUNT || handcard.GetHandCardLength() == 0)
	{
		return false;
	}
	BlockData	chi_info;
	chi_info.blocked_card = check_card;
	chi_info.block_type = BLOCK_TYPE_CHI;
	chi_info.chair_id = out_user;


	unsigned char	temp_handcard[HAND_CARD_COUNT] = { 0x00 };
	unsigned int	temp_count = handcard.GetAllHandCard(temp_handcard);

	//1�����ƣ����Ƴ� 
	if (check_card >= MJ_CARD_WIND_EAST && check_card <= MJ_CARD_WIND_NORTH)
	{
		bool	IsHave = false;

		if (m_allow_wind_chi)
		{
			switch (check_card)
			{
			case MJ_CARD_WIND_EAST:
			{
				if (handcard.IsHaveCard(check_card + 1) && handcard.IsHaveCard(check_card + 2))
				{
					IsHave = true;
					chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
					chi_info.card_value[0] = check_card;
					chi_info.card_value[1] = check_card + 1;
					chi_info.card_value[2] = check_card + 2;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
				if (handcard.IsHaveCard(check_card + 1) && handcard.IsHaveCard(check_card + 3))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
					chi_info.card_value[0] = check_card;
					chi_info.card_value[1] = check_card + 1;
					chi_info.card_value[2] = check_card + 3;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}

				if (handcard.IsHaveCard(check_card + 2) && handcard.IsHaveCard(check_card + 3))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
					chi_info.card_value[0] = check_card;
					chi_info.card_value[1] = check_card + 2;
					chi_info.card_value[2] = check_card + 3;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}

			}
				break;
			case MJ_CARD_WIND_SOUTH:
			{
				if (handcard.IsHaveCard(check_card - 1) && handcard.IsHaveCard(check_card + 1))
				{
					IsHave = true;
					chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
					chi_info.card_value[0] = check_card - 1;
					chi_info.card_value[1] = check_card;
					chi_info.card_value[2] = check_card + 1;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
				if (handcard.IsHaveCard(check_card - 1) && handcard.IsHaveCard(check_card + 2))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
					chi_info.card_value[0] = check_card - 1;
					chi_info.card_value[1] = check_card;
					chi_info.card_value[2] = check_card + 2;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}

				if (handcard.IsHaveCard(check_card + 1) && handcard.IsHaveCard(check_card + 2))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
					chi_info.card_value[0] = check_card;
					chi_info.card_value[1] = check_card + 1;
					chi_info.card_value[2] = check_card + 2;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
			}
				break;
			case MJ_CARD_WIND_WEST:
			{
				if (handcard.IsHaveCard(check_card - 1) && handcard.IsHaveCard(check_card - 2))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
					chi_info.card_value[0] = check_card - 2;
					chi_info.card_value[1] = check_card - 1;
					chi_info.card_value[2] = check_card;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
				if (handcard.IsHaveCard(check_card - 1) && handcard.IsHaveCard(check_card + 1))
				{
					IsHave = true;
					chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
					chi_info.card_value[0] = check_card - 1;
					chi_info.card_value[1] = check_card;
					chi_info.card_value[2] = check_card + 1;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}

				if (handcard.IsHaveCard(check_card + 1) && handcard.IsHaveCard(check_card - 2))
				{
					IsHave = true;
					chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
					chi_info.card_value[0] = check_card - 2;
					chi_info.card_value[1] = check_card;
					chi_info.card_value[2] = check_card + 1;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
			}
				break;
			case MJ_CARD_WIND_NORTH:
			{
				if (handcard.IsHaveCard(check_card - 1) && handcard.IsHaveCard(check_card - 2))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
					chi_info.card_value[0] = check_card - 2;
					chi_info.card_value[1] = check_card - 1;
					chi_info.card_value[2] = check_card;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
				if (handcard.IsHaveCard(check_card - 1) && handcard.IsHaveCard(check_card - 3))
				{
					IsHave = true;
					chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
					chi_info.card_value[0] = check_card - 3;
					chi_info.card_value[1] = check_card - 1;
					chi_info.card_value[2] = check_card;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}

				if (handcard.IsHaveCard(check_card - 3) && handcard.IsHaveCard(check_card - 2))
				{
					IsHave = true;
					chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
					chi_info.card_value[0] = check_card - 3;
					chi_info.card_value[1] = check_card - 2;
					chi_info.card_value[2] = check_card;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
			}
				break;
			default:
				break;
			}
			return IsHave;
			 
		}
		return false;
	}
	else if (check_card >= MJ_CARD_ARROW_HZ && check_card <= MJ_CARD_ARROW_BB)
	{
		if (m_allow_arrow_chi)
		{
			bool	IsHave = false;
			switch (check_card)
			{
			case MJ_CARD_ARROW_HZ:
			{
				if (handcard.IsHaveCard(check_card + 1) && handcard.IsHaveCard(check_card + 2))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
					chi_info.card_value[0] = check_card;
					chi_info.card_value[1] = check_card + 1;
					chi_info.card_value[2] = check_card + 2;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
			}
				break;
			case MJ_CARD_ARROW_FC:
			{
				if (handcard.IsHaveCard(check_card + 1) && handcard.IsHaveCard(check_card - 1))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
					chi_info.card_value[0] = check_card - 1;
					chi_info.card_value[1] = check_card;
					chi_info.card_value[2] = check_card + 1;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}

			}
				break;
			case MJ_CARD_ARROW_BB:
			{
				if (handcard.IsHaveCard(check_card - 2) && handcard.IsHaveCard(check_card - 1))
				{
					IsHave = true;

					chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
					chi_info.card_value[0] = check_card - 2;
					chi_info.card_value[1] = check_card - 1;
					chi_info.card_value[2] = check_card;
					chi_info.card_value[3] = 0x00;
					block_info.AddBlockCard(chi_info);
				}
			}
				break;
			default:
				break;
			}
			
			return IsHave;
		}
		return false;

	}
	else
	{


		//2������Ͳ����  
		//��ȡ������Ƶ���ֵ
		int check_card_value = this->get_card_value(check_card);
		int check_card_color = this->get_card_color(check_card);
		int card_index[10] = { 0 };// 1-9
		this->change_card_value(temp_handcard, temp_count, check_card_color, card_index);
		bool IsHave = false;
		//��check_card_value == 1 orcheck_card_value ==9 ʱ
		if (check_card_value == 1)
		{
			if (card_index[check_card_value + 1]>0 && card_index[check_card_value + 2]>0)
			{
				IsHave = true;
				chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
				chi_info.card_value[0] = check_card;
				chi_info.card_value[1] = check_card + 1;
				chi_info.card_value[2] = check_card + 2;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);

			}
		}
		else if (check_card_value == 9)
		{
			if (card_index[check_card_value - 1]>0 && card_index[check_card_value - 2]>0)
			{
				IsHave = true;
				chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
				chi_info.card_value[0] = check_card - 1;
				chi_info.card_value[1] = check_card - 2;
				chi_info.card_value[2] = check_card;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
		}
		else if (check_card_value == 2)
		{ 
			if (card_index[check_card_value + 1]>0 && card_index[check_card_value - 1]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
				chi_info.card_value[0] = check_card - 1;
				chi_info.card_value[1] = check_card;
				chi_info.card_value[2] = check_card + 1;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
			if (card_index[check_card_value + 1]>0 && card_index[check_card_value + 2]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
				chi_info.card_value[0] = check_card;
				chi_info.card_value[1] = check_card + 1;
				chi_info.card_value[2] = check_card + 2;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
		}
		else if (check_card_value == 8)
		{
			if (card_index[check_card_value - 1]>0 && card_index[check_card_value - 2]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
				chi_info.card_value[0] = check_card - 2;
				chi_info.card_value[1] = check_card - 1;
				chi_info.card_value[2] = check_card;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
			if (card_index[check_card_value - 1]>0 && card_index[check_card_value + 1]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
				chi_info.card_value[0] = check_card - 1;
				chi_info.card_value[1] = check_card;
				chi_info.card_value[2] = check_card + 1;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
		}
		else
		{ 
			if (card_index[check_card_value - 1]>0 && card_index[check_card_value - 2]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_END;
				chi_info.card_value[0] = check_card - 2;
				chi_info.card_value[1] = check_card - 1;
				chi_info.card_value[2] = check_card;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
			if (card_index[check_card_value - 1]>0 && card_index[check_card_value + 1]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_MID;
				chi_info.card_value[0] = check_card - 1;
				chi_info.card_value[1] = check_card;
				chi_info.card_value[2] = check_card + 1;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
			if (card_index[check_card_value + 1]>0 && card_index[check_card_value + 2]>0)
			{
				IsHave = true;

				chi_info.block_sub_type = BLOCK_TYPE_CHI_FIS;
				chi_info.card_value[0] = check_card;
				chi_info.card_value[1] = check_card + 1;
				chi_info.card_value[2] = check_card + 2;
				chi_info.card_value[3] = 0x00;
				block_info.AddBlockCard(chi_info);
			}
		} 
		return IsHave;

	}

	return false;
}
  
/*����Ƿ��������
@param: hand_card  �������
@param: hand_card_count  ���������
@param: check_card  ������� 
return  ���� ��������
*/
unsigned char CMJlogic::check_can_peng(const unsigned char hand_card[], unsigned int hand_card_count, unsigned char check_card)
{
	if (!check_cardvalue(check_card) || m_allow_peng == false)
	{
		return   BLOCK_TYPE_GAVE;
	}
	if (hand_card_count > HAND_CARD_COUNT)
	{
		return   BLOCK_TYPE_GAVE;
	}

	unsigned char  temp_hand_value[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_value, hand_card, hand_card_count);

	//��ȡ������Ƶ���ֵ 
	int chekc_card_count = this->get_card_count(temp_hand_value, hand_card_count, check_card);

	if (chekc_card_count>=2)
	{
		return BLOCK_TYPE_PENG;
	} 
	return   BLOCK_TYPE_GAVE;
}
 
/*����Ƿ��������
@param: hand_card  �������
@param: hand_card_count  ���������
@param: check_card  ������� 
return  ���� ��������
*/
unsigned char CMJlogic::check_can_gang(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card)
{
	unsigned char gang_type = BLOCK_TYPE_GANG_ERR;
	
	if (hand_card_count > HAND_CARD_COUNT || hand_card_count < 3 || m_allow_gang==false)
	{
		return gang_type;
	}

	unsigned char  temp_hand_value[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_value, hand_card, hand_card_count); 
	unsigned int  temp_card_count = hand_card_count;
	 
	int		gang_count = 0; 
	//���Ƽ��
	if (check_cardvalue(check_card))
	{ 
		int chekc_card_count = this->get_card_count(temp_hand_value, temp_card_count, check_card);
		if (chekc_card_count == 3)
		{
			gang_type = BLOCK_TYPE_GANG_MING;
		}
	}  
	return gang_type;

}
 
/*����Ƿ���԰��ܣ�����
@param: hand_card  �������
@param: hand_card_count  ���������
@param: bloci_datat  �������� 
@param: check_card  �������
*/

unsigned char CMJlogic::check_can_gang_self(const unsigned char hand_card[], const unsigned int hand_card_count, CMJBlockCard user_block, unsigned char check_card, unsigned char block_card[])
{ 
	unsigned char gang_type = BLOCK_TYPE_GANG_ERR;
	if (check_cardvalue(check_card)==false || m_allow_gang == false)
	{
		return gang_type;
	}
	//�������Ƿ��и�
	int  temp_card_index[CARD_INDEX_NUM] = { 0x00 };  
	change_card_index(hand_card, hand_card_count, temp_card_index);
	 
	int		gang_count = 0;

	for (unsigned int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (check_cardvalue(i))
		{
			if (temp_card_index[i]==4)
			{
				gang_type |= BLOCK_TYPE_GANG_AN; 
				block_card[gang_count++] = i;
			}
		} 
	} 
	//�������Ƿ�����
	for (unsigned int i = 0; i < user_block.GetBlockLength(); i++)
	{
		BlockData	block;
		user_block.GetBlock(i, block);
		if (user_block.GetBlock(i, block) && block.block_type == BLOCK_TYPE_PENG && block.blocked_card == check_card)
		{
			gang_type |= BLOCK_TYPE_GANG_BU;
			block_card[gang_count++] = check_card;
		}
	}


	return gang_type;
} 

/*����Ƿ��������--���ƺ�
@param: hand_card  �������
@param : hand_card_count  ���������
@param : ting_card  �����б� 
@param : bking  �Ƿ�������
@param : king_card  ����
return  result  
*/
unsigned char CMJlogic::check_can_ting(const unsigned char hand_card[], const unsigned int hand_card_count, TingData& ting_info, bool first_card)
{ 

	unsigned char gang_type =   BLOCK_TYPE_GAVE;
	if (hand_card_count>0 && hand_card_count <= HAND_CARD_COUNT)
	{   
		bool	bTileFalg[CARD_INDEX_NUM];
		memset(bTileFalg, 0, sizeof(bTileFalg));
		unsigned int  temp_ting_count = 0;
		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (!check_cardvalue(hand_card[i]))
			{
				return gang_type;
			}
			if (bTileFalg[hand_card[i]])
			{
				continue;
			} 
			unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
			memcpy(temp_hand_card, hand_card, hand_card_count); 
			if (del_card_value(temp_hand_card, hand_card_count, hand_card[i], 1))
			{
				bTileFalg[hand_card[i]] = true;

				unsigned char win_card[NORMAL_CARD_NUM] = { 0x00 };
				if (is_ting(temp_hand_card, hand_card_count - 1, win_card, first_card))
				{
					gang_type = BLOCK_TYPE_TING;
					ting_info.ting_card[temp_ting_count][0] = hand_card[i]; 
					memcpy(&ting_info.ting_card[temp_ting_count][1], win_card, sizeof(win_card));
					temp_ting_count++;
				}
			}  
		} 
	} 
	return  gang_type;
}
 
/*��������Ƿ�����-���ƺ�
@param : hand_card  ����
@param : hand_card_count ������
@param : win_card	���Ժ�����
@param : first_card �Ƿ��һ��
return result
*/
bool CMJlogic::is_ting(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char win_card[], bool first_card)
{  
	if (hand_card_count % 3 == 1)
	{   
		bool bTing = false;
		unsigned int ting_count = 0;
		for (unsigned int i = 0; i < NORMAL_CARD_NUM; i++)
		{   
			unsigned char check_reslut = 0x00;
			check_reslut = check_can_win(hand_card, hand_card_count, m_normal_card[i], first_card, true);
			if (check_reslut == BLOCK_TYPE_HU)
			{
				bTing = true;
				win_card[ting_count++] = m_normal_card[i];
			} 
		}

		return bTing;
	}
	return false;
	  
}

bool CMJlogic::check_can_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, bool jiang)
{

	if (hand_card_count == 0)
	{
		return true;
	}
	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_card, hand_card, hand_card_count);
	unsigned int temp_hand_card_count = hand_card_count;
	 

	//��ȡ���Ӹ���
	unsigned int king_count = 0;
	if (m_allow_laizi)
	{
		king_count = is_have_king_value(temp_hand_card, temp_hand_card_count);

		if (king_count >= m_king_count_limit)
		{//�Ĺ�ֱ�Ӻ���
			return true;
		} 
	}
	sort_card(temp_hand_card, temp_hand_card_count);
	bool check_reslut = false;

	unsigned char temp_no_king_card[HAND_CARD_COUNT] = { 0x00 };

	if (m_king_count == 0)
	{
		memcpy(temp_no_king_card, temp_hand_card, temp_hand_card_count);
	}
	else
	{
		int len = 0;
		for (int i = 0; i < HAND_CARD_COUNT; i++)
		{
			if (is_king(temp_hand_card[i]) == false)
			{
				temp_no_king_card[len++] = temp_hand_card[i];
			}  
		}
	}
	unsigned int temp_no_king_card_count = temp_hand_card_count - king_count;
	sort_card(temp_no_king_card, temp_no_king_card_count);
	if (temp_no_king_card_count <= 0)
	{ 
		return true;
	}  
	check_reslut = is_sequence(temp_no_king_card, temp_no_king_card_count, king_count, jiang);
	  
	return check_reslut;

}
/*
@param : handcard  �������--������������,�����
@param : block	   �������
@param : kingcount �����Ƹ���
@param : jiang  �Ƿ��⽫��
*/
//���ָ�����ܷ����˳�ӡ�����,�����
bool CMJlogic::is_sequence(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount, bool jiang)
{

	if (check_qidui(hand_card, hand_card_count, kingcount))
	{
		return true;
	}

	if (hand_card_count == 0)
	{
		if (jiang)
		{
			if (kingcount%3 == 2)
			{
				return true;
			} 
		}
		else
		{
			if (kingcount % 3 == 0)
			{
				return true;
			}
		}
		return false;
	}
	bool	check_jiang = true;
	bool	bwin = false;
	//��һ�μ�������Ƿ��н��ƣ��ڶ��β���������Ӵ���
	for (int s = 0; s < 2; s++)
	{
		bool check_card[CARD_INDEX_NUM];
		memset(check_card, 0, sizeof(check_card));

		if (jiang && check_jiang == false && kingcount == 0)
		{//����û��������û�����ӣ����ܺ���
			return false;
		}
		//��������
		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (check_card[hand_card[i]])
			{
				continue;
			}
			//��¼��������
			check_card[hand_card[i]] = true;

			//ÿ���������ƺ�������
			int temp_count = kingcount;

			//�ֽ��ƺ�ʣ�����
			int remain_index[CARD_INDEX_NUM] = { 0 };
			change_card_index(hand_card, hand_card_count, remain_index);

			if (jiang)
			{//�Ƿ���Ҫ��⽫��
				//��ȡ����
				if (check_jiang)
				{
					if (remain_index[hand_card[i]] < 2)
					{
						continue;
					}
					remain_index[hand_card[i]] -= 2;
				}
				else
				{
					//ȥһ������������
					remain_index[hand_card[i]] -= 1;
					temp_count -= 1;
				}
			}
			for (int iwind = 0; iwind < 4; iwind++)
			{
				remain_index[MJ_CARD_WIND_EAST + iwind] = remain_index[MJ_CARD_WIND_EAST + iwind] % 3;
			}
			for (int iarrow = 0; iarrow <3; iarrow++)
			{
				remain_index[MJ_CARD_ARROW_HZ + iarrow] = remain_index[MJ_CARD_ARROW_HZ + iarrow] % 3;
			}
			for (unsigned int j = 0; j < hand_card_count; j++)
			{
				unsigned char  temp_card = hand_card[j];

				if (remain_index[temp_card] == 0)
				{
					continue;
				}
				 
				if (temp_card  < MJ_CARD_WIND_EAST)
				{
					int same_flower_index[9] = { 0 };
					unsigned char first_card = 0x00;
					if (temp_card >= MJ_CARD_WAN_1 && temp_card <= MJ_CARD_WAN_9)
					{
						first_card = MJ_CARD_WAN_1;
					}
					else if (temp_card >= MJ_CARD_TIAO_1 && temp_card <= MJ_CARD_TIAO_9)
					{
						first_card = MJ_CARD_TIAO_1;
					}
					else if (temp_card >= MJ_CARD_TONG_1 && temp_card <= MJ_CARD_TONG_9)
					{
						first_card = MJ_CARD_TONG_1;
					}
					if (first_card != 0x00)
					{
						for (int num = 0; num < 9; num++)
						{
							same_flower_index[num] = remain_index[first_card + num]; 
							remain_index[first_card + num] = 0;
						}

						temp_count -= check_index_combox(same_flower_index);
					}
				} 
				else if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
				{
					
					unsigned int win_total_count = 0;
					unsigned int min_count = remain_index[temp_card]%3;
					unsigned int max_count = remain_index[temp_card]%3;
					unsigned int wind_kind = 0;
					unsigned int wind_count[4] = { 0 };
					for (int iwind = 0; iwind < 4; iwind++)
					{
						int card_count = remain_index[MJ_CARD_WIND_EAST + iwind] % 3;
						if (card_count > 0)
						{
							wind_kind++;
							wind_count[iwind] = card_count;
							win_total_count += wind_count[iwind];
							if (min_count > wind_count[iwind])
							{
								min_count = wind_count[iwind];
							}
							if (max_count < wind_count[iwind])
							{
								max_count = wind_count[iwind];
							}
						}
					}
					//����
					unsigned int wind_count_index[5] = { 0 };
					for (int index = 0; index < 4; index++)
					{
						unsigned int win_card_count = wind_count[index];
						if (win_card_count>0 && win_card_count <= 4)
						{
							wind_count_index[win_card_count]++;
						}
					}

					////��������С����
					unsigned int card_sort_index[4] = { 0 };
					unsigned int win_len = 0;
					for (int index = 0; index < 5; index++)
					{
						for (unsigned int m = 0; m < wind_count_index[index]; m++)
						{
							card_sort_index[win_len++] = index;
						}
					}

					for (int iwind = 0; iwind < 4; iwind++)
					{
						if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
						{
							remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
						}
					}

					if (wind_kind == 1)
					{
						if (min_count % 3 != 0)
						{
							temp_count -= (3 - (min_count % 3));
						}
					}
					else if (wind_kind == 2)
					{
						if (m_allow_wind_chi)
						{
							if (max_count == min_count)
							{
								temp_count -= (max_count % 3);
							}
							else
							{
								if (min_count == 1 && max_count == 2)
								{
									temp_count -= 3;
								}
								else
								{
									if (win_total_count % 3 != 0)
									{
										temp_count -= (3 - (win_total_count % 3));
									}
								}
							} 
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
					else if (wind_kind == 3)
					{
						if (m_allow_wind_chi)
						{
							if (card_sort_index[0] == 1 && card_sort_index[1] == 2 && card_sort_index[2] == 3)
							{
								temp_count -= 3;
							}
							else if (card_sort_index[0] == 2 && card_sort_index[1] == 3 && card_sort_index[2] == 4)
							{
								temp_count -= 3;
							}
							else
							{
								if (win_total_count % 3 != 0)
								{
									temp_count -= (3 - (win_total_count % 3));
								}
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
					else if (wind_kind == 4)
					{
						if (m_allow_wind_chi)
						{
							if (win_total_count % 3 != 0)
							{
								temp_count -= (3 - (win_total_count % 3));
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
				}
				else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
				{//��
					unsigned int arrow_total_count = 0;
					unsigned int min_count = remain_index[temp_card] % 3;
					unsigned int max_count = remain_index[temp_card] % 3;
					unsigned int arrow_kind = 0;
					unsigned int arrow_count[3] = { 0 };
					for (int iarrow = 0; iarrow < 3; iarrow++)
					{
						int card_count = remain_index[MJ_CARD_ARROW_HZ + iarrow] % 3;
						if (card_count > 0)
						{
							arrow_kind++;
							arrow_count[iarrow] = card_count;
							arrow_total_count += arrow_count[iarrow];
							if (min_count > arrow_count[iarrow])
							{
								min_count = arrow_count[iarrow];
							}
							if (max_count < arrow_count[iarrow])
							{
								max_count = arrow_count[iarrow];
							}
						}
					} 
					//����
					unsigned int arrow_count_index[5] = { 0 };
					for (int index = 0; index < 3; index++)
					{
						unsigned int arrow_card_count = arrow_count[index];
						if (arrow_card_count>0 && arrow_card_count <= 4)
						{
							arrow_count_index[arrow_card_count]++;
						}
					}

					////��������С����
					unsigned int card_sort_index[4] = { 0 };
					unsigned int win_len = 0;
					for (int index = 0; index < 5; index++)
					{
						for (unsigned int m = 0; m < arrow_count_index[index]; m++)
						{
							card_sort_index[win_len++] = index;
						}
					}

					for (int iarrow = 0; iarrow < 3; iarrow++)
					{
						remain_index[MJ_CARD_ARROW_HZ + iarrow] = 0;
					}

					if (m_allow_arrow_chi)
					{
						if (arrow_kind == 1)
						{//�����ɿ��� 
							remain_index[temp_card] = 0;
							if (min_count % 3 != 0)
							{
								temp_count -= (3 - min_count % 3);
							}
						}
						else if (arrow_kind == 2)
						{
							if (max_count == min_count)
							{
								temp_count -= (max_count % 3);
							}
							else
							{
								if (min_count == 1 && max_count == 2)
								{
									temp_count -= 3;
								}
								else
								{
									if (arrow_total_count % 3 != 0)
									{
										temp_count -= (3 - arrow_total_count % 3);
									}
								} 
							}

						}
						else if (arrow_kind == 3)
						{
							if (min_count == max_count)
							{

							}
							else
							{
								if (card_sort_index[0] == 1 && card_sort_index[1] == 2 && card_sort_index[2] == 3)
								{
									temp_count -= 3;
								}
								else if (card_sort_index[0] == 2 && card_sort_index[1] == 3 && card_sort_index[2] == 4)
								{
									temp_count -= 3;
								}
								else
								{
									if (arrow_total_count % 3 != 0)
									{
										temp_count -= (3 - (arrow_total_count % 3));
									}
								}
							}
						}
					}
					else
					{
						for (int temp_i = 0; temp_i < 3; temp_i++)
						{
							if (arrow_count[temp_i] % 3 != 0)
							{
								temp_count -= (3 - arrow_count[temp_i] % 3);
							}
						}
					} 
				}
				 
				if (temp_count < 0)
				{
					break;
				}  
			}  
			if (temp_count >= 0)
			{
				return true;
			}
		}
		check_jiang = false;
	}
	return false;
}

//����Ƿ�ȫ�Ƕ���
bool CMJlogic::check_all_double(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount)
{ 
	if (m_allow_qidui == false)
	{
		return false;
	}
	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_card, hand_card, hand_card_count);
	unsigned int temp_hand_card_count = hand_card_count;

	for (unsigned int i = 0; i < hand_card_count; i++)
	{
		if (!check_cardvalue(hand_card[i]))
		{
			continue;
		}
		int temp_card_count = get_card_count(temp_hand_card, temp_hand_card_count, hand_card[i]);
		if (temp_card_count >1 && temp_card_count <4)
		{ 

			if (del_card_value(temp_hand_card, temp_hand_card_count, hand_card[i], 2))
			{
				temp_hand_card_count -= 2;
			}

		}
		else if (temp_card_count == 4)
		{ 
			if (del_card_value(temp_hand_card, temp_hand_card_count, hand_card[i], 4))
			{
				temp_hand_card_count -= 4;
			}
		}
	}

	if (temp_hand_card_count == 0)
	{
		return true;
	}
	else if (kingcount >= temp_hand_card_count)
	{
		return true;
	}

	return false;


	return false;
}

/*
@param : handcard  �������--������������,�����
@param : block	   �������
@param : kingcount �����Ƹ���
*/
bool CMJlogic::checkwinnew(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount)
{

	if ((hand_card_count + kingcount) % 3 != 2)
	{
		return false;
	}
	if (hand_card_count==0)
	{
		if (kingcount %3==2)
		{
			return true;
		}
		return false;
	}
	//�ж��߶�
	if (check_qidui(hand_card, hand_card_count, kingcount))
	{
		return true;
	}

	//�ж��Ƿ���131
	if (check_131(hand_card, hand_card_count, kingcount))
	{
		return true;
	}

	//�ж�ʮ���� 
	if (check_qbk(hand_card, hand_card_count, kingcount))
	{
		return true;
	}

	bool	check_jiang = true;
	bool	bwin = false;
	//��һ�μ�������Ƿ��н��ƣ��ڶ��β���������Ӵ���
	for (int s = 0; s < 2; s++)
	{
		bool check_card[CARD_INDEX_NUM];
		memset(check_card, 0, sizeof(check_card));

		if (check_jiang == false && kingcount == 0)
		{//����û��������û�����ӣ����ܺ���
			return false;
		}
		//��������
		for (unsigned int i = 0; i < hand_card_count; i++)
		{
			if (check_card[hand_card[i]])
			{
				continue;
			}
			//��¼��������
			check_card[hand_card[i]] = true;

			//ÿ���������ƺ�������
			int temp_count = kingcount;

			//�ֽ��ƺ�ʣ�����
			int remain_index[CARD_INDEX_NUM] = { 0 };
			change_card_index(hand_card, hand_card_count, remain_index);

			//��ȡ����
			if (check_jiang)
			{
				if (remain_index[hand_card[i]] < 2)
				{
					continue;
				}
				if (m_allow_mix_jiang)
				{
					remain_index[hand_card[i]] -= 2;
				}
				else
				{
					if (is_258(hand_card[i]))
					{
						remain_index[hand_card[i]] -= 2;
					}
					else
					{ 
						continue;
					}
				}
			}
			else
			{//ȥһ������������
				if (m_allow_mix_jiang)
				{
					remain_index[hand_card[i]] -= 1;
					temp_count -= 1;
				}
				else
				{
					if (is_258(hand_card[i]))
					{
						remain_index[hand_card[i]] -= 1;
						temp_count -= 1;
					}
					else
					{
						continue;
					}

				}
			}
			//ȥ�����з��ƺ����ƵĿ���
			for (int iwind = 0; iwind < 4; iwind++)
			{
				remain_index[MJ_CARD_WIND_EAST + iwind] = remain_index[MJ_CARD_WIND_EAST + iwind] % 3;
			}
			for (int iarrow = 0; iarrow <3; iarrow++)
			{
				remain_index[MJ_CARD_ARROW_HZ + iarrow] = remain_index[MJ_CARD_ARROW_HZ + iarrow] % 3;
			}
			for (unsigned int j = 0; j < hand_card_count; j++)
			{
				unsigned char  temp_card = hand_card[j];

				if (remain_index[temp_card] == 0)
				{
					continue;
				}

				if (temp_card  < MJ_CARD_WIND_EAST)
				{
					int same_flower_index[9] = { 0 };
					unsigned char first_card = 0x00;
					if (temp_card >= MJ_CARD_WAN_1 && temp_card <= MJ_CARD_WAN_9)
					{
						first_card = MJ_CARD_WAN_1;
					}
					else if (temp_card >= MJ_CARD_TIAO_1 && temp_card <= MJ_CARD_TIAO_9)
					{
						first_card = MJ_CARD_TIAO_1;
					}
					else if (temp_card >= MJ_CARD_TONG_1 && temp_card <= MJ_CARD_TONG_9)
					{
						first_card = MJ_CARD_TONG_1;
					}
					if (first_card != 0x00)
					{
						for (int num = 0; num < 9; num++)
						{
							same_flower_index[num] = remain_index[first_card + num];

							remain_index[first_card + num] = 0;
						}

						temp_count -= check_index_combox(same_flower_index);
					}
					if (temp_count < 0)
					{
						break;
					}
				}
				else if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
				{
					unsigned int win_total_count = 0;
					unsigned int min_count = remain_index[temp_card]%3;
					unsigned int max_count = remain_index[temp_card]%3;
					unsigned int wind_kind = 0;
					unsigned int wind_count[4] = { 0 };
					for (int iwind = 0; iwind < 4; iwind++)
					{
						int card_count = remain_index[MJ_CARD_WIND_EAST + iwind]%3;
						if (card_count > 0)
						{
							wind_kind++;
							wind_count[iwind] = card_count;
							win_total_count += wind_count[iwind];
							if (min_count > wind_count[iwind])
							{
								min_count = wind_count[iwind];
							}
							if (max_count < wind_count[iwind])
							{
								max_count = wind_count[iwind];
							}
						}
					}
					 
					//����
					unsigned int wind_count_index[5] = { 0 };
					for (int index = 0; index < 4; index++)
					{
						unsigned int win_card_count = wind_count[index];
						if (win_card_count>0)
						{
							wind_count_index[win_card_count]++;
						}
					}

					////��������С����
					unsigned int card_sort_index[4] = { 0 };
					unsigned int win_len = 0;
					for (int index = 0; index < 5; index++)
					{
						for (unsigned int m = 0; m < wind_count_index[index]; m++)
						{
							card_sort_index[win_len++] = index;
						}
					}

					for (int iwind = 0; iwind < 4; iwind++)
					{
						if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
						{
							remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
						}
					}
					 
					if (wind_kind == 1)
					{
						if (min_count % 3 != 0)
						{
							temp_count -= (3 - (min_count % 3));
						}
					}
					else if (wind_kind == 2)
					{
						if (m_allow_wind_chi)
						{
							if (max_count == min_count)
							{
								temp_count -= (max_count % 3);
							}
							else
							{
								if (min_count == 1 && max_count == 2)
								{
									temp_count -= 3;
								}
								else
								{
									if (win_total_count % 3 != 0)
									{
										temp_count -= (3 - (win_total_count % 3));
									}
								}
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					}
					else if (wind_kind == 3)
					{
						if (m_allow_wind_chi)
						{
							if (card_sort_index[0] == 1 && card_sort_index[1] == 2 && card_sort_index[2] == 3)
							{
								temp_count -= 3;
							}
							else if (card_sort_index[0] == 2 && card_sort_index[1] == 3 && card_sort_index[2] == 4)
							{
								temp_count -= 3;
							}
							else
							{
								if (win_total_count % 3 != 0)
								{
									temp_count -= (3 - (win_total_count % 3));
								}
							}
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						} 
					}
					else if (wind_kind == 4)
					{
						if (m_allow_wind_chi)
						{ 
							if (win_total_count % 3 != 0)
							{
								temp_count -= (3 - (win_total_count % 3));
							} 
						}
						else
						{
							for (int temp_i = 0; temp_i < 4; temp_i++)
							{
								if (wind_count[temp_i] % 3 != 0)
								{
									temp_count -= (3 - (wind_count[temp_i] % 3));
								}
							}
						}
					} 
				}
				else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
				{//��
					unsigned int arrow_total_count = 0;
					unsigned int min_count = remain_index[temp_card]%3;
					unsigned int max_count = remain_index[temp_card]%3;
					unsigned int arrow_kind = 0;
					unsigned int arrow_count[3] = { 0 };
					for (int iarrow = 0; iarrow < 3; iarrow++)
					{
						int card_count = remain_index[MJ_CARD_ARROW_HZ + iarrow] % 3;
						if (card_count > 0)
						{
							arrow_kind++;
							arrow_count[iarrow] = card_count;
							arrow_total_count += arrow_count[iarrow]; 
							if (min_count > arrow_count[iarrow])
							{
								min_count = arrow_count[iarrow];
							}
							if (max_count < arrow_count[iarrow])
							{
								max_count = arrow_count[iarrow];
							}
						}
					} 

					//����
					unsigned int arrow_count_index[5] = { 0 };
					for (int index = 0; index < 3; index++)
					{
						unsigned int arrow_card_count = arrow_count[index];
						if (arrow_card_count>0 && arrow_card_count <= 4)
						{
							arrow_count_index[arrow_card_count]++;
						}
					}

					////��������С����
					unsigned int card_sort_index[4] = { 0 };
					unsigned int win_len = 0;
					for (int index = 0; index < 5; index++)
					{
						for (unsigned int m = 0; m < arrow_count_index[index]; m++)
						{
							card_sort_index[win_len++] = index;
						}
					}

					for (int iarrow = 0; iarrow < 3; iarrow++)
					{
						remain_index[MJ_CARD_ARROW_HZ + iarrow] = 0;
					}

					if (m_allow_arrow_chi)
					{
						if (arrow_kind == 1)
						{//�����ɿ��� 
							remain_index[temp_card] = 0;
							if (min_count % 3 != 0)
							{
								temp_count -= (3 - min_count % 3);
							}
						}
						else if (arrow_kind == 2)
						{
							if (max_count == min_count)
							{
								temp_count -= (max_count % 3);
							}
							else
							{ 
								if (min_count == 1 && max_count == 2)
								{
									temp_count -= 3;
								}
								else
								{ 
									if (arrow_total_count % 3 != 0)
									{
										temp_count -= (3 - arrow_total_count % 3);
									}
								}
							}

						}
						else if (arrow_kind == 3)
						{
							if (min_count == max_count)
							{

							}
							else
							{  
								if (card_sort_index[0] == 1 && card_sort_index[1] == 2 && card_sort_index[2] == 3)
								{
									temp_count -= 3;
								} 
								else
								{
									if (arrow_total_count % 3 != 0)
									{
										temp_count -= (3 - (arrow_total_count % 3));
									}
								}
							}
						}
					}
					else
					{
						for (int temp_i = 0; temp_i < 3; temp_i++)
						{
							if (arrow_count[temp_i] % 3 != 0)
							{
								temp_count -= (3 - arrow_count[temp_i] % 3);
							}
						}
					}

				}
				else
				{
					/*
					
						if (remain_index[temp_card] == 2)
						{
							if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
							{
								if (m_allow_wind_chi)
								{//����Է� 
									int wind_count = 0;
									for (int iwind = 0; iwind < 4; iwind++)
									{
										if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
										{
											wind_count++;
										}
									}

									if (wind_count == 1 || wind_count == 2)
									{
										remain_index[temp_card] = 0;
										temp_count -= 1;
									}
									else if (wind_count == 3)
									{
										int del_count = 0;
										for (int iwind = 0; iwind < 4; iwind++)
										{
											if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
											{
												del_count += remain_index[MJ_CARD_WIND_EAST + iwind];
												remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
											}
										}
										temp_count -= (6 - del_count);
									}
									else if (wind_count == 4)
									{
										int win_total_count = 0;
										for (int iwind = 0; iwind < 4; iwind++)
										{
											win_total_count += remain_index[MJ_CARD_WIND_EAST + iwind];
											remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
										}
										if (win_total_count % 3 != 0)
										{
											temp_count -= (3 - (win_total_count % 3));
										}
									}
								}
								else
								{
									//��ɿ���
									remain_index[temp_card] = 0;
									temp_count -= 1;
								}

							}
							else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
							{//�� 
								if (m_allow_arrow_chi)
								{//�������
									int arrow_count = 0;
									for (int iwind = 0; iwind < 3; iwind++)
									{
										if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0)
										{
											arrow_count++;
										}
									}

									if (arrow_count == 1 || arrow_count == 2)
									{
										remain_index[temp_card] = 0;
										temp_count -= 1;
									}
									else if (arrow_count == 3)
									{
										int del_count = 0;
										for (int iwind = 0; iwind < 3; iwind++)
										{
											if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0)
											{
												del_count += remain_index[MJ_CARD_ARROW_HZ + iwind];
												remain_index[MJ_CARD_ARROW_HZ + iwind] = 0;
											}
										}
										temp_count -= (6 - del_count);
									}

								}
								else
								{
									//��ɿ���
									remain_index[temp_card] = 0;
									temp_count -= 1;
								}
							}
						}
						if (temp_count < 0)
						{
							break;
						}
						if (remain_index[temp_card] == 1)
						{
							//�ж��Ƿ��Ƿ��ƣ���������
							if (temp_card >= MJ_CARD_WIND_EAST &&  temp_card <= MJ_CARD_WIND_NORTH)
							{
								if (m_allow_wind_chi)
								{
									int wind_count = 0;
									for (int iwind = 0; iwind < 4; iwind++)
									{
										if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0)
										{
											wind_count++;
										}

									}

									if (wind_count == 1)
									{
										remain_index[temp_card] = 0;
										temp_count -= 2;
									}
									else if (wind_count == 2)
									{
										remain_index[temp_card] = 0;
										temp_count -= 1;
										for (int iwind = 0; iwind < 4; iwind++)
										{
											if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0 && (MJ_CARD_WIND_EAST + iwind) != temp_card)
											{
												remain_index[MJ_CARD_WIND_EAST + iwind] -= 1;
												break;
											}
										}
									}
									else if (wind_count == 3)
									{
										remain_index[temp_card] = 0;
										for (int iwind = 0; iwind < 4; iwind++)
										{
											if (remain_index[MJ_CARD_WIND_EAST + iwind] > 0 && (MJ_CARD_WIND_EAST + iwind) != temp_card)
											{
												remain_index[MJ_CARD_WIND_EAST + iwind] -= 1;
											}
										}
									}
									else if (wind_count == 4)
									{
										int win_total_count = 0;
										for (int iwind = 0; iwind < 4; iwind++)
										{
											win_total_count += remain_index[MJ_CARD_WIND_EAST + iwind];
											remain_index[MJ_CARD_WIND_EAST + iwind] = 0;
										}
										if (win_total_count % 3 != 0)
										{
											temp_count -= (3 - (win_total_count % 3));
										} 
									 
									}

								}
								else
								{
									remain_index[temp_card] = 0;
									temp_count -= 2;

								}
							}
							else if (temp_card >= MJ_CARD_ARROW_HZ &&  temp_card <= MJ_CARD_ARROW_BB)
							{
								if (m_allow_arrow_chi)
								{
									int arrow_count = 0;
									for (int iwind = 0; iwind < 3; iwind++)
									{
										if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0)
										{
											arrow_count++;
										}

									}

									if (arrow_count == 1)
									{
										remain_index[temp_card] = 0;
										temp_count -= 2;
									}
									else if (arrow_count == 2)
									{
										remain_index[temp_card] = 0;
										temp_count -= 1;
										for (int iwind = 0; iwind < 3; iwind++)
										{
											if (remain_index[MJ_CARD_ARROW_HZ + iwind] > 0 && (MJ_CARD_ARROW_HZ + iwind) != temp_card)
											{
												remain_index[MJ_CARD_ARROW_HZ + iwind] -= 1;
												break;
											}
										}
									}
									else if (arrow_count == 3)
									{
										remain_index[MJ_CARD_ARROW_HZ] -= 1;
										remain_index[MJ_CARD_ARROW_HZ + 1] -= 1;
										remain_index[MJ_CARD_ARROW_HZ + 2] -= 1;
									}
								}
								else
								{
									remain_index[temp_card] = 0;
									temp_count -= 2;
								}
							}

							if (temp_count < 0)
							{
								break;
							}
						}
					*/
					   
				}

				if (temp_count < 0)
				{
					break;
				} 
			}

			if (temp_count >= 0)
			{
				return true;
			}
		}
		check_jiang = false;
	}
	 
	return false;
}
 
/* ����Ƿ����
@param : hand_card  �������
@param : hand_card_count  ���������
@param : check_card ��Ҵ������
#param : first_card �Ƿ�������
@param : add �Ƿ�����ӵ�����
return reslut;
*/
unsigned char CMJlogic::check_can_win(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned char check_card, bool first_card, bool add)
{

	unsigned char win_type =   BLOCK_TYPE_GAVE;
	if (hand_card_count > HAND_CARD_COUNT || hand_card_count==0)
	{
		return win_type;
	}
	if (check_cardvalue(check_card) == false )
	{
		return  win_type;
	}
	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_card, hand_card, hand_card_count);
	unsigned int temp_hand_card_count = hand_card_count;
	  
	//���������ӵ�������
	if (add )
	{
		add_card(temp_hand_card, temp_hand_card_count, &check_card, 1);
	} 

	//��ȡ���Ӹ���
	unsigned int king_count = 0;
	if (m_allow_laizi)
	{
		king_count = is_have_king_value(temp_hand_card, temp_hand_card_count);

		if (king_count >= m_king_count_limit )
		{//�Ĺ�ֱ�Ӻ���
			return BLOCK_TYPE_HU;
		}


	}
	sort_card(temp_hand_card, temp_hand_card_count);
	bool check_reslut = false;
	 
	unsigned char temp_no_king_card[HAND_CARD_COUNT] = { 0x00 };

	if (m_king_count == 0)
	{
		memcpy(temp_no_king_card, temp_hand_card, temp_hand_card_count);
	}
	else
	{
		int len = 0;
		for (int i = 0; i < HAND_CARD_COUNT; i++)
		{
			if (is_king(temp_hand_card[i]) == false)
			{
				temp_no_king_card[len++] = temp_hand_card[i];
			}
			/*for (int j = 0; j < m_king_count; j++)
			{
				if (temp_hand_card[i] != m_king_card[j])
				{
					break;
				}
			}*/

		}
	} 
	unsigned int temp_no_king_card_count = temp_hand_card_count - king_count;
	sort_card(temp_no_king_card, temp_no_king_card_count);
	if (temp_no_king_card_count <= 0)
	{
		if (king_count > 0)
		{
			return BLOCK_TYPE_HU;  // �ܺ�, ���ƿ��ܶ���������
		} 
		return win_type;
	}
	 
	 
	check_reslut = checkwinnew(temp_no_king_card, temp_no_king_card_count, king_count);
	 
	if (check_reslut)
	{
		win_type = BLOCK_TYPE_HU;
	} 

	return win_type;
}
  
//�ж��������Ƿ������ϳ�˳��-����
bool CMJlogic::check_comb_shun(unsigned char card_value[], unsigned int card_count)
{
	if (card_count != 3 && card_count != 4)
	{
		return false;
	}

	unsigned char temp_card[4] = { 0x00 };
	memcpy(temp_card, card_value, card_count);

	sort_card(temp_card, card_count);

	 //�����ֵ
	for (unsigned int i = 0; i < card_count; i++)
	{
		if (this->check_cardvalue(temp_card[i]) == false)
		{
			return false;
		}
	}

	if (card_count == 3)
	{	
		//��
		if (temp_card[0] == temp_card[1] && temp_card[0] == temp_card[2])
		{
			return true;
		}
		 
		//��
		if (temp_card[0] >= 0x31 && temp_card[2] <= 0x34 )
		{//�� 
			if (temp_card[0] != temp_card[1] && temp_card[1] != temp_card[2])
			{
				return true;
			}
		}
		else if (temp_card[0] >= 0x35 && temp_card[2] <= 0x37 || temp_card[2] <  0x30)
		{//�з���
			if (temp_card[2] == temp_card[0] + 1 && temp_card[2] == temp_card[0] + 2)
			{
				return true;
			}  
		} 
	}
	else
	{//��
		if (temp_card[0] == temp_card[1] && temp_card[0] == temp_card[2] && temp_card[0] == temp_card[3])
		{
			return true;
		}
	} 
	return false;
}


//�ж��Ƿ��13�� --- ������е��������š���Ͳ��������������֮�䶼���ܿ��ƻ��ظ�  ���ơ���Ԫ�Ʋ����ظ�
bool CMJlogic::check_qbk(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount)
{
	if (m_allow_qbk == false)
	{
		return false;
	}
	if (hand_card_count + kingcount != 14)
	{
		return false;
	}

	int temp_king_count = kingcount;
	int index_card[CARD_INDEX_NUM] = { 0 };
	change_card_index(hand_card, hand_card_count, index_card);
	//�����ж���
	for (unsigned int i = 0; i < hand_card_count; i++)
	{
		if (index_card[hand_card[i]] > 1)
		{
			return false;
		}
	}

	//ȥ�����ƣ�����
	int wind_count = 0;
	for (int i = 0; i < 7; i++)
	{
		if (index_card[MJ_CARD_WIND_EAST + i] == 1)
		{
			index_card[MJ_CARD_WIND_EAST + i] = 0;
			wind_count++;
		}
	}
	//ʮ���ñض��в�����5�ŷ��ƺͼ���
	if (wind_count < 5 && temp_king_count < (5 - wind_count))
	{
		return false;
	}
	//������ȫ��ת��Ϊ����
	temp_king_count = temp_king_count - (7 - wind_count);

	//������ļ��,ÿ�ֻ�ɫ�������,
	int distance[3][4] = { -1 };
	int last_card[3] = { 0 };
	int card_count[3] = { 0 };
	int last_pos[3] = { 0 };
	bool first[3] = { true, true, true };
	for (int i = 0; i < 9; i++)
	{
		//��
		if (index_card[MJ_CARD_WAN_1 + i] == 1)
		{
			if (card_count[0] > 3)
			{
				return false;
			}
			int temp_distance = i - last_card[0];
			if (first[0] == false && temp_distance < 3)
			{
				return false;
			}
			first[0] = false;
			distance[0][card_count[0]++] = temp_distance;
			last_card[0] = i;
			last_pos[0] = i;
		}

		//��
		if (index_card[MJ_CARD_TIAO_1 + i] == 1)
		{
			if (card_count[1] >3)
			{
				return false;
			}
			int temp_distance = i - last_card[1];
			if (first[1] == false && temp_distance < 3)
			{
				return false;
			}
			first[1] = false;
			distance[1][card_count[1]++] = temp_distance;
			last_card[1] = i;
			last_pos[1] = i;

		}
		//Ͳ
		if (index_card[MJ_CARD_TONG_1 + i] == 1)
		{
			if (card_count[2] > 3)
			{
				return false;
			}
			int temp_distance = i - last_card[2];
			if (first[2] == false && temp_distance < 3)
			{
				return false;
			}
			first[2] = false;
			distance[2][card_count[2]++] = temp_distance;
			last_card[2] = i;
			last_pos[2] = i;
		}
	}
	

	if (temp_king_count > 0)
	{//�������ӣ���Ҫ��������䵽���ʵĵط�
		//������һ���Ƶļ��
		for (int i = 0; i < 3; i++)
		{
			if (last_pos[i] != 9)
			{
				distance[i][card_count[i]] = 9 - last_card[i];
			}

		}
		
		for (int i = 0; i < 3; i++)
		{
			//���û�У�������������
			if (card_count[i] == 0)
			{
				temp_king_count -= 3;
			}
			else if (card_count[i] == 1)
			{ //һ���򣬿������2��
				temp_king_count -= 2;
			}
			else if (card_count[i] == 2)
			{//�����������������һ��

				if (distance[i][0] >= 3)
				{//��һ���
					temp_king_count--;
				}
				if (distance[i][1] >= 6)
				{//���һ�����
					temp_king_count--;
				}
				if (distance[i][2] >= 3)
				{//��ż��
					temp_king_count--;
				}
			}
		}

		if (temp_king_count > 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}
//�ж��Ƿ��131
bool CMJlogic::check_131(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount)
{
	
	if (m_allow_131 == false)
	{
		return false;
	}
	if (hand_card_count + kingcount != 14)
	{
		return false;
	} 
	int  right_num = 0;
	bool	bJiang = false;
	unsigned char  standard_card[13] = { MJ_CARD_WAN_1, MJ_CARD_WAN_9, 
								MJ_CARD_TONG_1,MJ_CARD_TONG_9, 
								MJ_CARD_TIAO_1, MJ_CARD_TIAO_9,
								MJ_CARD_WIND_EAST, MJ_CARD_WIND_SOUTH, MJ_CARD_WIND_WEST, MJ_CARD_WIND_NORTH,
								MJ_CARD_ARROW_HZ, MJ_CARD_ARROW_FC, MJ_CARD_ARROW_BB };
	unsigned int index = 0;


	int temp_count = 0;
	int index_card[CARD_INDEX_NUM] = { 0 };
	change_card_index(hand_card, hand_card_count, index_card);

	
	for (unsigned int i = 0; i < hand_card_count; i++)
	{
		if (get_card_count(standard_card, 13, hand_card[i]) == 0)
		{
			return false;
		}
	}

	for (unsigned int i = 0; i < hand_card_count; i++)
	{
		if (index_card[hand_card[i]] == 1)
		{
			right_num++;

		}
		else if (bJiang == false && index_card[hand_card[i]] == 2)
		{
			bJiang = true;
			right_num += 2;
		} 
	} 

	if (right_num + kingcount == 14 )
	{
		return true;
	}

	return false;
}
//�ж��Ƿ����߶�
bool CMJlogic::check_qidui(const unsigned char hand_card[], const unsigned int hand_card_count, unsigned int kingcount)
{
	if (m_allow_qidui == false)
	{
		return false;
	}
	if (hand_card_count + kingcount != 14)
	{
		return false;
	}

	unsigned char temp_hand_card[HAND_CARD_COUNT] = { 0x00 };
	memcpy(temp_hand_card, hand_card, hand_card_count);
	unsigned int temp_hand_card_count = hand_card_count;

	for (unsigned int i = 0; i < hand_card_count; i++)
	{
		if (!check_cardvalue(hand_card[i]))
		{
			continue;
		}
		int temp_card_count = get_card_count(temp_hand_card, temp_hand_card_count, hand_card[i]);
		if (temp_card_count >1 && temp_card_count <4)
		{
			/*unsigned char del[2] = { 0x00 };
			del[0] = hand_card[i];
			del[1] = hand_card[i];*/

			if (del_card_value(temp_hand_card, temp_hand_card_count, hand_card[i], 2))
			{
				temp_hand_card_count -= 2;
			}

		}else if (temp_card_count == 4)
		{  
		/*	unsigned char del[4] = { 0x00 };
			del[0] = hand_card[i];
			del[1] = hand_card[i];
			del[2] = hand_card[i];
			del[3] = hand_card[i];*/
			if (del_card_value(temp_hand_card, temp_hand_card_count, hand_card[i], 4))
			{
				temp_hand_card_count -= 4; 
			}
		}
	}

	if (temp_hand_card_count == 0)
	{
		return true;
	}
	else if (kingcount >= temp_hand_card_count)
	{
		return true;
	}

	return false;
}
 
//��ȡ��������
int CMJlogic::get_index_count(const  int card_index[], unsigned int card_count)
{
	int index_count = 0;
	for (unsigned int i = 0; i < card_count; i++)
	{
		if (card_index[i]>0)
		{
			index_count++;
		}
	}

	return index_count;
}

//��������
bool CMJlogic::analyze_card(const  int card_index[], const unsigned int jiang_index, int remain_index[])
{
	//�������� 
	int  temp_card_index[CARD_INDEX_NUM] = { 0x00 }; 
	for (int i = 0; i < CARD_INDEX_NUM; i++)
	{
		temp_card_index[i] = card_index[i];
	}
	//1����ȡ����
	if (jiang_index >0 && temp_card_index[jiang_index] >= 2)
	{ 
		temp_card_index[jiang_index] -= 2;
	} 
	//Ѱ�ҵ�һ����
	for (int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (temp_card_index[i]>0)
		{
			if ( temp_card_index[i] >=3 )
			{
				temp_card_index[i] -= 3;
				//2����ȡ˳�ӻ����
				if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
				{
					return true;
				}
				return analyze_card(temp_card_index, 0, remain_index);
			}
			if ((i & 0x0F) < 0x08 && i <= MJ_CARD_TONG_9 && temp_card_index[i + 1]>0 && temp_card_index[i + 2]>0)
			{ 
				temp_card_index[i] -= 1;
				temp_card_index[i+1] -= 1;
				temp_card_index[i + 2] -= 1;
				//2����ȡ˳�ӻ����
				if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
				{
					return true;
				}
				return analyze_card(temp_card_index, 0, remain_index);
			}
			//����
			if (m_allow_wind_chi && i >= MJ_CARD_WIND_EAST && i <= MJ_CARD_WIND_NORTH)
			{

				if (i == MJ_CARD_WIND_EAST)
				{
					if (temp_card_index[MJ_CARD_WIND_WEST]>0 && temp_card_index[MJ_CARD_WIND_SOUTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_SOUTH]>0 && temp_card_index[MJ_CARD_WIND_NORTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_WEST]>0 && temp_card_index[MJ_CARD_WIND_NORTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}

				}
				else if (i == MJ_CARD_WIND_SOUTH)
				{
					if (temp_card_index[MJ_CARD_WIND_WEST]>0 && temp_card_index[MJ_CARD_WIND_NORTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_EAST]>0 && temp_card_index[MJ_CARD_WIND_WEST]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_EAST]>0 && temp_card_index[MJ_CARD_WIND_NORTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
				}
				else if (i == MJ_CARD_WIND_WEST)
				{
					if (temp_card_index[MJ_CARD_WIND_SOUTH]>0 && temp_card_index[MJ_CARD_WIND_NORTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_EAST]>0 && temp_card_index[MJ_CARD_WIND_SOUTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_EAST]>0 && temp_card_index[MJ_CARD_WIND_NORTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
				}
				else if (i == MJ_CARD_WIND_NORTH)
				{
					if (temp_card_index[MJ_CARD_WIND_SOUTH]>0 && temp_card_index[MJ_CARD_WIND_WEST]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_EAST]>0 && temp_card_index[MJ_CARD_WIND_WEST]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
					if (temp_card_index[MJ_CARD_WIND_EAST]>0 && temp_card_index[MJ_CARD_WIND_SOUTH]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
				}
			}

			//����
			if (m_allow_arrow_chi && i >= MJ_CARD_ARROW_HZ && i <= MJ_CARD_ARROW_BB)
			{
				if (MJ_CARD_ARROW_HZ == i)
				{
					if (temp_card_index[MJ_CARD_ARROW_FC]>0 && temp_card_index[MJ_CARD_ARROW_BB]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
				}
				else if (MJ_CARD_ARROW_FC == i)
				{
					if (temp_card_index[MJ_CARD_ARROW_HZ]>0 && temp_card_index[MJ_CARD_ARROW_BB]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
				}
				else if(MJ_CARD_ARROW_BB == i)
				{
					if (temp_card_index[MJ_CARD_ARROW_FC]>0 && temp_card_index[MJ_CARD_ARROW_HZ]>0)
					{
						temp_card_index[i] -= 1;
						temp_card_index[i + 1] -= 1;
						temp_card_index[i + 2] -= 1;
						//2����ȡ˳�ӻ����
						if (get_index_count(temp_card_index, CARD_INDEX_NUM) == 0)
						{
							return true;
						}
						return analyze_card(temp_card_index, 0, remain_index);
					}
				}
			}

		}
	}

	if (get_index_count(temp_card_index, CARD_INDEX_NUM)==0)
	{
		return true;
	}

	for (int i = 0; i < CARD_INDEX_NUM; i++)
	{
		remain_index[i] = temp_card_index[i];
	}

	return false;

}

//������Ϸģʽ
void CMJlogic::set_mode(game_mode param)
{
	m_allow_qidui = param.bqidui;
	m_allow_mix_jiang = param.bmixjiang;
	m_allow_laizi = param.blaizi;
	m_allow_chi = param.bchi;
	m_allow_gang = param.bgang;
	m_allow_peng = param.bpeng;
	m_allow_wind_chi = param.bwind;
	m_allow_arrow_chi = param.barrow;
	m_allow_131 = param.b131; 
	m_allow_qbk = param.bqbk;
}

//
//����������
void CMJlogic::set_king_value(unsigned  char  kingcard[], unsigned int kingcount)
{
	if (kingcount <= 0 || m_allow_laizi == false || kingcount > KING_CARD_NUM)
	{
		return;
	}
	m_king_count = 0;
	memset(m_king_card, 0, sizeof(m_king_card));

	for (unsigned int i = 0; i < kingcount; i++)
	{
		if (check_cardvalue(kingcard[i]))
		{
			m_king_card[i] = kingcard[i];
			m_king_count++;
		}
	}  

}
//���� �����Ƴ���һ�����������ֱ�Ӻ���
void CMJlogic::set_king_over_win(unsigned int kingcount)
{
	m_king_count_limit = kingcount;
}
//������������Ƿ�Ϸ�
bool CMJlogic::check_all_data(CMJHandCard user_hand[], CMJBlockCard user_block[], CMJOutCard user_out[], CMJWallCard wall, unsigned int paly_count)
{
	int card_index[CARD_INDEX_NUM] = { 0 };
	 
	for (unsigned int i = 0; i < paly_count; i++)
	{
		//����
		unsigned char handcard[HAND_CARD_COUNT] = { 0 };
		unsigned int handcount = user_hand[i].GetAllHandCard(handcard); 
		change_card_index(handcard, handcount, card_index);
		//����
		unsigned char outcard[HAND_CARD_COUNT] = { 0 };
		unsigned int outcount = user_out[i].GetAllOutCard(outcard);
		change_card_index(outcard, outcount, card_index);
		//��ǽ
		unsigned char wallcard[MAX_CARD_NUM] = { 0 };
		unsigned int wallcount = wall.GetAllWallCard(wallcard);
		change_card_index(wallcard, wallcount, card_index);
		//����
		for (unsigned int m = 0; m < user_block[i].GetBlockLength(); m++)
		{ 
			BlockData	blockinfo;
			user_block[i].GetBlock(m, blockinfo);
			if ( blockinfo.block_type == BLOCK_TYPE_CHI )
			{ 
				int temp_card = get_card_value(blockinfo.card_value[0]);
				if (temp_card != -1)
				{
					card_index[temp_card] += 1;
				}
				temp_card = get_card_value(blockinfo.card_value[1]);
				if (temp_card != -1)
				{
					card_index[temp_card] += 1;
				}
				temp_card = get_card_value(blockinfo.card_value[2]);
				if (temp_card != -1)
				{
					card_index[temp_card] += 1;
				}

				 
			}
			else if (blockinfo.block_type == BLOCK_TYPE_PENG )
			{
				int temp_card = get_card_value(blockinfo.blocked_card);
				if (temp_card != -1)
				{
					card_index[temp_card] += 3;
				}
			}
			else if (blockinfo.block_type == BLOCK_TYPE_GANG)
			{
				int temp_card = get_card_value(blockinfo.blocked_card);
				if (temp_card != -1)
				{
					card_index[temp_card] += 4;
				}
			}  
		} 
	}

	for (int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (check_cardvalue(i) == false)
		{
			continue;
		}
		if (card_index[i] != 4 )
		{
			return false;
		}
	}
	return true; 
}

//���ٱ����Ƿ�任
bool CMJlogic::check_king_usedd(CMJHandCard user_hand)
{
	//��������
	CMJHandCard tempHand;
	tempHand.OnAddHandCards(user_hand);
	int kingcount = 0;
	
	for (unsigned int i = 0; i < m_king_count; i++)
	{
		kingcount += tempHand.OnGetCardCount(m_king_card[i]);
	}

	if (kingcount > 0 )
	{ 
		if (kingcount == 1)
		{
			return true;
		}
		//ɾ���ٱ���
		for (unsigned int i = 0; i < m_king_count; i++)
		{
			int tempcount = tempHand.OnGetCardCount(m_king_card[i]); 
			tempHand.OnDelHandCard(m_king_card[i], tempcount);
		}

		tempHand.CardSort();
		unsigned char  temp_card[HAND_CARD_COUNT] = { 0x00 };
		unsigned int  temp_count = tempHand.GetAllHandCard(temp_card);
		int  temp_index[CARD_INDEX_NUM] = { 0 };
		change_card_index(temp_card, temp_count, temp_index);
		 
		if (kingcount == 2)
		{//ֻ������
			int  remain_index[CARD_INDEX_NUM] = { 0 };
			return analyze_card(temp_index, 0, remain_index) ? false : true;
		}
		else if(kingcount == 3)
		{//ֻ��������
			return check_can_win(temp_card, temp_count, m_king_card[0], false, false) ? false : true;
		}
		else if (kingcount >= 4)
		{
			//ֻ������߶�
			for (unsigned i = 0; i < CARD_INDEX_NUM; i++)
			{
				if (check_cardvalue(i) && temp_index[i] != 2)
				{
					return true;
				}
			} 
		} 
	}

	return false;

}


//�ж��Ƿ�������
bool CMJlogic::is_king(unsigned char cardvalue)
{ 
	if (check_cardvalue(cardvalue) == false)
	{ 
		return false;
	}
	if (m_allow_laizi)
	{
		for (unsigned int i = 0; i < m_king_count; i++)
		{
			if (m_king_card[i] == cardvalue)
			{
				return true;
			}
		}
	}
	return false;
}
//����Ƿ�������,�������ӵĸ���
int CMJlogic::is_have_king_value(unsigned char handcard[],unsigned char cardcount)
{
	int king_count = 0;
	for (unsigned int i = 0; i < m_king_count; i++)
	{ 
		king_count += this->get_card_count(handcard, cardcount,m_king_card[i]);
	} 
	return king_count;
}

//ɾ�����е�����
int CMJlogic::del_king_value(unsigned char handcard[], unsigned char cardcount)
{
	int delcount = 0;
	for (unsigned int i = 0; i < m_king_count; i++)
	{
		int count = this->get_card_count(handcard, cardcount, m_king_card[i]);
		if (del_card_value(handcard, cardcount, m_king_card[i], count))
		{
			delcount += count;
		} 
	}

	return delcount;
}
//�ж��Ƿ�������
bool  CMJlogic::is_zi_card(unsigned char card)
{
	if (card >= MJ_CARD_WIND_EAST && MJ_CARD_ARROW_BB >= card)
	{
		return true;
	}
	return false;
}
//�ж��Ƿ��Ƿ��� �� �� �� ��
bool CMJlogic::is_wind_card(unsigned char card)
{
	if (card >= MJ_CARD_WIND_EAST && MJ_CARD_WIND_NORTH >= card)
	{
		return true;
	}
	return false;
}
//�ж��Ƿ��Ǽ��ƣ��� �� ��
bool CMJlogic::is_arrow_card(unsigned char card)
{
	if (card >= MJ_CARD_ARROW_HZ && MJ_CARD_ARROW_BB >= card)
	{
		return true;
	}
	return false;
}
//�ж��Ƿ���19
bool  CMJlogic::is_one_nine(unsigned char card)
{
	if (card > MJ_CARD_TONG_9)
	{
		return false;
	}
	int  value = MASK_VALUE & card;

	if (value == 1 || value == 9)
	{
		return true;
	}
	return false;
}

//�ж��Ƿ���2��5��8
bool CMJlogic::is_258(unsigned char card)
{
	if (card > MJ_CARD_TONG_9)
	{
		return false;
	}
	int  value = MASK_VALUE & card;

	if (value == 2 || value == 5 || value == 8)
	{
		return true;
	}
	return false;
}

//��������������˳��
int CMJlogic::check_index_combox(int index_card[])
{

	//ͬһ��ɫ
	int need_king = 0;
	int first_pos = -1;
	int card_count = 0;
	for (int i = 0; i < 9; i++)
	{
		if (index_card[i] > 0)
		{
			if (first_pos < 0)
			{
				first_pos = i;
			}
			card_count++;
		}
	}
	if (card_count == 0)
	{
		return 0;
	}
	int card_index[9] = { 0 };

	memcpy(card_index, index_card, sizeof(card_index));
	//���ֻ��һ������ֻ����ɿ���,��������֣������ѡ����ɿ��ֻ�˳��

	if (card_count == 1)
	{
		if (card_index[first_pos] % 3 != 0)
		{
			need_king += (3 - (card_index[first_pos] % 3));
		}
		card_index[first_pos] = 0;
	}
	else
	{
		//������
		int  one_need_king = 0;
		int f_index[9] = { 0 };
		memcpy(f_index, card_index, sizeof(f_index));

		if (f_index[first_pos] % 3 != 0)
		{
			one_need_king += (3 - f_index[first_pos] % 3);
		}
		f_index[first_pos] = 0;
		one_need_king += check_index_combox(f_index);

		//��˳��
		int  two_need_king = 0;
		int t_index[9] = { 0 };
		memcpy(t_index, card_index, sizeof(t_index));
		if (t_index[first_pos] >= 3)
		{
			t_index[first_pos] -= 3;
		}
		for (int i = 1; i < 3; i++)
		{
			if (first_pos + i >= 9)
			{
				two_need_king += t_index[first_pos];
			}
			else
			{
				if (t_index[first_pos + i] < t_index[first_pos])
				{
					two_need_king += (t_index[first_pos] - t_index[first_pos + i]);

					t_index[first_pos + i] = 0;
				}
				else
				{
					t_index[first_pos + i] -= t_index[first_pos];
				}
			}
		}
		t_index[first_pos] = 0;
		two_need_king += check_index_combox(t_index);


		if (one_need_king > two_need_king)
		{
			need_king = two_need_king;
		}
		else
		{
			need_king = one_need_king;
		}
	}

	return need_king;

}




