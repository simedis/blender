#ifndef __RAS_RENDER_NODE__
#define __RAS_RENDER_NODE__

#include "RAS_BaseNode.h"

class RAS_BucketManager;
class RAS_MaterialBucket;
class RAS_DisplayArrayBucket;
class RAS_MeshSlot;
class RAS_IRasterizer;
class MT_Transform;

class RAS_MaterialNode;
class RAS_ManagerNode;
class RAS_DisplayArrayNode;
class RAS_MeshSlotNode;

class RAS_ManagerNode : public RAS_Node<RAS_NullNode *, RAS_MaterialNode *, RAS_BucketManager *, RAS_NodeFlag::NEVER_FINAL,
	const MT_Transform&, RAS_IRasterizer *, bool>
{
	using RAS_Node::RAS_Node;
};

class RAS_MaterialNode : public RAS_Node<RAS_ManagerNode *, RAS_DisplayArrayNode *, RAS_MaterialBucket *, RAS_NodeFlag::NEVER_FINAL,
	const MT_Transform&, RAS_IRasterizer *, bool>
{
	using RAS_Node::RAS_Node;
};

class RAS_DisplayArrayNode : public RAS_Node<RAS_MaterialNode *, RAS_MeshSlotNode *, RAS_DisplayArrayBucket *, RAS_NodeFlag::MAYBE_FINAL,
	const MT_Transform&, RAS_IRasterizer *, bool>
{
	using RAS_Node::RAS_Node;
};

class RAS_MeshSlotNode : public RAS_Node<RAS_DisplayArrayNode *, RAS_NullNode *, RAS_MeshSlot *, RAS_NodeFlag::ALWAYS_FINAL,
	const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_Node::RAS_Node;
};

#endif  // __RAS_RENDER_NODE__
