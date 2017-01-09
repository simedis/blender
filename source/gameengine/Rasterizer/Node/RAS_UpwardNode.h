#ifndef __RAS_UPWARD_NODE_H__
#define __RAS_UPWARD_NODE_H__

#include "RAS_BaseNode.h"

/** RAS_UpwardNode is a node storing its parent node.
 *
 * A upward node is using for sorted render were two non-consecutive nodes could share
 * the same parent node. In this case the render can be proceed from top to bottom, but
 * from the bottom (leafs) to the bottom. This process is external in RAS_UpwardNodeVisitor.
 *
 * \param _ParentType The parent node type.
 */
template <class _ParentType, class InfoType, RAS_NodeFlag Flag, class Args>
class RAS_UpwardNode : public RAS_BaseNode<InfoType, Flag, Args>
{
public:
	using typename RAS_BaseNode<InfoType, Flag, Args>::Function;
	typedef _ParentType ParentType;

private:
	ParentType *m_parent;

public:
	RAS_UpwardNode(InfoType *info, Function bind, Function unbind)
		:RAS_BaseNode<InfoType, Flag, Args>(info, bind, unbind),
		m_parent(NULL)
	{
	}

	RAS_UpwardNode()
	{
	}

	~RAS_UpwardNode()
	{
	}

	inline ParentType *GetParent() const
	{
		return m_parent;
	}

	inline void SetParent(ParentType *parent)
	{
		m_parent = parent;
	}

	void Print(unsigned short level, bool recursive)
	{
		for (unsigned short i = 0; i < level; ++i) {
			std::cout << "\t";
		}

		char *demangled_name = abi::__cxa_demangle(typeid(InfoType).name(), NULL, NULL, NULL);
		std::cout << demangled_name << "(" << this->m_info << ") "<< std::endl;
		free(demangled_name);

		if (recursive) {
			m_parent->Print(level + 1, recursive);
		}
	}
};

#endif  // __RAS_UPWARD_NODE_H__
