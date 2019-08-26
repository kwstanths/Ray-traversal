# Ray-traversal
Using octrees for 3D ray traversal

## Visualising ray traversal:
The octree traversal algorithm described here https://lsi.ugr.es/curena/inves/wscg00/revelles-wscg00.pdf brings back all the leaf nodes of the octree that the ray intersects with. 
In order to visualize the correctness of the traversal, I use the direction of the camera as a 3D ray and assign random colors to the elements stored in the first hit octree leaf node.

By using the points octree that holds the vertices of the 3D models, I assign a random color the all vertices stored in the first hit octree leaf node:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/vB--9PR1jiY/0.jpg)](https://www.youtube.com/watch?v=vB--9PR1jiY&list=PLocuszpm1snXxhpHqbCkDrBpiXojGBJCR)

By using the triangles octree that holds the triangles of the 3D models, I assign a random color to all the triangles stored in the first hit octree leaf node:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/E_dVn5CLNt8/0.jpg)](https://www.youtube.com/watch?v=E_dVn5CLNt8&list=PLocuszpm1snXxhpHqbCkDrBpiXojGBJCR&index=2)


