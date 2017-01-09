#ifndef __RAS_UPWARD_NODE_VISITOR_H__
#define __RAS_UPWARD_NODE_VISITOR_H__

#include "RAS_BaseNode.h"
#include "RAS_DummyNode.h"

#include <memory>

/** RAS_UpwardNodeVisitor is class using to proceed the sorted render using RAS_UpwardNode.
 *
 * A sorted render is proceed comparing the parent node of the current node with the parent
 * node of the previous node. If the both parent nodes are not the same, then the previous
 * parent node is calling its unbinding function and the current parent node is calling its
 * binding function.
 * The same operation is done recursively for parent node of parent node.
 *
 * \param NodeType The upward node type.
 * \param Args The arguments type used for upward node binding and unbinding function.
 */
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
		// If the node type is dummy of the parent node is unchanged, nothing is done.
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

	/// This function is called at the end of the sorted render to unbind the previous nodes.
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
