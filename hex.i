%module hex

%{
/* Includes the header in the wrapper code */
#include "hex.h"
#include "hexsvg.h"
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
}

%ignore FIRETREE__HEX_H;
%ignore M_SQRT3;
%ignore hex::operator++;
%ignore hex::operator--;
%ignore hex::operator<<;
%ignore FIRETREE__HEXSVG_H;
%ignore hex::svg::operator<<;

/*
%extend hex::Hex
{
  std::string __str__(void)
  {
    std::ostringstream ss;
    ss<<"hex("<< self->i <<","<< self->j <<")";
    return ss.str();
  }
};
*/

%rename(__str__) str;

%feature("autodoc","1");

/* Parse the header file to generate wrappers */
%include "hex.h"
%include "hexsvg.h"
