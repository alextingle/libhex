
#ifndef FIRETREE__HEXSVG_H
#define FIRETREE__HEXSVG_H 1


#include "hex.h"


namespace hex {
namespace svg {


typedef std::map<std::string,std::string> Style;

/** Render a style dictionary to SVG. */
std::string style_str(const Style& st);

/** Construct a style dictionary from an SVG string. */
Style style_dict(const std::string& s) throw(hex::invalid_argument);


/** Abstract parent class for objects that render SVG elements. */
class Element
{
public:
  virtual               ~Element(void) {}
  virtual std::ostream& output(std::ostream& os) const =0;
  std::string           str(void) const;
};


inline std::ostream&
operator<<(std::ostream& os, const Element& e)
{
  return e.output(os);
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
  bool                             _closed;
  const Identity*                  _identity; 
public:
  Poly(const std::list<Point>& pp, bool closed, const Identity* identity=NULL);
  virtual std::ostream& output(std::ostream& os) const;
};


/** Set of points rendered as an SVG polygon. */
class Polygon: public Poly
{
public:
  Polygon(const std::list<Point>& pp, const Identity* identity=NULL)
    : Poly(pp,true,identity)
    {}
};



//
// Generic elements. Instantiate with an adapter.

template<class T_Adapter>
class Single: public Element
{
public:
  typedef typename T_Adapter::source_type  source_type;
  const source_type  source;
  const T_Adapter    adapter;
  /** This constructor only works in the adapter has a default constructor. */
  Single(const source_type& src): source(src), adapter() {}
  Single(const source_type& src, const T_Adapter& adp)
    : source(src), adapter(adp)
    {}
  virtual std::ostream& output(std::ostream& os) const
    {
      return this->adapter.output(os,this->source);
    }
};


template<class T_Adapter>
class Group: public Element, public Identity
{
public:
  typedef typename T_Adapter::source_type source_type;
  const std::list<source_type>  sources;
  const T_Adapter               adapter;
  /** Constructor for adapters with a default constructor. */
  Group(const std::list<source_type>& srcs)
    : sources(srcs), adapter()
    {}
  Group(const std::list<source_type>& srcs, const T_Adapter& adp)
    : sources(srcs), adapter(adp)
    {}
  std::ostream& output(std::ostream& os) const
    {
      os<<"<g"<<this->attributes()<<">\n";
      for(typename std::list<source_type>::const_iterator s =sources.begin();
                                                          s!=sources.end();
                                                        ++s)
      {
        this->adapter.output(os,*s);
      }
      os<<"</g>\n";
      return os;
    }
};


//
// Area Adapters

/** Draws the boundary around an area, without worrying about voids within. */
class SimpleArea
{
public:
  float  bias;
  SimpleArea(float bias_ =0.0): bias(bias_) {}
  typedef Area source_type;
  std::ostream& output(std::ostream& os, const Area& a) const;
};


/** Draws the boundary around an area, taking account of voids within. */
class ComplexArea 
{
public:
  float  bias;
  ComplexArea(float bias_ =0.0): bias(bias_) {}
  typedef Area source_type;
  std::ostream& output(std::ostream& os, const Area& a) const;
};


class Skeleton
{
public:
  bool  include_boundary;
  Skeleton(bool include_boundary_ =true): include_boundary(include_boundary_) {}
  typedef Area source_type;
  std::ostream& output(std::ostream& os, const Area& a) const;
};


//
// Boundary Adapters

/** Draws a boundary line. */
class BoundaryLine
{
public:
  float  bias;
  BoundaryLine(float bias_ =0.0): bias(bias_) {}
  typedef Boundary source_type;
  std::ostream& output(std::ostream& os, const Boundary& b) const;
};


//
// Path Adapters

/** Draws a path line. */
class PathLine
{
public:
  PathLine(void) {}
  typedef Path source_type;
  std::ostream& output(std::ostream& os, const Path& p) const;
};


//
// Document

class Document
{
  const Grid&         _grid;
public:
  std::list<Element*>  elements;
  Document(const Grid& grid): _grid(grid) {}
  void           header(std::ostream& os) const;
  void           footer(std::ostream& os) const;
  std::ostream&  output(std::ostream& os) const;
  std::string    str(void) const;
};


} // end namespace svg
} // end namespace hex

#endif
