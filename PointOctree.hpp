#ifndef __PointOctree_hpp__
#define __PointOctree_hpp__

#include <iostream>
#include <deque>

/**
    An octree in which leaf holds BUCKET_SIZE number of Data points
*/
template<typename Data, int BUCKET_SIZE=1>
class PointOctree {
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

        virtual void Destroy() = 0;
        virtual OctreeNode * Insert(Point3D point, Data data) = 0;
        virtual OctreeNode * Remove(Point3D point) = 0;
        virtual size_t Depth() = 0;

        virtual void RayCastProcessChild(Real_t tx0, Real_t ty0, Real_t tz0, Real_t tx1, Real_t ty1, Real_t tz1, unsigned char a, std::vector<Data>& results) = 0;
        
        virtual void ClusterNodes(size_t depth, size_t current_depth, std::vector<std::vector<Data> >& clusters) = 0;
        virtual void AddLeavesToCluster(std::vector<Data>& cluster) = 0;

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

        OctreeNode * Insert(Point3D point, Data data) {
            /* If this leaf has enough space, store here */
            if (buckets_.size() < BUCKET_SIZE) {
                buckets_.push_back(Bucket(point, data));
                return this;
            }

            /* Either-wise, split the leaf */
            OctreeInnerNode * temp = new  OctreeInnerNode(this->origin_, this->length_);
            for (size_t i = 0; i < buckets_.size(); i++) {
                temp->Insert(buckets_[i].point_, buckets_[i].data_);
            }
            temp->Insert(point, data);

            delete this;
            return temp;
        }
        
        OctreeLeafNode * Remove(Point3D point) {
            /* Check if the point is stored here, and delete it */
            for (typename std::deque<Bucket>::iterator itr = buckets_.begin(); itr != buckets_.end(); ++itr) {
                if ((*itr).point_ == point) {
                    buckets_.erase(itr);
                    break;
                }
            }

            /* If leaf is empty delete leaf */
            if (buckets_.size() == 0) {
                delete this;
                return nullptr;
            }

            return this;
        }
        
        size_t Depth() {
            return 0;
        }

        void RayCastProcessChild(Real_t tx0, Real_t ty0, Real_t tz0, Real_t tx1, Real_t ty1, Real_t tz1, unsigned char a, std::vector<Data>& results) {
            
            if (tx1 < 0 || ty1 < 0 || tz1 < 0) return;
            
            /* If ray casting hit a leaf, add all the points to the results */
            for (size_t i = 0; i < buckets_.size(); i++)
                results.push_back(buckets_[i].data_);

            return;
        }

        void ClusterNodes(size_t depth, size_t current_depth, std::vector<std::vector<Data> >& clusters) {
            std::vector<Data> cluster;
            
            /* 
                If ClusterNodes is called upon a leaf, then the octree is not deep enough 
                at that sub-space. Create a single cluster with these leaf points
            */

            for (size_t i = 0; i < buckets_.size(); i++)
                cluster.push_back(buckets_[i].data_);

            clusters.push_back(cluster);
        }

        void AddLeavesToCluster(std::vector<Data>& cluster) {
            /* Add all leaf points to that cluster */
            for (size_t i = 0; i < buckets_.size(); i++)
                cluster.push_back(buckets_[i].data_);
        }


    private:
        /* Holds a data point */
        struct Bucket {
            Bucket(Point3D point, Data data) : point_(point), data_(data) {};
            Point3D point_;
            Data data_;
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

        void Destroy() {
            for (size_t i = 0; i < children_.size(); i++) {
                if (children_[i] != nullptr) {
                    children_[i]->Destroy();
                    delete children_[i];
                }
            }
        }

        OctreeNode * Insert(Point3D point, Data data) {
            /* Find child */
            std::pair<Point3D, size_t> child = OctreeNode::FindChild(point);

            /* If null, create leaf */
            if (children_[child.second] == nullptr) {
                children_[child.second] = new OctreeLeafNode(child.first, this->length_ / 2.0f);
            }

            /* Insert at that subtree */
            children_[child.second] = children_[child.second]->Insert(point, data);

            return this;
        }
        
        OctreeNode * Remove(Point3D point) {
            /* Find child */
            std::pair<Point3D, size_t> child = OctreeNode::FindChild(point);

            if (children_[child.second] == nullptr) {
                return this;
            }
            
            /* Remove from child */
            children_[child.second] = children_[child.second]->Remove(point);

            /* Check the number of children that you have, if zero, delete this node */
            size_t number_of_non_null_children = 0;
            for (size_t i = 0; i < children_.size(); i++)
                if (children_[i] != nullptr) {
                    number_of_non_null_children++;
                }

            if (number_of_non_null_children == 0) {
                delete this;
                return nullptr;
            }

            return this;
        }

        size_t Depth() {
            size_t current_depth = 0;
            for (size_t i = 0; i < children_.size(); i++)
                if (children_[i] != nullptr) current_depth = std::max(current_depth, children_[i]->Depth());

            return current_depth + 1;
        }
        
        void RayCastProcessChild(Real_t tx0, Real_t ty0, Real_t tz0, Real_t tx1, Real_t ty1, Real_t tz1, unsigned char a, std::vector<Data>& results) {
            Real_t txm, tym, tzm;
            int current_node;

            if (tx1 < 0 || ty1 < 0 || tz1 < 0) return;
            
            /* Calculate the middle of the entry and exit point */
            txm = Real_t(0.5)*(tx0 + tx1);
            tym = Real_t(0.5)*(ty0 + ty1);
            tzm = Real_t(0.5)*(tz0 + tz1);

            /* Calculate the first node to be visited */
            current_node = RayCastFirstNode(tx0, ty0, tz0, txm, tym, tzm);

            /* Iteratively visit the nodes along the ray */
            do {
                int index = current_node ^ a;
                switch (current_node)
                {
                case 0:  {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(tx0, ty0, tz0, txm, tym, tzm, a, results);
                    current_node = RayCastNewNode(txm, 4, tym, 2, tzm, 1);
                    break;  
                } case 1:  {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(tx0, ty0, tzm, txm, tym, tz1, a, results);
                    current_node = RayCastNewNode(txm, 5, tym, 3, tz1, 8);
                    break; 
                } case 2:  {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(tx0, tym, tz0, txm, ty1, tzm, a, results);
                    current_node = RayCastNewNode(txm, 6, ty1, 8, tzm, 3);
                    break; 
                } case 3:  {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(tx0, tym, tzm, txm, ty1, tz1, a, results);
                    current_node = RayCastNewNode(txm, 7, ty1, 8, tz1, 8);
                    break; 
                } case 4:  {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(txm, ty0, tz0, tx1, tym, tzm, a, results);
                    current_node = RayCastNewNode(tx1, 8, tym, 6, tzm, 5);
                    break; 
                } case 5: {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(txm, ty0, tzm, tx1, tym, tz1, a, results);
                    current_node = RayCastNewNode(tx1, 8, tym, 7, tz1, 8);
                    break; 
                } case 6:  {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(txm, tym, tz0, tx1, ty1, tzm, a, results);
                    current_node = RayCastNewNode(tx1, 8, ty1, 8, tzm, 7);
                    break; 
                } case 7: {
                    if (children_[index] != nullptr) children_[index]->RayCastProcessChild(txm, tym, tzm, tx1, ty1, tz1, a, results);
                    current_node = 8;
                    break; 
                }
                }
            } while (current_node < 8);
        }

        void ClusterNodes(size_t depth, size_t current_depth, std::vector<std::vector<Data> >& clusters) {
            /* If depth not reached, propagate the call */
            if (current_depth < depth) {
                for (size_t i = 0; i < children_.size(); i++)
                    if (children_[i] != nullptr) children_[i]->ClusterNodes(depth, current_depth + 1, clusters);
                return;
            }

            /* Either-wise, create cluster and add all leaves below this level to that cluster */
            std::vector<Data> cluster;
            for (size_t i = 0; i < children_.size(); i++) {
                if (children_[i] != nullptr) children_[i]->AddLeavesToCluster(cluster);
            }

            /* Add cluster to results */
            clusters.push_back(cluster);
        }

        void AddLeavesToCluster(std::vector<Data>& cluster) {
            /* Propagate the call */
            for (size_t i = 0; i < children_.size(); i++)
                if (children_[i] != nullptr) children_[i]->AddLeavesToCluster(cluster);
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
            } else {
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

    /**
        @param origin The "bottom left" point in the octree area
        @parma length The size of the octree region in all direction
    */
    PointOctree(Point3D origin, Real_t length) {
        origin_ = origin;
        length_ = length;
        root_ = new OctreeLeafNode(origin, length);
    }

    ~PointOctree() {
        delete root_;
    }

    /**
        Delete everything, don't use after this call
    */
    void Destroy() {
        root_->Destroy();
        delete root_;
        root_ = nullptr;
    }

    /**
        Insert a data into the octree
        @param point The position in 3D space
        @param data The data to store
    */
    void Insert(Point3D point, Data data) {
        bool inside_x = point[0] > origin_[0] && (point[0] < origin_[0] + length_);
        bool inside_y = point[1] > origin_[1] && (point[1] < origin_[1] + length_);
        bool inside_z = point[2] > origin_[2] && (point[2] < origin_[2] + length_);
        
        if (!inside_x || !inside_y || !inside_z) {
            std::cout << "Point: " << point << " is outside octree region" << std::endl;
            return;
        }

        root_ = root_->Insert(point, data);
    }

    /**
        Remove a point from the octree
        @param point The 3D space point to remove
    */
    void Remove(Point3D point) {
        root_ = root_->Remove(point);
        
        if (root_ == nullptr) root_ = new OctreeLeafNode(origin_, length_);
    }

    size_t Depth() {
        return root_->Depth();
    }

    /**
        Perform ray traversal
        @param r The 3D space ray
        @param[out] results The results will be pushed back here, in first to hit order
    */
    void RayCast(Ray3D r, std::vector<Data>& results) {
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
            root_->RayCastProcessChild(tx0, ty0, tz0, tx1, ty1, tz1, a, results);
        }
    }

    /**
        Perform clustering
        @param depth The depth of the tree to perform clustering
        @param[out] clusters The output of this operation is pushed back here. Each 
            cluster is a vector of Data
    */
    void Cluster(size_t depth, std::vector<std::vector<Data> >& clusters) {
        root_->ClusterNodes(depth, 0, clusters);
    }

private:
    OctreeNode * root_;
    Point3D origin_;
    Real_t length_;
};

#endif