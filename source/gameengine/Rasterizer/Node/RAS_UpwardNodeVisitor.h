#ifndef __RAS_UPWARD_NODE_VISITOR_H__
#define __RAS_UPWARD_NODE_VISITOR_H__

#include "RAS_BaseNode.h"
#include "RAS_DummyNode.h"

template <class NodeType, class Args>
class RAS_UpwardNodeVisitor
{
public:
	typedef typename NodeType::ParentType ParentNodeType;
	typedef RAS_UpwardNodeVisitor<ParentNodeType, Args> ParentType;

private:
	NodeType *m_node;
	std::unique_ptr<ParentType> m_parent;

public:
	RAS_UpwardNodeVisitor()
		:m_node(NULL)
	{
		if (std::is_same<NodeType *, RAS_DummyNode *>()) {
			m_parent.reset(NULL);
		}
		else {
			m_parent.reset(new ParentType());
		}
	}

	~RAS_UpwardNodeVisitor()
	{
	}

	void NextNode(NodeType *node, const Args& args)
	{
		if (std::is_same<NodeType *, RAS_DummyNode *>() || node == m_node) {
			return;
		}

		m_parent->NextNode(node->GetParent(), args);

		if (m_node) {
			m_node->Unbind(args);
		}
		m_node = node;
		m_node->Bind(args);
	}

	void Unbind(const Args& args)
	{
		if (std::is_same<NodeType *, RAS_DummyNode *>()) {
			return;
		}

		m_node->Unbind(args);
		m_parent->Unbind(args);
	}
};

#endif  // __RAS_UPWARD_NODE_VISITOR_H__
