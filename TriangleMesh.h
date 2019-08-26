#ifndef _TRIANGLE_MESH_INCLUDE
#define _TRIANGLE_MESH_INCLUDE

#include <string>
#include <vector>
#include <deque>

#include <glm/glm.hpp>

#include "ShaderProgram.h"

#include "Ray.hpp"
#include "PointOctree.hpp"
#include "TrianglesOctree.hpp"


using namespace std;
// Class TriangleMesh renders a very simple room with textures
class TriangleMesh
{
public:
 	TriangleMesh();

	void addVertex(const glm::vec3 &position);
	void addTriangle(int v0, int v1, int v2);

	void buildCube();
    void buildTile();
    void buildDot();

    void Preprocess();

    TriangleMesh * VertexClustering(size_t depth);
    TriangleMesh * VertexClustering_GRID(size_t grid_size);

    int RayCast(Ray3D ray, bool use_triangles = true);
    void TestRaysPerSecond(size_t total_rays);

	void sendToOpenGL(ShaderProgram &program);
	void render(ShaderProgram &program) const;
    void renderPoints() const;
	void free();

private:
    

    vector<glm::vec3> vertices;
    vector<unsigned int> triangles;
    vector<glm::vec3> triangle_normals;
    vector<std::deque<int>> faces_per_vertex_;
    vector<glm::vec3> vertex_colors;
    vector<glm::vec3> vertex_normals;

    /* Bounding box info */
    GLfloat min_x, max_x, min_y, max_y, min_z, max_z;

    PointOctree<int, 1> * octree_vertices;
    TrianglesOctree<5, 15> * octree_triangles;

	GLuint vao;
    GLuint ebo;
	
    GLuint vbo_position;
    GLuint vbo_normals;
    GLint posLocation;
    GLint normalLocation;
};


#endif // _TRIANGLE_MESH_INCLUDE
