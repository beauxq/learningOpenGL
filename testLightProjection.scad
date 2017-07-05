transformedCorners = [

// insert produces code here

[0, 0, 93.0992],  // 0
[85.1548, 51.0929, -30.25],  // 1
[-85.1548, 51.0929, -30.25],  // 2
[85.1548, -51.0929, -30.25],  // 3
[-85.1548, -51.0929, -30.25],  // 4
[-30, 150, 45],  // 5

[0, 0, 0]];  // dummy

faces = [
    [0, 1, 2],
    [0, 2, 4],
    [0, 4, 3],
    [0, 3, 1],
    [1, 2, 4, 3],
    [5, 0, 1],
    [5, 0, 2],
    [5, 1, 2]
];

polyhedron(transformedCorners, faces);

boxSides = [

// insert box sides here

-96.7863, 85.4036, -89.1795, 56.4211, 0, 232.958

];

/*
translate([boxSides[0], boxSides[2], -boxSides[5]])
cube([
    boxSides[1] - boxSides[0],
    boxSides[3] - boxSides[2],
    boxSides[5] - boxSides[4]
]);
*/

backdropCoordinates = [
    -95.0, -62.0, -1.0,
    95.0, -62.0, -1.0,
    -95.0, 62.0, -21.0,
    95.0, -62.0, -1.0,
    95.0, 62.0, -21.0,
    -95.0, 62.0, -21.0
];