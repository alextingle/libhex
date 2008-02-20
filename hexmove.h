/*                            Package   : libhex
 * hexmove.h                  Created   : 2008/02/19
 *                            Author    : Alex Tingle
 *
 *    Copyright (C) 2008, Alex Tingle.
 *
 *    This file is part of the libhex application.
 *
 *    libhex is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    libhex is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef FIRETREE__HEXMOVE_H
#define FIRETREE__HEXMOVE_H 1


#include "hex.h"


namespace hex {

/** Find routes and movement horizons on a hex grid. */
namespace move {


class no_solution: public hex::exception
{
public:
  no_solution(const std::string& w) throw(): hex::exception(w) {}
  virtual ~no_solution() throw() {}
};


typedef double Cost;


/** Models movement costs in a hex grid. */
class Topography
{
  bool  _has_default;
  /** Default cost to enter a hex. If NULL, unlisted hexes are off-limits */
  Cost  _default;
  /** Cost to enter a hex. key: hex.Hex, val: cost */
  std::map<hex::Hex*,Cost>  _hexes;
  /** Cost to cross an edge. key: hex.Edge, val: cost
   *  *Over-rides* the normal costs. Must pay the cost of the 
   *  DESTINATION hex's edge. */
  std::map<hex::Edge*,Cost>  _edges;
public:
  Topography();
  Topography(Cost default_hex_cost);
  virtual ~Topography() {}
  
  /** Calculate set of accessible hexes. */
  std::set<hex::Hex*> accessible(void) const;

  /** Increases the cost of hex h by c. */
  void increase_hex_cost(hex::Hex* h, Cost c);
  /** Set the cost of hex h to c. */
  void override_hex_cost(hex::Hex* h, Cost c);
        
  /** Increases the cost of edge e by c. */
  void increase_edge_cost(hex::Edge* e, Cost c);
  /** Set the cost of edge e to c. */
  void override_edge_cost(hex::Edge* e, Cost c);

  // Convenience functions.
  void increase_cost(const hex::Area& a, Cost c);
  void override_cost(const hex::Area& a, Cost c);
  void increase_cost(const hex::Boundary& b, Cost c);
  void override_cost(const hex::Boundary& b, Cost c);

  /** Finds the least-cost hex::Path from start to goal.
   *  Uses the A* algorithm. */
  hex::Path best_path(hex::Hex* start, hex::Hex* goal) const throw(no_solution);

  /** Finds the hex::Area that can be reached from start with budget. */
  hex::Area horizon(hex::Hex* start, Cost budget) const throw(no_solution);

protected:
  struct Step
    {
      hex::Hex*  to_hex;
      Cost       cost;
    };

  /** Calculates the cost to move one step from_hex in direction.
   *  Returns tuple: (to_hex,cost)
   *  If the move is off-limits, then returns (None,None). */
  Step step(hex::Hex* from_hex, Direction direction) const;
};


/** A partial solution to the A* algorithm.
 *  Only used in the internal workings of the routing algorithms. You won't
 *  need this class unless you are writing your own routing algorithms.
 */
class _Route
{
  Cost             _value; ///< cost + distance to goal          (f() in A*)

public:
  std::list<Hex*>  path;  ///< from start to current hex
  Cost             cost;  ///< sum of weights of hexes in path. (g() in A*)

  /** Generate an entirely new route, just one hex long. */
  static _Route factory(hex::Hex* start, hex::Hex* goal=NULL);

  /** Return a new _Route based upon this one. */
  _Route step(hex::Hex* next, Cost cost, hex::Hex* goal=NULL) const;
     
  /** Distance between the end of this route and the goal, if any.
   *  Used for the h() heuristic function in A* algorithm. */
  hex::Distance distance(hex::Hex* goal) const;

  /** Routes sort by value. */
  bool operator<(const _Route& right) const { return _value < right._value; }
};


} // end namespace move
} // end namespace hex

#endif
