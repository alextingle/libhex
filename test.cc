#include <iostream>
#include <sstream>

#include "hex.h"
#include "hexsvg.h"

using namespace std;


ostream& operator<<(ostream& os, hex::Hex* h)
{
  if(h)
  {
    os<<"hex("<<h->i<<","<<h->j<<")";
  }
  else
  {
    os<<"hex(NULL)";
  }
  return os;
}

ostream& operator<<(ostream& os, hex::Edge* e)
{
  if(e)
  {
    os<<e->hex()<<"->"<<e->direction();
  }
  else
  {
    os<<"edge(NULL)";
  }
  return os;
}


double factor=40.0;


double X(hex::Distance x)
{
  return x;// * factor;
}
double Y(hex::Distance y)
{
  return y;//400.0 - y * factor;
}


ostream& operator<<(ostream& os, hex::Point p)
{
  os<<X(p.x)<<","<<Y(p.y);
  return os;
}


std::string polyline(
  const std::list<hex::Point>& points,
  const std::string&           col="black",
  double                          width=1.0
)
{
  std::ostringstream ss;
  ss<<"<polyline style=\"fill:none;stroke:"<<col<<";stroke-width:"<<width<<"\""
    " points=\"";
  for(std::list<hex::Point>::const_iterator p =points.begin();
                                            p!=points.end();
                                          ++p)
  {
    ss<<(*p)<<" ";
  }
  ss<<"\"/>\n";
  return ss.str();
}


std::string polylines(
  const std::list<hex::Boundary>& boundaries,
  const std::string&              col="black",
  double                          width=1.0
)
{
  std::string result;
  for(std::list<hex::Boundary>::const_iterator b =boundaries.begin();
                                               b!=boundaries.end();
                                             ++b)
  {
    result+=polyline(b->stroke(),col,width);
  }
  return result;
}


std::string circle(
  hex::Point          centre,
  hex::Distance       radius,
  const std::string&  col="black",
  double              width=1.0
)
{
  std::ostringstream ss;
  ss<<"<circle cx=\""<<X(centre.x)<<"\" cy=\""<<Y(centre.y)<<"\""
    " r=\""<<radius*factor<<"\""
    " stroke=\""<<col<<"\" stroke-width=\""<<width<<"\" fill=\"none\"/>";
  return ss.str();
}

std::string text(
  hex::Point          p,
  const std::string&  col="black",
  int                 width=1
)
{
  std::ostringstream ss;
  ss<<"<text x=\""<<X(p.x)<<"\" y=\""<<Y(p.y)<<"\">";
  return ss.str();
}


std::string _text()
{
  return "</text>";
}



int main()
{
  using namespace hex;
  hex::Grid g(10,10);

/*
  hex::Hex* h =g.hex(0,0);

  cout<<h<<endl;
  
  h=h->go(A);
  cout<<h<<endl;

  h=h->go(B);
  cout<<h<<endl;

  h=h->go(C);
  cout<<h<<endl;

  h=h->go(D);
  cout<<h<<endl;

  h=h->go(F);
  cout<<h<<endl;

  h=h->go(E);
  cout<<h<<endl;

  return 0;
  
  hex::Edge* e =&h->edge(E);

  e = e->next_out();
  cout<<e<<endl;

  e = e->next_out();
  cout<<e<<endl;

  cout<<h->go(hex::B)<<endl;

  return 0;
*/

/* 
  cout<<
    "<?xml version=\"1.0\" standalone=\"no\"?>\n"
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
      "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
    "<svg width=\"100%\" height=\"100%\" viewBox=\""
    "0 0 "<<g.width()+0.1<<" "<<g.height()+0.1<<
    "\" version=\"1.1\" "
      "xmlns=\"http://www.w3.org/2000/svg\">\n"
    "<g transform=\"translate(0.05 "<<g.height()+0.05<<") scale(1 -1)\">\n"
  ;
  for(int i=0; i<3; ++i)
    for(int j=0; j<3; ++j)
    {
      Hex* h =g.hex(i,j);
      cout<<circle(h->centre(),0.05)<<endl;
      cout<<text(h->centre())<<i<<","<<j<<_text()<<endl;
    }
*/

/*
  set<Hex*> s;
  Hex* h =g.hex(4,4);
  s.insert(h);
  s.insert(h->go(E));
  s.insert(h->go(C));
  s.insert(h->go(A));
  s.insert(h->go(A,2));
  s.insert(h->go(A,2)->go(B));
  

  hex::Area a =s;
*/

  hex::Area a =hex::Path(g.hex(1,1),g.hex(7,5)).to_area();

  cerr<<"a.size: "<<a.size()<<endl;
  hex::Boundary b =a.boundary();
  cerr<<"b.length: "<<b.length()<<endl;
  cerr<<"b.is_closed: "<<b.is_closed()<<endl;
  cerr<<"b.is_container: "<<b.is_container()<<endl;

  hex::svg::Document d(g);

    Area r =hex::range(g.hex(5,5),2);
    d.elements.push_back( new hex::svg::Single<hex::svg::ComplexArea>(r,+0.5) );
    r.style["fill"]="#ddf";
    r.style["stroke"]="#99f";
    r.style["stroke-width"]="0.04";

    std::list<hex::Area> areas;
    areas.push_back( hex::Path(g.hex(1,3),g.hex(7,8)).to_area() );
    a.style["stroke"]="red";
    areas.push_back( a );
    hex::svg::Group<hex::svg::ComplexArea>* sa =
      new hex::svg::Group<hex::svg::ComplexArea>(areas,-0.04);
    d.elements.push_back( sa );
    sa->style["fill"]="none";
    sa->style["stroke"]="green";
    sa->style["stroke-width"]="0.04";

//  cout<<polyline(b.stroke(-0.1),"red",0.04)<<endl;
//  cout<<polylines(g.to_area().skeleton(),"black",0.01)<<endl;

    Area ga =g.to_area();
    d.elements.push_back( new hex::svg::Single<hex::svg::Skeleton>(ga,true) );
    ga.style["fill"]="none";
    ga.style["stroke"]="gray";
    ga.style["stroke-width"]="0.01";
//    cout<<sk;

  hex::Path p(g.hex(3,7),g.hex(1,1));
  p.style["fill"]="none";
  p.style["stroke"]="#9f9";
  p.style["stroke-width"]="0.08";
  p.style["marker-end"]="url(#Triangle)";
  d.elements.push_back( new hex::svg::Single<hex::svg::PathLine>(p) );

//  cout<<"</g></svg>"<<endl;
  d.output(cout);

  return 0;
}
