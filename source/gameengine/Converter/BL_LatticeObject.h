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

/** \file BL_DeformableGameObject.h
 *  \ingroup bgeconv
 */

#ifndef __BL_LATTICEOBJECT_H__
#define __BL_LATTICEOBJECT_H__

#ifdef _MSC_VER
#  pragma warning (disable:4786) // get rid of stupid stl-visual compiler debug warning
#endif

#include "BL_DeformableGameObject.h"

class BL_LatticeObject : public BL_DeformableGameObject
{
public:
	BL_LatticeObject(Object* blendobj, void* sgReplicationInfo, SG_Callbacks callbacks);
	virtual ~BL_LatticeObject();
	virtual int GetGameObjectType() const;
	virtual CValue*	GetReplica();
	virtual void ProcessReplica();
	virtual bool GetShape(std::vector<float> &shape);
	// These functions must always be called in pair
	// Replace lattice cache with given vertices.
	// To be called just before computing the modifiers on the target object
	void SwapLatticeCache();
	// Restore lattice cache
	void RestoreLatticeCache();

protected:
	// to temporarily store the lattice vertex cache during deformation
	float (*m_vertCache)[3];
	// Store the original lattice object matrix.
	float m_obmat[4][4];

#ifdef WITH_CXX_GUARDEDALLOC
	MEM_CXX_CLASS_ALLOC_FUNCS("GE:BL_LatticeObject")
#endif

};
#endif // __BL_LATTICEOBJECT_H__
