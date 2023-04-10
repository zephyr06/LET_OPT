#include "sources/Utils/Interval.h"

double Overlap(Interval &v1, Interval &v2) {
  double f1 = v1.start + v1.length;
  double f2 = v2.start + v2.length;

  if (v1.start >= f2 || v2.start >= f1)
    return 0;
  else if (v2.start <= v1.start && f2 >= v1.start && f1 >= f2) {
    return (f2 - v1.start);
  } else if (v2.start > v1.start && f2 < f1) {
    return v2.length;
    // return coreError * (v1.length + v2.length - (f2 - v1.start));
  } else if (v1.start > v2.start && f1 < f2) {
    return v1.length;
    // return coreError * (v2.length + v1.length - (f2 - v1.start));
  } else if (f1 >= v2.start && f2 >= f1 && v1.start <= v2.start) {
    return (f1 - v2.start);
  } else {
    std::cout << Color::red << "Error in Overlap, no case found!" << Color::def
              << std::endl;
    throw;
  }
  return 0;
}

double IntervalOverlapError(std::vector<Interval> &intervalVec) {
  if (intervalVec.size() <= 1) return 0;
  sort(intervalVec.begin(), intervalVec.end(), compare);

  double overlapAll = 0;
  size_t n = intervalVec.size();
  for (size_t i = 0; i < n; i++) {
    double endTime = intervalVec[i].start + intervalVec[i].length;
    for (size_t j = i + 1; j < n; j++) {
      if (intervalVec[j].start >= endTime)
        break;
      else {
        double ttttt = Overlap(intervalVec[i], intervalVec[j]);
        overlapAll += ttttt;
      }
    }
  }
  return overlapAll;
}

bool WhetherSerialAdjacent(const Interval &v1, const Interval &v2) {
  double f1 = v1.start + v1.length;
  double f2 = v2.start + v2.length;
  if (f1 == v2.start || f2 == v1.start)
    return true;
  else
    return false;
}