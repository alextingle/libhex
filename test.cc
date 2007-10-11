#include <iostream>

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

ostream& operator<<(ostream& os, hex::Point p)
{
  os<<p.x<<","<<p.y;
  return os;
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

  set<Hex*> s;
  Hex* h =g.hex(5,5);
  s.insert(h);
  s.insert(h->go(E));
  s.insert(h->go(C));
  s.insert(h->go(A));
  

  hex::Area a =s;
  cout<<"a.size: "<<a.size()<<endl;
  hex::Boundary b =a.boundary();
  cout<<"b.length: "<<b.length()<<endl;
  cout<<"b.is_closed: "<<b.is_closed()<<endl;
  cout<<"b.is_container: "<<b.is_container()<<endl;
 
  std::list<Point> edges = b.stroke();
  for(std::list<Point>::const_iterator e=edges.begin(); e!=edges.end(); ++e)
    cout<<*e<<endl;

  return 0;
}
