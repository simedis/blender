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
 * Contributor(s): Tristan Porteries.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file RAS_BatchGroup.h
 *  \ingroup bgerast
 */

#ifndef __RAS_BATCH_GROUP_H__
#define __RAS_BATCH_GROUP_H__

#include "RAS_DisplayArrayBucket.h"
#include "MT_Matrix4x4.h"

class RAS_IDisplayArrayBatching;

class RAS_BatchGroup
{
private:
	std::map<RAS_MeshSlot *, RAS_DisplayArrayBucket *> m_originalDisplayArrayBucketList;

	struct Batch
	{
		RAS_DisplayArrayBucket *m_displayArrayBucket;
		RAS_IDisplayArrayBatching *m_displayArray;
	};

	std::map<RAS_IPolyMaterial *, Batch> m_batchs;

	bool Merge(Batch& batch, RAS_MeshSlot *slot, const MT_Matrix4x4& mat);
	bool Split(RAS_MeshSlot *slot);

public:
	RAS_BatchGroup();
	virtual ~RAS_BatchGroup();

	bool Merge(RAS_MeshUser *meshUser, const MT_Matrix4x4& mat);
	bool Split(RAS_MeshUser *meshUser);
};


#endif  // __RAS_BATCH_GROUP_H__
