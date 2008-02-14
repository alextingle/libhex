/*                            Package   : libhex
 * hexsvg.h                   Created   : 2007/10/25
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

#ifndef FIRETREE__HEXSVG_H
#define FIRETREE__HEXSVG_H 1


#include "hex.h"


namespace hex {

/** Output hex:: objects as Scalable Vector Graphics (SVG) - which you can
 *  easily convert to PNG or JPG.
 */
namespace svg {


typedef std::map<std::string,std::string> Style;

/** Render a style dictionary to SVG. */
std::string style_str(const Style& st);

/** Construct a style dictionary from an SVG string. */
Style style_dict(const std::string& s) throw(hex::invalid_argument);



inline std::ostream&
operator<<(std::ostream& os, hex::Point p)
{
  os<<p.x<<","<<p.y;
  return os;
}


//
// Document

class Document
{
  const Grid&  _grid;
public:
  Point                   p0; ///< Lower left corner of the document.
  Point                   p1; ///< Upper right corner of the document.
  std::list<std::string>  stylesheets; ///< List of stylesheets to import.
  std::list<std::string>  defs; ///< Fragments insert into the <defs> element.

  Document(const Grid& grid);

  /** Transform Point p into the document's co-ordinate system. */
  Point T(const Point& p) const;

  std::string  header(void) const;
  std::string  footer(void) const;

  /** Draws the boundary around an area, without worrying about voids within. */
  std::string draw_simple_area(const Area& a, float bias =0.0);
  
  /** Draws the boundary around an area, taking account of voids within. */
  std::string draw_complex_area(const Area& a, float bias =0.0);
  
  std::string draw_skeleton(const Area& a, bool include_boundary =true);
  
  /** Draws a boundary line. */
  std::string draw_boundary(const Boundary& b, float bias =0.0);
  
  /** Draws a path line. */
  std::string draw_path(const Path& p);
  
  /** Series of points, rendered as an SVG polygon, or polyline, depending upon
   *  whether they are closed or not. */
  std::string
  draw_poly(
    std::list<Point> points,
    bool closed,
    const Identity* identity =NULL
  );
};


} // end namespace svg
} // end namespace hex

#endif
