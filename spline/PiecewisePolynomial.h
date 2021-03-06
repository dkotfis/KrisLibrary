#ifndef PIECEWISE_POLYNOMIAL_H
#define PIECEWISE_POLYNOMIAL_H

#include "Polynomial.h"
class File;

namespace Spline {

/** A trajectory y(t) consisting of a set of trajectory segments
 * segments[], split among times times[].  segments[i] is defined over
 * the interval Ii = [times[i],times[i+1]).
 * 
 * The timeShift member is defined to allow a difference between the
 * segment's "local" time versus the "global" time of the overall polynomial.
 * More precisely, over interval Ii, we define:
 * 
 *        y(t)=segments[i](t-timeShift[i])
 *
 * And the segment's local time is defined over the domain
 * [times[i]-timeShift[i],times[i+1]-timeShift[i]).
 */
class PiecewisePolynomial
{
 public:
  typedef Polynomial<double> Poly;

  PiecewisePolynomial();
  PiecewisePolynomial(const Poly& p,double a,double b);
  PiecewisePolynomial(const std::vector<Poly>& _segments,const std::vector<double>& _times,bool relative=false);
  PiecewisePolynomial(const std::vector<Poly>& _segments,const std::vector<double>& _times,const std::vector<double>& _timeShifts);

  int FindSegment(double t) const;
  double Evaluate(double t) const;
  double Derivative(double t) const;
  double Derivative(double t,int n) const;
  double operator () (double t) const { return Evaluate(t); }

  PiecewisePolynomial Differentiate(int n=1) const;
  inline double Start() const { return segments.front()(times.front()-timeShift.front()); }
  inline double End() const { return segments.back()(times.back()-timeShift.back()); }
  inline double StartTime() const { return times.front(); }
  inline double EndTime() const { return times.back(); }

  /**
   * Appends the segment to the trajectory following the final
   * segment in the trajectory.
   *
   * If relative is true, then T is interpreted as the total length
   * of the segment, and segment(t) is defined on [0,T].
   *
   * If relative is false, then T is interpreted as the final time
   * of the segment, and segment(t) is defined on [self.endTime(),T]
   */
  void Append(const Poly& p,double t,bool relative=false);

  /**
   * Appends traj to the current trajectory.  If relative=true,
   * the domain of traj is shifted forward in time by EndTime()
   * before concatenating
   */
  void Concat(const PiecewisePolynomial& traj,bool relative=false);

  /** Moves time forward by dt */
  void TimeShift(double dt);

  /**
   * Changes the local time domain of every segment to be defined
   * with timeShift 0.
   */
  void ZeroTimeShift();

  /** Splits the trajectory in two pieces */
  void Split(double t,PiecewisePolynomial& front,PiecewisePolynomial& back) const;

  /** Sets a new start time tstart */
  void TrimFront(double tstart);

  /** Sets a new end time tend */
  void TrimBack(double tend);

  /** Selects a range */
  PiecewisePolynomial Select(double a,double b) const;

  /** Computes the maximum discontinuity of the i'th derivative.  Return value is
   * the time and absolute magnitude of the maximum discontinuity.
   */
  std::pair<double,double> MaxDiscontinuity(int derivative=0) const;

  bool Read(File& f);
  bool Write(File& f) const;

  void operator += (double val);
  void operator -= (double val);
  void operator *= (double val);
  void operator /= (double val);
  void operator += (const Polynomial<double>& b);
  void operator -= (const Polynomial<double>& b);
  void operator *= (const Polynomial<double>& b);

  std::vector<Poly> segments;
  std::vector<double> timeShift;
  std::vector<double> times;
};

class PiecewisePolynomialND
{
 public:
  typedef Polynomial<double> Poly;
  typedef std::vector<double> Vector;

  PiecewisePolynomialND();
  PiecewisePolynomialND(const std::vector<Poly>& elements,double a,double b);
  PiecewisePolynomialND(const std::vector<PiecewisePolynomial>& elements);

  Vector Evaluate(double t) const;
  Vector Derivative(double t) const;
  Vector Derivative(double t,int n) const;
  Vector operator () (double t) const { return Evaluate(t); }

  PiecewisePolynomialND Differentiate(int n=1) const;
  Vector Start() const;
  Vector End() const;
  double StartTime() const;
  double EndTime() const;

  void Concat(const PiecewisePolynomialND& traj,bool relative=false);
  void Append(const std::vector<Polynomial<double> >& polys,double t,bool relative=false);
  void TimeShift(double dt);
  void Split(double t,PiecewisePolynomialND& front,PiecewisePolynomialND& back) const;
  void TrimFront(double tstart);
  void TrimBack(double tend);
  PiecewisePolynomialND Select(double a,double b) const;
  std::pair<Vector,Vector> MaxDiscontinuity(int derivative=0) const;

  bool Read(File& f);
  bool Write(File& f) const;

  std::vector<PiecewisePolynomial> elements;
};


PiecewisePolynomial Constant(double x,double ta,double tb);
PiecewisePolynomial Linear(double a,double b,double ta,double tb);
PiecewisePolynomial PiecewiseLinear(const std::vector<double>& milestones,const std::vector<double>& times);

PiecewisePolynomialND Constant(const std::vector<double>& q,double ta,double tb);
PiecewisePolynomialND Linear(const std::vector<double>& a,const std::vector<double>& b,double ta,double tb);
PiecewisePolynomialND PiecewiseLinear(const std::vector<std::vector<double> >& milestones,const std::vector<double>& times);
PiecewisePolynomialND Subspace(const std::vector<double>& x0,const std::vector<double>& dx,PiecewisePolynomial& poly);

inline PiecewisePolynomial operator + (const PiecewisePolynomial& a,double b) {
  PiecewisePolynomial res=a;
  res += b;
  return res;
}
inline PiecewisePolynomial operator - (const PiecewisePolynomial& a,double b) {
  PiecewisePolynomial res=a;
  res -= b;
  return res;
}
inline PiecewisePolynomial operator * (const PiecewisePolynomial& a,double b) {
  PiecewisePolynomial res=a;
  res *= b;
  return res;
}
inline PiecewisePolynomial operator / (const PiecewisePolynomial& a,double b) {
  PiecewisePolynomial res=a;
  res /= b;
  return res;
}
inline PiecewisePolynomial operator + (double a,const PiecewisePolynomial& b) {
  PiecewisePolynomial res=b;
  res += a;
  return res;
}
inline PiecewisePolynomial operator * (double a,const PiecewisePolynomial& b) {
  PiecewisePolynomial res=b;
  res *= a;
  return res;
}
inline PiecewisePolynomial operator + (const PiecewisePolynomial& a,const Polynomial<double>& b) {
  PiecewisePolynomial res=a;
  res += b;
  return res;
}
inline PiecewisePolynomial operator - (const PiecewisePolynomial& a,const Polynomial<double>& b) {
  PiecewisePolynomial res=a;
  res -= b;
  return res;
}
inline PiecewisePolynomial operator * (const PiecewisePolynomial& a,const Polynomial<double>& b) {
  PiecewisePolynomial res=a;
  res *= b;
  return res;
}
inline PiecewisePolynomial operator + (const Polynomial<double>& a,const PiecewisePolynomial& b) {
  PiecewisePolynomial res=b;
  res += a;
  return res;
}
inline PiecewisePolynomial operator * (const Polynomial<double>& a,const PiecewisePolynomial& b) {
  PiecewisePolynomial res=b;
  res *= a;
  return res;
}


} //namespace Spline

#endif
