#pragma once

#include<memory>

/**
*	\brief Ĭ�ϵ��ڴ������
*/
template<typename T>
class DefaultMemoryAllocator
{
public:
	static inline void* Allocator(size_t size)
	{
		return ::operator new(size, ::std::nothrow);
	}

	static inline void DeAllocator(void* pointer)
	{
		::operator delete(pointer);
	}
};

/**
*	\brief �ڴ�����
*/
template<typename T, class MemoryAllocator = DefaultMemoryAllocator<T> >
class ObjectPool
{
private:
	/**
	*	\brief �ڴ�ڵ㣬objectPool����memoryNode������
	*	������һ���ڵ�����ʹ����Ϻ��ٷ���һ��2����С
	*	��node
	*/
	struct MemoryNode
	{
		void* _memory;
		size_t _nodeCapacity;
		MemoryNode* _nextNode;

		MemoryNode(size_t capacity) :
			_memory(nullptr),
			_nodeCapacity(0),
			_nextNode(nullptr)
		{
			_memory = MemoryAllocator::Allocator(mItemSize * capacity);
			if (_memory == nullptr)
			{
				throw std::bad_alloc();
			}
			_nodeCapacity = capacity;
			_nextNode = nullptr;
		}

		~MemoryNode()
		{
			MemoryAllocator::DeAllocator(_memory);
		}
	};

	/**
	*	\brief �����µ��ڴ�ڵ�
	*/
	void AllocateNewNode()
	{
		size_t capacity = mUsedCapacity;
		if (capacity >= mMaxCapacity)
		{
			capacity = mMaxCapacity;
		}
		else
		{
			capacity *= 2;
			if (capacity >= mMaxCapacity)
			{
				capacity = mMaxCapacity;
			}
		}
		MemoryNode* memoryNode = new MemoryNode(capacity);
		mUsedCapacity = 0;
		mCapacity = capacity;
		mLastNode->_nextNode = memoryNode;
		mLastNode = memoryNode;
		mMemory = memoryNode->_memory;
		mAmmountCapacity += capacity;
	}

	/**
	*	\brief �ͷŵ�δʹ�õ��ڴ�ڵ�
	*/
	void DeallocateOldNode()
	{

	}

public:
	ObjectPool(size_t initialCapacity = 32, size_t maxCapacity = 10000) :
		mCapacity(initialCapacity),
		mUsedCapacity(0),
		mMemoryNode(initialCapacity),
		mMaxCapacity(maxCapacity),
		mDeleteTopAddress(nullptr),
		mAmmountCapacity(initialCapacity)
	{
		mLastNode = &mMemoryNode;
		mMemory = mMemoryNode._memory;
	}

	~ObjectPool()
	{
		MemoryNode* memoryNode = mMemoryNode._nextNode;
		while (memoryNode != nullptr)
		{
			auto curNode = memoryNode;
			memoryNode = memoryNode->_nextNode;
			delete curNode;
		}
		mMemory = nullptr;
		mLastNode = nullptr;
	}

	/**
	*	\brief ���´���ʱ��δʹ�ý׶�
	*/
	void Update()
	{

	}

	T* New()
	{
		if (mDeleteTopAddress != nullptr)
		{
			T* result = mDeleteTopAddress;
			mDeleteTopAddress = *((T**)mDeleteTopAddress);
			new(result)T();
			return result;
		}

		if (mUsedCapacity >= mCapacity)
		{
			AllocateNewNode();
		}
		char* addres = (char*)mMemory;
		addres += mUsedCapacity * mItemSize;
		T* result = new(addres)T();
		mUsedCapacity++;
		return result;

	}

	/**
	*	\brief ���ͷŶ���ʱ�������øö�������������ͷſռ䣩
	*	ͬʱ�ڸô��洢��һ���ͷŵĶ���ĵ�ַ��T*�ĵ�ַ��
	*/
	void Delete(T* content)
	{
		content->~T();
		*((T**)content) = mDeleteTopAddress;
		mDeleteTopAddress = content;
	}


private:
	static const size_t	 mItemSize;
	const uint32_t mCheckDelat = 2000;

	size_t mCapacity;			/** ��ǰ���� */
	size_t mUsedCapacity;		/** ��ǰ��ʹ������ */
	size_t mMaxCapacity;
	size_t mAmmountCapacity;

	MemoryNode mMemoryNode;
	MemoryNode* mLastNode;
	void* mMemory;
	T* mDeleteTopAddress;

};

template<typename T, class MemoryAllocator = DefaultMemoryAllocator>
const size_t ObjectPool<T, MemoryAllocator>::mItemSize = ((sizeof(T) + sizeof(void*) - 1) / sizeof(void*));