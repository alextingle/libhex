
#ifndef FIRETREE__HEXSVG_H
#define FIRETREE__HEXSVG_H 1


#include "hex.h"

#include <sstream>
#include <map>

namespace hex {
namespace svg {

typedef std::map<std::string,std::string> Style;

/** Render a style dictionary to SVG. */
std::string to_string(const Style& st);

/** Abstract parent class for objects that render SVG elements. */
class Element
{
public:
  Style       style;
  std::string className;
  virtual std::ostream& output(std::ostream& os) const =0;
  virtual ~Element(void) {}
protected:
  /** Helper, used by child types. */
  std::string attributes(void) const;
};


inline std::ostream&
operator<<(std::ostream& os, const Element& e)
{
  return e.output(os);
}


inline std::ostream&
operator<<(std::ostream& os, const Element* e)
{
  return e->output(os);
}


inline std::ostream&
operator<<(std::ostream& os, hex::Point p)
{
  os<<p.x<<","<<p.y;
  return os;
}


/** Series of points, rendered as an SVG polygon, or polyline, depending upon
 *  whether they are closed or not. */
class Poly: public Element
{
  std::list<Point>::const_iterator _first, _last;
  bool _closed;
public:
  Poly(const std::list<Point>& pp, bool closed);
  virtual std::ostream& output(std::ostream& os) const;
};


/** Set of points rendered as an SVG polygon. */
class Polygon: public Poly
{
public:
  Polygon(const std::list<Point>& pp): Poly(pp,true) {}
};


/** Draws the boundary around each of a list of areas, without worrying about
 *  voids within. */
class SimpleArea: public Element
{
  const std::list<Area>& _areas;
  float                  _bias;
public:
  SimpleArea(const std::list<Area>& areas, float bias =0.0);
  virtual std::ostream& output(std::ostream& os) const;
};


/** Draws the boundary around each of a list of areas, taking account of
 *  voids within. */
class ComplexArea: public Element
{
  const std::list<Area>& _areas;
  float                  _bias;
public:
  ComplexArea(const std::list<Area>& areas, float bias =0.0);
  virtual std::ostream& output(std::ostream& os) const;
};


class Document
{
  std::ostringstream _buf;
public:
  Document();
  void header(void);
  void footer(void);
  const char* str() const { return _buf.str().c_str(); }
  
};


} // end namespace svg
} // end namespace hex

#endif
