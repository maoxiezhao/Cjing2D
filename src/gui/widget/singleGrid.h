#pragma once

#include"common\common.h"
#include"gui\widget\widget.h"
#include"gui\widget\multGrid.h"

#include<memory>
#include<map>

namespace gui
{
	/**
	*	\brief ��һԪ�ص���������
	*/
	class SingleGrid : public Widget
	{
		class Children;
	public:
		SingleGrid();
		SingleGrid(int row, int col);
		~SingleGrid();

		static const unsigned int VERTICAL_SHIFT = 0;			// ʹ�ó�����ֵ or ö��ֵ
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
		*	\brief ������grid�е��ӽڵ�
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
		void SetChildren(Widget* widget, int row, int col, const unsigned int flat);		// �Ƿ�ʹ������ָ��
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
		int mRows;		/** ��ǰ���� */
		int mCols;		/** ��ǰ���� */

		mutable std::vector<int> mRowsHeight;		/** ÿ������ĸ߶� */
		mutable std::vector<int> mColsWidth;		/** ÿ������Ŀ�� */
		mutable std::vector<float> mRowsFactory;	/** ÿ�е�����ֲ�Ȩ�� */
		mutable std::vector<float> mColsFactory;	/** ÿ�е�����ֲ�Ȩ�� */

		std::vector<Children> mChilds;			    /** ��������ʽ���ִ洢��ǰ����widget*/

		std::map<string, Children> mNamedChilds;	/** ��map��ʽ�洢children,����ID��Ϊ��ֵ,���ڲ��� */
													/** Ŀǰ���Ǵ��ڱ�Ҫ�ԣ���ΪGUI������һ��Ƚ�С */

	};


}