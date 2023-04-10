#pragma once
#include "sources/Utils/DeclareDAG.h"
#include "sources/Utils/colormod.h"

struct Interval {
  double start;
  double length;
  Interval() {}

  Interval(double s1, double l1) : start(s1), length(l1) {}

  inline double getFinish() const { return start + length; }

  inline bool IfBelong(double x) { return x >= start && x <= start + length; }
  // whether interval x belongs to this
  inline bool IfBelong(Interval x) {
    return start <= x.start && x.start + x.length <= start + length;
  }
  inline void UpdateSF(double startI, double finish) {
    start = startI;
    length = finish - start;
  }
};
inline bool compare(Interval &i1, Interval &i2) {
  return (i1.start < i2.start);
}

/**
 * @brief always return v1 - v2;
 *
 *
 * @param v1
 * @param v2
 * @return double, overlap error
 */
double Overlap(Interval &v1, Interval &v2);

bool WhetherSerialAdjacent(const Interval &v1, const Interval &v2);

double IntervalOverlapError(std::vector<Interval> &intervalVec);