#ifndef _MAP_H_
#define _MAP_H_

#include"common\common.h"
#include"lua\luaObject.h"
#include"utils\size.h"
#include"game\drawable.h"

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
*	map���������һ�¶���
*	tileset		��ͼ��Ƭ�ϼ�
*	entities	��ͼʵ��ϼ�
*	music		��ͼ��������
*	ground		��ͼ���κϼ�
*	mapcamera	��ͼ���ʵ��
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

	void DrawOnMap(Drawable& drawabel);
	void DrawOnMap(Drawable& drawabel, const Point2& pos);

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
	Entities& GetEntities();
	const std::shared_ptr<Camera>& GetCamera();

	// map property
	const string& GetMapID()const;
	const Tileset& GetTileset()const;
	void SetTileset(const string& tilesetID);
	const string& getTilesetID()const;
	int GetMinLayer()const;
	int GetMaxLayer()const;
	Size GetSize()const;
	int GetWidth()const;
	int GetHeight()const;

	// test collison
	bool TestCollison();
	
private:
	/** status */
	string mMapID;
	int mWidth;
	int mHeight;
	int mMinLayer;
	int mMaxLayer;
	Game* mGame;

	bool mIsLoaded;
	bool mIsStarted;
	bool mSuspended;

	/** entity */
	string mTilesetId;
	std::shared_ptr<Tileset> mTileset;

	std::shared_ptr<Camera> mCamera;

	std::unique_ptr<Entities> mEntities;

};

#endif