/*                            Package   : libhex
 * hex.h                      Created   : 2007/10/11
 *                            Author    : Alex Tingle
 *
 *    Copyright (C) 2007-2008, Alex Tingle.
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

/** @mainpage libhex - a library for manipulating hexagonal grids.
 *
 * Output to Scalable Vector Graphics (SVG) is supported. SVG can be viewed
 * directly in most modern web-browers, or converted to PNG or JPG using
 * readily-available utilities.
 * 
 * By default, the grid is orientated with the main "x" axis horizontal ('West'
 * to 'East). The six directions are labelled A to F anti-clockwise. Direction
 * 'A' is 'East' along the x-axis. 'B' is 'North-East' and so-on.
 *
 * If you prefer a different scheme (perhaps with the main axis running 'South'
 * to 'North', or with clockwise as the positive direction) then simply use
 * SVG transformations to rotate or mirror the grid as necessary.
 *
 * libhex is written in C++, and the main interface is in that language. A
 * Python interface is implemented using SWIG. Other language interfaces (Ruby,
 * TCL, whatever) could also be easily achieved with SWIG. If you do any work in
 * these other languages, then let me know.
 *
 * A JavaScript version of the library is also available (hex.js). This is a
 * 100% re-implementation in JavaScript. It enables Ajax applications to
 * manipulate the SVG directly.
 *
 * Most libhex objects (Hexes, Areas, Paths) can be represented as simple
 * text-strings. This makes it easy to store them, or for client-server (Ajax)
 * applications to communicate about them in high-level terms. See the
 * documentation for details.
 *
 * Download: http://sf.net/project/platformdownload.php?group_id=215125
 *
 * Author: Alex Tingle <alex.libhex@firetree.net>
 */

#ifndef FIRETREE__HEX_H
#define FIRETREE__HEX_H 1


#include <list>
#include <set>
#include <map>
#include <stdexcept>
#include <ostream>
#include <string>


#define HEX_PARANOID_CHECKS

/** Library for manipulating hexagonal grids.
 *  Supports arbitrary areas and paths. */
namespace hex {


//
// Exceptions

/** Base class for hex exceptions. */
class exception: public std::exception
{
public:
  std::string _what;
  exception(const std::string& w) throw(): _what(w) {}
  virtual ~exception() throw() {}
  virtual const char* what() const throw() { return this->_what.c_str(); }
};

class invalid_argument: public hex::exception
{
public:
  invalid_argument(const std::string& w) throw(): hex::exception(w) {}
  virtual ~invalid_argument() throw() {}
};

class out_of_range: public hex::exception
{
public:
  out_of_range(const std::string& w) throw(): hex::exception(w) {}
  virtual ~out_of_range() throw() {}
};


//
// Distance

typedef double Distance;

const Distance M_SQRT3 =1.73205080756887729352744634150587236; // sqrt(3)
const Distance I =1.0;         ///< Distance between centres of adjacent hexes.
const Distance J =M_SQRT3/2.0; ///< Distance between adjacent hex rows.
const Distance K =1.0/M_SQRT3; ///< Length of hex's edge.

//
// Direction

/** The six directions are labelled A to F anti-clockwise.
 *  Direction 'A' is 'East' along the x-axis. 'B' is 'North-East' and so-on. */
enum Direction { /** ---> */ A=0, B=1, C=2, /** <--- */ D=3, E=4, F=5 };
const int DIRECTIONS =6;

Direction to_direction(char c) throw(hex::invalid_argument);
char to_char(Direction d);

// Direction arithmetic.

Direction& operator+= (Direction& d, int i);
Direction  operator+  (const Direction& d, int i);
Direction& operator++ (Direction& d);
Direction  operator++ (const Direction& d,int);
Direction& operator-= (Direction& d, int i);
Direction  operator-  (const Direction& d, int i);
Direction& operator-- (Direction& d);
Direction  operator-- (const Direction& d,int);

std::ostream& operator<<(std::ostream& os, const Direction& d);

/** steps is a string of characters A-F, representing a set of Directions.
 *  This function rotates each step by i. */
std::string rotate(const std::string& steps, int i);


//
// svg::Identity

namespace svg {
  /** Parent class for objects that can have an id, SVG style &
   *  SVG class name. */
  class Identity
  {
  public:
    std::string  id;
    std::string  style;
    std::string  className;
    virtual ~Identity(void) {}
    /** Helper, used by child types to render attributes into SVG. */
    std::string attributes(void) const;
  };
} // end namespace svg


//
// Forward declarations

class Grid;
class Edge;
class Hex;
class Area;
class Path;
class Boundary;


/** X-Y coordinate. */
struct Point
{
  Distance x,y;
  Point offset(Distance dx, Distance dy) const { return Point(x+dx,y+dy); }
  Point(): x(0.0), y(0.0) {}
  Point(Distance x_, Distance y_): x(x_), y(y_) {}
  Point(const std::string s) throw (out_of_range,invalid_argument);
  Point& operator+=(const Point& p) { x+=p.x; y+=p.y; return *this; }
  Point& operator-=(const Point& p) { x-=p.x; y-=p.y; return *this; }
  Point operator+(const Point& p) const { return Point(x+p.x,y+p.y); }
  Point operator-(const Point& p) const { return Point(x-p.x,y-p.y); }
  Point operator*(double v) const { return Point(x*v,y*v); }
  Point operator/(double v) const { return Point(x/v,y/v); }
  bool operator==(const Point& p) const { return(x==p.x && y==p.y); }
  bool operator!=(const Point& p) const { return !operator==(p); }
  std::string str(void) const;
};

/** A square field of hexagons that form the universe in which the other
 *  objects exist. */
class Grid
{
  mutable std::map<int,Hex*> _hexes;
  int _cols;
  int _rows;
public:
  int  cols(void) const { return _cols; }
  int  rows(void) const { return _rows; }
  Distance width(void) const { return I/2.0 + I*cols(); }
  Distance height(void) const { return K*2 + J*(rows()-1); }
  bool is_in_range(int i, int j) const {return 0<=i&&i<cols()&&0<=j&&j<rows();}
  Hex* hex(int i, int j) const throw(hex::out_of_range);
  Hex* hex(Distance x, Distance y) const throw(hex::out_of_range);
  Hex* hex(const Point& p) const throw(hex::out_of_range);
  Area to_area(void) const;
public: // construction
  Grid(int cols, int rows) throw(hex::out_of_range);
public: // housekeeping
  Grid(const Grid& right);
  virtual ~Grid();
private:
  Grid& operator=(const Grid&); ///< No implementation
public: // factory methods
  /** Parse strings generated by Hex::str() */
  Hex* hex(const std::string& s) const
    throw(out_of_range,invalid_argument);
  /** Parse strings generated by set_str() */
  std::set<Hex*> hexes(const std::string& s) const
    throw(out_of_range,invalid_argument);
  /** Parse strings generated by Area::str() */
  Area area(const std::string& s) const
    throw(out_of_range,invalid_argument);
  /** Parse strings generated by Path::str() */
  Path path(const std::string& s) const
    throw(out_of_range,invalid_argument);
  /** Parse strings generated by Boundary::str() */
  Boundary boundary(const std::string& s) const
    throw(out_of_range,invalid_argument);
};


/** The interface between a hex and one of its six neighbours. Length K.
 * Each hex has its OWN set of edges, so each hex-hex interface has TWO edges-
 * one for each hex. */
class Edge
{
  Hex* const  _hex;
  const Direction  _direction;
public:
  Hex*       hex(void) const { return _hex; }
  Direction  direction(void) const { return _direction; }
  Edge*      complement(void) const;
  bool       is_next(const Edge& v) const;
  Edge*      next_in(bool clockwise=false) const;
  Edge*      next_out(bool clockwise=false) const;
  Point      start_point(float bias =0.0, bool clockwise=false) const;
  Point      end_point(float bias =0.0, bool clockwise=false) const;
  Point      join_point(const Edge* next, float bias =0.0) const;
private:
  friend class Hex;
  Edge(Hex* h, Direction d): _hex(h), _direction(d) {}
  ~Edge() {}
};


/** A single hexagonal quanta of area. */
class Hex
{
  Edge  _edgeA,_edgeB,_edgeC,_edgeD,_edgeE,_edgeF;
  const Grid& _grid;
public:
  const int  i; ///< in units of I
  const int  j; ///< in units of J
  const Grid&  grid(void) const { return _grid; }
  const Edge*  edge(const Direction& d) const;
  Edge*        edge(const Direction& d);
  Hex*         go(const Direction& d, int distance=1) const;
  Hex*         go(const std::string& steps) const;
  Point        centre(void) const;
  std::string  str(void) const;
  /** Generates a unique key for co-ordinates i,j. (i,j < 2^14) */
  static int   _key(int i, int j) { return (i<<14) | j; }
public: // construction
  Hex(const Grid& grid, int i_, int j_);
private:
  friend class Grid;
  Hex(const Hex& right);               ///< No implementation.
  Hex& operator=(const Hex&);          ///< No implementation.
  Hex(const Grid& grid, const Hex& h); ///< Only callable by Grid.
  ~Hex() {}                            ///< Only callable by Grid.
};


/** A connected set of hexes. */
class Area: public svg::Identity
{
  std::set<Hex*>  _hexes;
public:
  int                    size(void) const { return int(_hexes.size()); }
  bool                   contains(Hex* h) const { return _hexes.count(h)>0; }
  Boundary               boundary(void) const;
  const std::set<Hex*>&  hexes(void) const { return _hexes; }
  std::list<Area>        enclosed_areas(void) const;
  /** For drawing the structure. If include_boundary is TRUE, then the
   *  last item in the list is the external boundary of the area. */
  std::list<Boundary>    skeleton(bool include_boundary =true) const;
  /** A list of one or more paths that include every hex in the area once. */
  std::list<hex::Path>   fillpaths(Hex* origin =NULL) const;
  std::string            str(Hex* origin =NULL) const;
public: // transformations
  Area  go(const Direction& d, int distance=1) const throw(hex::out_of_range);
  Area  go(const std::string& steps) const throw(hex::out_of_range);
public: // construction
  Area(void): _hexes() {} ///< Required for list<Area>
  Area(const std::set<Hex*>& hexes);
  Area(Hex* h): _hexes() { _hexes.insert(h); }
  Area(Hex* h, int distance); ///< All hexes <= distance from h.
};


/** A sequence of adjacent hexes. */
class Path: public svg::Identity
{
  std::list<Hex*>  _hexes;
public:
  Area                    to_area(void) const;
  const std::list<Hex*>&  hexes(void) const { return _hexes; }
  int                     length(void) const; ///< in units of I
  std::string             steps(void) const;
  std::string             str(void) const;
public: // construction
  Path(void): _hexes() {} ///< Required for list<Path>
  Path(const std::list<Hex*>& hexes): _hexes(hexes) {}
  /** A Path starting at start, and proceeding in directions from steps. */
  Path(Hex* start, const std::string& steps)
    throw(hex::out_of_range,hex::invalid_argument);
  /** Calculates a minimum-length path between two hexes.
   *  The result is one of many possible solutions. */
  Path(Hex* from, const Hex* to) throw();
};


/** A sequence of adjacent edges. */
class Boundary: public svg::Identity
{
  std::list<Edge*>  _edges;
public:
  int                      length(void) const; ///< in units of K
  bool                     is_closed(void) const; ///< has no endpoints
  bool                     is_container(void) const; ///< contains finite area
  Boundary                 complement(void) const throw(hex::out_of_range);
  const std::list<Edge*>&  edges(void) const { return _edges; }
  Path                     to_path(void) const;
  bool                     clockwise(void) const; ///< normally false.
  std::string              str(void) const;
  /** Calculate the set of points required to draw this boundary. */
  std::list<Point>         stroke(float bias =0.0) const;
  /** Returns the area enclosed by the boundary.
   * It is an error to call this function when is_closed()==false */
  Area                     area(void) const;
public: // construction
  Boundary(void): _edges() {} ///< Required for list<Boundary>
  Boundary(const std::list<Edge*>& edges): _edges(edges) {}
};


//
// Algorithms


/** Translates coordinates i,j in direction d. */
void
go(int& i, int& j, Direction d, int distance=1);


/** Translates coordinates i,j along steps. */
void
go(int& i, int& j, const std::string& steps);


/** Calculates a minimum-length path between two hexes.
 *  The result is one of many possible solutions. */
std::string
steps(const Hex* from, const Hex* to);


/** The length of the shortest path between two hexes. */
int
distance(const Hex* from, const Hex* to);


/** Calculates the set of hexes that are range r from hex h.
 *  The result may NOT be a valid Area, since it may be cut into several
 *  pieces by the edge of the grid. */
std::set<Hex*>
range(Hex* h, int distance);


/** The set difference between a and b. */
std::set<Hex*>
set_difference(const std::set<Hex*>& a, const std::set<Hex*>& b);


/** The set intersection between a and b. */
std::set<Hex*>
set_intersection(const std::set<Hex*>& a, const std::set<Hex*>& b);


/** The set union between a and b. */
std::set<Hex*>
set_union(const std::set<Hex*>& a, const std::set<Hex*>& b);


/** Generates a string representation of hex-set a. */
std::string
set_str(const std::set<Hex*>& a);


/** Calculate a bounding box for hex-set a. Returns FALSE if a is empty. */
bool
bounding_box(const std::set<Hex*>& a, int& i0, int& j0, int& i1, int& j1);


/** Calc. the max. area that contains h, but does not intersect beyond. */
Area
fill(const std::set<Hex*>& beyond, Hex* h);


/** Max. area that contains hexes in a, but does not intersect beyond. */
Area
fill(const std::set<Hex*>& beyond, const Area& a);


/** Find all of the Areas (connected sets) in s. */
std::list<Area>
areas(const std::set<Hex*>& s);


/** TRUE iff s is connected. */
bool
is_connected(const std::set<Hex*>& s);


/** The skeletons of areas a. */
std::list<Boundary>
skeleton(const std::list<Area>& a, bool include_boundary =true);


} // end namespace hex

#endif
