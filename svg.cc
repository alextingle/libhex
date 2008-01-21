/*                            Package   : libhex
 * svg.cc                     Created   : 2007/10/25
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

#include "hexsvg.h"

#include <cassert>
#include <sstream>


namespace hex {
namespace svg {


/** Helper function - strip whitespace from the start & end of a string. */
std::string
strip(const std::string& s)
{
  std::string::size_type p0 =s.find_first_not_of(" \t\n");
  std::string::size_type p1 =s.find_last_of(" \t\n");
  return s.substr(p0,p1-p0);
}


std::string
style_str(const Style& st)
{
  std::string result ="";
  for(Style::const_iterator it=st.begin(); it!=st.end(); ++it)
  {
    if(!result.empty())
        result += ';';
    result += it->first + ":" + it->second;
  }
  return result;
}


Style
style_dict(const std::string& s) throw(hex::invalid_argument)
{
  Style result;
  std::string::size_type pos =0;
  while(true)
  {
    pos=s.find_first_not_of(";",pos);
    if(pos==std::string::npos)
        break;
    std::string::size_type semi =s.find(";",pos);
    std::string clause =s.substr(pos,semi-pos);
    std::string::size_type colon =clause.find(":");
    if(colon==std::string::npos || colon==0 || (colon+1)>=clause.size())
        throw hex::invalid_argument(s);
    result[ strip(s.substr(0,colon)) ] = strip(s.substr(colon+1));
    pos=semi;
  }
  return result;
}


/** Helper function. */
template<class InputIterator>
std::ostream&
output_path_data(std::ostream& os, InputIterator first, InputIterator last)
{
  assert(first!=last);
  --last;
  for(InputIterator p =first; p!=last; ++p)
  {
    if(p==first)
        os<<"M "<<(*p);
    else
        os<<" L "<<(*p);
  }
  os<<" Z";
  return os;
}


//
// Identity

std::string
Identity::attributes(void) const
{
  std::string result ="";
  if(!this->id.empty())
      result += std::string(" id=\"") + this->id + "\"";
  if(!this->style.empty())
      result += std::string(" style=\"") + this->style + "\"";
  if(!this->className.empty())
      result += std::string(" class=\"") + this->className + "\"";
  return result;
}


//
// Element

std::string
Element::str(void) const
{
  std::ostringstream ss;
  this->output(ss);
  return ss.str();
}


//
// Poly

Poly::Poly(const std::list<Point>& pp, bool closed, const Identity* identity)
  :_first(pp.begin()), _last(pp.end()), _closed(closed), _identity(identity)
{
  assert(_first!=_last);
  if(closed)
      --_last;
}


std::ostream&
Poly::output(std::ostream& os) const
{
  if(_closed)
      os<<"<polygon";
  else
      os<<"<polyline";
  if(_identity)
      os<<_identity->attributes();
  os<<" points=\"";
  for(std::list<Point>::const_iterator pos=_first; pos!=_last; ++pos)
  {
    if(pos!=_first)
       os<<" ";
    os<<pos->x<<","<<pos->y;
  }
  os<<"\"/>\n";
  return os;
}


//
// SimpleArea

std::ostream&
SimpleArea::output(std::ostream& os, const Area& a) const
{
  Polygon p(a.boundary().stroke(bias),&a);
  return p.output(os);
}


//
// ComplexArea

std::ostream&
ComplexArea::output(std::ostream& os, const Area& a) const
{
  using namespace std;
  os<<"<path fill-rule=\"nonzero\""<<a.attributes()<<" d=\"";
  const std::list<Point> apoints =a.boundary().stroke(bias);
  output_path_data(os,apoints.begin(),apoints.end());
  std::list<Area> voids =a.enclosed_areas();
  for(list<Area>::const_iterator v=voids.begin(); v!=voids.end(); ++v)
  {
    os<<" ";
    const std::list<Point> vpoints =v->boundary().stroke(-bias);
    output_path_data(os,vpoints.rbegin(),vpoints.rend());
  }
  os<<"\"/>\n";
  return os;
}


//
// Skeleton

std::ostream&
Skeleton::output(std::ostream& os, const Area& a) const
{
  os<<"<path"<<a.attributes()<<" d=\"";
  const std::list<Boundary> bb =a.skeleton(this->include_boundary);
  for(std::list<Boundary>::const_iterator b=bb.begin(); b!=bb.end(); ++b)
  {
    const std::list<Edge*> edges =b->edges();
    assert(!edges.empty());
    os<<"M "<<edges.front()->start_point();
    for(std::list<Edge*>::const_iterator e=edges.begin(); e!=edges.end(); ++e)
        os<<"L "<<(**e).end_point();
  }
  os<<"\"/>\n";
  return os;
}


//
// BoundaryLine

std::ostream&
BoundaryLine::output(std::ostream& os, const Boundary& b) const
{
  os<<Poly(b.stroke(bias), b.is_closed(), &b);
  return os;
}


//
// PathLine

std::ostream&
PathLine::output(std::ostream& os, const Path& p) const
{
  const std::list<Hex*>& hexes =p.hexes();
  assert(!hexes.empty());
  if(hexes.size()>1) // Nothing to draw if there is only one hex in the path.
  {
    std::list<Point> points;
    for(std::list<Hex*>::const_iterator h =hexes.begin(); h!=hexes.end(); ++h)
        points.push_back( (**h).centre() );
    bool is_closed =( hexes.front()==hexes.back() );
    os<<Poly(points,is_closed,&p);
  }
  return os;
}


//
// Document

void
Document::header(std::ostream& os) const
{
  Distance width  =this->_grid.width();
  Distance height =this->_grid.height();
  Distance hmargin =0.05;
  Distance vmargin =0.05;
  os<<
    "<?xml version=\"1.0\" standalone=\"no\"?>\n"
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
      "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
    "<svg width=\"100%\" height=\"100%\" viewBox=\""
    "0 0 "<<(width+hmargin*2.0)<<" "<<(height+vmargin*2.0)<<
    "\" version=\"1.1\" "
      "xmlns=\"http://www.w3.org/2000/svg\">\n"

    "<defs>\n"
    "<marker id=\"Triangle\""
    " viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\" "
    " markerUnits=\"strokeWidth\""
    " markerWidth=\"4\" markerHeight=\"3\""
    " orient=\"auto\">\n"
    "<path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n"
    "</defs>\n" 

    "<g transform=\"translate("<<hmargin<<" "
      <<(height+vmargin)<<") scale(1 -1)\">\n"
  ;
}


void
Document::footer(std::ostream& os) const
{
  os<<"</g></svg>"<<std::endl;
}


std::ostream&
Document::output(std::ostream& os) const
{
  this->header(os);
  for(std::list<Element*>::const_iterator e =this->elements.begin();
                                          e!=this->elements.end();
                                        ++e)
  {
    (**e).output(os);
  }
  this->footer(os);
  return os;
}


std::string
Document::str(void) const
{
  std::ostringstream ss;
  this->output(ss);
  return ss.str();
}


} // end namespace svg
} // end namespace hex
