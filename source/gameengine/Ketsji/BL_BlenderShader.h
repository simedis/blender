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
 * The Original Code is Copyright (C) Blender Foundation
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file BL_BlenderShader.h
 *  \ingroup ketsji
 */

#ifndef __BL_BLENDERSHADER_H__
#define __BL_BLENDERSHADER_H__

#include "RAS_Rasterizer.h"
#include "RAS_MeshObject.h"
#include "RAS_Texture.h" // for MaxUnits
#include <string>

struct Material;
struct Scene;
struct GPUMaterial;
class KX_Scene;
class RAS_MeshSlot;

/**
 * BL_BlenderShader
 * Blender GPU shader material
 */
class BL_BlenderShader
{
private:
	Scene *m_blenderScene;
	Material *m_mat;
	int m_lightLayer;
	int m_alphaBlend;
	GPUMaterial *m_GPUMat;

	/// The material attributes passed to the rasterizer.
	RAS_Rasterizer::TexCoGenList m_attribs;

	/// Generate the material atrributes onto m_attibs.
	void ParseAttribs();

public:
	BL_BlenderShader(KX_Scene *scene, Material *ma, int lightlayer);
	virtual ~BL_BlenderShader();

	bool Ok() const
	{
		return (m_GPUMat != nullptr);
	}
	void SetProg(bool enable, double time = 0.0, RAS_Rasterizer *rasty = nullptr);

	int GetAttribNum() const;
	void SetAttribs(RAS_Rasterizer *ras);

	/** Return a map of the corresponding attribut layer for a given attribut index.
	 * \param layers The list of the mesh layers used to link with uv and color material attributes.
	 * \return The map of attributes layers.
	 */
	const RAS_Rasterizer::AttribLayerList GetAttribLayers(const RAS_MeshObject::LayersInfo& layersInfo) const;

	void Update(RAS_MeshSlot *ms, RAS_Rasterizer * rasty);

	/// Return true if the shader uses a special vertex shader for geometry instancing.
	bool UseInstancing() const;
	void ActivateInstancing(void *matrixoffset, void *positionoffset, void *coloroffset, unsigned int stride);
	void DesactivateInstancing();

	void Reload();
	int GetAlphaBlend();
};

#endif /* __BL_BLENDERSHADER_H__ */
