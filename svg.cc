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
#include <fstream>
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


/** Helper function - append a new relative point to a path.
 *  @param cmd   in/out current command letter
 *  @param p     in/out current path point
 *  @param cmd1  desired command letter
 *  @param p1    desired point
 */
std::string
path_append(char& cmd, Point& p, char cmd1, const Point& p1)
{
  std::ostringstream ss;
  if(p != p1)
  {
    if(cmd != cmd1)
    {
      ss<<" "<<cmd1;
      cmd=cmd1;
    }
    ss<<" "<<(p1-p);
    p=p1;
  }
  return ss.str();
}


/** Helper function. */
template<class InputIterator>
std::ostream&
output_path_data(
    const Document&  doc,
    std::ostream&    os,
    InputIterator    first,
    InputIterator    last
  )
{
  assert(first!=last);
  --last;
  os<<"M "<<doc.T(*first)<<" L";
  for(InputIterator p =++first; p!=last; ++p)
      os<<" "<<doc.T(*p);
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
// Document

Point
Document::T(const Point& p) const
{
  return Point( p.x - p0.x, p1.y - p.y );
}


std::string
Document::header() const
{
  std::ostringstream os;
  os<<
    "<?xml version=\"1.0\" standalone=\"no\"?>\n"
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
      "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
#if defined(HEX_EXTERNAL_CSS) && HEX_EXTERNAL_CSS!=0
  for(std::list<std::string>::const_iterator s =this->stylesheets.begin();
                                             s!=this->stylesheets.end();
                                           ++s)
  {
    os<<"<?xml-stylesheet href=\""<<(*s)<<"\" type=\"text/css\"?>\n";
  }
#endif
  Point extent =p1-p0;
  os<<
    "<svg width=\"100%\" height=\"100%\" viewBox=\""
    << 0L <<" "<< 0L << " " << extent.x <<" "<< extent.y <<
    "\" version=\"1.1\""
      " xmlns=\"http://www.w3.org/2000/svg\""
      " xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n"

    "<defs>\n";
#if !defined(HEX_EXTERNAL_CSS) || HEX_EXTERNAL_CSS==0
  if(!this->stylesheets.empty())
  {
    os<<"<style type=\"text/css\"><![CDATA[\n";
    for(std::list<std::string>::const_iterator s =this->stylesheets.begin();
                                               s!=this->stylesheets.end();
                                             ++s)
    {
      std::ifstream css(s->c_str());

      // Copy the css file to os.
      if(css)
      {
        char buf[1024];
        std::streamsize total =0;
        while(css.good())
        {
          std::streamsize bytes =css.readsome(buf,sizeof(buf));
          if(!bytes)
            break;
          total+=bytes;
          os.write(buf,bytes);
        }
      }
    }
    os<<"]]></style>\n";
  }
#endif
  os<<
    "<marker id=\"Triangle\""
    " viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\" "
    " markerUnits=\"strokeWidth\""
    " markerWidth=\"4\" markerHeight=\"3\""
    " orient=\"auto\">\n"
    "<path d=\"M 0 0 L 10 5 L 0 10 z\" />\n"
    "</marker>\n";
  for(std::list<std::string>::const_iterator d =this->defs.begin();
                                             d!=this->defs.end();
                                           ++d)
  {
    os<<(*d)<<"\n";
  }
  os<<"</defs>\n";
  return os.str();
}


std::string
Document::footer(void) const
{
  return "</svg>\n";
}


//
// Draw functions

std::string
Document::draw_simple_area(const Area& a, float bias)
{
  return draw_poly(a.boundary().stroke(bias),true,&a);
}


std::string
Document::draw_complex_area(const Area& a, float bias)
{
  using namespace std;
  std::ostringstream os;
  os<<"<path fill-rule=\"nonzero\""<<a.attributes()<<" d=\"";
  const std::list<Point> apoints =a.boundary().stroke(bias);
  output_path_data(*this,os,apoints.begin(),apoints.end());
  std::list<Area> voids =a.enclosed_areas();
  for(list<Area>::const_iterator v=voids.begin(); v!=voids.end(); ++v)
  {
    os<<" ";
    const std::list<Point> vpoints =v->boundary().stroke(-bias);
    output_path_data(*this,os,vpoints.rbegin(),vpoints.rend());
  }
  os<<"\"/>\n";
  return os.str();
}


std::string
Document::draw_skeleton(const Area& a, bool include_boundary)
{
  std::ostringstream os;
  os<<"<path"<<a.attributes()<<" d=\"";
  const std::list<Boundary> bb =a.skeleton(include_boundary);
  Point curr =T( bb.front().edges().front()->start_point() );
  os<<"M "<<curr;
  char cmd ='\0';
  for(std::list<Boundary>::const_iterator b=bb.begin(); b!=bb.end(); ++b)
  {
    const std::list<Edge*> edges =b->edges();
    assert(!edges.empty());
    os<<path_append(cmd,curr,'m',T( edges.front()->start_point() ));
    for(std::list<Edge*>::const_iterator e=edges.begin(); e!=edges.end(); ++e)
    {
      os<<path_append(cmd,curr,'l',T( (**e).end_point() ));
    }
  }
  os<<"\"/>\n";
  return os.str();
}


std::string
Document::draw_boundary(const Boundary& b, float bias)
{
  return draw_poly(b.stroke(bias), b.is_closed(), &b);
}


std::string
Document::draw_path(const Path& p)
{
  std::ostringstream os;
  const std::list<Hex*>& hexes =p.hexes();
  assert(!hexes.empty());
  if(hexes.size()>1) // Nothing to draw if there is only one hex in the path.
  {
    std::list<Point> points;
    for(std::list<Hex*>::const_iterator h =hexes.begin(); h!=hexes.end(); ++h)
        points.push_back( (**h).centre() );
    bool is_closed =( hexes.front()==hexes.back() );
    os<<draw_poly(points,is_closed,&p);
  }
  return os.str();
}


std::string
Document::draw_poly(
    std::list<Point>  points,
    bool              closed,
    const Identity*   identity
  )
{
  assert(!points.empty());
  std::ostringstream os;
  if(closed)
      points.pop_back();
  if(closed)
      os<<"<polygon";
  else
      os<<"<polyline";
  if(identity)
      os<<identity->attributes();
  os<<" points=\"";
  for(std::list<Point>::const_iterator p=points.begin(); p!=points.end(); ++p)
  {
    if(p!=points.begin())
       os<<" ";
    os<<T(*p);
  }
  os<<"\"/>\n";
  return os.str();
}


// Construction

Document::Document(const Grid& grid)
  : _grid(grid),
    p0(0.0,0.0),
    p1(grid.width(),grid.height()), 
    stylesheets(),
    defs()
{}


} // end namespace svg
} // end namespace hex
