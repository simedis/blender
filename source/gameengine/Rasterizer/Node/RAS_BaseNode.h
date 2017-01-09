#ifndef __RAS_BASIC_NODE__
#define __RAS_BASIC_NODE__

#include <functional>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

enum class RAS_NodeFlag {
	/// The node is always a final node.
	ALWAYS_FINAL,
	/// The node is never a final node.
	NEVER_FINAL
};

/** RAS_BaseNode is a class wrapping a rendering class by simulating it with a
 * binding and unbinding function.
 * \param InfoType The class to wrap functions from.
 * \param Flag The node flag to know the final state of a node.
 * \param Args The arguments type to pass to the binding and unbinding functions.
 */
template <class InfoType, RAS_NodeFlag Flag, class Args>
class RAS_BaseNode
{
public:
	/** The type of function to call for binding and unbinding.
	 * It takes as arguments the class the node is wrapping and the structure
	 * containing the arguments.
	 */
	typedef std::function<void(InfoType *, const Args&)> Function;

protected:
	/// An instance of the wrapped class.
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

	inline InfoType *GetInfo() const
	{
		return m_info;
	}

	inline void Bind(const Args& args)
	{
		if (m_bind) {
			m_bind(m_info, args);
		}
	}

	inline void Unbind(const Args& args)
	{
		if (m_unbind) {
			m_unbind(m_info, args);
		}
	}
};

#endif  // __RAS_BASIC_NODE__
