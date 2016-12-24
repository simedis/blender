#ifndef __RAS_BASIC_NODE__
#define __RAS_BASIC_NODE__

#include <functional>
#include <algorithm>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <cxxabi.h>

class RAS_NullNode
{
public:
	void Print(unsigned short level)
	{
	}
};

template <class SubNodeType, class InfoType, class ... Args>
class RAS_BaseNode
{
public:
	typedef std::vector<SubNodeType> SubNodeTypeList;
	typedef typename SubNodeTypeList::iterator SubNodeTypeListIterator;
	typedef std::function<void(InfoType, SubNodeTypeList, Args ...)> Function;

public: //private:
	InfoType m_info;
	Function m_function;
	SubNodeTypeList m_subNodes;
	unsigned int m_orderStart;
	unsigned int m_orderEnd;

public:
	RAS_BaseNode(InfoType info, Function function)
		:m_info(info),
		m_function(function),
		m_orderStart(0),
		m_orderEnd(0)
	{
	}

	RAS_BaseNode()
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

	inline const void Execute(Args ... args) const
	{
// 		std::cout << __PRETTY_FUNCTION__ << std::endl;
		m_function(m_info, m_subNodes, args...);
	}

	inline bool operator<(const RAS_BaseNode& other)
	{
		return m_orderEnd < other.m_orderStart;
	}

	void Print(unsigned short level)
	{
		for (unsigned short i = 0; i < level; ++i) {
			std::cout << "\t";
		}

		char *demangled_name = abi::__cxa_demangle(typeid(InfoType).name(), NULL, NULL, NULL);
		std::cout << demangled_name << "(" << m_info << "): "<< std::endl;
		free(demangled_name);

		for (SubNodeTypeListIterator it = m_subNodes.begin(), end = m_subNodes.end(); it != end; ++it) {
			(*it)->Print(level + 1);
		}
	}
};

#endif  // __RAS_BASIC_NODE__
