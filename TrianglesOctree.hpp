#ifndef __TrianglesOctree_hpp__
#define __TrianglesOctree_hpp__

#include <bitset>

#include <glm/glm.hpp>

#include "TriangleBoxOverlapping.hpp"

template<int BUCKET_SIZE = 5, int MAX_DEPTH = 19>
class TrianglesOctree {
private:

    class OctreeNode {
    public:
        enum NodeType {
            LEAF,
            INNER,
        };

        virtual ~OctreeNode() {
        }

        NodeType GetNodeType() { return type_; }

        virtual OctreeNode * Insert(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, int triangle_id, size_t depth) = 0;
        virtual size_t Depth() = 0;

        virtual bool RayCastProcessChild(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, Ray3D& ray, Real_t tx0, Real_t ty0, Real_t tz0, Real_t tx1, Real_t ty1, Real_t tz1, unsigned char a, std::vector<unsigned int>& results) = 0;

        bool Overlaps(Point3D origin, Real_t length, std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, int triangle_id) {
            Real_t half_size = length / 2;
            glm::vec3 box_center = glm::vec3(origin[0], origin[1], origin[2]) + glm::vec3(half_size, half_size, half_size);
            int tp = 3 * triangle_id;
            bool overlaps = triBoxOverlap(box_center, glm::vec3(half_size), in_vertices[in_triangles[tp]], in_vertices[in_triangles[tp + 1]], in_vertices[in_triangles[tp + 2]]);
            return overlaps;
        }

    protected:
        NodeType type_;
        Point3D origin_;
        Real_t length_;

        /**
           Find the index of the child, and the new orign
        */
        std::pair<Point3D, size_t> FindChild(Point3D& point) {
            /**
                Find in which octant this point lies to, and what's the origin
                point of this octant. Return both
            */
            size_t index = 0;
            Real_t H = length_ / 2.0f;
            Point3D new_node_origin = origin_;

            Point3D center = origin_ + Point3D(H);
            Point3D V = point - center;

            if (V[0] >= 0) {
                index += 4;
                new_node_origin[0] += H;
            }
            if (V[1] >= 0) {
                index += 2;
                new_node_origin[1] += H;
            }
            if (V[2] >= 0) {
                index += 1;
                new_node_origin[2] += H;
            }

            return std::make_pair(new_node_origin, index);
        }
        
        Point3D GetOctantOrigin(short octant) {
            Point3D new_node_origin = origin_;

            std::bitset<3> bits(octant);
            Real_t H = length_ / 2.0f;
            new_node_origin[2] += H * bits[0];
            new_node_origin[1] += H * bits[1];
            new_node_origin[0] += H * bits[2];

            return new_node_origin;
        }

    };

    class OctreeLeafNode : public OctreeNode {
    public:
        OctreeLeafNode(Point3D origin, Real_t length) {
            this->origin_ = origin;
            this->length_ = length;
            this->type_ = OctreeNode::NodeType::LEAF;
        }

        void Destroy() {

        }

        OctreeNode * Insert(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, int triangle_id, size_t depth) {

            if (!OctreeNode::Overlaps(this->origin_, this->length_, in_vertices, in_triangles, triangle_id)) return this;

            if (buckets_.size() < BUCKET_SIZE || depth >= MAX_DEPTH) {
                buckets_.push_back(triangle_id);
                return this;
            }

            OctreeInnerNode * temp = new  OctreeInnerNode(this->origin_, this->length_);
            for (size_t i = 0; i < buckets_.size(); i++) {
                temp->Insert(in_vertices, in_triangles, buckets_[i].triangle_id_, depth + 1);
            }
            temp->Insert(in_vertices, in_triangles, triangle_id, depth + 1);

            delete this;
            return temp;
        }

        size_t Depth() {
            return 0;
        }

        bool RayCastProcessChild(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, Ray3D& ray, Real_t tx0, Real_t ty0, Real_t tz0, Real_t tx1, Real_t ty1, Real_t tz1, unsigned char a, std::vector<unsigned int>& results) {

            if (tx1 < 0 || ty1 < 0 || tz1 < 0) return false;

            /* 
                If ray traversing hit a leaf, check if the triangles stored here actually intersect with the ray
                Find the first triangle of intersection, calculate the reflected ray, and return
                If ray does not hit a triangle, return false
            */
            /* This is not implemented, I just return all the triangles and end the traversal */
            for (size_t i = 0; i < buckets_.size(); i++)
                results.push_back(buckets_[i].triangle_id_);
            return true;

        }

    private:
        struct Bucket {
            Bucket(unsigned int triangle_id) : triangle_id_(triangle_id) {};
            unsigned int triangle_id_;
        };
        std::deque<Bucket> buckets_;

    };

    class OctreeInnerNode : public OctreeNode {
    public:
        OctreeInnerNode(Point3D origin, Real_t length) {
            children_ = std::vector<OctreeNode *>(8, nullptr);

            this->origin_ = origin;
            this->length_ = length;
            this->type_ = OctreeNode::NodeType::INNER;
        }
        ~OctreeInnerNode() {

        }

        OctreeNode * Insert(std::vector<glm::vec3>& in_vertices, std::vector<unsigned  int>& in_triangles, int triangle_id, size_t depth) {

            Real_t H = this->length_ / 2.0f;
            for (size_t i = 0; i < children_.size(); i++) {
                Point3D child_origin = OctreeNode::GetOctantOrigin(i);
                if (!OctreeNode::Overlaps(child_origin, H, in_vertices, in_triangles, triangle_id)) continue;

                if (children_[i] == nullptr) children_[i] = new OctreeLeafNode(child_origin, H);
                children_[i] = children_[i]->Insert(in_vertices, in_triangles, triangle_id, depth + 1);
            }

            return this;
        }

        size_t Depth() {
            size_t current_depth = 0;
            for (size_t i = 0; i < children_.size(); i++)
                if (children_[i] != nullptr) current_depth = std::max(current_depth, children_[i]->Depth());

            return current_depth + 1;
        }

        bool RayCastProcessChild(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, Ray3D& ray, Real_t tx0, Real_t ty0, Real_t tz0, Real_t tx1, Real_t ty1, Real_t tz1, unsigned char a, std::vector<unsigned int>& results) {
            Real_t txm, tym, tzm;
            int current_node;

            if (tx1 < 0 || ty1 < 0 || tz1 < 0) return false;

            /* Calculate the middle of the entry and exit point */
            txm = Real_t(0.5)*(tx0 + tx1);
            tym = Real_t(0.5)*(ty0 + ty1);
            tzm = Real_t(0.5)*(tz0 + tz1);

            /* Calculate the first node to be visited */
            current_node = RayCastFirstNode(tx0, ty0, tz0, txm, tym, tzm);

            bool found = false;
            /* Iteratively visit the nodes along the ray */
            do {
                int index = current_node ^ a;
                switch (current_node)
                {
                case 0: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, tx0, ty0, tz0, txm, tym, tzm, a, results);
                    current_node = RayCastNewNode(txm, 4, tym, 2, tzm, 1);
                    break;
                } case 1: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, tx0, ty0, tzm, txm, tym, tz1, a, results);
                    current_node = RayCastNewNode(txm, 5, tym, 3, tz1, 8);
                    break;
                } case 2: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, tx0, tym, tz0, txm, ty1, tzm, a, results);
                    current_node = RayCastNewNode(txm, 6, ty1, 8, tzm, 3);
                    break;
                } case 3: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, tx0, tym, tzm, txm, ty1, tz1, a, results);
                    current_node = RayCastNewNode(txm, 7, ty1, 8, tz1, 8);
                    break;
                } case 4: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, txm, ty0, tz0, tx1, tym, tzm, a, results);
                    current_node = RayCastNewNode(tx1, 8, tym, 6, tzm, 5);
                    break;
                } case 5: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, txm, ty0, tzm, tx1, tym, tz1, a, results);
                    current_node = RayCastNewNode(tx1, 8, tym, 7, tz1, 8);
                    break;
                } case 6: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, txm, tym, tz0, tx1, ty1, tzm, a, results);
                    current_node = RayCastNewNode(tx1, 8, ty1, 8, tzm, 7);
                    break;
                } case 7: {
                    if (children_[index] != nullptr) found = found | children_[index]->RayCastProcessChild(in_vertices, in_triangles, ray, txm, tym, tzm, tx1, ty1, tz1, a, results);
                    current_node = 8;
                    break;
                }
                }
                if (found) return true;
            } while (current_node < 8);

            return found;
        }

    private:
        std::vector<OctreeNode *> children_;


        int RayCastFirstNode(Real_t tx0, Real_t ty0, Real_t tz0, Real_t txm, Real_t tym, Real_t tzm) {
            unsigned char answer = 0;

            if (tx0 > ty0) {
                if (tx0 > tz0) {
                    if (tym < tx0) answer |= 2;
                    if (tzm < tx0) answer |= 1;
                    return (int)answer;
                }
            }
            else {
                if (ty0 > tz0) {
                    if (txm < ty0) answer |= 4;
                    if (tzm < ty0) answer |= 1;
                    return (int)answer;
                }
            }

            if (txm < tz0) answer |= 4;
            if (tym < tz0) answer |= 2;
            return (int)answer;
        }

        int RayCastNewNode(Real_t txm, int x, Real_t tym, int y, Real_t tzm, int z) {
            if (txm < tym) {
                if (txm < tzm) { return x; }
            }
            else {
                if (tym < tzm) { return y; }
            }
            return z;
        }
    };

public:

    TrianglesOctree(Point3D origin, Real_t length) {
        origin_ = origin;
        length_ = length;
        root_ = new OctreeLeafNode(origin, length);
    }

    ~TrianglesOctree() {
        delete root_;
    }

    void Insert(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, int triangle_id) {
        root_ = root_->Insert(in_vertices, in_triangles, triangle_id, 0);
    }

    size_t Depth() {
        return root_->Depth();
    }

    void RayCast(std::vector<glm::vec3>& in_vertices, std::vector<unsigned int>& in_triangles, Ray3D r, std::vector<unsigned int>& results) {
        unsigned char a = 0;

        /**
            If ray has negative components calculate the reflection of the ray
        */
        if (r.Direction()[0] < 0) {
            r.Origin()[0] = (origin_[0] + length_ / 2.0) * 2 - r.Origin()[0];
            r.Direction()[0] = -r.Direction()[0];
            a |= 4;
        }
        if (r.Direction()[1] < 0) {
            r.Origin()[1] = (origin_[1] + length_ / 2.0) - r.Origin()[1];
            r.Direction()[1] = -r.Direction()[1];
            a |= 2;
        }
        if (r.Direction()[2] < 0) {
            r.Origin()[2] = (origin_[2] + length_ / 2.0) - r.Origin()[2];
            r.Direction()[2] = -r.Direction()[2];
            a |= 1;
        }

        /*
            Compute the starting parametric values of entry and exit for the root node
        */

        Real_t divx = Real_t(1) / r.Direction()[0];
        Real_t divy = Real_t(1) / r.Direction()[1];
        Real_t divz = Real_t(1) / r.Direction()[2];

        Real_t tx0 = (origin_[0] - r.Origin()[0]) * divx;
        Real_t tx1 = (origin_[0] + length_ - r.Origin()[0]) * divx;
        Real_t ty0 = (origin_[1] - r.Origin()[1]) * divy;
        Real_t ty1 = (origin_[1] + length_ - r.Origin()[1]) * divy;
        Real_t tz0 = (origin_[2] - r.Origin()[2]) * divz;
        Real_t tz1 = (origin_[2] + length_ - r.Origin()[2]) * divz;

        /* If there is intersection, continue */
        if (std::max(std::max(tx0, ty0), tz0) < std::min(std::min(tx1, ty1), tz1)) {
            root_->RayCastProcessChild(in_vertices, in_triangles, r, tx0, ty0, tz0, tx1, ty1, tz1, a, results);
        }
    }

    Point3D GetOrigin() {
        return origin_;
    }

    Real_t GetLength() {
        return length_;
    }

private:
    OctreeNode * root_;
    Point3D origin_;
    Real_t length_;
};

#endif
