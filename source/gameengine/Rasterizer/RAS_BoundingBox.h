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

/** \file RAS_BoundingBox.h
 *  \ingroup bgerast
 */

#ifndef __RAS_BOUNDING_BOX_H__
#define __RAS_BOUNDING_BOX_H__

#include "RAS_IDisplayArray.h"
#include "MT_Vector3.h"

class RAS_BoundingBoxManager;

class RAS_BoundingBox
{
protected:
	/// True when the bounding box is modified.
	bool m_modified;

	/// The AABB minimum.
	MT_Vector3 m_aabbMin;
	/// The AABB maximum.
	MT_Vector3 m_aabbMax;

	/// The number of mesh user using this bounding box.
	int m_users;
	/// The manager of all the bounding boxes of a scene.
	RAS_BoundingBoxManager *m_manager;

public:
	RAS_BoundingBox(RAS_BoundingBoxManager *manager);
	virtual ~RAS_BoundingBox();

	virtual RAS_BoundingBox *GetReplica();
	void ProcessReplica();

	/// Notice that the bounding box is used by one more mesh user.
	void AddUser();
	/// Notice that the bounding box is left by one mesh user.
	void RemoveUser();

	/// Change the bounding box manager. Used only for the libloading scene merge.
	void SetManager(RAS_BoundingBoxManager *manager);

	/** Return true when the bounding box AABB was set or when the display
	 * array were modified in case of RAS_MeshBoundingBox instance.
	 */
	bool GetModified() const;
	/// Set the bounding box unmodified.
	void ClearModified();

	void GetAabb(MT_Vector3& aabbMin, MT_Vector3& aabbMax) const;
	void SetAabb(const MT_Vector3& aabbMin, const MT_Vector3& aabbMax);
	/// Compute the AABB of the bounding box AABB mixed with the passed AABB.
	void ExtendAabb(const MT_Vector3& aabbMin, const MT_Vector3& aabbMax);

	void CopyAabb(RAS_BoundingBox *other);

	virtual void Update(bool force);
};

class RAS_MeshBoundingBox : public RAS_BoundingBox
{
private:
	/// The display arrays used to compute the AABB.
	RAS_IDisplayArrayList m_displayArrayList;

public:
	RAS_MeshBoundingBox(RAS_BoundingBoxManager *manager, const RAS_IDisplayArrayList displayArrayList);
	virtual ~RAS_MeshBoundingBox();

	virtual RAS_BoundingBox *GetReplica();

	/** Check if one of the display array was modified, and then recompute the AABB.
	 * \param force Force the AABB computation even if none display arrays are modified.
	 */
	virtual void Update(bool force);
};

typedef std::vector<RAS_BoundingBox *> RAS_BoundingBoxList;

#endif  // __RAS_BOUNDING_BOX_H__
