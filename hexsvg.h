
#ifndef FIRETREE__HEXSVG_H
#define FIRETREE__HEXSVG_H 1


#include "hex.h"

#include <sstream>


namespace hex {
namespace svg {


/** Render a style dictionary to SVG. */
std::string to_string(const Style& st);


/** Abstract parent class for objects that render SVG elements. */
class Element
{
public:
  virtual std::ostream& output(std::ostream& os) const =0;
  virtual ~Element(void) {}
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
  typedef typename T_Adapter::source_type source_type;
  Single(const source_type& source, const T_Adapter& adapter)
    : _source(source), _adapter(adapter)
    {}
  /** This constructor only works in the adapter has a default constructor. */
  Single(const source_type& source): _source(source), _adapter() {}
  virtual std::ostream& output(std::ostream& os) const
    {
      return _adapter.output(os,_source);
    }
private:
  const source_type&  _source;
  const T_Adapter     _adapter;
};


template<class T_Adapter>
class Group: public Element, public Identity
{
public:
  typedef typename T_Adapter::source_type source_type;
  Group(const std::list<source_type>& sources, const T_Adapter& adapter)
    : _sources(sources), _adapter(adapter)
    {}
  std::ostream& output(std::ostream& os) const
    {
      os<<"<g"<<this->attributes()<<">\n";
      for(typename std::list<source_type>::const_iterator s =_sources.begin();
                                                          s!=_sources.end();
                                                        ++s)
      {
        _adapter.output(os,*s);
      }
      os<<"</g>\n";
      return os;
    }
private:
  const std::list<source_type>&  _sources;
  const T_Adapter                _adapter;
};


//
// Area Adapters

/** Draws the boundary around an area, without worrying about voids within. */
class SimpleArea
{
  float _bias;
public:
  SimpleArea(float bias =0.0): _bias(bias) {}
  typedef Area source_type;
  std::ostream& output(std::ostream& os, const Area& a) const;
};


/** Draws the boundary around an area, taking account of voids within. */
class ComplexArea 
{
  float _bias;
public:
  ComplexArea(float bias =0.0): _bias(bias) {}
  typedef Area source_type;
  std::ostream& output(std::ostream& os, const Area& a) const;
};


class Skeleton
{
  bool _include_boundary;
public:
  Skeleton(bool include_boundary =true): _include_boundary(include_boundary) {}
  typedef Area source_type;
  std::ostream& output(std::ostream& os, const Area& a) const;
};


//
// Boundary Adapters

/** Draws a boundary line. */
class BoundaryLine
{
  float _bias;
public:
  BoundaryLine(float bias =0.0): _bias(bias) {}
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
  std::list<Element*> elements;
  Document(const Grid& grid): _grid(grid) {}
  void header(std::ostream& os) const;
  void footer(std::ostream& os) const;
  std::ostream& output(std::ostream& os) const;
};


} // end namespace svg
} // end namespace hex

#endif
