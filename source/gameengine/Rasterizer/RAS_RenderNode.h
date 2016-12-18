#include <functional>
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

template <class SubNodeType, class InfoType, class ... Args>
class RAS_BaseNode
{
public:
	typedef std::vector<SubNodeType> SubNodeTypeList;
	typedef std::function<void(InfoType, SubNodeTypeList, Args ...)> Function;

public: //private:
	InfoType m_info;
	Function m_function;
	SubNodeTypeList m_subNodes;

public:
	RAS_BaseNode(InfoType info, Function function)
		:m_info(info),
		m_function(function)
	{
	}
	virtual ~RAS_BaseNode()
	{
	}

	RAS_BaseNode(const RAS_BaseNode& node)
	{
		m_info = node.m_info;
		m_function = node.m_function;
		m_subNodes = node.m_subNodes;
		std::cout << __func__ << ", " << m_subNodes.size() << std::endl;
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
};

class RAS_ManagerNode : public RAS_BaseNode<RAS_MaterialNode, RAS_BucketManager *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_MaterialNode : public RAS_BaseNode<RAS_DisplayArrayNode, RAS_MaterialBucket *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_DisplayArrayNode : public RAS_BaseNode<RAS_MeshSlotNode, RAS_DisplayArrayBucket *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_MeshSlotNode : public RAS_BaseNode<int, RAS_MeshSlot *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};
