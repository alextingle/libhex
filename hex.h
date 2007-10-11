
#ifndef FIRETREE__HEX_H
#define FIRETREE__HEX_H 1


#include <list>
#include <set>
#include <vector>
#include <stdexcept>
#include <ostream>


#define HEX_PARANOID_CHECKS


namespace hex {

//
// Distance

typedef double Distance;

const Distance M_SQRT3 =1.73205080756887729352744634150587236; // sqrt(3)
const Distance I =1.0;
const Distance J =M_SQRT3/2.0;
const Distance K =1.0/M_SQRT3;

//
// Direction

enum Direction { A=0,B=1,C=2,D=3,E=4,F=5 };
const int DIRECTIONS =6;

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
  Point offset(Distance dx, Distance dy) { return Point(x+=dx,y+=dy); }
  Point(): x(0.0), y(0.0) {}
  Point(Distance x_, Distance y_): x(x_), y(y_) {}
};


class Grid
{
  typedef std::vector<Hex*> Row;
  std::vector<Row*> _rows;
public:
  int  cols(void) const { return _rows.front()->size(); }
  int  rows(void) const { return _rows.size(); }
  Hex* hex(int i, int j) const throw(std::out_of_range);
  Hex* hex(Distance x, Distance y) const throw(std::out_of_range);
  Hex* hex(const Point& p) const throw(std::out_of_range);
  Area to_area(void) const;
public: // construction
  Grid(int cols, int rows);
public: // housekeeping
  Grid(const Grid& right);
  virtual ~Grid();
private:
  Grid& operator=(const Grid&); ///< No implementation
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
  Edge*      next_in(bool reverse=false) const;
  Edge*      next_out(bool reverse=false) const;
  Point      start_point(float bias =0.0) const;
  Point      end_point(float bias =0.0) const;
private:
  friend class Hex;
  Edge(Hex* h, Direction d): _hex(h), _direction(d) {}
  virtual ~Edge() {}
};


/** A single hexagonal quanta of area. */
class Hex
{
  Edge  _edgeA,_edgeB,_edgeC,_edgeD,_edgeE,_edgeF;
  const Grid& _grid;
  const int  _i, _j;
public:
  const Grid&  grid(void) const { return _grid; }
  const Edge*  edge(const Direction& d) const;
  Edge*        edge(const Direction& d);
  Hex*         go(const Direction& d, int distance=1) const;
  int          i(void) const { return _i; } ///< in units of I
  int          j(void) const { return _j; } ///< in units of J
  Point        centre(void) const;
public: // construction
  Hex(const Grid& grid, int i, int j);
  Hex(const Grid& grid, const Hex& h);
private:
  friend class Grid;
  Hex(const Hex& right);
  Hex& operator=(const Hex&);
  virtual ~Hex() {}
};


/** A connected set of hexes. */
class Area
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
public: // construction
  Area(const std::set<Hex*>& hexes);
  Area(Hex* h): _hexes() { _hexes.insert(h); }
};


/** A sequence of adjacent hexes. */
class Path
{
  std::list<Hex*>  _hexes;
public:
  Area                    to_area(void) const;
  const std::list<Hex*>&  hexes(void) const { return _hexes; }
  int                     length(void) const; ///< in units of I
public: // construction
  Path(const std::list<Hex*>& hexes): _hexes(hexes) {}
};


/** A sequence of adjacent edges. */
class Boundary
{
  std::list<Edge*>  _edges;
public:
  int                      length(void) const; ///< in units of K
  bool                     is_closed(void) const; ///< has no endpoints
  bool                     is_container(void) const; ///< contains finite area
  Boundary                 complement(void) const throw(std::range_error);
  const std::list<Edge*>&  edges(void) const { return _edges; }
  Path                     to_path(void) const;
  /** Calculate the set of points required to draw this boundary. */
  std::list<Point>  stroke(float bias =0.0) const;
  /** Returns the area enclosed by the boundary.
   * It is an error to call this function when is_closed()==false */
  Area  area(void) const;
public: // construction
  Boundary(const std::list<Edge*>& edges): _edges(edges) {}
};


//
// Algorithms


/** Translates coordinates i,j in direction d. */
void
go(int& i, int& j, Direction d, int distance=1);


/** Calculates a minimum-length path between two hexes.
 *  The result is one of many possible solutions. */
Path
path(Hex* from, Hex* to);


/** The length of the shortest path between two hexes. */
int
distance(Hex* h0, Hex* h1);


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