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
class RAS_DisplayArrayNode;
class RAS_MeshSlotNode;

class RAS_ManagerNode : public RAS_BaseNode<RAS_MaterialNode *, RAS_BucketManager *, const MT_Transform&, RAS_IRasterizer *, bool>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_MaterialNode : public RAS_BaseNode<RAS_DisplayArrayNode *, RAS_MaterialBucket *, const MT_Transform&, RAS_IRasterizer *, bool>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_DisplayArrayNode : public RAS_BaseNode<RAS_MeshSlotNode *, RAS_DisplayArrayBucket *, const MT_Transform&, RAS_IRasterizer *, bool>
{
	using RAS_BaseNode::RAS_BaseNode;
};

class RAS_MeshSlotNode : public RAS_BaseNode<RAS_NullNode *, RAS_MeshSlot *, const MT_Transform&, RAS_IRasterizer *>
{
	using RAS_BaseNode::RAS_BaseNode;
};

#endif  // __RAS_RENDER_NODE__
