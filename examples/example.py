import hex

def main():
  # Make a 10x10 grid of hexagons. This is the universe in which our other
  # objects will live.
  grid = hex.Grid(10,10)

  #
  # HEX - The single hex is our fundamental building block.

  # A single hex, at co-ordinate (1,1) - near the bottom left corner.
  hex_1 = grid.hex(1,1)

  # Another hex, two steps to the right (direction A).
  hex_2 = hex_1.go( hex.A, 2 )
  
  # And another, two steps up.
  #  You can express complicated moves as a string of "steps".
  hex_3 = hex_2.go( "BC" )

 
  #
  # PATH - A sequence of adjacent hexes. A 'move' from one hex to another.
  
  # Let the library choose a path from hex_1 to hex_3.
  path_from_1_to_3 = hex.Path( hex_1, hex_3 )

  # Specify an exact path from hex_2.
  path_from_2 = hex.Path( hex_2, "AAFAAABCDD" )

  #
  # AREA - A connected set of hexes.

  # An area can be constructed from a single hex,
  area_1 = hex.Area( hex_1 )
  
  # ...or from a set of hexes,
  set_4 = []
  set_4.append( grid.hex(1,5) )
  set_4.append( grid.hex(1,6) )
  set_4.append( grid.hex(2,5) )
  set_4.append( grid.hex(2,6) )
  area_4 = hex.Area( set_4 )

  # ...or from all hexes within a certain distance.
  area_5 = hex.Area( grid.hex(6,6), 2 )


  #
  # SVG - DRAWING THE OBJECTS

  # Make an SVG document to represent the grid.
  doc = hex.Document(grid)


  # DRAWING HEXES

  # There is no way to draw a single hex. If you want to draw its outline, then
  # turn it into an area and draw that!


  # DRAWING AREAS
  
  # Areas are the most commonly drawn objects. Just set the SVG drawing-style,
  area_5.style="fill:lightblue; stroke:darkblue; stroke-width:0.04"
  # ...put the area into an adapter,
  area_5_element = hex.SimpleAreaSingle( area_5, hex.SimpleArea(+0.1) )
  # ...and add it to the document.
  doc.elements.append( area_5_element )

  # Similar areas may be grouped together. Start with a list of Areas.
  area_list = []
  area_list.append( area_1 )
  area_list.append( hex.Area(hex_2) )
  area_list.append( hex.Area(hex_3) )
  # You can over-ride styles for individual group members.
  area_4.style="fill:lightgreen; stroke:darkgreen"
  area_list.append( area_4 )
  # Now put the list of Areas into a Group.
  area_group = hex.SimpleAreaGroup( area_list, hex.SimpleArea(-0.1) )
  # The default style for all group members is set here.
  area_group.style="fill:pink; stroke:darkred; stroke-width:0.04"
  doc.elements.append( area_group )

  
  # DRAWING PATHS
  
  # Just use the PathLine adapter to draw a Path.
  path_list = []
  path_list.append(path_from_1_to_3)
  path_list.append(path_from_2)
  path_group = hex.PathLineGroup( path_list )
  path_group.style="fill:none; stroke:black; stroke-width:0.08; marker-end:url(#Triangle)"
  doc.elements.append( path_group )
  

  # DRAWING THE GRID
  
  # The grid is just another Area, but we usually want to draw all of its
  # constituent Hexes, as a faint skeleton, on top of all the other
  # objects.

  grid_area = grid.to_area()
  grid_area.style="fill:none; stroke:gray; stroke-width:0.01"
  skeleton = hex.SkeletonSingle(grid_area,hex.Skeleton(True))
  doc.elements.append( skeleton )


  # Finally, write the SVG to standard output.

  print doc


if __name__=='__main__':
  main()
