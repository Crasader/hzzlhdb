
#ifndef GAMES_DDZ_CARD_H_
#define GAMES_DDZ_CARD_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../../base/basictypes.h"
#include "CMD_Ddz.h"

namespace ddz
{

	class OutCard;

	class Card : public cocos2d::Layer
	{
	public:
		Card();
		virtual ~Card();
		CREATE_FUNC(Card);

		virtual bool init();

		// �����˿�����
		void SetCardData(uint8* card_data, uint8 card_count);
		uint8 GetCardCount() { return card_count_; }
		// �����˿˻�׼λ��
		void SetCenterPosition(const cocos2d::Vec2& pos);
		// ������Ӧ
		void SetTouchEnable(bool enable) { touch_enable_ = enable; }
		// ���ó�������
		void SetOutCardData(OutCard* outcard);
		void SetOutCardPos(const cocos2d::Vec2& pos) { outcard_position_ = pos; }
		// �����˿�����
		int GetShootCardCount();
		// ��ȡ�����˿�
		uint8 GetShootCardData(uint8 card_data_buffer[], uint8 card_buffer_count);
		// ���õ����˿�
		void SetShootCardData(uint8 card_data[], uint8 card_count);

	private:
		virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);
		virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event);
		virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event);
		virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

		void onMouseDown(cocos2d::Event* event);
		void onMouseUp(cocos2d::Event* event);

	public:
		typedef std::function<void()> MouseClickCallback;
		MouseClickCallback on_mouse_right_;
		MouseClickCallback On_mouse_left_;

	private:
		uint8 card_data_[kMaxCardCount];	// �˿�����
		uint8 card_count_;					// �˿�����
		cocos2d::Vec2 center_position_;		// ��׼λ��
		int last_select_index_;				// �ϴ�ѡ������
		int moving_index_;					// �ƶ�����
		bool double_click_;					// ˫����־
		bool touch_enable_;					// �Ƿ���Բ���
		cocos2d::Vec2 outcard_position_;		// ����λ��
	};

}

#endif	// GAMES_DDZ_CARD_H_
