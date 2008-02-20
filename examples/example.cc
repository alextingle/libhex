#include <iostream>
#include <sstream>

// Import the hex:: namespace.
#include "hex.h"

// Import the hex::svg:: namespace.
#include "hexsvg.h"


using namespace std;


int main()
{
  // Make a 10x10 grid of hexagons. This is the universe in which our other
  // objects will live.
  hex::Grid grid(10,10);

  //
  // HEX - The single hex is our fundamental building block.

  // A single hex, at co-ordinate (1,1) - near the bottom left corner.
  hex::Hex* hex_1 = grid.hex(1,1);

  // Another hex, two steps to the right (direction A).
  hex::Hex* hex_2 = hex_1->go( hex::A, 2 );
  
  // And another, two steps up.
  //  You can express complicated moves as a string of "steps".
  hex::Hex* hex_3 = hex_2->go( "BC" );

 
  //
  // PATH - A sequence of adjacent hexes. A 'move' from one hex to another.
  
  // Let the library choose a path from hex_1 to hex_3.
  hex::Path path_from_1_to_3( hex_1, hex_3 );

  // Specify an exact path from hex_2.
  hex::Path path_from_2( hex_2, "AAFAAABCDD" );

  //
  // AREA - A connected set of hexes.

  // An area can be constructed from a single hex,
  hex::Area area_1( hex_1 );
  
  // ...or from a set of hexes,
  std::set<hex::Hex*> set_4;
  set_4.insert( grid.hex(1,5) );
  set_4.insert( grid.hex(1,6) );
  set_4.insert( grid.hex(2,5) );
  set_4.insert( grid.hex(2,6) );
  hex::Area area_4( set_4 );

  // ...or from all hexes within a certain distance.
  hex::Area area_5( grid.hex(6,6), 2 );


  //
  // SVG - DRAWING THE OBJECTS

  // Make an SVG document to represent the grid.
  hex::svg::Document doc(grid);
  cout << doc.header();


  // DRAWING HEXES

  // There is no way to draw a single hex. If you want to draw its outline, then
  // turn it into an area and draw that!


  // DRAWING AREAS
  
  // Areas are the most commonly drawn objects. Just set the SVG drawing-style.
  area_5.style="fill:lightblue; stroke:darkblue; stroke-width:0.04";
  cout << doc.draw_simple_area( area_5, +0.1 );

  // Now put a list of similar Areas into a Group.
  cout << "<g style=\"fill:pink; stroke:darkred; stroke-width:0.04\">\n";
  cout << doc.draw_simple_area( area_1, -0.1 );
  cout << doc.draw_simple_area( hex::Area(hex_2), -0.1 );
  cout << doc.draw_simple_area( hex::Area(hex_3), -0.1 );
  // You can over-ride styles for individual group members.
  area_4.style="fill:lightgreen; stroke:darkgreen";
  cout << doc.draw_simple_area( area_4, -0.1 );
  cout << "</g>\n";

  
  // DRAWING PATHS
  
  // Just use the draw_path() function to draw a Path.
  cout << "<g style=\"fill:none; stroke:black; stroke-width:0.08; marker-end:url(#Triangle)\">\n";
  cout << doc.draw_path(path_from_1_to_3);
  cout << doc.draw_path(path_from_2);
  cout << "</g>\n";
  

  // DRAWING THE GRID
  
  // The grid is just another Area, but we usually want to draw all of its
  // constituent Hexes, as a faint skeleton, on top of all the other
  // objects.

  hex::Area grid_area =grid.to_area();
  grid_area.style="fill:none; stroke:gray; stroke-width:0.01";
  cout << doc.draw_skeleton( grid_area );


  // Finally, write the SVG footer to standard output.

  cout << doc.footer();

  return 0;
}
