
#ifndef GAMES_DDZ_OUT_CARD_H_
#define GAMES_DDZ_OUT_CARD_H_

#include "cocos2d.h"

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

namespace ddz
{

	class OutCard : public cocos2d::Layer
	{
	public:
		OutCard();
		virtual ~OutCard();
		CREATE_FUNC(OutCard);

		virtual bool init();

		// �����˿�����
		void SetCardData(uint8* card_data, uint8 card_count, bool anim = true);
		// �����˿˻�׼λ��
		void SetPosition(const cocos2d::Vec2& pos);
		void SetDispatchPos(const cocos2d::Vec2& pos) { dispatch_pos_ = pos; }
		// ������λ
		void SetChairID(uint16 chair_id) { chair_id_ = chair_id; }
		// ��ȡ����λ��
		cocos2d::Vec2 GetCenterPos();

	private:
		uint8 card_data_[kMaxCardCount];	// �˿�����
		uint8 card_count_;					// �˿�����
		cocos2d::Vec2 position_;		// ��׼λ��
		cocos2d::Vec2 dispatch_pos_;	// ���ƶ�����ʼλ��
		uint16 chair_id_;
	};

}

#endif	// GAMES_DDZ_OUT_CARD_H_