#pragma once
#include "LfdRpmWrapper.h"
#include <iostream>
#include "simulation/util.h"
#include "robots/ravens.h"

using namespace std;

/** Simple class which helps apply lfd by transforming the joints angles. */
class RavensLfdRpm {
private:
	/** Extract the joints indexed by INDS from IN_JOINT_VALS and store them into OUT_JOINT_VALS.*/
	void extractJoints (const vector<int> &inds, const vector<dReal> &in_joint_vals,
			vector<dReal> &out_joint_vals);

	/** Does smooth IK on transforms (in joint space: basically chooses the closest subsequent joint-set [l2 normwise].
	 *  Ik is done for each transform in TRANSFORMS and the corresponding joints are stored in JOINTS.*/
	bool doSmoothIK(RaveRobotObject::Manipulator::Ptr manip, const vector<btTransform> & transforms, vector< vector<dReal> > &joints);

	Ravens  &ravens;
	RegistrationModule::Ptr lfdrpm;

	vector<int> larm_indices;
	vector<int> rarm_indices;

	void plotTransforms (vector< btTransform > &, vector< btTransform > &, vector< btTransform > &, vector< btTransform > &);

	void plotPointos (vector< btTransform > &, vector< btTransform > &, vector< btTransform > &, vector< btTransform > &);

public:
	/** Ravens   : the robot to transform the joints for.
	 *  SRC_PTS_ : the reference point locations.
	 *  TARGET_PTS_: the new point locations. */
	RavensLfdRpm (Ravens &ravens_, const vector<btVector3> &src_pts, const vector<btVector3> &target_pts);

	/** Warp the joint angles of ravens using warping and IK.*/
	bool transformJoints(const vector<vector<dReal> > &joint_vals, vector< vector<dReal> > &new_joint_vals);

	/** Warp the joint angles of ravens using warping and trajectory optimization.*/
	bool transformJointsTrajOpt(const vector<vector<dReal> > &joints, vector<vector<dReal> > &new_joints);

};


/** Do trajectory optimization to solve for the new joint angles for getting to the new warped transforms. */
vector< vector<double> > doTrajectoryOptimization(RaveRobotObject::Manipulator::Ptr manip, const vector<btTransform> & transforms,
		const vector< vector<dReal> > &old_joints);

/** Warp the joint values of the ravens using SRC_PTS as the reference
 *  and TARGETR_PTS as the new points for warping.*/
bool warpRavenJoints(Ravens &ravens,
		const vector<btVector3> &src_pts, const vector<btVector3> &target_pts,
		const vector< vector<dReal> >& in_joints, vector< vector<dReal> > & out_joints);
