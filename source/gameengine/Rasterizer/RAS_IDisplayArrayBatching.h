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

/** \file RAS_IDisplayArrayBatching.h
 *  \ingroup bgerast
 */

#ifndef __RAS_IDISPLAY_ARRAY_BATCHING_H__
#define __RAS_IDISPLAY_ARRAY_BATCHING_H__

#include "RAS_IDisplayArray.h"

class RAS_IDisplayArrayBatching : public virtual RAS_IDisplayArray
{
protected:
	std::vector<unsigned int> m_indicesArray;
	std::vector<unsigned int> m_counts;

public:
	RAS_IDisplayArrayBatching(PrimitiveType type, const RAS_TexVertFormat &format);
	virtual ~RAS_IDisplayArrayBatching();

	/** Construct the display array batching corresponding of the vertex of the given format.
	 * \param type The type of primitives, one of the enumeration PrimitiveType.
	 * \param format The format of vertex to use.
	 */
	static RAS_IDisplayArrayBatching *ConstructArray(PrimitiveType type, const RAS_TexVertFormat &format);

	inline unsigned int GetArrayIndice(const unsigned short index)
	{
		return m_indicesArray[index];
	}

	inline unsigned int GetArrayCount(const unsigned short index)
	{
		return m_counts[index];
	}

	virtual void Merge(RAS_IDisplayArray *iarray, const MT_Matrix4x4& mat) = 0;

	virtual Type GetType() const;
};

#endif  // __RAS_IDISPLAY_ARRAY_BATCHING_H__
