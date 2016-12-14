#ifndef __RAS_DOWNWARD_NODE_H__
#define __RAS_DOWNWARD_NODE_H__

#include "RAS_BaseNode.h"

#include <vector>

template <class _ChildType, class InfoType, RAS_NodeFlag Flag, class Args>
class RAS_DownwardNode : public RAS_BaseNode<InfoType, Flag, Args>
{
public:
	using typename RAS_BaseNode<InfoType, Flag, Args>::Function;
	typedef _ChildType ChildType;
	typedef std::vector<ChildType *> ChildTypeList;
	typedef RAS_DownwardNode<ChildType, InfoType, Flag, Args> SelfType;

private:
	ChildTypeList m_children;

public:
	RAS_DownwardNode(InfoType *info, Function bind, Function unbind)
		:RAS_BaseNode<InfoType, Flag, Args>(info, bind, unbind)
	{
	}

	RAS_DownwardNode()
	{
	}

	~RAS_DownwardNode()
	{
	}

	bool GetValid() const
	{
		if (Flag == RAS_NodeFlag::NEVER_FINAL) {
			return m_children.size() > 0;
		}
		return true;
	}

	void AddChild(ChildType *child)
	{
		if (child->GetValid()) {
			m_children.push_back(child);
		}
	}

	void Clear()
	{
		if (Flag == RAS_NodeFlag::NEVER_FINAL) {
			m_children.clear();
		}
	}

	void Execute(const Args& args)
	{
		// TODO ajouter une reference vers une struture passée en argument de toutes les fonctions
		this->Bind(args);

		for (ChildType *child : m_children) {
			child->Execute(args);
		}

		this->Unbind(args);

		Clear();
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
			for (ChildType *child : m_children) {
				child->Print(level + 1, recursive);
			}
		}
	}
};

#endif  // __RAS_DOWNWARD_NODE_H__
