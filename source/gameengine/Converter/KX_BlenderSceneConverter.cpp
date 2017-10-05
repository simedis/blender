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

/** \file gameengine/Converter/KX_BlenderSceneConverter.cpp
 *  \ingroup bgeconv
 */

#include "KX_BlenderSceneConverter.h"
#include "KX_GameObject.h"

void KX_BlenderSceneConverter::RegisterGameObject(KX_GameObject *gameobject, Object *for_blenderobject)
{
// 	CM_FunctionDebug("object name: " << gameobject->GetName());
	// only maintained while converting, freed during game runtime
	m_map_blender_to_gameobject[for_blenderobject] = gameobject;
}

/** only need to run this during conversion since
 * m_map_blender_to_gameobject is freed after conversion */
void KX_BlenderSceneConverter::UnregisterGameObject(KX_GameObject *gameobject)
{
	Object *bobp = gameobject->GetBlenderObject();
	if (bobp) {
		std::map<Object *, KX_GameObject *>::iterator it = m_map_blender_to_gameobject.find(bobp);
		if (it->second == gameobject) {
			// also maintain m_map_blender_to_gameobject if the gameobject
			// being removed is matching the blender object
			m_map_blender_to_gameobject.erase(it);
		}
	}
}

KX_GameObject *KX_BlenderSceneConverter::FindGameObject(Object *for_blenderobject)
{
	return m_map_blender_to_gameobject[for_blenderobject];
}

void KX_BlenderSceneConverter::RegisterGameMesh(RAS_MeshObject *gamemesh, Mesh *for_blendermesh)
{
	if (for_blendermesh) { // dynamically loaded meshes we don't want to keep lookups for
		m_map_mesh_to_gamemesh[for_blendermesh] = gamemesh;
	}
	m_meshobjects.push_back(gamemesh);
}

RAS_MeshObject *KX_BlenderSceneConverter::FindGameMesh(Mesh *for_blendermesh)
{
	return m_map_mesh_to_gamemesh[for_blendermesh];
}

void KX_BlenderSceneConverter::RegisterMaterial(KX_BlenderMaterial *blmat, Material *mat)
{
	if (mat) {
		m_map_mesh_to_polyaterial[mat] = blmat;
	}
	m_materials.push_back(blmat);
}

KX_BlenderMaterial *KX_BlenderSceneConverter::FindMaterial(Material *mat)
{
	return m_map_mesh_to_polyaterial[mat];
}

void KX_BlenderSceneConverter::RegisterGameActuator(SCA_IActuator *act, bActuator *for_actuator)
{
	m_map_blender_to_gameactuator[for_actuator] = act;
}

SCA_IActuator *KX_BlenderSceneConverter::FindGameActuator(bActuator *for_actuator)
{
	return m_map_blender_to_gameactuator[for_actuator];
}

void KX_BlenderSceneConverter::RegisterGameController(SCA_IController *cont, bController *for_controller)
{
	m_map_blender_to_gamecontroller[for_controller] = cont;
}

SCA_IController *KX_BlenderSceneConverter::FindGameController(bController *for_controller)
{
	return m_map_blender_to_gamecontroller[for_controller];
}
