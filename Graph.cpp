#include "Graph.h"
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm>
using namespace std;

Graph::Graph(const char* const & edgelist_csv_fn) {
    ifstream my_file(edgelist_csv_fn);      // open the file
    string line;                     // helper var to store current line
    while(getline(my_file, line)) {  // read one line from the file
        istringstream ss(line);      // create istringstream of current line
        string first, second, third; // helper vars
        getline(ss, first, ',');     // store first column in "first"
        getline(ss, second, ',');    // store second column in "second"
        getline(ss, third, '\n');    // store third column column in "third"
        tuple<string,string,double> edge = make_tuple(first,second,stod(third));
        edges.push_back(edge);
    }
    my_file.close();
    //std::sort(edges.begin(), edges.end());
}

bool contains(vector<string> vtor, string test) {
    for (string s : vtor) {
        if (s == test) {
            return true;
        }
    }
    return false;
}

unsigned int Graph::num_nodes() {
    vector<string> nodes;
    for (auto t : edges) {
        if (!contains(nodes, get<0>(t))) {
            nodes.push_back(get<0>(t));
        }
        if (!contains(nodes, get<1>(t))) {
            nodes.push_back(get<1>(t));
        }
    }
    return nodes.size();
}

vector<string> Graph::nodes() {
    vector<string> nodes;
    for (auto t : edges) {
        if (!contains(nodes, get<0>(t))) {
            nodes.push_back(get<0>(t));
        }
        if (!contains(nodes, get<1>(t))) {
            nodes.push_back(get<1>(t));
        }
    }
    return nodes;
}

unsigned int Graph::num_edges() {
    return edges.size();
}

unsigned int Graph::num_neighbors(string const & node_label) {
    unsigned int num_neighbors;
    for (auto t : edges) {
        if (get<0>(t) == node_label) {
            num_neighbors++;
        }
        if (get<1>(t) == node_label) {
            num_neighbors++;
        }
    }
    return num_neighbors;
}

double Graph::edge_weight(string const & u_label, string const & v_label) {
    for (auto t : edges) {
        if (get<0>(t) == u_label && get<1>(t) == v_label) {
            return get<2>(t);
        }
        if (get<1>(t) == u_label && get<0>(t) == v_label) {
            return get<2>(t);
        }
    }
    return -1;
}

vector<string> Graph::neighbors(string const & node_label) {
    vector<string> neighbors;
    for (auto t : edges) {
        if (get<0>(t) == node_label) {
            neighbors.push_back(get<1>(t));
        }
        if (get<1>(t) == node_label) {
            neighbors.push_back(get<0>(t));
        }
    }
    return neighbors;
}

vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label) {
    vector<string> path;
    //BFS from start to end
    queue<string> q;
    q.push(start_label);
    string curr;
    vector<string> visited;
    vector<tuple<string,string>> preds; //keep track of predecessor (pred,curr)
    while (!q.empty()) {
        curr = q.front();
        q.pop();
        if (!contains(visited,curr)) {
            visited.push_back(curr);
            if (curr == end_label) {
                while (curr !=start_label) {
                    for (auto t : preds) {
                        if (get<1>(t) == curr) {
                            path.push_back(curr);
                            //path.push_back(get<0>(t)); this was the error
                            curr = get<0>(t);
                            break;
                        }
                    }
                }
                path.push_back(curr);//curr=startlabel, path is backwards
                reverse(path.begin(),path.end());
                return path;
            }
            for (string s : neighbors(curr)) {
                if (!contains(visited, s)) {
                    q.push(s);
                    tuple<string,string> p= make_tuple(curr,s);
                    preds.push_back(p);
                }
            }
        }
    }
    return path;//empty down here
}

vector<tuple<string,string,double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label) {
    // TODO
    vector<tuple<string,string,double>> path;
    if (start_label == end_label) {
        tuple<string,string, double> pathtuple = make_tuple(start_label, start_label, -1);
        path.push_back(pathtuple);
        return path;
    }
    priority_queue<tuple<double, string>, vector<tuple<double, string>>, greater<tuple<double, string>>> q; //min heap
    tuple<double,string> first = make_tuple(0,start_label);
    q.push(first);//
    string curr;
    vector<string> visited;
    vector<tuple<double,string,string>> preds; //keep track of predecessor (pred,curr)
    while (!q.empty()) {
        curr = get<1>(q.top()); //remove next node from to visit
        q.pop(); 
        if (!contains(visited,curr)) { //if curr has not been visited
            visited.push_back(curr); //visit curr
            if (curr == end_label) {
                //sort(preds);
                while (curr !=start_label) {
                    for (auto t : preds) { //preds is sorted so that the shortest distance to each node would come first
                        if (get<2>(t) == curr) {
                            tuple<string,string, double> pathtuple = make_tuple(get<1>(t),curr,edge_weight(get<1>(t),curr));
                            path.push_back(pathtuple);
                            //path.push_back(get<0>(t)); this was the error
                            curr = get<1>(t);
                            break;
                        }
                    }
                }
                /*if(path.empty()) {
                    tuple<string,string, double> pathtuple = make_tuple(start_label,start_label,-1));
                    path.push_back(pathtuple);
                }*/
                //path.push_back(curr);//curr=startlabel, path is backwards
                reverse(path.begin(),path.end());
                return path;
            }
            for (string s : neighbors(curr)) { //add unvisited neighbors
                if (!contains(visited, s)) {
                    tuple<double,string> p1 = make_tuple(edge_weight(curr,s),s);
                    q.push(p1);
                    double length=0;
                    for (auto t : preds) {//finded shortest distance to curr
                        if (get<2>(t)==curr) {
                            length=get<0>(t);
                        }
                    }
                    tuple<double,string,string> p = make_tuple(length+edge_weight(curr,s),curr,s);
                    preds.push_back(p);
                    sort(preds.begin(),preds.end());
                }
            }
        }
    }
    return path;
}

vector<vector<string>> Graph::connected_components(double const & threshold) {
    vector<vector<string>> comps;
    vector<string> visited;
    //vector<string> nodes = nodes();
    vector<string> c;//1 set of connectede compenents
    while (visited.size() < nodes().size()) {
        for (auto u : nodes()) {
            if (!contains(visited, u)) {
                //BFS starting at u considering threshold
                queue<string> q;
                q.push(u); //u
                string curr;
                //vector<string> visited;
                vector<tuple<string,string>> preds; //keep track of predecessor (pred,curr)
                while (!q.empty()) {
                    curr = q.front();
                    q.pop();
                    if (!contains(visited,curr)) {
                        visited.push_back(curr);
                        c.push_back(curr);
                        for (string s : neighbors(curr)) {
                            if ((!contains(visited, s)) && edge_weight(curr,s) <= threshold) {
                                q.push(s);
                                tuple<string,string> p= make_tuple(curr,s);
                                preds.push_back(p);
                            }
                        }
                    }
                } //end of internal while (BFS)
                comps.push_back(c);
                c.clear();
                break; // breaks out of for loop to reset after finding one cluster
            } //end of if 
        }
    }//while visited < nodes
    return comps;
}

double Graph::smallest_connecting_threshold(string const & start_label, string const & end_label) {
    // TODO
    double threshold=0;


    return threshold;
}
