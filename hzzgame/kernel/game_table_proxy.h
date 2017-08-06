
#ifndef GAME_TABLE_PROXY_H_
#define GAME_TABLE_PROXY_H_

#include <vector>

#include "../base/basictypes.h"
#include "../../../../global/Define.h"
#include "game_user_manager.h"

class GameTable
{
public:
	GameTable();
	~GameTable();

	void InitTable(uint16 table_id, uint16 chair_count);
	uint16 GetTableID() const { return table_id_; }
	IGameUserItem* GetTableUserItem(uint16 chair_id);
	bool SetTableUserItem(uint16 chair_id, IGameUserItem* user_item);
	bool IsPlaying() const { return playing_; }
	bool IsLocking() const { return locking_; }
	void SetTableStatus(bool playing, bool locking);

private:
	bool locking_;
	bool playing_;

	uint16 table_id_;
	uint16 chair_count_;

	IGameUserItem* game_user_item_[MAX_CHAIR];
};

class GameTableProxy
{
public:
	class Delegate
	{
	public:
		Delegate() { }
		virtual ~Delegate() { }

		// 设置用户
		virtual bool SetTableUserItem(uint16 table_id, uint16 chair_id, IGameUserItem* user_item) = 0;
		// 桌子状态 
		virtual void SetTableStatus(uint16 table_id, bool playing, bool locker) = 0;
	};

	GameTableProxy();
	~GameTableProxy();

	void SetDelegate(Delegate* delegate) { delegate_ = delegate;  }
	bool InitGameTable(uint16 table_count, uint16 chair_count, uint32 server_rule, uint16 server_type);
	uint16 GetTableCount() const { return table_count_; }
	uint16 GetChairCount() const { return chair_count_; }
	IGameUserItem* GetTableUserItem(uint16 table_id, uint16 chair_id);
	bool SetTableUserItem(uint16 table_id, uint16 chair_id, IGameUserItem* user_item);
	bool IsPlaying(uint16 table_id);
	bool IsLocking(uint16 table_id);
	void SetTableStatus(uint16 table_id, bool playing, bool locking);

private:
	uint16 table_count_;
	uint16 chair_count_;
	uint32 server_rule_;
	uint16 server_type_;

	typedef std::vector<GameTable*> GameTableVector;
	GameTableVector game_table_vector_;

	Delegate* delegate_;
};

#endif // GAME_TABLE_PROXY_H_