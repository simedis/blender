/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Converter/KX_SoftBodyDeformer.cpp
 *  \ingroup bgeconv
 */


#ifdef _MSC_VER
#  pragma warning (disable:4786)
#endif //WIN32

#include "BLI_utildefines.h"

#include "KX_SoftBodyDeformer.h"
#include "RAS_MeshObject.h"
#include "RAS_DisplayArray.h"
#include "RAS_BoundingBoxManager.h"
#include "RAS_TexVert.h"

#ifdef WITH_BULLET

#include "CcdPhysicsEnvironment.h"
#include "CcdPhysicsController.h"
#include "BulletSoftBody/btSoftBody.h"

#include "btBulletDynamicsCommon.h"

KX_SoftBodyDeformer::KX_SoftBodyDeformer(RAS_MeshObject *pMeshObject, BL_DeformableGameObject *gameobj)
	:RAS_Deformer(pMeshObject),
	m_gameobj(gameobj),
	m_needUpdateAabb(true)
{
	KX_Scene *scene = m_gameobj->GetScene();
	RAS_BoundingBoxManager *boundingBoxManager = scene->GetBoundingBoxManager();
	m_boundingBox = boundingBoxManager->CreateBoundingBox();
	// Set AABB default to mesh bounding box AABB.
	m_boundingBox->CopyAabb(m_mesh->GetBoundingBox());
}

KX_SoftBodyDeformer::~KX_SoftBodyDeformer()
{
}

void KX_SoftBodyDeformer::Relink(std::map<SCA_IObject *, SCA_IObject *>& map)
{
	BL_DeformableGameObject *obj = static_cast<BL_DeformableGameObject *>(map[m_gameobj]);

	if (obj) {
		m_gameobj = obj;
		m_mesh = m_gameobj->GetMesh(0);
	}
	else {
		m_gameobj = nullptr;
		m_mesh = nullptr;
	}
}

bool KX_SoftBodyDeformer::Apply(RAS_IPolyMaterial *polymat, RAS_MeshMaterial *meshmat)
{
	CcdPhysicsController *ctrl = (CcdPhysicsController *)m_gameobj->GetPhysicsController();
	if (!ctrl)
		return false;

	btSoftBody *softBody = ctrl->GetSoftBody();
	if (!softBody)
		return false;

	// update the vertex in m_transverts
	Update();

	RAS_MeshSlot *slot = meshmat->m_slots[(void *)m_gameobj->getClientInfo()];
	if (!slot) {
		return false;
	}

	RAS_IDisplayArray *array = slot->GetDisplayArray();

	btSoftBody::tNodeArray&   nodes(softBody->m_nodes);


	if (m_needUpdateAabb) {
		m_boundingBox->SetAabb(MT_Vector3(0.0f, 0.0f, 0.0f), MT_Vector3(0.0f, 0.0f, 0.0f));
		m_needUpdateAabb = false;
	}

	// AABB Box : min/max.
	MT_Vector3 aabbMin;
	MT_Vector3 aabbMax;

	for (unsigned int i = 0, size = array->GetVertexCount(); i < size; ++i) {
		RAS_ITexVert *v = array->GetVertex(i);
		const RAS_TexVertInfo& vinfo = array->GetVertexInfo(i);
		/* The physics converter write the soft body index only in the original
		 * vertex array because at this moment it doesn't know which is the
		 * game object. It didn't cause any issues because it's always the same
		 * vertex order.
		 */
		const unsigned int softbodyindex = vinfo.getSoftBodyIndex();

		MT_Vector3 pt(
		    nodes[softbodyindex].m_x.getX(),
		    nodes[softbodyindex].m_x.getY(),
		    nodes[softbodyindex].m_x.getZ());
		v->SetXYZ(pt);

		MT_Vector3 normal(
		    nodes[softbodyindex].m_n.getX(),
		    nodes[softbodyindex].m_n.getY(),
		    nodes[softbodyindex].m_n.getZ());
		v->SetNormal(normal);

		if (!m_gameobj->GetAutoUpdateBounds()) {
			continue;
		}

		const MT_Vector3& scale = m_gameobj->NodeGetWorldScaling();
		const MT_Vector3& invertscale = MT_Vector3(1.0f / scale.x(), 1.0f / scale.y(), 1.0f / scale.z());
		const MT_Vector3& pos = m_gameobj->NodeGetWorldPosition();
		const MT_Matrix3x3& rot = m_gameobj->NodeGetWorldOrientation();

		// Extract object transform from the vertex position.
		pt = (pt - pos) * rot * invertscale;
		// if the AABB need an update.
		if (i == 0) {
			aabbMin = aabbMax = pt;
		}
		else {
			aabbMin.x() = std::min(aabbMin.x(), pt.x());
			aabbMin.y() = std::min(aabbMin.y(), pt.y());
			aabbMin.z() = std::min(aabbMin.z(), pt.z());
			aabbMax.x() = std::max(aabbMax.x(), pt.x());
			aabbMax.y() = std::max(aabbMax.y(), pt.y());
			aabbMax.z() = std::max(aabbMax.z(), pt.z());
		}
	}
	/*
	array->UpdateFrom(origarray, origarray->GetModifiedFlag() &
					 (RAS_IDisplayArray::TANGENT_MODIFIED |
					  RAS_IDisplayArray::UVS_MODIFIED |
					  RAS_IDisplayArray::COLORS_MODIFIED));
	*/
	m_boundingBox->ExtendAabb(aabbMin, aabbMax);

	return true;
}


class KX_SoftBodyRefineCallback: public PHY_IRefineCallback
{
public:
	KX_SoftBodyRefineCallback(RAS_IDisplayArray *array, btSoftBody *softBody, std::vector<int> *soft2vertex)
	{
		m_array = array;
		m_softBody = softBody;
		m_soft2vertex = soft2vertex;
		m_uvsize = array->GetVertexUvSize();
		m_rgbsize = array->GetVertexColorSize();
		m_vertexnb = m_array->GetVertexCount();
		m_endNodes.reserve(8);
	}

	virtual void NewNode(int newnode, int node0, int node1, float t)
	{
		int vindex, i;

		if (newnode >= m_soft2vertex->size())
			// should always be the case
			m_soft2vertex->resize(newnode+1,-1);
		if (m_soft2vertex->at(newnode) != -1)
			// should not happen
			return;
		if ((vindex = m_soft2vertex->at(node0)) == -1)
			// should not happen
			return;
		RAS_ITexVert *vertex0 = m_array->GetVertexNoCache(vindex);
		RAS_TexVertInfo vinfo = m_array->GetVertexInfo(vindex);
		vinfo.setSoftBodyIndex(newnode);
		m_array->AddVertexInfo(vinfo);
		m_array->AddVertex(vertex0);
		vindex = m_array->GetVertexCount()-1;
		m_soft2vertex->at(newnode) = vindex;
		if (node1 != -1) {\
			unsigned int color;
			unsigned char *c = (unsigned char*)&color;
			float xyz[3];
			float uv[2];
			float ct = 1.f-t;
			vertex0 = m_array->GetVertexNoCache(vindex);
			if ((vindex = m_soft2vertex->at(node1)) == -1)
				// should not happen
				return;
			RAS_ITexVert *vertex1 = m_array->GetVertexNoCache(vindex);
			// now interpolate the position
			// don't interpolate the tangent (not used) and the normal (recomputed)
			const float *f0, *f1;
			f0 = vertex0->getXYZ();
			f1 = vertex1->getXYZ();
			xyz[0] = f0[0]*ct+f1[0]*t;
			xyz[1] = f0[1]*ct+f1[1]*t;
			xyz[2] = f0[2]*ct+f1[2]*t;
			vertex0->SetXYZ(xyz);
			for (i=0; i<m_uvsize; i++)
			{
				f0 = vertex0->getUV(i);
				f1 = vertex1->getUV(i);
				uv[0] = f0[0]*ct+f1[0]*t;
				uv[1] = f0[1]*ct+f1[1]*t;
				vertex0->SetUV(i,uv);
			}
			for (i=0; i<m_rgbsize; i++)
			{
				const unsigned char *c0, *c1;
				c0 = vertex0->getRGBA(i);
				c1 = vertex1->getRGBA(i);
				c[0] = (unsigned char)(c0[0]*ct+c1[0]*t);
				c[1] = (unsigned char)(c0[1]*ct+c1[1]*t);
				c[2] = (unsigned char)(c0[2]*ct+c1[2]*t);
				c[3] = (unsigned char)(c0[3]*ct+c1[3]*t);
				vertex0->SetRGBA(i, color);
			}
		}
	}

	void EndNode(int endnode)
	{
		m_endNodes.push_back(endnode);
	}

	int GetEndNodes(int **nodes)
	{
		if (nodes == NULL || m_endNodes.size() == 0)
			return 0;
		*nodes = &m_endNodes[0];
		return m_endNodes.size();
	}

	virtual void Finalize(bool success)
	{
		if (success)
		{
			// refine successful - load the new face structure
			btSoftBody::tFaceArray&   faces(m_softBody->m_faces);
			btSoftBody::Node *base = &m_softBody->m_nodes[0];

			// note that the entire face structure must be rebuild because new faces may use old nodes
			m_array->SetIndexCount(faces.size()*3);
			int i, j, size;
			for (i=j=0, size=faces.size(); i<size; i++)
			{
				const btSoftBody::Face& face=faces[i];
				m_array->SetIndex(j++,m_soft2vertex->at(face.m_n[0]-base));
				m_array->SetIndex(j++,m_soft2vertex->at(face.m_n[1]-base));
				m_array->SetIndex(j++,m_soft2vertex->at(face.m_n[2]-base));
			}
			// now tell the GPU that the mesh is changed
			m_array->AppendModifiedFlag(RAS_IDisplayArray::MESH_MODIFIED|RAS_IDisplayArray::INDEX_MODIFIED);
			m_array->UpdateCache();
		}
		else
		{
			// refine did not result in any change after all, must delete all the vertices that we've created
			if (m_array->GetVertexCount() > m_vertexnb)
			{
				// something to clear
				m_array->SetVertexCount(m_vertexnb);
			}
		}
		// Finalize also destroys the object, this will also delete m_soft2vertex
		delete this;
	}

protected:
	RAS_IDisplayArray *m_array;
	btSoftBody *m_softBody;
	std::vector<int> *m_soft2vertex;
	btAlignedObjectArray<int> m_endNodes;
	int m_uvsize;
	int m_rgbsize;
	unsigned int m_vertexnb;

	~KX_SoftBodyRefineCallback()
	{
		delete m_soft2vertex;
	}

};

PHY_IRefineCallback* KX_SoftBodyDeformer::GetRefineCallback()
{
	CcdPhysicsController *ctrl = (CcdPhysicsController *)m_gameobj->GetPhysicsController();
	if (!ctrl)
		return nullptr;

	btSoftBody *softBody = ctrl->GetSoftBody();
	if (!softBody)
		return nullptr;

	// TODO: must process all the mesh slots of the object
	RAS_MeshObject *mesh = m_gameobj->GetMesh(0);
	if (!mesh)
		return nullptr;
	RAS_MeshMaterial *meshmat = mesh->GetMeshMaterial(0);
	if (!meshmat)
		return nullptr;
	RAS_MeshSlot *slot = meshmat->m_slots[(void *)m_gameobj->getClientInfo()];
	if (!slot)
		return nullptr;

	// lookup table to go from softbody index to mesh vertex
	// Note: This assume a one-to-one relation between softbody nodes and mesh vertices
	//       It is not the case if the graphics mesh is using flat face model
	//       Flat face is normally disabled when the soft body is converted.
	RAS_IDisplayArray *array = slot->GetDisplayArray();
	btSoftBody::tNodeArray&   nodes(softBody->m_nodes);

	if (array->GetPrimitiveType() != RAS_IDisplayArray::TRIANGLES)
		return nullptr;

	int nodesize = nodes.size();
	std::vector<int> *soft2vertex = new std::vector<int>();
	soft2vertex->reserve(nodesize+256);
	soft2vertex->resize(nodesize,-1);
	unsigned int i, size=array->GetVertexCount();

	for (i = 0; i < size; ++i) {
		const RAS_TexVertInfo& vinfo = array->GetVertexInfo(i);
		short softindex = vinfo.getSoftBodyIndex();
		if (softindex < 0 || softindex >= nodesize)
			// should not happen
			break;
		if (soft2vertex->at(softindex) != -1)
			// multiple vertex per soft node, no support
			break;
		soft2vertex->at(softindex) = i;
	}
	if (i < size)
	{
		delete soft2vertex;
		return nullptr;
	}

	PHY_IRefineCallback *cb = new KX_SoftBodyRefineCallback(array, softBody, soft2vertex);
	if (!cb)
		delete soft2vertex;
	// soft2vertex will be deleted when cb is finalized
	return cb;
}

bool KX_SoftBodyDeformer::GetNodePosition(int idx, float pos[3])
{
	CcdPhysicsController *ctrl = (CcdPhysicsController *)m_gameobj->GetPhysicsController();
	if (!ctrl)
		return false;

	btSoftBody *softBody = ctrl->GetSoftBody();
	if (!softBody)
		return false;

	if (idx < 0 ||  idx >= softBody->m_nodes.size())
		return false;

	const btVector3& x=softBody->m_nodes[idx].m_x;
	pos[0] = x.getX();
	pos[1] = x.getY();
	pos[2] = x.getZ();

	return true;
}


#endif
