#include <bits/stdc++.h>

using namespace std;

/*
void quicksort(int arr[], int size){

}

int median_of_medians(int* arr, int size){
	// this function stores the new median in the temp_arr
	// So if starting len of arr is n then maximum total additional space required is n/4
	// In case of 2 medians (when size is even) lower median is returned
	if (size<=5){
		// base condition
		quicksort(arr,size);
		return arr[(size-1)/2]; // it is (size+1)/2 - 1  (-1 is for index of the median)
	}
	int* temp_arr = (int*)malloc((size/5+1)*sizeof(int));
	int i;
	for (i=0; 5*i<size; i++){
		if (size-5*i>=5){
			quicksort(arr+5*i,5);
			temp_arr[i] = arr[5*i+2];
		} else {
			quicksort(arr+5*i,size-5*i);
			temp_arr[i] = arr[5*i+(size-5*i-1)/2]; // it is (len+1)/2 - 1 which is (size-i-1+1)/2 - 1 = (size-i)/2 - 1
			// here i made a mistake instead of writing 5*i i wrote only i
		}
	}
	return median_of_medians(temp_arr,i); // very important point here
	// it should be i not i+1 because lets say we have 13 elements i=0 then i=1 then i=2 then i=3(this fails so loop breaks from here)
	// instead of i being 2 i is 3 as one extra condition is checked so size would=i rather than i+1
}

int median_by_motm(int* arr, int size, int median = -1){
	// assigning a default median value of -1 if not called does not work in C
	// if median is -1 then median is not passed as an input
	// median_by_motm (median of the medians)
	// maximum total additional space required is 3n/4
	if (size==1) return arr[0]; // base condition
	if (median==-1) median=(size-1)/2;
	int pivot = median_of_medians(arr,size);
	int index = partition_by_pivot(arr,size,pivot);
	if (index==median){
		return pivot; // another base condition
	} else if (index>median){
		// initially made by a mistake by making the condition as index<median
		return median_by_motm(arr,index,median);
	} else {
		return median_by_motm(arr+index+1,size-index-1,median-index-1);
	}
}
*/

class EmptyTreeException : public std::exception {
public:
    const char* what() const noexcept override {
        return "The KD-tree is empty.";
    }
};

template <typename dtype>
class KDTree{

private:
    int d = 0; // dimensionality of the tree

    struct node{
        std::vector<dtype> point;
        node* lchild = nullptr;
        node* rchild = nullptr;
        int depth = 0; // the depth of the current node

        node(const std::vector<dtype>& init, const int& level){
            point = init;
            depth = level;
        }
    };

    node* root = nullptr;

public:

    KDTree(int level) : d(level), root(nullptr) {}

    KDTree(int arr[], int size, int d){
        // constructing a KDTree using the median of medians algorithm 
        // in O(nlogn) time complexity
    }

    void insert(const std::vector<dtype>& point){
        // inserts an point into the KDTree
        if (root == nullptr){
            root = new node(point, 0);
            return;
        }
        
        node** curnode = &root;
        int depth = 0;
        while (*curnode != nullptr){
            if ((*curnode)->point[depth%d] < point[depth%d]){
                curnode = &(*curnode)->lchild;
            } else {
                curnode = &(*curnode)->rchild;
            }
            ++depth;
        }
        *curnode = new node(point, depth);
    }

    double distance(const std::vector<dtype>& point1, const std::vector<dtype>& point2){
        if (point1.size() != point2.size() && point1.size() != d){
            std::cerr << "Invalid dimension of the points" << std::endl;
            return 0.0;
        }
        
        double dist = 0;
        for (int i=0; i<d; ++i){
            dist += (double)((point1[i] - point2[i]) * (point1[i] - point2[i]));
        }
        
        return sqrt(dist);
    }

    void findNearestNeighbour(node* curnode, const std::vector<dtype>& point, node& best, double& bestDist){
        if (curnode == nullptr) return;

        if (distance(point, curnode->point) < bestDist){
            bestDist = distance(point, curnode->point);
            best = *curnode;
        }

        // calculating good child
        int depth = curnode->depth;
        if (curnode->point[depth%d] > point[depth%d]){
            // left child is good
            findNearestNeighbour(curnode->lchild, point, best, bestDist);

            // checking if bad child is worthy
            if (curnode->rchild != nullptr){
                double val = (double)(curnode->rchild->point[depth%d] - point[depth%d]);
                if (val < 0) val *= -1; // making it positive

                if (val < bestDist) findNearestNeighbour(curnode->rchild, point, best, bestDist);
            }
        } else {
            // right child is good
            findNearestNeighbour(curnode->rchild, point, best, bestDist);

            // checking if bad child is worthy
            if (curnode->lchild != nullptr){
                double val = (double)(curnode->lchild->point[depth%d] - point[depth%d]);
                if (val < 0) val *= -1; // making it positive

                if (val < bestDist) findNearestNeighbour(curnode->lchild, point, best, bestDist);
            }
        }
    }

    std::vector<dtype> nearestNeighbour(const std::vector<dtype>& point){
        if (root == nullptr){
            throw EmptyTreeException();
        }

        node best(root->point, 0);
        double bestDist = DBL_MAX;
        findNearestNeighbour(root, point, best, bestDist);
        return best.point;
    }

    void findkNearestNeighbour(node* curnode, const std::vector<dtype>& point, const int& k, priority_queue<pair<double, node>>& q){
        if (curnode == nullptr) return;

        double dist = distance(point, curnode->point);
        if (q.size() < k || dist < q.top().first){
            if (q.size() == k) q.pop();
            q.push({dist, *curnode});
        }

        // calculating good child
        int depth = curnode->depth;
        if (curnode->point[depth%d] > point[depth%d]){
            // left child is good
            findkNearestNeighbour(curnode->lchild, point, k, q);

            // checking if bad child is worthy
            if (curnode->rchild != nullptr){
                double val = (double)(curnode->rchild->point[depth%d] - point[depth%d]);
                if (val < 0) val *= -1; // making it positive

                if (q.size() < k || val < q.top().first) findkNearestNeighbour(curnode->rchild, point, k, q);
            }
        } else {
            // right child is good
            findkNearestNeighbour(curnode->rchild, point, k, q);

            // checking if bad child is worthy
            if (curnode->lchild != nullptr){
                double val = (double)(curnode->lchild->point[depth%d] - point[depth%d]);
                if (val < 0) val *= -1; // making it positive

                if (q.size() < k || val < q.top().first) findkNearestNeighbour(curnode->lchild, point, k, q);
            }
        }
    }

    vector<vector<dtype>> kNearestNeighbour(const std::vector<dtype>& point, const int& k){
        if (root == nullptr){
            throw EmptyTreeException();
        }

        priority_queue<pair<double, node>, greater<double>> q;
        findkNearestNeighbour(root, point, k, q);
        vector<vector<dtype>> p;
        
        while (!q.empty()){
            p.push_back(q.top().second.point);
            q.pop();
        }

        return p;
    }
};

int main(){
    KDTree<double> t(2);
    t.insert({0,0});
    t.insert({1, 1});
    t.insert({2, 2});
    auto v = t.nearestNeighbour({0, 0});
    std::cout << v[0] << " " << v[1] << std::endl;
    auto u = t.kNearestNeighbour({0,0}, 2);
    for (auto x : u){
        std::cout << x[0] << " " << x[1] << ", ";
    }
    std::cout << std::endl;
    return 0;
}