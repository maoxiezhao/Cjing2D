#ifndef _MAP_H_
#define _MAP_H_

#include"common\common.h"
#include"lua\luaObject.h"

class Game;
class Camera;
class LuaContext;
class Tileset;
class Entities;
class InputEvent;
class Tileset;
/**
*	\brief map��
*
*	map�����������Ϣ��
*	tileset
*	entities
*	music
*	ground
*	mapcamera
*/
class Map : public LuaObject
{
public:
	explicit Map();
	explicit Map(const std::string& id);
	~Map();

	// system
	virtual void Load(Game* game);
	virtual void UnLoad();
	virtual void Start();
	virtual void Leave();
	virtual void Update();
	virtual bool NotifyInput(const InputEvent& event);
	virtual void Draw();
	virtual void DrawBackground();		// ���Ʊ���ͼ���󾰣�
	virtual void DrawForeground();		// ���Ʊ���ͼ��ǰ����

	// status
	bool IsLoaded()const;
	bool IsStarted()const;
	bool IsSuspended()const;
	void CheckSuspended();
	void SetSuspended(bool suspended);
	bool IsValidLayer(int layer)const;

	Game& GetGame();
	LuaContext& GetLuaContext();
	const string GetLuaObjectName()const;

	const std::shared_ptr<Camera>& GetCamera();

	// map property
	const string& GetMapID()const;
	const Tileset& GetTileset()const;
	void SetTileset(const string& tilesetID);
	const string& getTilesetID()const;

	// test collison
	bool TestCollison();
	
private:
	string mMapID;
	int mWidth;
	int mHeight;
	int mMinLayer;
	int mMaxLayer;

	Game* mGame;

	string mTilesetId;
	std::shared_ptr<Tileset> mTileset;

	std::shared_ptr<Camera> mCamera;

	std::unique_ptr<Entities> mEntities;

	bool mIsLoaded;
	bool mIsStarted;
	bool mSuspended;
};


#endif