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

/** \file RAS_DisplayArrayBatching.h
 *  \ingroup bgerast
 */

#ifndef __RAS_DISPLAY_ARRAY_BATCHING_H__
#define __RAS_DISPLAY_ARRAY_BATCHING_H__

#include "RAS_DisplayArray.h"
#include "RAS_IDisplayArrayBatching.h"

/// An array with data used for OpenGL drawing
template <class Vertex>
class RAS_DisplayArrayBatching : public RAS_DisplayArray<Vertex>, public RAS_IDisplayArrayBatching
{
protected:
	using RAS_DisplayArray<Vertex>::m_vertexes;
	using RAS_DisplayArray<Vertex>::m_indices;
// 	using RAS_IDisplayArray::m_indices;

public:
	RAS_DisplayArrayBatching(RAS_IDisplayArray::PrimitiveType type, const RAS_TexVertFormat& format)
		:RAS_IDisplayArray(type, format),
		RAS_DisplayArray<Vertex>(type, format),
		RAS_IDisplayArrayBatching(type, format)
	{
	}

	virtual ~RAS_DisplayArrayBatching()
	{
	}

	/** Merge array in the batching array.
	 * \param iarray The array to merge, must be of the same vertex format than the
	 * batching array.
	 * \param mat The transformation to apply on each vertex in the merging.
	 */
	virtual void Merge(RAS_IDisplayArray *iarray, const MT_Matrix4x4& mat)
	{
		RAS_DisplayArray<Vertex> *array = dynamic_cast<RAS_DisplayArray<Vertex> *>(iarray);
		const unsigned int vertexcount = iarray->GetVertexCount();
		const unsigned int indexcount = iarray->GetIndexCount();

		const unsigned int startvertex = m_vertexes.size();
		const unsigned int startindex = m_indices.size();

		// Add the array start index and count.
		m_indicesArray.push_back(startindex);
		m_counts.push_back(indexcount);

		m_vertexes.reserve(startvertex + vertexcount);
		m_indices.reserve(startindex + indexcount);

		// Normal and tangent matrix.
		MT_Matrix4x4 nmat = mat.inverse().transposed();
		nmat[0][3] = nmat[1][3] = nmat[2][3] = 0.0f;

		for (typename std::vector<Vertex>::iterator it = array->m_vertexes.begin(), end = array->m_vertexes.end(); it != end; ++it) {
			// Copy the vertex.
			Vertex vert = *it;
			vert.Transform(mat, nmat);
			m_vertexes.push_back(vert);
		}

		for (std::vector<unsigned int>::iterator it = array->m_indices.begin(), end = array->m_indices.end(); it != end; ++it) {
			m_indices.push_back(startindex + *it);
		}

		RAS_DisplayArray<Vertex>::UpdateCache();
	}
};

#endif  // __RAS_DISPLAY_ARRAY_BATCHING_H__
