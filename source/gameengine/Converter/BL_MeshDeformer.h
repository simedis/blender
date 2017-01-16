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

/** \file BL_MeshDeformer.h
 *  \ingroup bgeconv
 */

#ifndef __BL_MESHDEFORMER_H__
#define __BL_MESHDEFORMER_H__

#include "RAS_Deformer.h"
#include "BL_DeformableGameObject.h"
#include "DNA_object_types.h"
#include "DNA_key_types.h"
#include "MT_Vector3.h"
#include <stdlib.h>

#ifdef _MSC_VER
#  pragma warning (disable:4786)  /* get rid of stupid stl-visual compiler debug warning */
#endif

struct Object;
struct Mesh;
struct Lattice;
class SCA_IObject;
class BL_DeformableGameObject;
class RAS_MeshObject;
class RAS_IPolyMaterial;

class BL_MeshDeformer : public RAS_Deformer
{
public:
	void VerifyStorage();
	void RecalcNormals();
	virtual void Relink(std::map<void *, void *>& map);
	virtual SCA_IObject* GetParent()
	{
		return (SCA_IObject*)m_gameobj;
	}

	BL_MeshDeformer(BL_DeformableGameObject *gameobj, Object *obj, RAS_MeshObject *meshobj);
	virtual ~BL_MeshDeformer();
	virtual void SetSimulatedTime(double time)
	{
	}
	virtual bool Apply(RAS_IPolyMaterial *polymat, RAS_MeshMaterial *meshmat);
	virtual bool Update()
	{
		return false;
	}
	virtual bool UpdateBuckets()
	{
		return false;
	}
	virtual RAS_Deformer *GetReplica()
	{
		return NULL;
	}
	virtual void ProcessReplica();
	Mesh *GetMesh()
	{
		return m_bmesh;
	}
	virtual RAS_MeshObject *GetRasMesh()
	{
		return m_mesh;
	}
	virtual float(*GetTransVerts(int *tot))[3]
	{
		*tot = m_tvtot; return m_transverts;
	}

protected:
	Mesh *m_bmesh;
	Lattice *m_lattice;
	int m_totvert;

	// this is so m_transverts doesn't need to be converted
	// before deformation
	float (*m_transverts)[3];
	float (*m_transnors)[3];
	Object *m_objMesh;

	int m_tvtot;
	BL_DeformableGameObject *m_gameobj;
	double m_lastDeformUpdate;

#ifdef WITH_CXX_GUARDEDALLOC
	MEM_CXX_CLASS_ALLOC_FUNCS("GE:BL_MeshDeformer")
#endif
};

#endif

