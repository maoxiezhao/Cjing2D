#pragma once

#include"common\common.h"
#include"game\sprite.h"
#include"lua\luaData.h"
#include"entity\tilepattern.h"

/**
*	\brief ���ڴ�lua��ʽ��dat�ж�ȡtileset����
*/
class TilesetData : public LuaData
{
public:
	virtual bool ImportFromLua(lua_State*l);

	const std::vector<TilePatternData>& GetTilePatterns();
	void PushTilePattern(const TilePatternData& data);

private:
	static int LuaTilesetData(lua_State* l);
	std::vector<TilePatternData> mTilePatterns;
};

/**
*	\brief tileset ����tileImage��tilePattern
*/
class Tileset 
{
public:
	Tileset(const string& id);
	
	void Load();
	void UnLoad();
	void AddTilePattern(int id, const TilePatternData& data);
	const TilePattern& GetTilePattern(int id)const;
	SpritePtr GetTileImage()const;

private:
	string mTilesetID;
	SpritePtr mTilesImage;

	std::map<int, std::unique_ptr<TilePattern> >mTilePatterns;

};