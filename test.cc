#include <iostream>
#include <sstream>

#include "hex.h"

using namespace std;


ostream& operator<<(ostream& os, hex::Hex* h)
{
  if(h)
  {
    os<<"hex("<<h->i()<<","<<h->j()<<")";
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
  return x * factor;
}
double Y(hex::Distance y)
{
  return 400.0 - y * factor;
}


ostream& operator<<(ostream& os, hex::Point p)
{
  os<<X(p.x)<<","<<Y(p.y);
  return os;
}


std::string polyline(
  const std::list<hex::Point>& points,
  const std::string&           col="black",
  int                          width=1
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
  int                             width=1
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
  int                 width=1
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

  cout<<
    "<?xml version=\"1.0\" standalone=\"no\"?>\n"
    "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
      "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
    "<svg width=\"100%\" height=\"100%\" version=\"1.1\" "
      "xmlns=\"http://www.w3.org/2000/svg\">\n";
/* 
  for(int i=0; i<3; ++i)
    for(int j=0; j<3; ++j)
    {
      Hex* h =g.hex(i,j);
      cout<<circle(h->centre(),0.05)<<endl;
      cout<<text(h->centre())<<i<<","<<j<<_text()<<endl;
    }
*/

 
  set<Hex*> s;
  Hex* h =g.hex(4,4);
  s.insert(h);
  s.insert(h->go(B));
//  s.insert(h->go(C));
//  s.insert(h->go(A));
  

  hex::Area a =s;
  cerr<<"a.size: "<<a.size()<<endl;
  hex::Boundary b =a.boundary();
  cerr<<"b.length: "<<b.length()<<endl;
  cerr<<"b.is_closed: "<<b.is_closed()<<endl;
  cerr<<"b.is_container: "<<b.is_container()<<endl;
 
  cout<<polyline(b.stroke(-0.1),"red",4)<<endl;
  cout<<polylines(a.skeleton())<<endl;

  cout<<"</svg>"<<endl;

  return 0;
}
