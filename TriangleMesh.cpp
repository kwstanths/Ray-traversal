#include "TriangleMesh.h"

#include <iostream>
#include <algorithm>
#include <ctime>
#include <deque>
#include <vector>
#include <unordered_map>
#include <ctime>

#include "MersenneTwister.hpp"
#include "UniformGrid.hpp"

static MersenneTwisterGenerator rng(MersenneTwisterGenerator::ONE);

float get_rand() {
    return rng.genrand_real3();
}

using namespace std;


TriangleMesh::TriangleMesh() {
    
}

void TriangleMesh::addVertex(const glm::vec3 &position)
{
    /* Add position to mesh, add constant color */
    vertices.push_back(position);
    vertex_colors.push_back(glm::vec3(0.3, 0.3, 0.3));
}

void TriangleMesh::addTriangle(int v0, int v1, int v2)
{
  triangles.push_back(v0);
  triangles.push_back(v1);
  triangles.push_back(v2);
}

void TriangleMesh::buildCube() {

    /* cube's half size */
    float hs = 0.5;
	float vertices[] = {-hs, -hs, -hs,
                      hs, -hs, -hs,
                      hs,  hs, -hs,
                      -hs,  hs, -hs,
                      -hs, -hs,  hs,
                      hs, -hs,  hs,
                      hs,  hs,  hs,
                      -hs,  hs,  hs };

	int faces[] = {3, 1, 0, 3, 2, 1,
                 5, 6, 7, 4, 5, 7,
                 7, 3, 0, 0, 4, 7,
                 1, 2, 6, 6, 5, 1,
                 0, 1, 4, 5, 4, 1,
                 2, 3, 7, 7, 6, 2 };

	int i;

	for(i=0; i<8; i+=1)
		addVertex(glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	for(i=0; i<12; i++)
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

void TriangleMesh::buildTile() {
    float vertices[] = { -1, 0, -1,
                      1, 0, -1,
                      1,  0, 1,
                      -1,  0, 1
    };

    int faces[] = { 0, 2, 1, 0, 3, 2 };

    int i;

    for (i = 0; i < 4; i += 1)
        addVertex(0.5f * glm::vec3(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]));
    for (i = 0; i < 2; i++)
        addTriangle(faces[3 * i], faces[3 * i + 1], faces[3 * i + 2]);
}

void TriangleMesh::buildDot() {

}

void TriangleMesh::Preprocess() {

    glm::vec3 a(1, 2, 3);
    glm::vec3 b(3.1, 4.2, 3.3);
    float c = glm::dot(a, b);

    /*  Calculate bounding box of the mesh */
    min_x = max_x = vertices[0].x;
    min_y = max_y = vertices[0].y;
    min_z = max_z = vertices[0].z;
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].x < min_x) min_x = vertices[i].x;
        if (vertices[i].x > max_x) max_x = vertices[i].x;
        if (vertices[i].y < min_y) min_y = vertices[i].y;
        if (vertices[i].y > max_y) max_y = vertices[i].y;
        if (vertices[i].z < min_z) min_z = vertices[i].z;
        if (vertices[i].z > max_z) max_z = vertices[i].z;
    }

    /* Calculate faces per vertex */
    faces_per_vertex_ = std::vector<std::deque<int> >(vertices.size());
    for (int i = 0; i < triangles.size(); i += 3) {
        int v0 = triangles[i];
        int v1 = triangles[i + 1];
        int v2 = triangles[i + 2];

        faces_per_vertex_[v0].push_back(i / 3);
        faces_per_vertex_[v1].push_back(i / 3);
        faces_per_vertex_[v2].push_back(i / 3);
    }

    /* Calculate nornals per face */
    for (unsigned int tri = 0; tri < triangles.size(); tri += 3) {
        glm::vec3 normal;

        normal = glm::cross(vertices[triangles[tri + 1]] - vertices[triangles[tri]],
            vertices[triangles[tri + 2]] - vertices[triangles[tri]]);
        normal = glm::normalize(normal);
        triangle_normals.push_back(normal);
    }

    /* 
        Calculate normal per vertex. We have to do this because
        the original models do not have normals stored. Because 
        of this normal averaging, the simplified models look very
        "smooth"
    */
    for (size_t i = 0; i < vertices.size(); i++) {
        glm::vec3 normal = glm::vec3(0, 0, 0);
        for (int face = 0; face < faces_per_vertex_[i].size(); face++) {
            normal += triangle_normals[faces_per_vertex_[i][face]];
        }
        vertex_normals.push_back(glm::vec3(normal.x / faces_per_vertex_[i].size(), normal.y / faces_per_vertex_[i].size(), normal.z / faces_per_vertex_[i].size()));
    }

    /*
        Use bounding box information to calculate the octree region. add some delta to
        make sure that ray casting works for vertices at the edges
    */
    float octree_origin = std::min(std::min(min_x, min_y), min_z) - 0.2f;
    float octree_length = std::max(std::max(max_x - octree_origin, max_y - octree_origin), max_z - octree_origin) + 0.2f;

    /*
        Makre sure that the octree region is uniform along all sides
        and covers all sides. I assume here that meshes go from negative
        values to positive, meaning that they are "centered in the origin
    */
    octree_origin = std::min(octree_origin, octree_length / 2.0f);
    octree_length = -1.0f * 2.0f * octree_origin;

    {
        /* Measure octree creation time */
        std::clock_t start = clock_t();
        
        octree_vertices = new PointOctree<int, 1>(Point3D({ octree_origin, octree_origin, octree_origin }), octree_length);

        /* Insert all vertices to octree */
        for (size_t v = 0; v < vertices.size(); v++) {
            octree_vertices->Insert(Point3D({ vertices[v].x, vertices[v].y, vertices[v].z }), v);
        }
        std::cout << "Vertices octree depth: " << octree_vertices->Depth() << std::endl;

        std::clock_t end = clock();
        double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
        std::cout << "Vertices octree creation time: " << elapsed_secs << std::endl;

    }

    {
        std::clock_t start = clock();
        octree_triangles = new TrianglesOctree<5, 15>(Point3D({ octree_origin, octree_origin, octree_origin }), octree_length);

        for (size_t i = 0; i < triangles.size() / 3; i++) {
            octree_triangles->Insert(vertices, triangles, i);
        }
        std::cout << "Triangles octree depth: " << octree_triangles->Depth() << std::endl;

        std::clock_t end = clock();
        double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
        std::cout << "Triangles octree creation time: " << elapsed_secs << std::endl;

    }
}

int TriangleMesh::RayCast(Ray3D ray, bool use_triangles) {

    if (use_triangles) {
        std::vector<unsigned int> results;
        octree_triangles->RayCast(vertices, triangles, ray, results);

        for (size_t i = 0; i < results.size(); i++) {
            vertex_colors[triangles[results[i] * 3]] = glm::vec3(get_rand(), get_rand(), get_rand());
            vertex_colors[triangles[results[i] * 3 + 1]] = glm::vec3(get_rand(), get_rand(), get_rand());
            vertex_colors[triangles[results[i] * 3 + 2]] = glm::vec3(get_rand(), get_rand(), get_rand());
        }

        return results.size() > 0;
    }

    std::vector<int> results;
    octree_vertices->RayCast(ray, results);
    
    /* If results has elements, change the color of the first vertex to a random one */
    if (results.size() > 0) vertex_colors[results[0]] = glm::vec3(get_rand(), get_rand(), get_rand());
    
    return results.size() > 0;

}

TriangleMesh * TriangleMesh::VertexClustering(size_t depth) {

    std::clock_t start = clock();

    std::vector<std::vector<int>> clusters;
    octree_vertices->Cluster(depth, clusters);

    TriangleMesh * new_mesh = new TriangleMesh();
    
    /* Do vertex clustering */
    /* Holds the mapping between old and new vertices */
    std::unordered_map<int, int> old_vertices_to_new_vertices;
    
    /* Create new vertices. one for each cluster using averaging */
    int new_vertex_id = 0;
    for (size_t i = 0; i < clusters.size(); i++) {
        std::vector<int>& c = clusters[i];

        glm::vec3 mean = glm::vec3(0, 0, 0);
        for (size_t c_i = 0; c_i < c.size(); c_i++) {
            int vertex_id = c[c_i];

            mean += vertices[vertex_id];
            /* Set the mapping between old and new vertices */
            old_vertices_to_new_vertices[vertex_id] = new_vertex_id;
        }
        
        if (c.size() != 0) {
            mean.x = mean.x / c.size();
            mean.y = mean.y / c.size();
            mean.z = mean.z / c.size();
            new_mesh->addVertex(mean);
            new_vertex_id++;
        }
    }

    /* Create new triangles */
    size_t new_triangles = 0;
    for (size_t i = 0; i < triangles.size(); i += 3) {
        /* Find the old vertices that formed this old triangle */
        int old_vertex_v0 = triangles[i];
        int old_vertex_v1 = triangles[i + 1];
        int old_vertex_v2 = triangles[i + 2];

        /* Find the new vertex represenative for each one of  old vertices */
        int new_vertex_v0 = old_vertices_to_new_vertices[old_vertex_v0];
        int new_vertex_v1 = old_vertices_to_new_vertices[old_vertex_v1];
        int new_vertex_v2 = old_vertices_to_new_vertices[old_vertex_v2];

        /* 
            If the all three of the old vertices have fell different representative,
            that means that they into a different cluster and we have a proper triangle
            If two of them are the same, we have a line, if all of them are the same
            we have a point
        */
        if (new_vertex_v0 != new_vertex_v1 && new_vertex_v1 != new_vertex_v2 && new_vertex_v0 != new_vertex_v2) {
            new_mesh->addTriangle(new_vertex_v0, new_vertex_v1, new_vertex_v2);
            new_triangles++;
        }
    }

    std::clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Octree Vertex clustering: \n\tOld triangles: " << triangles.size() / 3 << ", New triangles: " << new_triangles << ", Time: " << elapsed_secs << std::endl;

    return new_mesh;
}

TriangleMesh * TriangleMesh::VertexClustering_GRID(size_t grid_size) {
    
    std::clock_t start = clock();

    /* grid_end holds the end of the grid as a floating point number */
    float grid_end = 1.0f * grid_size;
    /* 3D uniform grid of size +1, each cell holds a queue with integers */
    UniformGrid<std::deque<int>, 3> grid(grid_size + 1);

    /* Traverse old vertices, find where they belong */
    for (size_t i = 0; i < vertices.size(); i++) {
        size_t grid_i = static_cast<size_t>(map_to_range(min_x, max_x, vertices[i].x, 0.0f, grid_end));
        size_t grid_j = static_cast<size_t>(map_to_range(min_y, max_y, vertices[i].y, 0.0f, grid_end));
        size_t grid_k = static_cast<size_t>(map_to_range(min_z, max_z, vertices[i].z, 0.0f, grid_end));

        grid.at(grid_i, grid_j, grid_k).push_back(i);
    }

    TriangleMesh * new_mesh = new TriangleMesh();
    int new_vertex_id = 0;
    std::unordered_map<int, int> old_vertices_to_new_vertices;

    for (size_t i = 0; i < grid_size + 1; i++) {
        for (size_t j = 0; j < grid_size + 1; j++) {
            for (size_t k = 0; k < grid_size + 1; k++) {

                glm::vec3 mean = glm::vec3(0, 0, 0);
                
                for (size_t v = 0; v < grid.at(i,j,k).size(); v++) {
                    int vertex_id = grid.at(i, j, k)[v];

                    mean += vertices[vertex_id];
                    /* Set the mapping between old and new vertices */
                    old_vertices_to_new_vertices[vertex_id] = new_vertex_id;
                }

                if (grid.at(i,j,k).size() != 0) {
                    mean.x = mean.x / grid.at(i,j,k).size();
                    mean.y = mean.y / grid.at(i,j,k).size();
                    mean.z = mean.z / grid.at(i,j,k).size();
                    new_mesh->addVertex(mean);
                    new_vertex_id++;
                }
            }
        }
    }
    
    /* Create new triangles */
    size_t new_triangles = 0;
    for (size_t i = 0; i < triangles.size(); i += 3) {
        /* Find the old vertices that formed this old triangle */
        int old_vertex_v0 = triangles[i];
        int old_vertex_v1 = triangles[i + 1];
        int old_vertex_v2 = triangles[i + 2];

        /* Find the new vertex represenative for each one of  old vertices */
        int new_vertex_v0 = old_vertices_to_new_vertices[old_vertex_v0];
        int new_vertex_v1 = old_vertices_to_new_vertices[old_vertex_v1];
        int new_vertex_v2 = old_vertices_to_new_vertices[old_vertex_v2];

        /*
            If the all three of the old vertices have fell different representative,
            that means that they into a different cluster and we have a proper triangle
            If two of them are the same, we have a line, if all of them are the same
            we have a point
        */
        if (new_vertex_v0 != new_vertex_v1 && new_vertex_v1 != new_vertex_v2 && new_vertex_v0 != new_vertex_v2) {
            new_mesh->addTriangle(new_vertex_v0, new_vertex_v1, new_vertex_v2);
            new_triangles++;
        }
    }
    std::clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Grid Vertex clustering: \n\tOld triangles: " << triangles.size() / 3 << ", New triangles: " << new_triangles << ", Time: " << elapsed_secs << std::endl;

    return new_mesh;
}

void TriangleMesh::TestRaysPerSecond(size_t total_rays) {

    std::clock_t start = clock();

    size_t total_intersections = 0;
    size_t rays_that_hit_the_target = 0;

    Point3D origin = octree_triangles->GetOrigin();
    Real_t length = octree_triangles->GetLength();

    for (size_t ray = 0; ray < total_rays; ray++) {
    
        std::vector<unsigned int> results;
        Ray3D r(Point3D({ get_rand(), get_rand(), get_rand()}), Point3D({ get_rand() - 0.5f, get_rand() - 0.5f, get_rand() - 0.5f }));
        
        octree_triangles->RayCast(vertices, triangles, r, results);
        total_intersections += results.size();
        rays_that_hit_the_target += (results.size() > 0) ? 1 : 0;
    }

    std::clock_t end = clock();
    double elapsed_secs = double(end - start) / CLOCKS_PER_SEC;

    float rayss = (float)total_rays / elapsed_secs;
    std::cout << "Rays test, Time: " << elapsed_secs << std::endl;
    std::cout << "\tTotal rays: " << total_rays << ", Rays/s: " << rayss << std::endl;
    std::cout << "\tRays that hit the target: " << rays_that_hit_the_target << "\n\tIntersections: " << total_intersections << ", Intersections/ray: " << (float)total_intersections/rays_that_hit_the_target << std::endl;
}

void TriangleMesh::sendToOpenGL(ShaderProgram &program) {

    /* Allocate Opengl drawing stuff */
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    posLocation = program.bindVertexAttribute("position", 3, 3 * sizeof(float), 0);

    glGenBuffers(1, &vbo_normals);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_normals.size() * sizeof(float), &vertex_normals[0], GL_STATIC_DRAW);
    normalLocation = program.bindVertexAttribute("normal", 3, 3 * sizeof(float), 0);

    /* Vertex colors are allocated per frame, they change from frame to frame */

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(unsigned int), &triangles[0], GL_STATIC_DRAW);
}

void TriangleMesh::render(ShaderProgram &program) const
{
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
    glEnableVertexAttribArray(normalLocation);

    /* Send vertex colors to OpenGL */
    GLuint vbo_colors;
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, 3 * vertex_colors.size() * sizeof(float), &vertex_colors[0], GL_STATIC_DRAW);
    GLint colorLocation = program.bindVertexAttribute("vertex_color", 3, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(colorLocation);

    /* Draw */
    glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

    /* Delete colors */
    glDeleteBuffers(1, &vbo_colors);
}

void TriangleMesh::renderPoints() const {

    glBindVertexArray(vao);
    glEnableVertexAttribArray(posLocation);
    glEnableVertexAttribArray(normalLocation);
    glDrawArrays(GL_POINTS, 0, vertices.size());
}

void TriangleMesh::free()
{
    glDeleteBuffers(1, &vbo_position);
    glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
	
	vertices.clear();
	triangles.clear();
}


