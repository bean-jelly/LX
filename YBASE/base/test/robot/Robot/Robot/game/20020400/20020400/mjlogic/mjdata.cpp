#include "mjdata.h" 
#include <time.h>
#include <random>
//���������
bool IsCardValid(unsigned char card_value)
{  
	 
	unsigned char color = (card_value&MASK_COLOR) >> 4;
	unsigned char value = (card_value&MASK_VALUE);
	return (((value >= 0x01) && (value <= 0x09) && (color <= 2)) || ((value >= 0x01) && (value <= 0x0F) && (color == 3)));


	//if (CARD_COUNT == 108)
	//{
	//	return (value >= 0x01) && (value <= 0x09) && (color <= 2);
	//}
	//else if (CARD_COUNT == 112)
	//{//ֻ��������
	//	return ((value >= 0x01) && (value <= 0x09) && (color <= 2) || ((value == 0x05) && (color == 3)));
	//}
	//else if (CARD_COUNT == 136)
	//{
	//	return (((value >= 0x01) && (value <= 0x09) && (color <= 2)) || ((value >= 0x01) && (value <= 0x07) && (color == 3)));
	//}
	//else if (CARD_COUNT == 144)
	//{
	//	return (((value >= 0x01) && (value <= 0x09) && (color <= 2)) || ((value >= 0x01) && (value <= 0x0F) && (color == 3)));
	//}
	//return false;
}
 
//ת��������ֵ
int ChangeCardToIndex(const unsigned char card_value[], unsigned int card_count, unsigned int card_index[])
{
	if (card_count == 0)
	{
		return 0;
	}
	int change_count = 0;
	int temp_value = -1;
	for (unsigned int i = 0; i < card_count; i++)
	{
		if (IsCardValid(card_value[i]))
		{
			temp_value = card_value[i];
			card_index[temp_value]++;
			change_count++;
		}
	}
	return change_count;
}

int GetRandValue(int iLeft, int iRight)
{
	if (iLeft > iRight)
	{
		iRight = iLeft;
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(iLeft, iRight);
	return dis(gen);
}
CMJHandCard::CMJHandCard()
{

	m_hand_length = 0;
	memset(m_hand_card, 0, sizeof(m_hand_card));
}

CMJHandCard::~CMJHandCard()
{
	m_hand_length = 0;
	memset(m_hand_card, 0, sizeof(m_hand_card));
} 

CMJHandCard::CMJHandCard(const CMJHandCard& info)
{
	m_hand_length = info.m_hand_length;
	memcpy(m_hand_card, info.m_hand_card, sizeof(m_hand_card));
}

/*�ж������Ƿ���ָ����
@param: cardvalue  ָ����
return   if find this card return true else return false
*/
bool CMJHandCard::IsHaveCard(unsigned char cardvalue, unsigned int count )
{
	if (IsCardValid(cardvalue) == false || count > 4)
	{
		return false;
	}
	int cardcount = 0;
	for (unsigned int i = 0; i < m_hand_length; i++)
	{
		if (m_hand_card[i] == cardvalue)
		{
			cardcount++;
			if (cardcount == count)
			{
				return true;
			}
		}
	}
	return false;
}

/*��ȡָ���Ƹ���
@param: cardvalue  ָ����
return  card count
*/
unsigned int CMJHandCard::OnGetCardCount(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return 0;
	}
	unsigned int cardcount = 0;
	for (unsigned int i = 0; i < m_hand_length; i++)
	{
		if (m_hand_card[i] == cardvalue)
		{
			cardcount++;
		}
	} 
	return cardcount;
}
 
/*��ȡָ���Ƶ�λ�� 
@param: cardvalue  ָ����
return   if not find return -1 else return pos
*/
int CMJHandCard::OnGetCardPos(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return -1;
	} 
	for (unsigned int i = 0; i < m_hand_length; i++)
	{
		if (m_hand_card[i] == cardvalue)
		{
			return i;
		}
	}
	return -1;
}

/*�����������
@param: cardvalue ����ӵ���
@param: count ��ӵĸ���
return  if success  return 0 else return -1
*/
int CMJHandCard::OnAddHandCard(unsigned char cardvalue, unsigned int count)
{
	//�ж��Ƿ񳬳���󳤶�,��ֵ�Ƿ�Ϸ�
	if (m_hand_length + count > HAND_CARD_COUNT || IsCardValid(cardvalue) == false)
	{
		return -1;
	} 
	for (unsigned int i = 0; i < count; i++)
	{
		m_hand_card[m_hand_length++] = cardvalue; 
	}

	return 0;
} 

//���һ������
bool CMJHandCard::OnAddHandCards(CMJHandCard cards)
{
	unsigned int card_len = cards.GetHandCardLength();
	if (m_hand_length + card_len > HAND_CARD_COUNT)
	{
		return false;
	}

	for (unsigned int i = 0; i < card_len; i++)
	{
		m_hand_card[m_hand_length++] = cards.OnGetHandCard(i); 
	}
	return true;
}

/*ɾ����������
@param: cardvalue ��Ҫɾ������
@param: ɾ���ĸ���
return  if success  return 0 else return -1
*/
int CMJHandCard::OnDelHandCard(unsigned char cardvalue, unsigned int count)
{
	if ( IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	if (OnGetCardCount(cardvalue) < count)
	{
		return -1;
	}
	if (m_hand_length > HAND_CARD_COUNT)
	{
		return -1;
	}
	unsigned int del_count = 0;
	unsigned char temp_card[HAND_CARD_COUNT] = { 0x00 };


	memcpy(temp_card, m_hand_card, m_hand_length);

	for (unsigned int i = 0; i < count; i++)
	{
		int pos = OnGetCardPos(cardvalue);
		if (pos != -1)
		{
			m_hand_card[pos] = 0x00; 
			del_count++;
		}
	}
	if (del_count != count)
	{//ɾ�������ָ�����
		memset(m_hand_card, 0, sizeof(m_hand_card));
		memcpy(m_hand_card, temp_card, m_hand_length);
		return -1;
	}
	m_hand_length -= del_count;
	CardSort();
	  
	return 0;
}
 
/*ɾ��������
@param: cardvalue  Ҫɾ����ֵ  
*/ 
int CMJHandCard::OnDelHandCardAll(unsigned char cardvalue[],unsigned int cardcount)
{ 
	unsigned int del_count = 0;
	for (unsigned int i = 0; i < m_hand_length; i++)
	{ 
		for (unsigned int j = 0; j < cardcount; j++)
		{
			if (IsCardValid(cardvalue[j]) == false)
			{
				continue;
			}

			if (m_hand_card[i] == cardvalue[j])
			{
				m_hand_card[i] = 0x00;
				del_count++;
			}
		}
		
	} 
	m_hand_length -= del_count;
	CardSort();

	return del_count;
}
/*������������
@param: cardvalue  ���õ�ֵ
@param: pos  �޸ĵ�λ�� 
return  if success  return 0 else return -1
*/
int CMJHandCard::OnSetHandCard(unsigned char cardvalue, unsigned int pos)
{
	if (pos >= m_hand_length || IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	m_hand_card[pos] = cardvalue;
	return 0;
}

 
/*��ȡ��������
@param: pos ָ��λ��
return card_value
*/
unsigned char CMJHandCard::OnGetHandCard(unsigned int pos) 
{
	if (pos >= m_hand_length)
	{
		return 0x00;
	}
	return m_hand_card[pos]; 
}
 
/*��ȡ��������
@param: cardvalue[] �洢���ص�����
return ���Ƴ���
*/
unsigned int CMJHandCard::GetAllHandCard(unsigned char cardvalue[]) 
{
	memcpy(cardvalue, m_hand_card, m_hand_length);
	return m_hand_length;
}

/*��ȡ���Ƴ���
return ���Ƴ���
*/
unsigned int CMJHandCard::GetHandCardLength()
{
	return m_hand_length;
}

//����-����
void CMJHandCard::CardSort()
{ 
	unsigned int card_index[CARD_INDEX_NUM] = { 0 };
	ChangeCardToIndex(m_hand_card, HAND_CARD_COUNT, card_index);
	ClearAllData();
	for (unsigned int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (IsCardValid(i) && m_hand_length + card_index[i] <= HAND_CARD_COUNT)
		{
			for (unsigned int s = 0; s < card_index[i]; s++)
			{
				m_hand_card[m_hand_length++] = i; 
			}
		}
	}

}

//�����������
void CMJHandCard::ClearAllData()
{
	m_hand_length = 0;
	for (unsigned int i = 0; i < HAND_CARD_COUNT; i++)
	{
		m_hand_card[i] = 0x00;
	} 
} 


CMJBlockCard::CMJBlockCard()
{
	m_length = 0;
}
CMJBlockCard::~CMJBlockCard()
{
	m_length = 0;
}

//���һ������
bool CMJBlockCard::AddBlockCard(unsigned char chair_id, unsigned char blocked_card, unsigned char block_type, unsigned char block_sub_type,unsigned char card_value[4])
{
	if (m_length >= BLOCK_MAX)
	{
		return false;
	}

	BlockData temp_block;

	memcpy(temp_block.card_value, card_value, sizeof(temp_block.card_value));
	temp_block.blocked_card = blocked_card;
	temp_block.block_type = block_type;
	temp_block.chair_id = chair_id;
	temp_block.block_sub_type = block_sub_type;
	m_block[m_length] = temp_block;
	m_length = m_length + 1;
	return true;
}
bool CMJBlockCard::AddBlockCard(BlockData blockinfo)
{
	if (m_length >= BLOCK_MAX)
	{
		return false;
	} 
	m_block[m_length] = blockinfo;
	m_length = m_length + 1;
	return true;
}
bool CMJBlockCard::AddBlockCards(CMJBlockCard& block)
{
	int nLen = block.GetBlockLength();
	if (nLen <= 0 || nLen + m_length > BLOCK_MAX)
	{
		return false;
	}
	BlockData temp_block;
	for ( int i = 0; i < nLen; i++)
	{
		block.GetBlock(i, temp_block);
		m_block[m_length] = temp_block;
		m_length = m_length + 1;
	}
	return true;
}
//��ȡָ��λ�õ���������
bool CMJBlockCard::GetBlock(unsigned int pos, BlockData& blockinfo)
{ 
	if (pos >= m_length)
	{ 
		memset(&blockinfo, 0, sizeof(blockinfo));
		return false;
	}
	blockinfo = m_block[pos];
	return true;
}
//����ָ��λ�õ���������
bool CMJBlockCard::SetBlock(unsigned int pos, BlockData blockinfo)
{
	if (pos >= m_length  )
	{
		return false;
	}
	m_block[pos] = blockinfo;
	return true;
}
//�Ƴ����һ��
bool CMJBlockCard::PopBack(BlockData& blockinfo)
{
	if (m_length == 0)
	{
		return false;
	} 
	blockinfo = m_block[m_length - 1]; 
	BlockData temp_block;
	m_block[m_length - 1] = temp_block;
	m_length--;
	return true;

}
//ɾ������
bool CMJBlockCard::DelBlock(unsigned int pos)
{
	if (pos >= m_length)
	{
		return false;
	}
	for (int i = pos; i < m_length && (i + 1) < m_length ; i++)
	{  
		m_block[i] = m_block[i+1];
	}

	BlockData temp_block;
	m_block[m_length-1] = temp_block;
	m_length--;
	return true;
}
//�����������
void CMJBlockCard::ClearAllData()
{
	m_length = 0; 
	memset(m_block, 0, sizeof(m_block));
}
//��ȡ���Ƹ���
unsigned int CMJBlockCard::GetBlockLength()
{
	return m_length;
}
  
//������Ϊ��
bool CMJBlockCard::SetPengToGang(unsigned char card_value)
{
	if (IsCardValid(card_value) == false)
	{
		return false;
	}
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_block[i].block_type == BLOCK_TYPE_PENG && m_block[i].blocked_card == card_value)
		{
			m_block[i].block_type = BLOCK_TYPE_GANG;
			m_block[i].block_sub_type = BLOCK_TYPE_GANG_BU;
			m_block[i].card_value[3] = card_value;
			return true;
		}
	}

	return false;
}
//��ԭ��Ϊ��--�����ܺ���Ҫ��ԭ
bool CMJBlockCard::BackGangToPeng(unsigned char card_value)
{
	if (IsCardValid(card_value) == false)
	{
		return false;
	}
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_block[i].block_type == BLOCK_TYPE_GANG && m_block[i].blocked_card == card_value)
		{
			m_block[i].block_type = BLOCK_TYPE_PENG;
			m_block[i].block_sub_type = 0x00;
			m_block[i].card_value[3] = 0x00;
			return true;
		}
	}
	return false;
}
//����������Ƿ���ĳ����
bool CMJBlockCard::IsHaveCard(unsigned char card_value, unsigned char block_type)
{
	if (IsCardValid(card_value) == false)
	{
		return false;
	}
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_block[i].block_type == block_type && m_block[i].blocked_card == card_value)
		{
			return true;
		}
	}
	return false;
}























CMJOutCard::CMJOutCard()
{
	m_length = 0;
	memset(m_out_card, 0, sizeof(m_out_card));

}
CMJOutCard::~CMJOutCard()
{ 
	m_length = 0;
	memset(m_out_card, 0, sizeof(m_out_card));
}

/*�ж������Ƿ���ָ����
@param: cardvalue  ָ����
return   if find this card return true else return false
*/
bool CMJOutCard::IsHaveCard(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return false;
	}
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_out_card[i] == cardvalue)
		{
			return true;
		}
	}
	return false;
}

/*��ȡָ���Ƹ���
@param: cardvalue  ָ����
return  card count
*/
unsigned int CMJOutCard::OnGetCardCount(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return 0;
	}
	int cardcount = 0;
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_out_card[i] == cardvalue)
		{
			cardcount++;
		}
	}
	return cardcount;
}

/*��ȡָ���Ƶ�λ��
@param: cardvalue  ָ����
return   if not find return -1 else return pos
*/
int CMJOutCard::OnGetCardPos(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_out_card[i] == cardvalue)
		{
			return i;
		}
	}
	return -1;
}

/*��ӳ�������
@param: cardvalue ����ӵ���
@param: count ��ӵĸ���
return  if success  return 0 else return -1
*/
int CMJOutCard::OnAddOutCard(unsigned char cardvalue, unsigned int count)
{
	//�ж��Ƿ񳬳���󳤶�,��ֵ�Ƿ�Ϸ�
	if (m_length + count > MAX_OUT_CARD || IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	for (unsigned int i = 0; i < count; i++)
	{
		m_out_card[m_length] = cardvalue;
		m_length = m_length + 1;
	}

	return 0;
}

//���һ������
bool CMJOutCard::OnAddHandCards(CMJOutCard cards)
{
	unsigned int card_len = cards.GetOutCardLength();
	if (m_length + card_len > MAX_OUT_CARD)
	{
		return false;
	}

	for (unsigned int i = 0; i < card_len; i++)
	{
		m_out_card[m_length] = cards.OnGetOutCard(i);
		m_length = m_length + 1;
	}
	return true;
}

/*ɾ����������
@param: cardvalue ��Ҫɾ������
@param: ɾ���ĸ���
return  if success  return 0 else return -1
*/
int CMJOutCard::OnDelOutCard(unsigned char cardvalue, unsigned int count)
{
	if ( IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	if (OnGetCardCount(cardvalue) < count)
	{
		return -1;
	}

	if (m_length > MAX_OUT_CARD)
	{
		return -1;
	}
	unsigned int del_count = 0;
	unsigned char temp_card[MAX_OUT_CARD] = { 0x00 };
	memcpy(temp_card, m_out_card, m_length);

	for (unsigned int i = 0; i < count; i++)
	{
		int pos = OnGetCardPos(cardvalue);
		if (pos != -1)
		{
			m_out_card[pos] = 0x00; 
			del_count++;
		}
	}
	if (del_count != count)
	{//ɾ�������ָ����� 
		memcpy(m_out_card, temp_card, m_length);
		return -1;
	}
	m_length -= del_count;
	CardSort();
	return 0;
}

/*���ó�������
@param: cardvalue  ���õ�ֵ
@param: pos  �޸ĵ�λ��
return  if success  return 0 else return -1
*/ 
int CMJOutCard::OnSetOutCard(unsigned char cardvalue, unsigned int pos)
{
	if (pos >= m_length || IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	m_out_card[pos] = cardvalue;
	return 0;
}

//��ȡָ��λ��������
unsigned char CMJOutCard::OnGetOutCard(unsigned int pos)
{
	if (pos >= m_length)
	{
		return 0x00;
	}
	return m_out_card[pos];
} 

/*��ȡ���г���
@param: cardvalue[] �洢���ص�����
return ���Ƴ���
*/
unsigned int CMJOutCard::GetAllOutCard(unsigned char cardvalue[])
{
	memcpy(cardvalue, m_out_card, m_length);
	return m_length;
}

//��ȡ���Ƴ���
unsigned int CMJOutCard::GetOutCardLength()
{
	return m_length;
}

//����
void CMJOutCard::CardSort()
{
	unsigned int card_index[CARD_INDEX_NUM] = { 0 };
	ChangeCardToIndex(m_out_card, MAX_OUT_CARD, card_index);
	ClearAllData();
	for (unsigned int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (IsCardValid(i) && m_length + card_index[i] < MAX_OUT_CARD)
		{
			for (unsigned int s = 0; s < card_index[i]; s++)
			{
				m_out_card[m_length] = i;
				m_length = m_length + 1;
			}
		}
	}
}

//�����������
void CMJOutCard::ClearAllData()
{
	m_length = 0;
	for (unsigned int i = 0; i < MAX_OUT_CARD; i++)
	{
		m_out_card[i] = 0x00;
	}
}

//�Ƴ����һ������
unsigned char CMJOutCard::PopEndData()
{
	if (m_length == 0)
	{
		return 0x00;
	}
	unsigned char last_card = m_out_card[m_length - 1];
	m_out_card[m_length - 1] = 0x00;
	m_length--;

	return last_card;

}


CMJWallCard::CMJWallCard()
{
	m_wall_len = 0;
	memset(m_wall_card, 0, sizeof(m_wall_card));
}
CMJWallCard::~CMJWallCard()
{
	m_wall_len = 0;
	memset(m_wall_card, 0, sizeof(m_wall_card));
}
//�ж������Ƿ���ָ����
bool CMJWallCard::IsHaveCard(unsigned char cardvalue)
{ 
	if (IsCardValid(cardvalue) == false)
	{
		return false;
	}
	for (unsigned int i = 0; i < m_wall_len; i++)
	{
		if (m_wall_card[i] == cardvalue)
		{
			return true;
		}
	}
	return false;
}

//ϴ��
void CMJWallCard::RandWallCard(int randcount)
{
	if (m_wall_len == 0)
	{
		return ;
	}
	srand((unsigned int)(time(NULL) + randcount));
	//// ��ȡ��ʼ��ֵ
	unsigned char  temp_value[MAX_CARD_NUM] = { 0x00 };
	memcpy(temp_value, m_wall_card, m_wall_len);

	unsigned int value_pos = 0;
	unsigned int get_count = 0;

	do{
		//�ɵ����
		//value_pos = rand() % (m_wall_len - get_count);
		//�����µ����
		value_pos = (value_pos + GetRandValue(0, m_wall_len - get_count - 1)) % (m_wall_len - get_count);

		m_wall_card[get_count++] = temp_value[value_pos];
		temp_value[value_pos] = temp_value[m_wall_len - get_count];
	} while (get_count < m_wall_len);
	  
}
//��ȡָ���Ƹ���
int CMJWallCard::OnGetCardCount(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return 0;
	}
	int count = 0;
	for (unsigned int i = 0; i < m_wall_len; i++)
	{
		if (m_wall_card[i] == cardvalue)
		{
			count++;
		}
	}
	return count;
}
//��ȡָ���Ƶ�λ��
int CMJWallCard::OnGetCardPos(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return -1;
	} 
	for (unsigned int i = 0; i < m_wall_len; i++)
	{
		if (m_wall_card[i] == cardvalue)
		{ 
			return i;
		}
	}
	return -1;

}

//��ȡĳ��λ�õ���
unsigned char CMJWallCard::OnGetCardValue(unsigned int pos)
{
	if (pos>= m_wall_len)
	{
		return 0x00;
	}
	return m_wall_card[pos];
}
//�����ǽ����
bool CMJWallCard::OnAddWallCard(unsigned char cardvalue, unsigned int cardcount)
{
	if (IsCardValid(cardvalue) == false)
	{
		return false;
	}
	if (m_wall_len + cardcount > MAX_CARD_NUM)
	{
		return false;
	}
	for (unsigned i = 0; i < cardcount; i++)
	{
		m_wall_card[m_wall_len] = cardvalue;
		m_wall_len = m_wall_len + 1;
	}
	return true;

}

bool CMJWallCard::OnAddWallCards(unsigned char cardvalue[], unsigned int cardcount)
{
	if (m_wall_len + cardcount > MAX_CARD_NUM)
	{
		return false;
	}

	for (unsigned int i = 0; i < cardcount; i++)
	{
		if (IsCardValid(cardvalue[i]) == false)
		{
			return false;
		}

		m_wall_card[m_wall_len] = cardvalue[i];
		m_wall_len = m_wall_len + 1;
	}
	return true;
}

bool CMJWallCard::OnAddWallCards(CMJWallCard wall)
{
	unsigned int card_len = wall.GetWallCardLength();
	if (m_wall_len + card_len > MAX_CARD_NUM)
	{
		return false;
	}

	for (unsigned int i = 0; i < card_len; i++)
	{
		m_wall_card[m_wall_len] = wall.OnGetCardValue(i);
		m_wall_len = m_wall_len + 1;
	} 
	return true;
}
//ɾ��ָ��λ�õ���
unsigned char CMJWallCard::OnDelWallCard(unsigned int pos )
{
	if (pos >= m_wall_len)
	{
		return 0x00;
	}
	unsigned char delcard = m_wall_card[pos];
	m_wall_card[pos] = 0x00;
	//������ֵ
	for (unsigned int i = pos; i < m_wall_len-1; i++)
	{
		Swap(i, i + 1);
	} 
	m_wall_len--;

	return delcard;
}
//�Ƴ����һ������
unsigned char CMJWallCard::PopEndData()
{
	if (m_wall_len == 0 || m_wall_len > MAX_CARD_NUM)
	{
		return 0x00;
	}
	unsigned char last_card = m_wall_card[m_wall_len - 1];
	m_wall_card[m_wall_len - 1] = 0x00;
	m_wall_len--;

	return last_card;
}

//�Ƴ���һ������
unsigned char CMJWallCard::PopBeginData()
{
	if (m_wall_len == 0 || m_wall_len > MAX_CARD_NUM)
	{
		return 0x00;
	}
	return this->OnDelWallCard();
}

//��ȡ��ǽ����
unsigned int CMJWallCard::GetWallCardLength()
{
	return m_wall_len;
}

//��ȡ������ǽֵ
unsigned int CMJWallCard::GetAllWallCard(unsigned char wall_card[])
{
	memcpy(wall_card, m_wall_card, m_wall_len);
	return m_wall_len;
}
//������ֵ
bool CMJWallCard::Swap(unsigned int first_pos, unsigned int second_pos)
{

	if (first_pos >= m_wall_len || second_pos >= m_wall_len)
	{
		return false;
	}
	unsigned char temp_card = m_wall_card[first_pos];

	m_wall_card[first_pos] = m_wall_card[second_pos];

	m_wall_card[second_pos] = temp_card;

	return true;
}

//��ȡpos1 ��pos2��ֵ
int CMJWallCard::GetCards(unsigned int first_pos, unsigned int count, unsigned char card[])
{
	if (count > m_wall_len)
	{
		return 0;
	}

	for (unsigned i = 0; i < count; i++)
	{
		card[i] = this->OnGetCardValue(first_pos+i);
	}
	return count;
}

//���
void CMJWallCard::ClearData()
{
	m_wall_len = 0;
	memset(m_wall_card, 0, sizeof(m_wall_card));
}
 
 
//����������
 
CMJFlowerCard::CMJFlowerCard()
{ 
	m_length = 0;
	memset(m_flower_card, 0, sizeof(m_flower_card));
}
CMJFlowerCard::~CMJFlowerCard()
{
	m_length = 0;
	memset(m_flower_card, 0, sizeof(m_flower_card));
}
//�жϻ����Ƿ���ָ����
bool CMJFlowerCard::IsHaveCard(unsigned char cardvalue, unsigned int count)
{
	if (IsCardValid(cardvalue) == false || count > 4)
	{
		return false;
	}
	int cardcount = 0;
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_flower_card[i] == cardvalue)
		{
			cardcount++;
			if (cardcount == count)
			{
				return true;
			}
		}
	}
	return false;
}
//��ȡָ���Ƹ���
unsigned int CMJFlowerCard::OnGetCardCount(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return 0;
	}
	int cardcount = 0;
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_flower_card[i] == cardvalue)
		{
			cardcount++;
		}
	}
	return cardcount;
}
//��ȡָ���Ƶ�λ��
int CMJFlowerCard::OnGetCardPos(unsigned char cardvalue)
{
	if (IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	for (unsigned int i = 0; i < m_length; i++)
	{
		if (m_flower_card[i] == cardvalue)
		{
			return i;
		}
	}
	return -1;
}
//��ӻ�������
int CMJFlowerCard::OnAddFlowerCard(unsigned char cardvalue, unsigned int count)
{ 
	//�ж��Ƿ񳬳���󳤶�,��ֵ�Ƿ�Ϸ�
	if (m_length + count > MAX_FLOWER_NUM || IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	for (unsigned int i = 0; i < count; i++)
	{
		m_flower_card[m_length] = cardvalue;
		m_length = m_length + 1;
	}

	return 0;
}
//���һ�黨��
bool CMJFlowerCard::OnAddFlowerCards(CMJFlowerCard cards)
{
	unsigned int card_len = cards.GetFlowerCardLength();
	if (m_length + card_len > MAX_FLOWER_NUM)
	{
		return false;
	}

	for (unsigned int i = 0; i < card_len; i++)
	{
		m_flower_card[m_length] = cards.OnGetFlowerCard(i);
		m_length = m_length + 1;
	}
	return true;
}
//ɾ����������
int CMJFlowerCard::OnDelFlowerCard(unsigned char cardvalue, unsigned int count)
{
	if ( IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	if (OnGetCardCount(cardvalue) < count)
	{
		return -1;
	}
	if (m_length > MAX_FLOWER_NUM)
	{
		return -1;
	}
	unsigned int del_count = 0;
	unsigned char temp_card[MAX_FLOWER_NUM] = { 0x00 };


	memcpy(temp_card, m_flower_card, m_length);

	for (unsigned int i = 0; i < count; i++)
	{
		int pos = OnGetCardPos(cardvalue);
		if (pos != -1)
		{
			m_flower_card[pos] = 0x00;
			del_count++;
		}
	}
	if (del_count != count)
	{//ɾ�������ָ�����
		memset(m_flower_card, 0, sizeof(m_flower_card));
		memcpy(m_flower_card, temp_card, m_length);
		return -1;
	}
	m_length -= del_count;
	CardSort();

	return 0;
}
//���û�������
int CMJFlowerCard::OnSetFlowerCard(unsigned char cardvalue, unsigned int pos)
{
	if (pos >= m_length || IsCardValid(cardvalue) == false)
	{
		return -1;
	}
	m_flower_card[pos] = cardvalue;
	return 0;
}
//��ȡָ��λ��������
unsigned char CMJFlowerCard::OnGetFlowerCard(unsigned int pos)
{
	if (pos >= m_length)
	{
		return 0x00;
	}
	return m_flower_card[pos];
}
//��ȡ��������
unsigned int CMJFlowerCard::GetAllFlowerCard(unsigned char cardvalue[])
{ 
	memcpy(cardvalue, m_flower_card, m_length);
	return m_length;
}
//��ȡ���Ƴ���
unsigned int CMJFlowerCard::GetFlowerCardLength()
{
	return m_length;
}
//����
void CMJFlowerCard::CardSort()
{
	unsigned int card_index[CARD_INDEX_NUM] = { 0 };
	ChangeCardToIndex(m_flower_card, MAX_FLOWER_NUM, card_index);
	ClearAllData();
	for (unsigned int i = 0; i < CARD_INDEX_NUM; i++)
	{
		if (IsCardValid(i) && m_length + card_index[i] <= MAX_FLOWER_NUM)
		{
			for (unsigned int s = 0; s < card_index[i]; s++)
			{
				m_flower_card[m_length] = i;
				m_length = m_length + 1;
			}
		}
	}
}
//�����������
void CMJFlowerCard::ClearAllData()
{ 
	m_length = 0;
	memset(m_flower_card, 0, sizeof(m_flower_card));
} 