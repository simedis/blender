#ifndef __RAS_RENDER_NODE__
#define __RAS_RENDER_NODE__

#include "RAS_DownwardNode.h"
#include "RAS_UpwardNode.h"
#include "RAS_UpwardNodeVisitor.h"

class RAS_BucketManager;
class RAS_MaterialBucket;
class RAS_DisplayArrayBucket;
class RAS_MeshSlot;
class RAS_IRasterizer;
class MT_Transform;

class RAS_MaterialDownwardNode;
class RAS_ManagerDownwardNode;
class RAS_DisplayArrayDownwardNode;

class RAS_MaterialUpwardNode;
class RAS_ManagerUpwardNode;
class RAS_DisplayArrayUpwardNode;
class RAS_MeshSlotUpwardNode;

class RAS_RenderNodeArguments
{
public:
	const MT_Transform& m_trans;
	RAS_IRasterizer *m_rasty;
	bool m_sort;

	RAS_RenderNodeArguments(const MT_Transform& trans, RAS_IRasterizer *rasty, bool sort)
		:m_trans(trans),
		m_rasty(rasty),
		m_sort(sort)
	{
	}
};

class RAS_ManagerDownwardNode : public RAS_DownwardNode<RAS_MaterialDownwardNode, RAS_BucketManager, RAS_NodeFlag::NEVER_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_DownwardNode::RAS_DownwardNode;
};

class RAS_MaterialDownwardNode : public RAS_DownwardNode<RAS_DisplayArrayDownwardNode, RAS_MaterialBucket, RAS_NodeFlag::NEVER_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_DownwardNode::RAS_DownwardNode;
};

class RAS_DisplayArrayDownwardNode : public RAS_DownwardNode<RAS_DummyNode, RAS_DisplayArrayBucket, RAS_NodeFlag::ALWAYS_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_DownwardNode::RAS_DownwardNode;
};

class RAS_ManagerUpwardNode : public RAS_UpwardNode<RAS_DummyNode, RAS_BucketManager, RAS_NodeFlag::NEVER_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_UpwardNode::RAS_UpwardNode;
};

class RAS_MaterialUpwardNode : public RAS_UpwardNode<RAS_ManagerUpwardNode, RAS_MaterialBucket, RAS_NodeFlag::NEVER_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_UpwardNode::RAS_UpwardNode;
};

class RAS_DisplayArrayUpwardNode : public RAS_UpwardNode<RAS_MaterialUpwardNode, RAS_DisplayArrayBucket, RAS_NodeFlag::NEVER_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_UpwardNode::RAS_UpwardNode;
};

class RAS_MeshSlotUpwardNode : public RAS_UpwardNode<RAS_DisplayArrayUpwardNode, RAS_MeshSlot, RAS_NodeFlag::ALWAYS_FINAL,
	RAS_RenderNodeArguments>
{
	using RAS_UpwardNode::RAS_UpwardNode;
};

typedef std::vector<RAS_MeshSlotUpwardNode *> RAS_UpwardTreeLeafs;

class RAS_MeshSlotUpwardNodeVisitor : public RAS_UpwardNodeVisitor<RAS_MeshSlotUpwardNode, RAS_RenderNodeArguments>
{
	using RAS_UpwardNodeVisitor::RAS_UpwardNodeVisitor;
};

#endif  // __RAS_RENDER_NODE__
