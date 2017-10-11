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
 * Contributor(s): Mitchell Stokes
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file KX_LibLoadStatus.h
 *  \ingroup bgeconv
 */

#ifndef __KX_LIBLOADSTATUS_H__
#define __KX_LIBLOADSTATUS_H__

#include "EXP_PyObjectPlus.h"
#include "BL_BlenderSceneConverter.h"
#include "BL_BlenderConverter.h"

class BL_BlenderConverter;
class KX_KetsjiEngine;
class KX_Scene;

class KX_LibLoadStatus : public PyObjectPlus
{
	Py_Header
private:
	BL_BlenderConverter *m_converter;
	KX_KetsjiEngine *m_engine;
	KX_Scene *m_mergescene;
	std::vector<Scene *> m_blenderScenes;
	std::vector<BL_BlenderSceneConverter> m_sceneConvertes;
	std::string m_libname;
	BL_BlenderConverter::LibLoadOptions m_options;

	float	m_progress;
	double	m_starttime;
	double	m_endtime;

	// The current status of this libload, used by the scene converter.
	bool m_finished;

#ifdef WITH_PYTHON
	PyObject*	m_finish_cb;
	PyObject*	m_progress_cb;
#endif

public:
	KX_LibLoadStatus(class BL_BlenderConverter* kx_converter,
						class KX_KetsjiEngine* kx_engine,
						class KX_Scene* merge_scene,
						const std::string& path);

	void Finish(); // Called when the libload is done
	void RunFinishCallback();
	void RunProgressCallback();

	class BL_BlenderConverter *GetConverter();
	class KX_KetsjiEngine *GetEngine();
	class KX_Scene *GetMergeScene();
	BL_BlenderConverter::LibLoadOptions GetOptions() const;

	const std::vector<Scene *>& GetBlenderScenes() const;
	void SetBlenderScenes(const std::vector<Scene *>& scenes);
	const std::vector<BL_BlenderSceneConverter>& GetSceneConverters() const;
	void AddSceneConverter(BL_BlenderSceneConverter&& converter);

	inline bool IsFinished() const
	{
		return m_finished;
	}

	void SetProgress(float progress);
	float GetProgress();
	void AddProgress(float progress);

#ifdef WITH_PYTHON
	static PyObject*	pyattr_get_onfinish(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int			pyattr_set_onfinish(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);
	static PyObject*	pyattr_get_onprogress(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static int			pyattr_set_onprogress(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);

	static PyObject*	pyattr_get_timetaken(PyObjectPlus *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
#endif
};

#endif // __KX_LIBLOADSTATUS_H__
