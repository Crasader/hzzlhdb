
#include "game_table_proxy.h"

GameTable::GameTable()
	: locking_(false), playing_(false), table_id_(0), chair_count_(0)
{
	memset(game_user_item_, 0, sizeof(game_user_item_));
}

GameTable::~GameTable()
{

}

void GameTable::InitTable(uint16 table_id, uint16 chair_count)
{
	table_id_ = table_id;
	chair_count_ = chair_count;
}

IGameUserItem* GameTable::GetTableUserItem(uint16 chair_id)
{
	assert(chair_id < chair_count_);
	if (chair_id >= chair_count_)
		return nullptr;

	return game_user_item_[chair_id];
}

bool GameTable::SetTableUserItem(uint16 chair_id, IGameUserItem* user_item)
{
	assert(chair_id < chair_count_);
	if (chair_id >= chair_count_)
		return false;

	game_user_item_[chair_id] = user_item;

	return true;
}

void GameTable::SetTableStatus(bool playing, bool locking)
{
	playing_ = playing;
	locking_ = locking;
}

GameTableProxy::GameTableProxy()
	: table_count_(0), chair_count_(0), server_rule_(0), server_type_(0), delegate_(nullptr)
{

}

GameTableProxy::~GameTableProxy()
{

}

bool GameTableProxy::InitGameTable(uint16 table_count, uint16 chair_count, uint32 server_rule, uint16 server_type)
{
	table_count_ = table_count;
	chair_count_ = chair_count;
	server_rule_ = server_rule;
	server_type_ = server_type;
	game_table_vector_.resize(table_count_);

	for (uint16 i = 0; i < table_count_; i++)
	{
		game_table_vector_[i] = new GameTable();
		game_table_vector_[i]->InitTable(i, chair_count);
	}

	return true;
}

IGameUserItem* GameTableProxy::GetTableUserItem(uint16 table_id, uint16 chair_id)
{
	assert(table_id < table_count_);
	if (table_id >= table_count_)
		return nullptr;

	return game_table_vector_[table_id]->GetTableUserItem(chair_id);
}

bool GameTableProxy::SetTableUserItem(uint16 table_id, uint16 chair_id, IGameUserItem* user_item)
{
	assert(table_id < table_count_);
	if (table_id >= table_count_)
		return false;

	bool success = game_table_vector_[table_id]->SetTableUserItem(chair_id, user_item);
	if (success && delegate_)
		delegate_->SetTableUserItem(table_id, chair_id, user_item);

	return success;
}

bool GameTableProxy::IsPlaying(uint16 table_id)
{
	assert(table_id < table_count_);
	if (table_id >= table_count_)
		return false;

	return game_table_vector_[table_id]->IsPlaying();
}

bool GameTableProxy::IsLocking(uint16 table_id)
{
	assert(table_id < table_count_);
	if (table_id >= table_count_)
		return false;

	return game_table_vector_[table_id]->IsLocking();
}

void GameTableProxy::SetTableStatus(uint16 table_id, bool playing, bool locking)
{
	assert(table_id < table_count_);
	if (table_id >= table_count_)
		return;

	game_table_vector_[table_id]->SetTableStatus(playing, locking);
	if (delegate_)
		delegate_->SetTableStatus(table_id, playing, locking);
}
