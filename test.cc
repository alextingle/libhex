#include <iostream>
#include <sstream>

#include "hex.h"
#include "hexsvg.h"

using namespace std;


int main()
{
  using namespace hex;
  hex::Grid g(10,10);
  hex::svg::Document d(g);

  hex::Area a =hex::Path(g.hex(1,1),g.hex(7,5)).to_area();

  hex::Boundary b =a.boundary();


  Area r =hex::range(g.hex(5,5),2);
  r.style="fill:#ddf; stroke:#99f; stroke-width:0.04";
  d.elements.push_back( new hex::svg::Single<hex::svg::ComplexArea>(r,+0.5) );

  cerr<<r.str()<<endl;

  std::list<hex::Area> areas;
  areas.push_back( hex::Path(g.hex(1,3),g.hex(7,8)).to_area() );
  a.style="stroke:red";
  areas.push_back( a );
  hex::svg::Group<hex::svg::ComplexArea>* sa =
    new hex::svg::Group<hex::svg::ComplexArea>(areas,-0.04);
  d.elements.push_back( sa );
  sa->style="fill:none; stroke:green; stroke-width:0.04";

  Area ga =g.to_area();
  ga.style="fill:none; stroke:gray; stroke-width:0.01";
  d.elements.push_back( new hex::svg::Single<hex::svg::Skeleton>(ga,true) );

  hex::Path p =g.path("3_1:ABCDBBAFE");
  p.style="fill:none;stroke:#9f9; stroke-width:0.08; marker-end:url(#Triangle)";
  d.elements.push_back( new hex::svg::Single<hex::svg::PathLine>(p) );
  
  list<Edge*> le =p.to_area().boundary().edges();//.complement();
  le.pop_back();
  Boundary pb(le);
//  pb =g.boundary( "4_2+DEFEFAFAFAFABABCBCDCDCDEFEDC" );
  pb.style="fill:none; stroke:#800; stroke-width:0.04; marker-end:url(#Triangle)";
  cerr<<pb.str()<<" "<<pb.is_closed()<<endl;
  d.elements.push_back( new hex::svg::Single<hex::svg::BoundaryLine>(pb,-0.3) );

  d.output(cout);

  return 0;
}
