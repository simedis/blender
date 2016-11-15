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

/** \file gameengine/Rasterizer/RAS_BatchGroup.cpp
 *  \ingroup bgerast
 */

#include "RAS_BatchGroup.h"
#include "RAS_IDisplayArrayBatching.h"
#include "RAS_MaterialBucket.h"
#include "RAS_MeshUser.h"
#include "RAS_MeshSlot.h"

#include "CM_Message.h"

#include <algorithm>

RAS_BatchGroup::RAS_BatchGroup()
	:m_users(0)
{
	CM_Debug("New batch group");
}

RAS_BatchGroup::~RAS_BatchGroup()
{
	for (std::map<RAS_IPolyMaterial *, Batch>::iterator it = m_batchs.begin(), end = m_batchs.end(); it != end; ++it) {
		Batch& batch = it->second;
		batch.m_displayArrayBucket->Release();
	}

	CM_Debug("Delete batch group");
}

RAS_BatchGroup *RAS_BatchGroup::AddMeshUser(RAS_MeshUser *UNUSED(meshUser))
{
	++m_users;
	return this;
}

RAS_BatchGroup *RAS_BatchGroup::RemoveMeshUser(RAS_MeshUser *meshUser)
{
	SplitMeshUser(meshUser);

	--m_users;
	if (m_users == 0) {
		delete this;
		return NULL;
	}
	return this;
}

bool RAS_BatchGroup::MergeMeshSlot(RAS_BatchGroup::Batch& batch, RAS_MeshSlot *slot, const MT_Matrix4x4& mat)
{
	RAS_DisplayArrayBucket *origArrayBucket = slot->m_displayArrayBucket;
	RAS_IDisplayArray *origArray = origArrayBucket->GetDisplayArray();
	RAS_DisplayArrayBucket *arrayBucket = batch.m_displayArrayBucket;
	RAS_IDisplayArrayBatching *array = batch.m_displayArray;

	// Don't merge if the vertex format or pimitive type is not the same.
	if (origArray->GetFormat() != array->GetFormat() || origArray->GetPrimitiveType() != array->GetPrimitiveType()) {
		CM_Error("failed merge ")
		return false;
	}

	// Store original display array bucket.
	batch.m_originalDisplayArrayBucketList[slot] = origArrayBucket->AddRef();
	batch.m_meshSlots.push_back(slot);

	// Merge display array.
	const unsigned int index = array->Merge(origArray, mat);
	slot->m_batchPartIndex = index;

	arrayBucket->DestructStorageInfo();

	slot->SetDisplayArrayBucket(arrayBucket);
	arrayBucket->AddRef();

	return true;
}

bool RAS_BatchGroup::SplitMeshSlot(RAS_MeshSlot *slot)
{
	RAS_IPolyMaterial *material = slot->m_bucket->GetPolyMaterial();

	std::map<RAS_IPolyMaterial *, Batch>::iterator bit = m_batchs.find(material);
	if (bit == m_batchs.end()) {
		return false;
	}

	Batch& batch = bit->second;

	RAS_DisplayArrayBucket *origArrayBucket = batch.m_originalDisplayArrayBucketList[slot];

	if (!origArrayBucket) {
		return false;
	}

	slot->SetDisplayArrayBucket(origArrayBucket->AddRef());
	origArrayBucket->Release();

	batch.m_displayArray->Split(slot->m_batchPartIndex);

	batch.m_displayArrayBucket->DestructStorageInfo();

	batch.m_originalDisplayArrayBucketList.erase(slot);

	RAS_MeshSlotList::iterator mit = batch.m_meshSlots.erase(std::find(batch.m_meshSlots.begin(), batch.m_meshSlots.end(), slot));
	for (RAS_MeshSlotList::iterator it = mit, end = batch.m_meshSlots.end(); it != end; ++it) {
		RAS_MeshSlot *meshSlot = *it;
		--meshSlot->m_batchPartIndex;
	}

	return true;
}

bool RAS_BatchGroup::MergeMeshUser(RAS_MeshUser *meshUser, const MT_Matrix4x4& mat)
{
	const RAS_MeshSlotList& meshSlots = meshUser->GetMeshSlots();
	for (RAS_MeshSlotList::const_iterator it = meshSlots.begin(), end = meshSlots.end(); it != end; ++it) {
		RAS_MeshSlot *meshSlot = *it;
		RAS_IPolyMaterial *material = meshSlot->m_bucket->GetPolyMaterial();

		Batch& batch = m_batchs[material];
		// Create the batch if it is empty.
		if (!batch.m_displayArray && !batch.m_displayArrayBucket) {
			RAS_IDisplayArray *origarray = meshSlot->GetDisplayArray();
			batch.m_displayArray = RAS_IDisplayArrayBatching::ConstructArray(origarray->GetPrimitiveType(), origarray->GetFormat());
			batch.m_displayArrayBucket = new RAS_DisplayArrayBucket(meshSlot->m_bucket, batch.m_displayArray,
																	meshSlot->m_mesh, meshSlot->m_meshMaterial);
			CM_Debug("Created batching array: " << batch.m_displayArray << ", for material: " << material);
		}
		else {
			CM_Debug("Reuse batching array: " << batch.m_displayArray << ", for material: " << material);
		}

		if (!MergeMeshSlot(batch, meshSlot, mat)) {
			return false;
		}
	}

	meshUser->SetBatchGroup(this);

	return true;
}

bool RAS_BatchGroup::SplitMeshUser(RAS_MeshUser *meshUser)
{
	const RAS_MeshSlotList& meshSlots = meshUser->GetMeshSlots();
	for (RAS_MeshSlotList::const_iterator it = meshSlots.begin(), end = meshSlots.end(); it != end; ++it) {
		RAS_MeshSlot *meshSlot = *it;
		if (!SplitMeshSlot(meshSlot)) {
			return false;
		}
	}

	return true;
}
