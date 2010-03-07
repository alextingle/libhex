/*                            Package   : libhex
 * boundingbox.cc             Created   : 2008/02/24
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

#include "hex.h"

#include <cmath>
#include <limits>

namespace hex {


std::set<Hex*>
BoundingBox::hexes(void) const
{
  // FIXME: Correct this algorithm so that it only yields hexes that are
  // actually in the bbox - and includes all of the hexes that ARE.
  std::set<Hex*> result;
  for(int i=hex0->i; i<=hex1->i; ++i)
    for(int j=hex0->j; j<=hex1->j; ++j)
      result.insert(hex0->grid().hex(i,j));
  return result;
}


void BoundingBox::frame(void)
{
  point0.x -= (hex::I / 2.0);
  point0.y -= hex::K;
  point1.x += (hex::I / 2.0);
  point1.y += hex::K;
}


BoundingBox::BoundingBox(
    const Point& point0_, const Point& point1_,
    Hex* const hex0_, Hex* const hex1_
  )
  : point0( point0_ ),
    point1( point1_ ),
    hex0( hex0_ ),
    hex1( hex1_ )
{}


BoundingBox::BoundingBox(const Grid& grid, bool frame_)
{
  Hex* h0 = grid.hex(0,0);
  Hex* h1 = grid.hex( grid.cols()-1, grid.rows()-1 );
  *this = BoundingBox(h0,h1,frame_);
}


BoundingBox::BoundingBox(const std::set<Hex*>& a, bool frame_)
  throw (hex::invalid_argument)
{
  if(a.empty())
      throw hex::invalid_argument("BoundingBox(<empty set>)");
  int i0 = std::numeric_limits<int>::max();
  int j0 = std::numeric_limits<int>::max();
  int i1 = std::numeric_limits<int>::min();
  int j1 = std::numeric_limits<int>::min();
  for(std::set<Hex*>::const_iterator h=a.begin(); h!=a.end(); ++h)
  {
    i0 = std::min(i0,(**h).i);
    j0 = std::min(j0,(**h).j);
    i1 = std::max(i1,(**h).i);
    j1 = std::max(j1,(**h).j);
  }
  const Grid& g = (**a.begin()).grid();
  Hex* hex0 = g.hex(i0,j0);
  Hex* hex1 = g.hex(i1,j1);
  *this = BoundingBox(hex0,hex1,frame_);
}


BoundingBox::BoundingBox(const Grid& grid, const Point& p0, const Point& p1)
{
  int i0 = std::max( 0, int(p0.x)-1 ); // Calculate the boundary hexes.
  int j0 = std::max( 0, int(p0.y / hex::J) );
  int i1 = std::min( grid.cols()-1, int( ceil(p1.x) ) );
  int j1 = std::min( grid.rows()-1, int( ceil(p1.y / hex::J) ) );
  *this = BoundingBox(p0, p1, grid.hex(i0,j0), grid.hex(i1,j1));
}


BoundingBox::BoundingBox(Hex* hex0_, Hex* hex1_, bool frame_)
{
  Point p0 =hex0_->centre();
  Point p1 =hex1_->centre();
  if(hex1_->j != hex0_->j)
  {
    if( hex1_->j % 2 )
      p1.x += (hex::I/2.0);
    if( (hex0_->j % 2)==0 )
      p1.x += (hex::I/2.0);
  }
  *this = BoundingBox(p0, p1, hex0_, hex1_);
  if(frame_)
      frame();
}


} // end namespace hex
