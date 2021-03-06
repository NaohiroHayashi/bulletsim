#ifndef __OPHYS_PARTICLE_SYSTEM_2_H__
#define __OPHYS_PARTICLE_SYSTEM_2_H__

#include "sqp/sqp.h"
#include "ophys_config.h"

#include <Eigen/Dense>
using Eigen::Matrix;

#include <boost/multi_array.hpp>
#include <boost/format.hpp>
using boost::multi_array;
#include <vector>
using std::vector;

#include "simulation/plotting.h"
class Scene;


namespace ophys {


class ParticleSystemOptimizer2 : public Optimizer {
public:
  typedef boost::shared_ptr<ParticleSystemOptimizer2> Ptr;

  // constant system parameters
  const int m_numParticles, m_horizon;
  const string m_varPrefix;

  // variable layout:
  // we want to represent the states of m_numParticles over m_horizon time intervals
  // each row is the (position, velocity, accel) of one particle, and c is the ground contact auxiliary variable
  static const int PARTICLE_STATE_DIM = (3 + 3 + 3) + (1 + 3);
  static bool isContactAuxVar(int j) { return j == 9; }
  typedef Matrix<double, Dynamic, PARTICLE_STATE_DIM> SysState;
  typedef vector<SysState> SysStatesOverTime;
  SysStatesOverTime m_sys;
  SysStatesOverTime m_sys_backup;

  // Gurobi variables
  // indices to VarSysState: m_varsys[time][particle_idx][component]
  typedef multi_array<GRBVar, 3> VarSysState;
  VarSysState m_varsys;


  explicit ParticleSystemOptimizer2(int numParticles, int horizon, const string &varPrefix="particlesystem");

  void initializeVariables();
  void updateValues();
  void storeValues();
  void rollbackValues();
  void preOptimize();
  void postOptimize();

  void initialize(const SysStatesOverTime &init);
  void initializeFromSingleState(const SysState &s0);


  typedef boost::function<double(const SysStatesOverTime*)> FuncOnStatesOverTime;
  GRBLinExpr linearize(const FuncOnStatesOverTime &fn, SysStatesOverTime &x0);
  GRBQuadExpr quadraticize(const FuncOnStatesOverTime &fn, SysStatesOverTime &x0);

  MatrixXd hessian(const FuncOnStatesOverTime &fn, SysStatesOverTime &x0);


  friend class ParticleSystem2;


  struct InitCondConstraint : public Constraint {
    typedef boost::shared_ptr<InitCondConstraint> Ptr;
    ParticleSystemOptimizer2 &m_opt;
    ParticleSystemOptimizer2::SysState m_s0;
    InitCondConstraint(ParticleSystemOptimizer2 &opt, const ParticleSystemOptimizer2::SysState &s0) : m_opt(opt), m_s0(s0) { }
    ConvexConstraintPtr convexify(GRBModel* );
  };


  struct GroundConstraint : public Constraint {
    typedef boost::shared_ptr<GroundConstraint> Ptr;
    ParticleSystemOptimizer2 &m_opt;
    const double m_groundZ;
    GroundConstraint(ParticleSystemOptimizer2 &opt, double groundZ) : m_opt(opt), m_groundZ(groundZ) { }
    ConvexConstraintPtr convexify(GRBModel* );
  };


  struct GroundCost : public Cost {
    typedef boost::shared_ptr<GroundCost> Ptr;
    ParticleSystemOptimizer2 &m_opt;
    const double m_groundZ;
    GroundCost(ParticleSystemOptimizer2 &opt, double groundZ) : m_opt(opt), m_groundZ(groundZ) { }
    string getName() { return "ground_cost"; }
    double evaluate();
    ConvexObjectivePtr convexify(GRBModel* model);
  };

  struct AccelCost : public Cost {
    typedef boost::shared_ptr<AccelCost> Ptr;
    ParticleSystemOptimizer2 &m_opt;
    AccelCost(ParticleSystemOptimizer2 &opt) : m_opt(opt) { }
    string getName() { return "accel_cost"; }
    double evaluate();
    ConvexObjectivePtr convexify(GRBModel* model);
  };

  struct PosUpdateConstraint : public Constraint {
    typedef boost::shared_ptr<PosUpdateConstraint> Ptr;
    ParticleSystemOptimizer2 &m_opt;
    PosUpdateConstraint(ParticleSystemOptimizer2 &opt) : m_opt(opt) { }
    ConvexConstraintPtr convexify(GRBModel *);
  };

  struct PhysicsCost : public Cost {
    typedef boost::shared_ptr<PhysicsCost> Ptr;
    ParticleSystemOptimizer2 &m_opt;
    PhysicsCost(ParticleSystemOptimizer2 &opt) : m_opt(opt) { }
    string getName() { return "physics_cost"; }
    double evaluateAt(const SysStatesOverTime *s0) const;
    double evaluate();
    ConvexObjectivePtr convexify(GRBModel* model);
  };

private:

  static Eigen::Block<SysState, 1, 3> ptPos(SysState &s, int i) { return s.block<1,3>(i,0); }
  static const Eigen::Block<const SysState, 1, 3> ptPos(const SysState &s, int i) { return s.block<1,3>(i,0); }
  static Eigen::Block<SysState, 1, 3> ptVel(SysState &s, int i) { return s.block<1,3>(i,3); }
  static const Eigen::Block<const SysState, 1, 3> ptVel(const SysState &s, int i) { return s.block<1,3>(i,3); }
  static Eigen::Block<SysState, 1, 3> ptAcc(SysState &s, int i) { return s.block<1,3>(i,6); }
  static const Eigen::Block<const SysState, 1, 3> ptAcc(const SysState &s, int i) { return s.block<1,3>(i,6); }

  static double &ptGroundCont(SysState &s, int i) { return s(i,9); }
  static const double &ptGroundCont(const SysState &s, int i) { return s(i,9); }
  static Eigen::Block<SysState, 1, 3> ptGroundContForce(SysState &s, int i) { return s.block<1,3>(i,10); }
  static const Eigen::Block<const SysState, 1, 3> ptGroundContForce(const SysState &s, int i) { return s.block<1,3>(i,10); }

  Eigen::Block<SysState, 1, 3> ptPos(int t, int i) { return m_sys[t].block<1,3>(i,0); }
  const Eigen::Block<const SysState, 1, 3> ptPos(int t, int i) const { return m_sys[t].block<1,3>(i,0); }
  Eigen::Block<SysState, 1, 3> ptVel(int t, int i) { return m_sys[t].block<1,3>(i,3); }
  const Eigen::Block<const SysState, 1, 3> ptVel(int t, int i) const { return m_sys[t].block<1,3>(i,3); }
  Eigen::Block<SysState, 1, 3> ptAcc(int t, int i) { return m_sys[t].block<1,3>(i,6); }
  const Eigen::Block<const SysState, 1, 3> ptAcc(int t, int i) const { return m_sys[t].block<1,3>(i,6); }

  double &ptGroundCont(int t, int i) { return m_sys[t](i,9); }
  const double &ptGroundCont(int t, int i) const { return m_sys[t](i,9); }
  Eigen::Block<SysState, 1, 3> ptGroundContForce(int t, int i) { return m_sys[t].block<1,3>(i,10); }
  const Eigen::Block<const SysState, 1, 3> ptGroundContForce(int t, int i) const { return m_sys[t].block<1,3>(i,10); }

  typedef boost::multi_array_types::index_range range;
  typedef VarSysState::array_view<1>::type VarVec3View;
  VarVec3View varPtPos(int t, int i) { return m_varsys[ boost::indices[t][i][range(0,3)] ]; }
  VarVec3View varPtVel(int t, int i) { return m_varsys[ boost::indices[t][i][range(3,6)] ]; }
  VarVec3View varPtAcc(int t, int i) { return m_varsys[ boost::indices[t][i][range(6,9)] ]; }

  GRBVar &varPtGroundCont(int t, int i) { return m_varsys[t][i][9]; }
  const GRBVar &varPtGroundCont(int t, int i) const { return m_varsys[t][i][9]; }
  VarVec3View varPtGroundContForce(int t, int i) { return m_varsys[ boost::indices[t][i][range(10,13)] ]; }
};


struct ParticleSysTrustRegion : public TrustRegion {
  typedef boost::shared_ptr<ParticleSysTrustRegion> Ptr;

  ParticleSystemOptimizer2 &m_opt;
  multi_array<double, 3> m_radii;
  bool m_infinite;

  ParticleSysTrustRegion(ParticleSystemOptimizer2 &opt);

  void setInfinite(bool b) { m_infinite = b; }
  void adjustTrustRegion(double ratio);
  ConvexConstraintPtr convexify(GRBModel* model);
};

class ParticleSystem2 {
public:
  //ParticleSystemOptimizer2::Ptr m_opt;
  ParticleSystemOptimizer2::SysState m_currState;
  ParticleSystemOptimizer2::SysStatesOverTime m_currAllStates;

  PlotSpheres::Ptr m_plotSpheres;
  Scene *m_scene;

  ParticleSystem2(const ParticleSystemOptimizer2::SysState &initState);
  void step(double dt, int numSteps=1);
  void step();
  void setupOpt(ParticleSystemOptimizer2 &opt);
  void setupOpt2(ParticleSystemOptimizer2 &opt);

  void attachToScene(Scene *);
  void draw();
  void draw(const ParticleSystemOptimizer2::SysState &);

  ParticleSystemOptimizer2::SysStatesOverTime getAllStates() const { return m_currAllStates; }

protected:
  ParticleSysTrustRegion::Ptr m_trustRegion;
  ParticleSystemOptimizer2::InitCondConstraint::Ptr m_initCondCnt;
  //ParticleSystemOptimizer2::GroundConstraint::Ptr m_groundCnt;
  ParticleSystemOptimizer2::GroundCost::Ptr m_groundCost;
  ParticleSystemOptimizer2::AccelCost::Ptr m_accelCost;

  ParticleSystemOptimizer2::PosUpdateConstraint::Ptr m_posUpdateCnt;
  ParticleSystemOptimizer2::PhysicsCost::Ptr m_physicsCost;
};


} // namespace ophys


#endif // __OPHYS_PARTICLE_SYSTEM_2_H__
