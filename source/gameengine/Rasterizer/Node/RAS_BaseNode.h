#ifndef __RAS_BASIC_NODE__
#define __RAS_BASIC_NODE__

#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <thread>
#include <typeinfo>
#include <type_traits>
#include <cxxabi.h>

class RAS_BaseNode
{
public:
	virtual ~RAS_BaseNode()
	{
	}
};

class RAS_NullNode // RAS_DummyNode
{
public:
	inline int GetOrderBegin() const
	{
		return -1;
	}

	inline int GetOrderEnd() const
	{
		return -1;
	}

	void Print(unsigned short level, bool recursive)
	{
	}

	template<class NodeTypeList>
	void AddSubNodes(NodeTypeList subNodes)
	{
	}

	template<class NodeType>
	void AddSubNode(NodeType subNode)
	{
	}

	template<class NodeType, class FunctionType>
	void ForEach(const FunctionType& function)
	{
	}

	template<class NodeType>
	void Split(NodeType parent, std::vector<RAS_BaseNode *>& collector)
	{
	}
};

enum class RAS_NodeFlag {
	ALWAYS_FINAL,
	MAYBE_FINAL,
	NEVER_FINAL
};

template <class _ParentNodeType, class _SubNodeType, class InfoType, RAS_NodeFlag Flag, class ... Args>
class RAS_Node : public RAS_BaseNode
{
public:
	typedef _ParentNodeType ParentNodeType;
	typedef _SubNodeType SubNodeType;
	typedef std::vector<SubNodeType> SubNodeTypeList;
	typedef typename SubNodeTypeList::iterator SubNodeTypeListIterator;
	typedef std::function<void(InfoType, SubNodeTypeList&, Args ...)> Function;

public:
	typedef RAS_Node<SubNodeType, InfoType, Flag, Args ...> SelfType;

	InfoType m_info;
	Function m_function;
	ParentNodeType m_parentNode;
	SubNodeTypeList m_subNodes;
	int m_orderBegin;
	int m_orderEnd;
	bool m_final;

public:
	RAS_Node(InfoType info, Function function, int isfinal = false)
		:m_info(info),
		m_function(function),
		m_orderBegin(-1),
		m_orderEnd(-1),
		m_final(isfinal)
	{
	}

	RAS_Node()
	{
	}

	RAS_Node(const RAS_Node& other)
	{
		m_info = other.m_info;
		m_function = other.m_function;
		m_orderBegin = -1;
		m_orderEnd = -1;
		m_final = other.m_final;
		m_subNodes.clear();
	}

	virtual ~RAS_Node()
	{
	}

	inline InfoType GetInfo() const
	{
		return m_info;
	}

	inline int GetOrderBegin() const
	{
		return m_orderBegin;
	}

	inline int GetOrderEnd() const
	{
		return m_orderEnd;
	}

	inline void SetOrder(int begin, int end)
	{
		m_orderBegin = begin;
		m_orderEnd = end;
	}

	void SetParentNode(ParentNodeType parentNode)
	{
		m_parentNode = parentNode;
	}

	inline void AddSubNode(const SubNodeType& subNode)
	{
		if (subNode->GetValid()) {
			m_subNodes.push_back(subNode);
			subNode->SetParentNode(this);
		}
	}

	inline void AddSubNodes(const SubNodeTypeList& subNodes)
	{
		m_subNodes.insert(m_subNodes.begin(), subNodes.begin(), subNodes.end());
	}

	template<RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag == RAS_NodeFlag::ALWAYS_FINAL, bool>::type GetValid() const
	{
		return true;
	}

	template<RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag == RAS_NodeFlag::NEVER_FINAL, bool>::type GetValid() const
	{
		return !GetEmpty();
	}

	template<RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag == RAS_NodeFlag::MAYBE_FINAL, bool>::type GetValid() const
	{
		return m_final || !GetEmpty();
	}

	inline bool GetEmpty() const
	{
		return m_subNodes.size() == 0;
	}

	inline const void Execute(Args ... args)
	{
		m_function(m_info, m_subNodes, args...);
		m_subNodes.clear();
	}

	template<class NodeType, class FunctionType>
	inline typename std::enable_if<std::is_same<NodeType, SelfType>::value, void>::type ForEach(const FunctionType& function)
	{
		function(this);
	}

	template<class NodeType, class FunctionType>
	inline typename std::enable_if<!std::is_same<NodeType, SelfType>::value, void>::type ForEach(const FunctionType& function)
	{
		for (SubNodeType& node : m_subNodes) {
			node->ForEach<NodeType>(function);
		}
	}

	void Split(std::vector<RAS_BaseNode *>& collector)
	{
		const SubNodeTypeList auxSubNodes = m_subNodes;
		for (SubNodeType node : auxSubNodes) {
			node->Split(this, collector);
		}

		if (m_subNodes.size() < 2) {
			return;
		}

		std::sort(m_subNodes.begin(), m_subNodes.end(),
				  [](const SubNodeType node1, const SubNodeType node2) { return node1->GetOrderBegin() < node2->GetOrderEnd(); });
	}

	template<class NodeType, RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag == RAS_NodeFlag::ALWAYS_FINAL, void>::type Split(NodeType parent, std::vector<RAS_BaseNode *>& collector)
	{
	}

	template<class NodeType, RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag != RAS_NodeFlag::ALWAYS_FINAL, void>::type Split(NodeType parent, std::vector<RAS_BaseNode *>& collector)
	{
		const SubNodeTypeList auxSubNodes = m_subNodes;
		for (SubNodeType node : auxSubNodes) {
			node->Split(this, collector);
		}

		if (m_subNodes.size() < 2) {
			return;
		}

		std::sort(m_subNodes.begin(), m_subNodes.end(),
				  [](const SubNodeType node1, const SubNodeType node2) { return node1->GetOrderBegin() < node2->GetOrderEnd(); });

		std::vector<SelfType *> nodes;
		SubNodeTypeListIterator begin = m_subNodes.begin();
		SubNodeTypeListIterator end = m_subNodes.end();
		for (SubNodeTypeListIterator it = begin, jt = std::next(it, 1); jt != end; ++it, ++jt) {
			SubNodeType first = *it;
			SubNodeType second = *jt;
			if (first->GetOrderEnd() != (second->GetOrderBegin() - 1)) {
//				std::cout << "discontinuity:" << std::endl;
//				first->Print(1, false);
//				second->Print(1, false);

				SelfType *replica = new SelfType(*this);
				replica->m_subNodes.insert(replica->m_subNodes.begin(), begin, jt);
				replica->m_orderBegin = replica->m_subNodes.front()->GetOrderBegin();
				replica->m_orderEnd = replica->m_subNodes.back()->GetOrderEnd();
				nodes.push_back(replica);
				collector.push_back(replica);
				begin = jt;
			}
		}
		m_subNodes.erase(m_subNodes.begin(), begin);
		parent->AddSubNodes(nodes);

		m_orderBegin = m_subNodes.front()->GetOrderBegin();
		m_orderEnd = m_subNodes.back()->GetOrderEnd();
	}

	template<RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag == RAS_NodeFlag::ALWAYS_FINAL, void>::type Clear()
	{
	}

	template<RAS_NodeFlag _Flag = Flag>
	typename std::enable_if<_Flag != RAS_NodeFlag::ALWAYS_FINAL, void>::type Clear()
	{
		for (SubNodeType& node : m_subNodes) {
			node->Clear();
		}

		m_subNodes.clear();
	}

	void Print(unsigned short level, bool recursive)
	{
		for (unsigned short i = 0; i < level; ++i) {
			std::cout << "\t";
		}

		char *demangled_name = abi::__cxa_demangle(typeid(InfoType).name(), NULL, NULL, NULL);
		std::cout << demangled_name << "(" << m_info << ", " << m_final << ") [" << m_orderBegin << ", " << m_orderEnd << "]: "<< std::endl;
		free(demangled_name);

		if (recursive) {
			for (const SubNodeType& node : m_subNodes) {
				node->Print(level + 1, recursive);
			}
		}
	}
};

#endif  // __RAS_BASIC_NODE__
