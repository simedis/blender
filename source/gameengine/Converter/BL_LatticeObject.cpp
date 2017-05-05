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

/** \file gameengine/Converter/BL_DeformableGameObject.cpp
 *  \ingroup bgeconv
 */

#include <map>

#include "BL_ShapeDeformer.h"
#include "BL_LatticeObject.h"

extern "C" {
	#include "BKE_lattice.h"
	#include "BLI_threads.h"
	#include "BLI_utildefines.h"
	#include "DNA_key_types.h"
}

// internal class for managing multi-threading during deformation
class RefCountThreadMutex
{
public:
	RefCountThreadMutex()
	{
		m_mutex = BLI_mutex_alloc();
		m_ref = 1;
	}
	~RefCountThreadMutex()
	{
		BLI_mutex_free(m_mutex);
	}
	int Release()
	{
		return --m_ref;
	}
	void AddRef()
	{
		m_ref++;
	}
	void Lock()
	{
		BLI_mutex_lock(m_mutex);
	}
	void Unlock()
	{
		BLI_mutex_unlock(m_mutex);
	}

protected:
	int m_ref;
	ThreadMutex *m_mutex;
};

static std::map<Object *, RefCountThreadMutex *> s_latticeLockMap;


BL_LatticeObject::BL_LatticeObject(Object* blendobj, void* sgReplicationInfo, SG_Callbacks callbacks) :
	BL_DeformableGameObject(blendobj,sgReplicationInfo,callbacks)
{
	m_vertCache = NULL;
	std::pair<std::map<Object *, RefCountThreadMutex *>::iterator, bool> ret =
	        s_latticeLockMap.insert(std::pair<Object *, RefCountThreadMutex *>(blendobj, NULL));
	if (!ret.second)
	{
		// the object is already in the map increase the reference count
	    ret.first->second->AddRef();
	}
	else
	{
		// the object was inserted, create the lock
		ret.first->second = new RefCountThreadMutex();
	}
	// remember the initial object matrix
	memcpy(m_obmat, m_blendobj->obmat, sizeof(m_obmat));
}

BL_LatticeObject::~BL_LatticeObject()
{
	BLI_assert(m_vertCache == NULL);
	if (s_latticeLockMap[m_blendobj]->Release() == 0)
	{
		delete s_latticeLockMap[m_blendobj];
		s_latticeLockMap.erase(m_blendobj);
	}
}

void BL_LatticeObject::ProcessReplica()
{
	BL_DeformableGameObject::ProcessReplica();
	m_vertCache = NULL;
	s_latticeLockMap[m_blendobj]->AddRef();
}

CValue*	BL_LatticeObject::GetReplica()
{
	BL_LatticeObject* replica = new BL_LatticeObject(*this);
	replica->ProcessReplica();
	return replica;
}

bool BL_LatticeObject::GetShape(std::vector<float> &shape)
{
	shape.clear();
	BL_ShapeDeformer* shape_deformer = dynamic_cast<BL_ShapeDeformer*>(m_pDeformer);
	if (shape_deformer)
	{
		Key* key = shape_deformer->GetKey();
		if (key && key->type==KEY_RELATIVE)
		{
			KeyBlock *kb;
			for (kb = (KeyBlock *)key->block.first; kb; kb = (KeyBlock *)kb->next)
			{
				shape.push_back(kb->curval);
			}
		}
	}
	return !shape.empty();
}

void BL_LatticeObject::SwapLatticeCache()
{
	BL_MeshDeformer* deformer = dynamic_cast<BL_MeshDeformer*>(m_pDeformer);

	// SwapLatticeCache and RestoreLatticeCache are always called in pairs,
	// m_vertCache must be NULL
	BLI_assert(m_vertCache == NULL);

	// carefull! this code is threaded but the blender object is common resource
	// use a lock to be sure that no other lattice deformer is trying change the object at the same time
	s_latticeLockMap[m_blendobj]->Lock();
	if (deformer)
	{
		int totvert;
		float (*transverts)[3] = deformer->GetTransVerts(&totvert);
		if (transverts) {
			m_vertCache = BKE_lattice_cache_vertexcos_swap(m_blendobj, transverts);
		}
	}
	// no deformer but the object could still have an animation
	UpdateBlenderObjectMatrix(m_blendobj);
}

void BL_LatticeObject::RestoreLatticeCache()
{
	if (m_vertCache)
	{
		BKE_lattice_cache_vertexcos_swap(m_blendobj, m_vertCache);
		m_vertCache = NULL;
	}
	memcpy(m_blendobj->obmat, m_obmat, sizeof(m_obmat));
	s_latticeLockMap[m_blendobj]->Unlock();
}


int BL_LatticeObject::GetGameObjectType() const
{
	return OBJ_LATTICE;
}
