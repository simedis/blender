#ifndef __RAS_BASIC_NODE__
#define __RAS_BASIC_NODE__

#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <forward_list>
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

	void Clear()
	{
	}

	template<class NodeType, class FunctionType>
	void ForEach(FunctionType function)
	{
	}

	template<class NodeType>
	void Split(NodeType parent, std::forward_list<RAS_BaseNode *>& collector)
	{
	}
};

template <class SubNodeType, class InfoType, class ... Args>
class RAS_Node : public RAS_BaseNode
{
public:
	typedef std::vector<SubNodeType> SubNodeTypeList;
	typedef typename SubNodeTypeList::iterator SubNodeTypeListIterator;
	typedef std::function<void(InfoType, SubNodeTypeList, Args ...)> Function;

protected:
	typedef RAS_Node<SubNodeType, InfoType, Args ...> SelfType;

	InfoType m_info;
	Function m_function;
	SubNodeTypeList m_subNodes;
	int m_orderBegin;
	int m_orderEnd;
	bool m_final;

public:
	RAS_Node(InfoType info, Function function, bool isfinal = false)
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

	inline void AddSubNode(const SubNodeType& subNode)
	{
		m_subNodes.push_back(subNode);
	}

	inline void AddSubNodes(const SubNodeTypeList& subNodes)
	{
		m_subNodes.insert(m_subNodes.begin(), subNodes.begin(), subNodes.end());
	}

	inline bool GetEmpty() const
	{
		return m_subNodes.empty();
	}

	inline const void Execute(Args ... args) const
	{
		m_function(m_info, m_subNodes, args...);
	}

	template<class NodeType, class FunctionType, typename std::enable_if<std::is_same<NodeType, SelfType>::value, int>::type = 0>
	inline void ForEach(FunctionType function)
	{
		function(this);
	}

	template<class NodeType, class FunctionType, typename std::enable_if<!std::is_same<NodeType, SelfType>::value, int>::type = 0>
	inline void ForEach(FunctionType function)
	{
		for (SubNodeType& node : m_subNodes) {
			node->ForEach<NodeType>(function);
		}
	}

	void Split(std::forward_list<RAS_BaseNode *>& collector)
	{
		if (m_subNodes.empty()) {
			return;
		}

		const SubNodeTypeList auxSubNodes = m_subNodes;
		for (SubNodeType node : auxSubNodes) {
			node->Split(this, collector);
		}

		if (m_subNodes.size() < 2) {
			return;
		}

		std::sort(m_subNodes.begin(), m_subNodes.end(),
				  [](const SubNodeType node1, const SubNodeType node2) { return node1->GetOrderBegin() < node2->GetOrderEnd(); });

		m_orderBegin = m_subNodes.front()->GetOrderBegin();
		m_orderEnd = m_subNodes.back()->GetOrderEnd();
	}

	template <class NodeType>
	void Split(NodeType parent, std::forward_list<RAS_BaseNode *>& collector)
	{
		if (m_subNodes.empty()) {
			return;
		}

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
				collector.push_front(replica);
				begin = jt;
			}
		}
		m_subNodes.erase(m_subNodes.begin(), begin);
		parent->AddSubNodes(nodes);

		m_orderBegin = m_subNodes.front()->GetOrderBegin();
		m_orderEnd = m_subNodes.back()->GetOrderEnd();
	}

	void Reduce()
	{
	}

	inline void Clear()
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
