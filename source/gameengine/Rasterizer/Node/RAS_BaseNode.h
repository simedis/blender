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

enum class RAS_NodeFlag {
	ALWAYS_FINAL,
	NEVER_FINAL
};

template <class InfoType, RAS_NodeFlag Flag, class Args>
class RAS_BaseNode
{
public:
	typedef std::function<void(InfoType *, const Args&)> Function;

protected:
	InfoType *m_info;

	Function m_bind;
	Function m_unbind;

public:
	RAS_BaseNode(InfoType *info, Function bind, Function unbind)
		:m_info(info),
		m_bind(bind),
		m_unbind(unbind)
	{
	}

	RAS_BaseNode()
	{
	}

	~RAS_BaseNode()
	{
	}

	InfoType *GetInfo() const
	{
		return m_info;
	}

	void Bind(const Args& args)
	{
		if (m_bind) {
			m_bind(m_info, args);
		}
	}

	void Unbind(const Args& args)
	{
		if (m_unbind) {
			m_unbind(m_info, args);
		}
	}
};

#endif  // __RAS_BASIC_NODE__
