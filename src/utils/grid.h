#pragma once

#include"common\common.h"
#include"utils\size.h"
#include"utils\rectangle.h"

#include<set>

namespace util
{

/**
*	\brief 网格加速结构
*
*	这里的网格结构是一种简单的网格结构，用来获取指定
*	rect范围内的网格中所有T对象
*/
template<typename T>
class Grid
{
public:
	Grid(const std::vector<T>& objs);
	Grid(const Size& size, const Size& cellSize);
	~Grid() { Clear(); }

	bool Add(const T& element, const Rect& boundingBox);
	bool Add(const T& element, int col, int row);
	std::vector<T> GetElements(int col, int row)const;
	std::vector<T> GetElements(int cellIndex)const;
	std::vector<T> GetElements(const Rect& boundingBox)const;
	void GetElements(const Rect& boundingBox, std::vector<T>& elements)const;
	
	void Clear(){
		for (auto& cell : mCells){
			cell.Clear();
		}
		mCells.clear();
	}
	Size GetGridSize()const{
		return mSize;
	}
	Size GetCellSize()const{
		return mCellSize;
	}
	void SetCellCapacity(int capacity)	{
		mCellCapacity = capacity;
		for (auto& cell : mCells)
			cell.SetCellCapacity(capacity);
	}
	int GetCellCapacity()const{
		return mCellCapacity;
	}
	int GetCols()const {
		return mColsCount;
	}
	int GetRows()const {
		return mRowCount;
	}
private:
	/**
	*	\brief Cell 网格子结构
	*	用于管理每个单元格的对象数据
	*/
	class Cell
	{
	public:
		Cell()
		{
		}
		Cell(const T& t)
		{
			mObjects.push_back(t);
		}
		bool Add(const T& t)
		{
			if (mCellCapacity >= 0 && mObjects.size() >= mCellCapacity)
				return false;

			mObjects.push_back(t);
			return true;
		}
		void Clear()
		{
			mObjects.clear();
		}
		const std::vector<T>& GetElements()const
		{
			return mObjects;
		}
		void SetCellCapacity(int capacity)
		{
			mCellCapacity = capacity;
		}
	private:
		int mCellCapacity;
		std::vector<T> mObjects;
	};

private:
	Size mSize;
	Size mCellSize;
	int mColsCount;
	int mRowCount;
	int mCellCapacity;
	std::vector<Cell > mCells;
};

template<typename T>
Grid<T>::Grid(const std::vector<T>& objs):
	mCellCapacity(-1)
{
	for (auto& obj : objs)
	{

	}
}

template<typename T>
inline Grid<T>::Grid(const Size & size, const Size & cellSize) :
	mSize(size),
	mCellSize(cellSize),
	mCellCapacity(-1)
{
	mColsCount = mSize.width / mCellSize.width  + (mSize.width % mCellSize.width != 0 ? 1 : 0 );
	mRowCount = mSize.height / mCellSize.height + (mSize.height % mCellSize.height != 0 ? 1 : 0);;

	mCells.resize(mColsCount * mRowCount, Cell());
}

template<typename T>
inline bool Grid<T>::Add(const T & element, const Rect & boundingBox)
{
	int colStart = boundingBox.x / mCellSize.width;
	int colEnd = (boundingBox.x + boundingBox.width - 1) / mCellSize.width;
	int rowStart = boundingBox.y / mCellSize.height;
	int rowEnd = (boundingBox.y + boundingBox.height - 1) / mCellSize.height;

	for (int row = rowStart; row <= rowEnd; row++)  
	{
		if (row < 0 || row >= mRowCount)
		{
			continue;
		}
		for (int col = colStart; col <= colEnd; col++) 
		{
			if (col < 0 || col >= mColsCount)
			{
				continue;
			}

			int index = row * mColsCount + col;
			auto& cell = mCells[index];
			if (!cell.Add(element))
				return false;
		}
	}
	return true;
}

template<typename T>
inline bool Grid<T>::Add(const T & element, int col, int row)
{
	int index = row * mColsCount + col;
	Debug::CheckAssertion(index >= 0 && index < mColsCount * mRowCount,
		"Invalid index Add elements in grid. col:" + std::to_string(col) + ",row:" + std::to_string(row));
	auto& cell = mCells[index];
	if (!cell.Add(element))
		return false;
	return true;
}

template<typename T>
inline std::vector<T> Grid<T>::GetElements(int col, int row) const
{
	return GetElements(row * mColsCount + col);
}

template<typename T>
inline std::vector<T> Grid<T>::GetElements(int cellIndex) const
{
	/*Debug::CheckAssertion(cellIndex >= 0 && cellIndex < mColsCount * mRowCount,
		"Invalid index getting elements in grid.");*/
	if (cellIndex >= 0 && cellIndex < mColsCount * mRowCount)
		return mCells.at(cellIndex).GetElements();
	else
		return std::vector<T>();
}

template<typename T>
inline std::vector<T> Grid<T>::GetElements(const Rect & boundingBox) const
{
	int colStart = boundingBox.x / mCellSize.width;
	int colEnd = (boundingBox.x + boundingBox.width) / mCellSize.width;
	int rowStart = boundingBox.y / mCellSize.height;
	int rowEnd = (boundingBox.y + boundingBox.height) / mCellSize.height;

	std::vector<T> elements;
	std::set<T> addElements;
	for (int row = rowStart; row <= rowEnd; row++)
	{
		if (row < 0 || row >= mRowCount)
		{
			continue;
		}
		for (int col = colStart; col <= colEnd; col++)
		{
			if (col < 0 || col >= mColsCount)
			{
				continue;
			}
		
			int index = row * mColsCount + col;
			auto& cell = mCells[index];
			for (auto& element : cell.GetElements())
			{
				if (addElements.find(element) == addElements.end())
				{
					addElements.insert(element);
					elements.push_back(element);
				}
			}
		}
	}

	return elements;
}

template<typename T>
inline void Grid<T>::GetElements(const Rect & boundingBox, std::vector<T>& elements) const
{
	const std::vector<T> getElements = GetElements(boundingBox);
	for (auto& element : getElements)
	{
		elements.push_back(element);
	}
}

}