untransformedCorners = [

// insert produced code here

[0, 0, 131.399],  // 0
[111.595, 66.9572, -30.25],  // 1
[-111.595, 66.9572, -30.25],  // 2
[111.595, -66.9572, -30.25],  // 3
[-111.595, -66.9572, -30.25],  // 4
[-30, 150, 45],  // 5

[0, 0, 0]];  // dummy


transformedCorners = [

// insert produced code here

[24.722, -123.61, -122.369],  // 0
[112.066, 64.6058, -125.997],  // 1
[-104.963, 33.7976, -84.0052],  // 2
[93.5807, 23.116, -251.973],  // 3
[-123.448, -7.69221, -209.981],  // 4
[0, 0, 0],  // 5

[0, 0, 0]];  // dummy


faces = [
    [0, 1, 2],
    [0, 2, 4],
    [0, 4, 3],
    [0, 3, 1],
    [1, 2, 4, 3] /*,
    [5, 0, 1],
    [5, 0, 2],
    [5, 1, 2]*/
];

polyhedron(untransformedCorners, faces);

boxSides = [

// insert box sides here

-123.448, 112.066, -123.61, 64.6058, 0, 251.973

];

/*
translate([boxSides[0], boxSides[2], -boxSides[5]])
cube([
    boxSides[1] - boxSides[0],
    boxSides[3] - boxSides[2],
    boxSides[5] - boxSides[4]
]);
*/

untransformedBackdropCoordinates = [
    [-95.0, -62.0, -1.0],
    [95.0, -62.0, -1.0],
    [95.0, 62.0, -21.0],
    [-95.0, 62.0, -21.0],
    [-95.0, -62.0, -1.001],
    [95.0, -62.0, -1.001],
    [95.0, 62.0, -21.001],
    [-95.0, 62.0, -21.001]
];
backdropFaces = [
    [0, 1, 2, 3],
    [4, 5, 6, 7],
    [0, 3, 7, 4],
    [2, 3, 7, 6],
    [1, 2, 6, 5],
    [0, 1, 5, 4]
];

polyhedron(
    untransformedBackdropCoordinates,
    backdropFaces
);
