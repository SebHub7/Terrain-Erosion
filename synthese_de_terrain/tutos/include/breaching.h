#ifndef __BREACHING__HPP__
#define __BREACHING__HPP__

#include <queue>

/**
 * @brief Classe correpondant à des points d'une grille => Entier
 * 
 */
struct QPoint {
    QPoint (int x, int y) : value{x,y} {};
    inline int x()const{return value[0];}
    inline int y()const{return value[1];}

    private:
        int value[2];
};

using Element = std::pair<double, QPoint>;

struct comp {

  // Compare elevation, then x, then y
  bool operator()(const Element& a, const Element& b)
  {
    const QPoint& lhs = a.second;
    const QPoint& rhs = b.second;

    if (a.first > b.first) return true;
    else if ((a.first == b.first) && (lhs.x() > rhs.x() || (lhs.x() == rhs.x() && lhs.y() > rhs.y())))
      return true;
    else return false;
  }
};

using ZPriorityQueue = std::priority_queue< Element, std::vector<Element>, comp >;


#endif