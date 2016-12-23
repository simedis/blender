#ifndef __RAS_RENDER_NODE__
#define __RAS_RENDER_NODE__

#include <functional>
#include <algorithm>
#include <vector>

class RAS_BucketManager;
class RAS_MaterialBucket;
class RAS_DisplayArrayBucket;
class RAS_MeshSlot;
class RAS_IRasterizer;
class MT_Transform;

class RAS_MaterialNode;
class RAS_DisplayArrayNode;
class RAS_MeshSlotNode;

class RAS_NullNode
{
// public:
// 	typedef std::vector<int> SubNodeTypeList;
};

template <class ParentNodeType, class SubNodeType, class InfoType, class ... Args>
class RAS_BaseNode
{
public:
	typedef std::vector<SubNodeType> SubNodeTypeList;
	typedef typename SubNodeTypeList::iterator SubNodeTypeListIterator;
	typedef std::function<void(InfoType, SubNodeTypeList, Args ...)> Function;

public: //private:
	InfoType m_info;
	Function m_function;
	ParentNodeType m_parentNode;
	SubNodeTypeList m_subNodes;
	unsigned int m_orderStart;
	unsigned int m_orderEnd;

public:
	RAS_BaseNode(ParentNodeType parentNode, InfoType info, Function function)
		:m_info(info),
		m_function(function),
		m_parentNode(parentNode),
		m_orderStart(0),
		m_orderEnd(0)
	{
	}

	virtual ~RAS_BaseNode()
	{
	}

	inline void AddNode(const SubNodeType& subNode)
	{
// 		std::cout << __PRETTY_FUNCTION__ << std::endl;
		m_subNodes.push_back(subNode);
	}

	inline bool GetEmpty() const
	{
		return m_subNodes.empty();
	}

	inline const void operator()(Args ... args) const
	{
// 		std::cout << __PRETTY_FUNCTION__ << std::endl;
		m_function(m_info, m_subNodes, args...);
	}

	inline void Sort()
	{
		std::sort(m_subNodes.begin(), m_subNodes.end());
		for (SubNodeTypeListIterator it = m_subNodes.begin(), end = m_subNodes.end(); it != end; ++it) {
			(*it).Sort();
		}
	}

	inline void Split()
	{
		for (SubNodeTypeListIterator it = m_subNodes.begin(), end = m_subNodes.end(); it != end; ++it) {
			(*it).Split();
		}

		SubNodeTypeListIterator it = m_subNodes.begin();
		SubNodeTypeListIterator end = m_subNodes.end();

		
		while (it != end) {
			SubNodeTypeListIterator jt = it;
		}
	}

	inline bool operator<(const RAS_BaseNode& other)
	{
		return m_orderEnd < other.m_orderStart;
	}
};

class RAS_ManagerNode : public RAS_BaseNode<RAS_NullNode, RAS_MaterialNode, RAS_BucketManager *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_MaterialNode : public RAS_BaseNode<RAS_ManagerNode, RAS_DisplayArrayNode, RAS_MaterialBucket *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_DisplayArrayNode : public RAS_BaseNode<RAS_MaterialNode, RAS_MeshSlotNode, RAS_DisplayArrayBucket *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_MeshSlotNode : public RAS_BaseNode<RAS_DisplayArrayNode, RAS_NullNode, RAS_MeshSlot *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

#endif  // __RAS_RENDER_NODE__
