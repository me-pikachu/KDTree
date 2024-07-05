#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

template <size_t K, typename dtype>
class KDTree {
private:
    struct Node {
        std::vector<dtype> point;
        Node* left;
        Node* right;
        int level;  // level of the node in the tree, starts at 0 for the root
        dtype value;

        Node(const std::vector<dtype>& _pt, int _level, const dtype& _value = dtype())
            : point(_pt), left(nullptr), right(nullptr), level(_level), value(_value) {}
    };

    Node* root_;

    struct Compare {
        size_t axis;
        Compare(size_t axis) : axis(axis) {}
        bool operator()(const std::vector<dtype>& p1, const std::vector<dtype>& p2) const {
            return p1[axis] < p2[axis];
        }
    };

    Node* buildTree(typename std::vector<std::vector<dtype>>::iterator start,
                    typename std::vector<std::vector<dtype>>::iterator end, int currLevel) {
        if (start >= end) return nullptr;

        int axis = currLevel % K;
        Compare cmp(axis);

        auto mid = start + (end - start) / 2;
        std::nth_element(start, mid, end, cmp);

        Node* newNode = new Node(*mid, currLevel);
        std::vector<std::vector<dtype>> leftPoints(start, mid);
        std::vector<std::vector<dtype>> rightPoints(mid + 1, end);
        newNode->left = buildTree(leftPoints.begin(), leftPoints.end(), currLevel + 1);
        newNode->right = buildTree(rightPoints.begin(), rightPoints.end(), currLevel + 1);
        return newNode;
    }

    void findNearestNeighbor(const std::vector<dtype>& key, Node* currNode, Node*& bestNode, dtype& bestDist) const {
        if (currNode == nullptr) return;

        dtype currDist = distance(currNode->point, key);
        if (currDist < bestDist) {
            bestNode = currNode;
            bestDist = currDist;
        }

        int axis = currNode->level % K;
        if (key[axis] < currNode->point[axis]) {
            findNearestNeighbor(key, currNode->left, bestNode, bestDist);
            if (key[axis] + bestDist >= currNode->point[axis]) {
                findNearestNeighbor(key, currNode->right, bestNode, bestDist);
            }
        } else {
            findNearestNeighbor(key, currNode->right, bestNode, bestDist);
            if (key[axis] - bestDist <= currNode->point[axis]) {
                findNearestNeighbor(key, currNode->left, bestNode, bestDist);
            }
        }
    }

    dtype distance(const std::vector<dtype>& p1, const std::vector<dtype>& p2) const {
        dtype dist = 0;
        for (size_t i = 0; i < K; ++i) {
            dist += (p1[i] - p2[i]) * (p1[i] - p2[i]);
        }
        return std::sqrt(dist);
    }

public:
    KDTree() : root_(nullptr) {}

    KDTree(std::vector<std::vector<dtype>>& points) {
        root_ = buildTree(points.begin(), points.end(), 0);
    }

    void insert(const std::vector<dtype>& pt, const dtype& value = dtype()) {
        Node** curr = &root_;
        int level = 0;
        while (*curr != nullptr) {
            int axis = level % K;
            if (pt[axis] < (*curr)->point[axis]) {
                curr = &((*curr)->left);
            } else {
                curr = &((*curr)->right);
            }
            ++level;
        }
        *curr = new Node(pt, level, value);
    }

    std::vector<dtype> nearestNeighbor(const std::vector<dtype>& key) const {
        Node* best = nullptr;
        dtype bestDist = std::numeric_limits<dtype>::max();
        findNearestNeighbor(key, root_, best, bestDist);
        return best ? best->point : std::vector<dtype>(K);
    }
};

int main() {
    // Example usage
    std::vector<std::vector<int>> points = {{3, 6}, {17, 15}, {13, 15}, {6, 12}, {9, 1}, {2, 7}, {10, 19}};

    KDTree<2, int> tree(points);

    std::vector<int> searchPoint = {9, 2};
    std::vector<int> nearest = tree.nearestNeighbor(searchPoint);
    std::cout << "Nearest neighbor to (" << searchPoint[0] << ", " << searchPoint[1] << ") is (" << nearest[0] << ", " << nearest[1] << ")" << std::endl;

    return 0;
}
