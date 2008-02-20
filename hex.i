/*                            Package   : libhex
 * hex.i                      Created   : 2007/11/04
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

%module hex

%{
/* Includes the header in the wrapper code */
#include "hex.h"
#include "hexsvg.h"
#include "hexmove.h"
#include <sstream>
%}

%include <std_common.i>
%include <std_list.i>
%include <std_set.i>
%include <std_map.i>
%include <std_pair.i>
%include <std_except.i>
//%include <std_iostream.i>
%include <std_string.i>

/* Useful???
%include <exception.i>
%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}
*/

namespace std {
  %template(Style)        std::map<std::string,std::string>;
  %template(StringList)   std::list<std::string>;
  %template(Set)          std::set<hex::Hex*>;
  %template(HexList)      std::list<hex::Hex*>;
  %template(EdgeList)     std::list<hex::Edge*>;
  %template(AreaList)     std::list<hex::Area>;
  %template(PathList)     std::list<hex::Path>;
  %template(PointList)    std::list<hex::Point>;
  %template(BoundaryList) std::list<hex::Boundary>;
}

%ignore FIRETREE__HEX_H;
%ignore M_SQRT3;
%ignore hex::operator++;
%ignore hex::operator--;
%ignore hex::operator<<;
%ignore FIRETREE__HEXSVG_H;
%ignore hex::svg::operator<<;


%extend hex::Hex
{
  // Ensures that the == operator works correctly in Python.
  %pythoncode %{
    def __cmp__(self,other):
      return cmp( self.__hash__(), other.__hash__() )
    def __hash__(self):
      return (self.i << 14) | (self.j)
  %}
};
%extend hex::Edge
{
  // Ensures that the == operator works correctly in Python.
  %pythoncode %{
    def __cmp__(self,other):
      return cmp( self.__hash__(), other.__hash__() )
    def __hash__(self):
      return (hash(self.hex()) << 3) | self.direction()
  %}
};

// Lots of classes have str() member functions. This renames them so they work
// with the Python str operator.
%rename(__str__) str;

%feature("autodoc","1");

/* Parse the header file to generate wrappers */
%include "hex.h"
%include "hexsvg.h"
%include "hexmove.h"
