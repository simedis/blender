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

/** \file KX_BatchGroup.h
*  \ingroup ketsji
*/

#ifndef __KX_BATCH_GROUP_H__
#define __KX_BATCH_GROUP_H__

#include "RAS_BatchGroup.h"
#include "EXP_Value.h"

class KX_GameObject;
class CListValue;

class KX_BatchGroup : public CValue, public RAS_BatchGroup
{
	Py_Header
private:
	CListValue *m_objects;

public:
	KX_BatchGroup();
	virtual ~KX_BatchGroup();

	virtual STR_String& GetName();

	CListValue *GetObjects() const;

	void MergeObjects(const std::vector<KX_GameObject *>& objects);
	void SplitObjects(const std::vector<KX_GameObject *>& objects);

#ifdef WITH_PYTHON

	static PyObject *pyattr_get_objects(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);

	KX_PYMETHOD_DOC(KX_BatchGroup, merge);
	KX_PYMETHOD_DOC(KX_BatchGroup, split);
	KX_PYMETHOD_DOC(KX_BatchGroup, destruct);

#endif
};

#endif  // __KX_BATCH_GROUP_H__
