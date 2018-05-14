#pragma once

#include"common\common.h"
#include"gui\widget\widget.h"
#include"gui\widget\multGrid.h"

#include<memory>
#include<map>

namespace gui
{
	/**
	*	\brief 单一元素的网格容器
	*/
	class SingleGrid : public Widget
	{
		class Children;
	public:
		SingleGrid();
		SingleGrid(int row, int col);
		~SingleGrid();

		static const unsigned int VERTICAL_SHIFT = 0;			// 使用常量数值 or 枚举值
		static const unsigned int HORIZONTAL_SHIFT = 3;
		static const unsigned int BORDER_SHIFT = 6;

		/**** **** memeber data **** ****/

		void AddRows(int rowCount = 1);
		void AddCols(int colCount = 1);

		void SetCols(int cols);
		int GetCols()const
		{
			return mCols;
		}

		void SetRows(int rows);
		int GetRows()const
		{
			return mRows;
		}
		void SetRowCols(int row, int col);

		void SetRowsFactory(const std::vector<float> factoies);
		void SetColsFactory(const std::vector<float> factoies);

		/**** **** *** Layout *** **** ****/

		virtual void InitLayout();

		virtual void Place(const Point2& pos, const Size& size);
		virtual void RefreshPlace();
		virtual void SetPosition(const Point2& position);

		void ReduceWidth(const int maxnumWidth);
		virtual void RequestReduceWidth(const int maxnumWidth);
		virtual void DemandReduceWidth(const int maxnumWidth);

		void ReduceHeight(const int maxnumHeight);
		virtual void RequestReduceHeight(const int maxnumHeight);
		virtual void DemandReduceHeight(const int maxnumHeight);

		virtual void Layout(const Point2& origin);
		virtual void LayoutChildren(const Point2& origin);

		virtual Widget* Find(string& id, const bool activited);
		virtual const Widget* Find(string& id, const bool activied)const;
		virtual bool HasWidget(const Widget& widget)const;
		virtual bool IsAt(const Point2& pos)const;
		virtual Widget* FindAt(const Point2& pos);
		virtual Size CalculateBestSize()const;
		virtual Size ReCalculateBestSize();

	private:
		int RequestReduceColWidth(int col, const int maxnumWidth);
		void RequestReduceCellWidth(Children& child, const int maxnumWidth);
		int RequestReduceRowHeight(int row, const int maxnumHeight);
		void RequestReduceCellHeight(Children& child, const int maxnumHeight);

		virtual void ImplDrawChildren(const Point2& offset);

		void RequesetPlacement(Dispatcher&, const gui::ui_event, bool&handle, bool& halt);

		/**** **** **** **** Child **** **** **** ****/

		/**
		*	\brief 保存在grid中的子节点
		*/
		class Children
		{
		public:
			Children();

			const string GetID()const;

			void InitLayout();
			void Place(const Point2& pos, const Size& size);
			Size GetBestSize()const;
			Size GetBorderSpace()const;

			void SetFlag(const unsigned int flag)
			{
				mFlag = flag;
			}
			unsigned int GetFlag()const
			{
				return mFlag;
			}
			void SetWidget(const WidgetPtr& widget)
			{
				mWidget = widget;
			}
			int GetBorderSize()const
			{
				return mBorderSize;
			}
			void SetBorderSize(int borderSize)
			{
				mBorderSize = borderSize;
			}
			WidgetPtr GetWidget()
			{
				return mWidget;
			}
			const WidgetPtr GetWidget()const
			{
				return mWidget;
			}
		private:
			unsigned int mFlag;
			int mBorderSize;
			WidgetPtr mWidget;
		};

	public:
		void SetChildren(Widget* widget, int row, int col, const unsigned int flat);		// 是否使用智能指针
		void SetChildren(const WidgetPtr& widget, int row, int col, const unsigned int flag, int borderSize);
		void RemoveChildren(int row, int col);
		void RemoveChildren(const string& id);
		void SetChildrenAlignment(WidgetPtr widget, const unsigned int setflag, const unsigned int modeMask);

		Children* GetChildren(const WidgetPtr& widget);
		Children& GetChildren(int row, int col)
		{
			return mChilds[col * mRows + row];
		}
		const Children& GetChildren(int row, int col)const
		{
			return mChilds[col * mRows + row];
		}
		WidgetPtr GetWidget(int row, int col)
		{
			return GetChildren(row, col).GetWidget();
		}
		const WidgetPtr GetWidget(int row, int col)const
		{
			return GetChildren(row, col).GetWidget();
		}

		/****** Iterator for child item. ******/
		class iterator
		{
		public:
			iterator(std::vector<Children>::iterator itor) :mItor(itor) {};

			iterator operator ++()
			{
				return iterator(++mItor);
			}

			iterator operator --()
			{
				return iterator(--mItor);
			}

			WidgetPtr operator *()
			{
				return mItor->GetWidget();
			}

			WidgetPtr operator ->()
			{
				return mItor->GetWidget();
			}

			bool operator==(const iterator& it)const
			{
				return it.mItor == mItor;
			}

			bool operator!=(const iterator& it)const
			{
				return it.mItor != mItor;
			}
		private:
			std::vector<Children>::iterator mItor;
		};

		iterator begin()
		{
			return iterator(mChilds.begin());
		}

		iterator end()
		{
			return iterator(mChilds.end());
		}
	private:
		int mRows;		/** 当前行数 */
		int mCols;		/** 当前列数 */

		mutable std::vector<int> mRowsHeight;		/** 每行网格的高度 */
		mutable std::vector<int> mColsWidth;		/** 每列网格的宽度 */
		mutable std::vector<float> mRowsFactory;	/** 每行的网格分布权重 */
		mutable std::vector<float> mColsFactory;	/** 每列的网格分布权重 */

		std::vector<Children> mChilds;			    /** 以数组形式划分存储当前所有widget*/

		std::map<string, Children> mNamedChilds;	/** 以map形式存储children,并以ID作为键值,用于查找 */
													/** 目前考虑存在必要性，因为GUI的网格一般比较小 */

	};


}