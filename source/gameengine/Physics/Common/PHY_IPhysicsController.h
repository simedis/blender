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

/** \file PHY_IPhysicsController.h
 *  \ingroup phys
 */

#ifndef __PHY_IPHYSICSCONTROLLER_H__
#define __PHY_IPHYSICSCONTROLLER_H__

#include <vector>
#include "PHY_IController.h"

class PHY_IMotionState;
class PHY_IPhysicsEnvironment;

class MT_Vector3;
class MT_Matrix3x3;

class KX_GameObject;
class RAS_MeshObject;

/**
 * PHY_IRefineCallback class is used as callback in the Refine function
 */
class PHY_IRefineCallback
{
public:
	PHY_IRefineCallback() {}

	// Called by the Refine function when new nodes are created.
	// The function should update the graphic mesh in consequence.
    // 	 newnode is the index of the new node in the softbody structure
    //   node0 is the existing node index that it is copied from
    //   node1 is the second existing node index that it is copied from, or -1 if none
    //   t is the interpolation factor between node0 (t=0.f) and node 1 (t=1.f), only if node1 is used
    // All index in softbody numbering plan (the callee must convert to graphic vertex index)
	virtual void NewNode(int newnode, int node0, int node1, float t) = 0;
	// If success is true, the nodes that have been added are confirmed
	// and the indices are updated from the new softbody faces
	// The object is also deleted on return
	virtual void Finalize(bool success) = 0;
protected:
	// destructor private because the object must be deleted through the Finalize function
	virtual ~PHY_IRefineCallback() {}
};

/**
 * The PHY_IRefineCut class is used to define the cut surface in the Refine function
 */
class PHY_IRefineCut
{
public:
	PHY_IRefineCut() {}
	virtual ~PHY_IRefineCut() {}

	// This function defines the cut surface, it returns 0 and x,y,z is on the cut surface
	// For points not on the cut serface, the return value should behave line a signed distance
	// to the cut surface:
	// if should return >0 on one side, <0 on the other side, it should be derivable
	// and the gradient should not be 0 close to the cut surface.
	virtual float Cut(float x, float y, float z) = 0;
};

/**
 * The PHY_IRefineSelect class is used to optionaly limit the cut surface to a zone
 */
class PHY_IRefineSelect
{
public:
	PHY_IRefineSelect() {}
	virtual ~PHY_IRefineSelect() {}

	// only called for x,y,z that are on the cut function (Cut(x,y,z) ~= 0 at accuracy)
	// it should return true if the point is in the cut zone, false otherwise
	virtual bool Select(float x, float y, float z) = 0;
};

/**
 * PHY_IPhysicsController is the abstract simplified Interface to a physical object.
 * It contains the IMotionState and IDeformableMesh Interfaces.
 */
class PHY_IPhysicsController : public PHY_IController
{
public:
	virtual ~PHY_IPhysicsController()
	{
	}
	/**
	 * SynchronizeMotionStates ynchronizes dynas, kinematic and deformable entities (and do 'late binding')
	 */
	virtual bool SynchronizeMotionStates(float time) = 0;
	/**
	 * WriteMotionStateToDynamics ynchronizes dynas, kinematic and deformable entities (and do 'late binding')
	 */

	virtual void WriteMotionStateToDynamics(bool nondynaonly) = 0;
	virtual void WriteDynamicsToMotionState() = 0;
	virtual class PHY_IMotionState *GetMotionState() = 0;
	// controller replication
	virtual void PostProcessReplica(class PHY_IMotionState *motionstate, class PHY_IPhysicsController *parentctrl) = 0;
	virtual void SetPhysicsEnvironment(class PHY_IPhysicsEnvironment *env) = 0;

	// kinematic methods
	virtual void RelativeTranslate(const MT_Vector3& dloc, bool local) = 0;
	virtual void RelativeRotate(const MT_Matrix3x3 &, bool local) = 0;
	virtual MT_Matrix3x3 GetOrientation() = 0;
	virtual void SetOrientation(const MT_Matrix3x3& orn) = 0;
	virtual void SetPosition(const MT_Vector3& pos) = 0;
	virtual void GetPosition(MT_Vector3& pos) const = 0;
	virtual void SetScaling(const MT_Vector3& scale) = 0;
	virtual void SetTransform() = 0;

	virtual MT_Scalar GetMass() = 0;
	virtual void SetMass(MT_Scalar newmass) = 0;

	// physics methods
	virtual void ApplyImpulse(const MT_Vector3& attach, const MT_Vector3& impulse, bool local) = 0;
	virtual void ApplyTorque(const MT_Vector3& torque, bool local) = 0;
	virtual void ApplyForce(const MT_Vector3& force, bool local) = 0;
	virtual void SetAngularVelocity(const MT_Vector3& ang_vel, bool local) = 0;
	virtual void SetLinearVelocity(const MT_Vector3& lin_vel, bool local) = 0;
	virtual void ResolveCombinedVelocities(float linvelX, float linvelY, float linvelZ, float angVelX, float angVelY, float angVelZ) = 0;

	virtual float GetLinearDamping() const = 0;
	virtual float GetAngularDamping() const = 0;
	virtual void SetLinearDamping(float damping) = 0;
	virtual void SetAngularDamping(float damping) = 0;
	virtual void SetDamping(float linear, float angular) = 0;

	virtual void RefreshCollisions() = 0;
	virtual void SuspendPhysics() = 0;
	virtual void RestorePhysics() = 0;
	virtual void SuspendDynamics(bool ghost = false) = 0;
	virtual void RestoreDynamics() = 0;
	// return true when the cut was effective
	virtual bool Refine(PHY_IRefineCut& cut, float accuracy, PHY_IRefineSelect *select, PHY_IRefineCallback *cb) = 0;

	virtual void SetActive(bool active) = 0;

	// reading out information from physics
	virtual MT_Vector3 GetLinearVelocity() = 0;
	virtual MT_Vector3 GetAngularVelocity() = 0;
	virtual MT_Vector3 GetVelocity(const MT_Vector3& pos) = 0;
	virtual MT_Vector3 GetLocalInertia() = 0;

	// dyna's that are rigidbody are free in orientation, dyna's with non-rigidbody are restricted
	virtual void SetRigidBody(bool rigid) = 0;

	virtual PHY_IPhysicsController *GetReplica()
	{
		return nullptr;
	}
	virtual PHY_IPhysicsController *GetReplicaForSensors()
	{
		return nullptr;
	}

	virtual void CalcXform() = 0;
	virtual void SetMargin(float margin) = 0;
	virtual float GetMargin() const = 0;
	virtual float GetRadius() const = 0;
	virtual void SetRadius(float margin) = 0;

	virtual float GetLinVelocityMin() const = 0;
	virtual void SetLinVelocityMin(float val) = 0;
	virtual float GetLinVelocityMax() const = 0;
	virtual void SetLinVelocityMax(float val) = 0;

	virtual void SetAngularVelocityMin(float val) = 0;
	virtual float GetAngularVelocityMin() const = 0;
	virtual void SetAngularVelocityMax(float val) = 0;
	virtual float GetAngularVelocityMax() const = 0;

	// Shape control
	virtual void AddCompoundChild(PHY_IPhysicsController *child) = 0;
	virtual void RemoveCompoundChild(PHY_IPhysicsController *child) = 0;

	virtual bool IsDynamic() = 0;
	virtual bool IsCompound() = 0;
	virtual bool IsDynamicsSuspended() const = 0;
	virtual bool IsPhysicsSuspended() = 0;

	virtual bool ReinstancePhysicsShape(KX_GameObject *from_gameobj, RAS_MeshObject *from_meshobj, bool dupli = false) = 0;
	virtual void ReplacePhysicsShape(PHY_IPhysicsController *phyctrl) = 0;

	/* Method to replicate rigid body joint contraints for group instances. */
	virtual void ReplicateConstraints(KX_GameObject *gameobj, std::vector<KX_GameObject *> constobj) = 0;
};

#endif  /* __PHY_IPHYSICSCONTROLLER_H__ */
