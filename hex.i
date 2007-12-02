%module hex

%{
/* Includes the header in the wrapper code */
#include "hex.h"
#include "hexsvg.h"
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
  %template(Set)          std::set<hex::Hex*>;
  %template(HexList)      std::list<hex::Hex*>;
  %template(EdgeList)     std::list<hex::Edge*>;
  %template(AreaList)     std::list<hex::Area>;
  %template(PointList)    std::list<hex::Point>;
  %template(BoundaryList) std::list<hex::Boundary>;
  %template(ElementList)  std::list<hex::svg::Element*>;
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
      return (self.i * 10000) ^ (self.j)
  %}
};
%extend hex::svg::Document
{
  std::string header_str(void)
  {
    std::ostringstream ss;
    self->header(ss);
    return ss.str();
  }
  std::string footer_str(void)
  {
    std::ostringstream ss;
    self->footer(ss);
    return ss.str();
  }
};
%rename(_Document) Document;

// Lots of classes have str() member functions. This renames them so they work
// with the Python str operator.
%rename(__str__) str;

%feature("autodoc","1");

/* Parse the header file to generate wrappers */
%include "hex.h"
%include "hexsvg.h"

namespace hex {
  // Singles
  %template(BoundaryLineSingle) svg::Single<svg::BoundaryLine>;
  %template(ComplexAreaSingle)  svg::Single<svg::ComplexArea>;
  %template(PathLineSingle)     svg::Single<svg::PathLine>;
  %template(SimpleAreaSingle)   svg::Single<svg::SimpleArea>;
  %template(SkeletonSingle)     svg::Single<svg::Skeleton>;
  // Groups
  %template(BoundaryLineGroup)  svg::Group<svg::BoundaryLine>;
  %template(ComplexAreaGroup)   svg::Group<svg::ComplexArea>;
  %template(PathLineGroup)      svg::Group<svg::PathLine>;
  %template(SimpleAreaGroup)    svg::Group<svg::SimpleArea>;
  %template(SkeletonGroup)      svg::Group<svg::Skeleton>;
}

%pythoncode %{
class Document:
  '''Pure-Python implementation of the Document class.'''
  def __init__(self,grid):
      self.grid=grid
      self.document=_Document(grid)
      self.elements=[]
  def header(self):
      return self.document.header_str()
  def footer(self):
      return self.document.footer_str()
  def __str__(self):
      result = self.document.header_str()
      for e in self.elements:
        result += str(e)
      result += self.document.footer_str()
      return result
%}
