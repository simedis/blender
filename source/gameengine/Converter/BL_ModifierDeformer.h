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

/** \file BL_ModifierDeformer.h
 *  \ingroup bgeconv
 */

#ifndef __BL_MODIFIERDEFORMER_H__
#define __BL_MODIFIERDEFORMER_H__

#ifdef _MSC_VER
#  pragma warning (disable:4786)  /* get rid of stupid stl-visual compiler debug warning */
#endif

#include "BL_ShapeDeformer.h"
#include "BL_DeformableGameObject.h"
#include "BL_LatticeObject.h"
#include <vector>

class RAS_MeshObject;
struct DerivedMesh;
struct Object;

class BL_ModifierDeformer : public BL_ShapeDeformer
{
public:
	static bool HasCompatibleDeformer(Object *ob);
	static bool HasArmatureDeformer(Object *ob);
	static Object *GetLatticeDeformer(Object *obj);

	BL_ModifierDeformer(BL_DeformableGameObject *gameobj,
						Scene *scene,
						Object *bmeshobj,
						RAS_MeshObject *mesh);

	/* this second constructor is needed for making a mesh deformable on the fly. */
	BL_ModifierDeformer(BL_DeformableGameObject *gameobj,
						Scene *scene,
						Object *bmeshobj_old,
						Object *bmeshobj_new,
						RAS_MeshObject *mesh,
						BL_ArmatureObject *arma = nullptr,
						BL_LatticeObject *lattice = nullptr);

	void SetLattice(BL_LatticeObject *latticeObj);

	virtual void Relink(std::map<SCA_IObject *, SCA_IObject *>& map);
	virtual void ProcessReplica();
	virtual RAS_Deformer *GetReplica();
	virtual bool UnlinkObject(SCA_IObject* clientobj);
	virtual ~BL_ModifierDeformer();

	bool Update();
	bool UpdateInternal(bool shape_applied);
	virtual bool Apply(RAS_IPolyMaterial *polymat, RAS_MeshMaterial *meshmat);
	void ForceUpdate()
	{
		m_lastModifierUpdate = -1.0;
	}
	virtual DerivedMesh *GetFinalMesh()
	{
		return m_dm;
	}
	// The derived mesh returned by this function must be released!
	virtual DerivedMesh *GetPhysicsMesh();

	bool LatticeUpdated()
	{
		if (m_latticeObj &&
		    ((m_gameobj->GetParent() != m_latticeObj && m_lastLatticeUpdate < m_latticeObj->GetLastFrameIPO()) ||
		     (m_lastLatticeUpdate < m_latticeObj->GetLastFrameAction())))
		{
			m_bDynamic = true;
			return true;
		}
		return false;
	}
	virtual bool IsDependent()
	{
		if (m_latticeObj) {
			return true;
		}
		return BL_SkinDeformer::IsDependent();
	}
	virtual void AddAnimatedParent()
	{
		if (m_latticeObj)
			m_latticeObj->EnsureAnimated();
		BL_SkinDeformer::AddAnimatedParent();
	}

	BL_LatticeObject *GetLatticeObject()
	{
		return m_latticeObj;
	}

protected:
	double m_lastModifierUpdate;
	Scene *m_scene;
	DerivedMesh *m_dm;
	BL_LatticeObject *m_latticeObj;
	double m_lastLatticeUpdate;
};

#endif  /* __BL_MODIFIERDEFORMER_H__ */
