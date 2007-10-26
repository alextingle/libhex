#include "hexsvg.h"

#include <cassert>


namespace hex {
namespace svg {


std::string
to_string(const Style& st)
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


std::string
Element::attributes(void) const
  {
    std::string result ="";
    if(!style.empty())
        result += std::string(" style=\"") + to_string(style) + "\"";
    if(!className.empty())
        result += std::string(" class=\"") + className + "\"";
    return result;
  }


//
// Poly

Poly::Poly(const std::list<Point>& pp, bool closed)
  :_first(pp.begin()), _last(pp.end()), _closed(closed)
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
  os<<attributes()<<" points=\"";
  for(std::list<Point>::const_iterator pos=_first; pos!=_last; ++pos)
  {
    if(pos!=_first)
       os<<" ";
    os<<(*pos);
  }
  os<<"\"/>\n";
  return os;
}


//
// SimpleArea

SimpleArea::SimpleArea(const std::list<Area>& areas, float bias)
  :_areas(areas),_bias(bias)
{}


std::ostream&
SimpleArea::output(std::ostream& os) const
{
  os<<"<g"<<attributes()<<">\n";
  for(std::list<Area>::const_iterator a=_areas.begin(); a!=_areas.end(); ++a)
      os<<Polygon(a->boundary().stroke(_bias));
  os<<"</g>\n";
  return os;
}


//
// ComplexArea

ComplexArea::ComplexArea(const std::list<Area>& areas, float bias)
  :_areas(areas),_bias(bias)
{}


std::ostream&
ComplexArea::output(std::ostream& os) const
{
  using namespace std;
  os<<"<g fill-rule=\"nonzero\""<<attributes()<<">\n";
  for(list<Area>::const_iterator a=_areas.begin(); a!=_areas.end(); ++a)
  {
    os<<"<path d=\"";
    const std::list<Point> apoints =a->boundary().stroke(_bias);
    output_path_data(os,apoints.begin(),apoints.end());
    std::list<Area> voids =a->enclosed_areas();
    for(list<Area>::const_iterator v=voids.begin(); v!=voids.end(); ++v)
    {
      os<<" ";
      const std::list<Point> vpoints =v->boundary().stroke(-_bias); // ?? bias?
      output_path_data(os,vpoints.rbegin(),vpoints.rend());
    }
    os<<"\"/>\n";
  }
  os<<"</g>\n";
  return os;
}


} // end namespace svg
} // end namespace hex
