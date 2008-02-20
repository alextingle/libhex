#include <iostream>
#include <sstream>

#include "hex.h"
#include "hexsvg.h"
#include "hexmove.h"

using namespace std;


int main()
{
  using namespace hex;
  hex::Grid g(10,10);
  hex::svg::Document d(g);
  std::ostringstream os;
  hex::Point margin(0.5,0.5);
  d.p0 -= margin;
  d.p1 += margin;
  os<<d.header();

  hex::Area a =hex::Path(g.hex(1,1),g.hex(7,4)).to_area();

  hex::Boundary b =a.boundary();


  Area r =hex::range(g.hex(5,5),2);
  r.style="fill:#ddf; stroke:#99f; stroke-width:0.04";
  os<<d.draw_complex_area(r,+0.5);

//cerr<<r.str()<<endl;

  os<<"<g style=\"fill:none; stroke:green; stroke-width:0.04\">\n";
  float bias=0.04;
//  os<<d.draw_complex_area( hex::Path(g.hex(1,3),g.hex(7,8)).to_area(),bias);
  a.style="stroke:red";
  os<<d.draw_complex_area( a, bias );
  os<<"</g>\n";

  Area ga =g.to_area();
  ga.style="fill:none; stroke:gray; stroke-width:0.01";
  os<<d.draw_skeleton( ga, true );
/*
  hex::Path p =g.path("3_1:A");
  p.style="fill:none;stroke:#9f9; stroke-width:0.08; marker-end:url(#Triangle)";
  os<<d.draw_path( p );
  
  list<Edge*> le =p.to_area().boundary().edges();//.complement();
  le.pop_back();
  Boundary pb(le);
//  pb =g.boundary( "4_2+DEFEFAFAFAFABABCBCDCDCDEFEDC" );
  pb.style="fill:none; stroke:#800; stroke-width:0.04; marker-end:url(#Triangle)";
  cerr<<pb.str()<<" "<<pb.is_closed()<<endl;
  os<<d.draw_boundary( pb, -0.3 );
*/
  hex::move::Topography move(1);
  move.increase_cost(r,2);
  move.increase_cost(a,100);
cerr<<a.str()<<endl;
  hex::Path bp =move.best_path( g.hex(1,3),g.hex(8,5) );
  bp.style="fill:none;stroke:#33f; stroke-width:0.08; marker-end:url(#Triangle)";
  os<<d.draw_path(bp);
  hex::Area hz =move.horizon( g.hex(1,3), 3 );
  hz.style="fill:none;stroke:#33f; stroke-width:0.04; marker-end:url(#Triangle)";
  os<<d.draw_simple_area(hz);

  os<<d.footer();

  cout<<os.str()<<endl;

  return 0;
}
