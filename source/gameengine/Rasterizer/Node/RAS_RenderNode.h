#ifndef __RAS_RENDER_NODE__
#define __RAS_RENDER_NODE__

#include "RAS_BaseNode.h"

class RAS_BucketManager;
class RAS_MaterialBucket;
class RAS_DisplayArrayBucket;
class RAS_MeshSlot;
class RAS_IRasterizer;
class MT_Transform;

typedef RAS_Node<RAS_NullNode *, RAS_MeshSlot *, const MT_Transform&, RAS_IRasterizer *> RAS_MeshSlotNode;
typedef RAS_Node<RAS_MeshSlotNode *, RAS_DisplayArrayBucket *, const MT_Transform&, RAS_IRasterizer *, bool> RAS_DisplayArrayNode;
typedef RAS_Node<RAS_DisplayArrayNode *, RAS_MaterialBucket *, const MT_Transform&, RAS_IRasterizer *, bool> RAS_MaterialNode;
typedef RAS_Node<RAS_MaterialNode *, RAS_BucketManager *, const MT_Transform&, RAS_IRasterizer *, bool> RAS_ManagerNode;

#endif  // __RAS_RENDER_NODE__
