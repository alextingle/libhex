#include "hex.h"

namespace hex {


Direction& operator+=(Direction& d, int i)
{
  int d_ =static_cast<int>( d );
  d_= (d_+i) % DIRECTIONS;
  while(d_<0)
    d_ += DIRECTIONS;
  d=static_cast<Direction>( d_ );
  return d;
}

Direction operator+(const Direction& d, int i)
{
  Direction result =d;
  result+=i;
  return result;
}

/** Prefix increment */
Direction& operator++(Direction& d)
{
  return d+=1;
}

/** Postfix increment */
Direction operator++(const Direction& d,int)
{
  Direction result =d;
  ++result;
  return result;
}

Direction& operator-=(Direction& d, int i)
{
  return d += (-i);
}

Direction operator-(const Direction& d, int i)
{
  return d + (-i);
}

/** Prefix decrement */
Direction& operator--(Direction& d)
{
  return d-=1;
}

/** Postfix decrement */
Direction operator--(const Direction& d,int)
{
  Direction result =d;
  --result;
  return result;
}

std::ostream& operator<<(std::ostream& os, const Direction& d)
{
  static const char root =static_cast<char>( A );
  os << static_cast<char>( 'A' + (static_cast<char>(d) - root) );
  return os;
}


} // end namespace hex
