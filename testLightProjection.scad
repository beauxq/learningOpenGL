transformedCorners = [

// insert produces code here

[0.752576, -3.76289, -158.323],  // 0
[13.8556, 24.4725, -158.868],  // 1
[-18.7024, 19.8508, -152.568],  // 2
[11.0825, 18.2484, -177.766],  // 3
[-21.4754, 13.6266, -171.467],  // 4
[0, 0, 0],  // 5

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

-21.4754, 13.8556, -3.76289, 24.4725, 0, 177.766

];

/*
translate([boxSides[0], boxSides[2], -boxSides[5]])
cube([
    boxSides[1] - boxSides[0],
    boxSides[3] - boxSides[2],
    boxSides[5] - boxSides[4]
]);
*/
