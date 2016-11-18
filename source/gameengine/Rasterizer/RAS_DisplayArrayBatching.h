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
#include "CM_Message.h"

/// An array with data used for OpenGL drawing
template <class Vertex>
class RAS_DisplayArrayBatching : public RAS_DisplayArray<Vertex>, public RAS_IDisplayArrayBatching
{
protected:
	using RAS_DisplayArray<Vertex>::m_vertexes;
	using RAS_DisplayArray<Vertex>::m_indices;

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

	virtual RAS_IDisplayArray *GetReplica()
	{
		/* A batch display array must never be replicated.
		 * Display arrays are replicated when a deformer is used for a mesh slot
		 * but batch display array are not used in the case of deformer.
		 */
		BLI_assert(false);
		return NULL;
	}

	/** Merge array in the batching array.
	 * \param iarray The array to merge, must be of the same vertex format than the
	 * batching array.
	 * \param mat The transformation to apply on each vertex in the merging.
	 */
	virtual unsigned int Merge(RAS_IDisplayArray *iarray, const MT_Matrix4x4& mat)
	{
		RAS_DisplayArray<Vertex> *array = dynamic_cast<RAS_DisplayArray<Vertex> *>(iarray);
		const unsigned int vertexcount = iarray->GetVertexCount();
		const unsigned int indexcount = iarray->GetIndexCount();

		const unsigned int startvertex = m_vertexes.size();
		const unsigned int startindex = m_indices.size();

		// Add the array start index and count.
		Part part;
		part.m_startVertex = startvertex;
		part.m_vertexCount = vertexcount;
		part.m_startIndex = startindex;
		part.m_indexCount = indexcount;
		part.m_indexOffset = (void *)(part.m_startIndex * sizeof(unsigned int));
		m_parts.push_back(part);

		m_vertexes.reserve(startvertex + vertexcount);
		m_indices.reserve(startindex + indexcount);

		CM_Debug("Add part : " << (m_parts.size() - 1) << ", start index: " << startindex << ", index count: " << indexcount << ", start vertex: " << startvertex << ", vertex count: " << vertexcount);

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
			m_indices.push_back(startvertex + *it);
		}

		RAS_DisplayArray<Vertex>::UpdateCache();

		return (m_parts.size() - 1);
	}

	virtual void Split(unsigned int partIndex)
	{
		const Part &part = m_parts[partIndex];

		const unsigned int startindex = part.m_startIndex;
		const unsigned int startvertex = part.m_startVertex;

		const unsigned int indexcount = part.m_indexCount;
		const unsigned int vertexcount = part.m_vertexCount;

		const unsigned int endvertex = startvertex + vertexcount;

		CM_Debug("Move indices from " << startindex << " to " << m_indices.size() - indexcount << ", shift of " << indexcount);
		for (unsigned int i = startindex, size = m_indices.size() - indexcount; i < size; ++i) {
			m_indices[i] = m_indices[i + indexcount] - vertexcount;
		}

		m_indices.erase(m_indices.end() - indexcount, m_indices.end());

		CM_Debug("Remove vertexes : start vertex: " << startvertex << ", end vertex: " << endvertex);
		m_vertexes.erase(m_vertexes.begin() + startvertex, m_vertexes.begin() + endvertex);

		for (unsigned i = partIndex + 1, size = m_parts.size(); i < size; ++i) {
			Part& nextPart = m_parts[i];
			nextPart.m_startVertex -= vertexcount;
			nextPart.m_startIndex -= indexcount;
			nextPart.m_indexOffset = (void *)(nextPart.m_startIndex * sizeof(unsigned int));
		}

		m_parts.erase(m_parts.begin() + partIndex);

		UpdateCache();
	}
};

#endif  // __RAS_DISPLAY_ARRAY_BATCHING_H__
