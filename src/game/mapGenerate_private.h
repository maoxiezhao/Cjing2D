#pragma once

#include"utils\rectangle.h"
#include"utils\geometry.h"

#include<vector>
#include<limits>

namespace MapGenerateImplemention
{
	int MaxInteger = std::numeric_limits<int>::max();

	/** 树节点 */
	struct TreeNode
	{
		int firstNode;
		int lastNode;

		TreeNode() :firstNode(0), lastNode(0) {}
		TreeNode(int f, int l) : firstNode(f), lastNode(l) {}
	};

	using LinkGraph = vector<vector<int> >;
	/** 基于Prim的最小生成树算法 */
	class GenerateTreePrim
	{
	public:
		GenerateTreePrim() = default;
		GenerateTreePrim(LinkGraph& graph) :mGraph(graph) { Generate(); }
		std::vector<TreeNode> GetTree() { return mMinTree; }
	private:
		void Generate()
		{
			int nodeCount = mGraph.size();
			vector<bool> bFlag(mGraph.size(), false);
			int firstNode = 0;
			int lastNode = 0;
			bFlag[0] = true;
			for (int k = 0; k < nodeCount - 1; k++) {

				int maxWeight = MaxInteger;
				int nodeIndex = 0;
				while (nodeIndex < nodeCount)
				{
					if (!bFlag[nodeIndex])
					{
						++nodeIndex;
						continue;
					}

					auto& graphNode = mGraph[nodeIndex];
					for (int j = 0; j < nodeCount; ++j)
					{
						if (!bFlag[j] && maxWeight > graphNode[j])
						{
							maxWeight = graphNode[j];
							firstNode = nodeIndex;
							lastNode = j;
						}
					}
					nodeIndex++;
				}
				bFlag[lastNode] = true;
				mMinTree.push_back({ firstNode, lastNode });
			}
		}

		LinkGraph& mGraph; 
		std::vector<TreeNode> mMinTree;    
	};

	/** 生成最下生成树返回链接节点 */
	std::vector<TreeNode> CaculateMinimumTree(std::vector<Rect>& roomRects)
	{
		int size = roomRects.size();
		LinkGraph graph(size, {});
		for (int i = 0; i < size; i++)
		{
			auto& graphNode = graph[i];
			graphNode.resize(size, MaxInteger);
			auto srcRect = roomRects[i];
			for (int j = 0; j < size; j++)
			{
				if (i == j)
					continue;
				
				auto dstRect = roomRects[j];			
				graphNode[j] = (int)Geometry::GetDistance(srcRect.GetCenterPos(), dstRect.GetCenterPos());
			}
		}

		GenerateTreePrim prim(graph);
		return prim.GetTree();
	}

}