#include "../Headers/gridfull.h"
#include "../Headers/claim.h"
#include <algorithm> // find vector
#include <iostream> // testing
#include <time.h> // sleep for testing
#include <cmath>

// These are node constants
const int kSink = 1;
const int kSource = 0;
const int kFree = -1;
const int kBorder = -2;
const int kVisited = -3;
const int kBlock = -4;
const int kPath = -5;

// past direction to prevent repeated double calls
// for backtrack only
const int kNorth = 1;
const int kEast = 2;
const int kSouth = -1;
const int kWest = -2;

// inherit constructor from Grid class
Utilities::GridFull::GridFull(ProblemObject* problem_object){
    int height = problem_object->get_height();
    int width = problem_object->get_width();
    for(int y = 0; y < height; y++) {
        vector<NodeFull*> temp_row;
        for(int x = 0; x < width; x++){
            temp_row.push_back(new NodeFull(x, y));
        }
        this->grid.push_back(temp_row);
    }
    
    include_blockers(problem_object);
    check_connections(problem_object);
    // print_grid();
    //TODO
}

int Utilities::GridFull::get_width() {
	//Assumes a perfect rectangle
	return grid.empty()?0:grid.at(0).size();
}

int Utilities::GridFull::get_height() {
	//Assumes a perfect rectangle
	return this->grid.size();
}

inline void testCode(){
    string pause;
    std::getline(std::cin, pause);
    if(pause == "q"){
        exit(1);
    }
}

// add blockers into the grid. Blocker's initial Point is at top left corner
void Utilities::GridFull::include_blockers(ProblemObject* problem_object){
    vector<Blocker> block = problem_object->get_blockers();
    int height = problem_object->get_height();
    int width = problem_object->get_width();
    for(int i = 0; i < block.size(); i++){
        
        // Ensure that blocker's location and size are within the grid's limits
        string name = block.at(i).name;
        int block_width = block.at(i).width;
        int block_height = block.at(i).height;
        int block_x = block.at(i).location.x;
        int block_y = block.at(i).location.y;
        
        if(block_x >= width || block_x < 0){
            claim("Blocker " + name + " location must be within range from 0 to (grid's width - 1)", kError);
            return;
        }
        if(block_y >= height || block_y < 0){
            claim("Blocker " + name + " location must be within range from 0 to (grid's height - 1)", kError);
            return;
        }
        if(block_width + block_x >= width){
            claim("Blocker " + name + " width extends beyond the grid's width", kError);
            return;
        }
        if(block_height > block_y){
            claim("Blocker " + name + " height extends beyond the grid's height", kError);
            return;
        }
        
        // insert blocked nodes into set beginning from top left corner
        for(int y = block_y; y > block_y - block_height; --y){
            for(int x = block_x; x < block_x + block_width; ++x){
                // TEST
                // std::cout << "x: " << x << "    y: " << y << endl;
                grid.at(y).at(x)->set_status(kBlock);
            }
        }
    }
}

// Ensure that connection's location are within the grid's limits
void Utilities::GridFull::check_connections(ProblemObject* problem_object){
    vector<Connection> connection = problem_object->get_connections();
    int height = problem_object->get_height();
    int width = problem_object->get_width();
    int connection_counter = 0;
    for(int i = 0; i < connection.size(); i++){
        
        //Ensure that connection's location are within the grid's limits
        string name = connection.at(i).name;
        int source_x = connection.at(i).source.x;
        int source_y = connection.at(i).source.y;
        int sink_x = connection.at(i).sink.x;
        int sink_y = connection.at(i).sink.y;
        
        if(source_x >= width || source_x < 0){
            claim("Connection " + name + " source must be within range from 0 to (grid's width - 1)", kError);
            return;
        }
        if(source_y >= height || source_y < 0){
            claim("Connection " + name + " source must be within range from 0 to (grid's height - 1)", kError);
            return;
        }
        if(sink_x >= width || sink_x < 0){
            claim("Connection " + name + " sink must be within range from 0 to (grid's width - 1)", kError);
            return;
        }
        if(sink_y >= height || sink_y < 0){
            claim("Connection " + name + " sink must be within range from 0 to (grid's height - 1)", kError);
            return;
        }
        
        //Ensure source and sink are not inside blocks
        NodeFull* current_source = grid.at(source_y).at(source_x);
        NodeFull* current_sink = grid.at(sink_y).at(sink_x);
        // This code relies on the fact that blockers are already added onto grid_nodes
        if(current_source->get_status() == kBlock){
            claim("Connection " + name + " source is in a blocker", kError);
            return;
        }
        if(current_sink->get_status() == kBlock){
            claim("Connection " + name + " sink is in a blocker", kError);
            return;
        }
        
        //Make sure sinks and sources aren't the same
        if(source_y == sink_y && source_x == sink_x){
            claim("Connection " + name + " source and sink are the same coordinates", kError);
            return;
        }
        
        // Insert connection's source and sink nodes into queue
        connections.push(std::pair<NodeFull*, NodeFull*> (current_source, current_sink));
    }
}

Utilities::NodeFull* Utilities::GridFull::load_connection(){
    if(connections.empty()){
        claim("Empty connection vector", kError);
        return 0;
    }
    if(connections.front().first->get_status() == kPath || connections.front().second->get_status() == kPath){
        claim("Connection point(s) is on a blocked path", kError);
        return 0;
    }
    connections.front().first->set_status(kSource);
    connections.front().first->set_cost(0);
    connections.front().second->set_status(kSink);
    connections.front().second->set_cost(0);
    return connections.front().first;
}

void Utilities::GridFull::print_grid(){
    int width = get_width();    // decrease call cost
    for(int y = get_height() - 1; y >= 0; --y){
        for(int x = 0; x < width; ++x){
            int status = grid.at(y).at(x)->get_status();
            switch(status){
                case kFree:
                    // the current_node is free/empty;
                    std::cout << ' ';
                    break;
                case kBlock:
                    // the current_node is a block
                    std::cout << '-';
                    break;
                case kSource:
                    std::cout << '@';
                    break;
                case kSink:
                    std::cout << '&';
                    break;
                case kVisited:
                    std::cout << grid.at(y).at(x)->get_cost() % 10;
                    break;
                case kBorder:
                    std::cout << '+';
                    break;
                case kPath:
                    std::cout << '*';
                    break;
            }
        }
        std::cout << endl;
    }
    std::cout << endl;
}

vector<Utilities::Path*> Utilities::GridFull::run_algorithm(Type type){
    while(!connections.empty()){
        run_algorithm_step(type);
    }
    // Test
    print_grid();
    
    return paths;
}

bool Utilities::GridFull::search(Type type){
    if(type == k2bit){
        // k2bit uses a different type of queue that sections off nodes using null-nodes 
        // hence push(0) which signals for a break  
        border.push(0);
        
        int custom_cost = 1;
        int cost_counter = 0;
        while(!border.empty()){
            NodeFull* front = border.front();
            if(front == 0){
                border.pop();
                if(border.size() == 0){
                    break;
                }
                border.push(0); // put null-node at end of new section
                cost_counter++;
                if(cost_counter >= 2){
                    if(custom_cost == 1){
                        custom_cost = 2;
                    }else{
                        custom_cost = 1;
                    }
                    cost_counter = 0;
                }
                // testCode();
                // print_grid();
                // std::cout << "custom_cost: " << custom_cost << endl;
                // std::cout << "cost_counter: " << cost_counter << endl;
                // continue;
            } else{
                // start searching
                if(search_north(front, type, custom_cost) || search_east(front, type, custom_cost) 
                || search_south(front, type, custom_cost) || search_west(front, type, custom_cost)){
                        return true;
                        claim("Found Path!", kNote);
                        break;    
                }
                border.front()->set_status(kVisited);
                border.pop();
            }
        }
    }else if(type == kHadlock){
        int detour = 0;
        int sink_y = connections.front().second->get_y();
        int sink_x = connections.front().second->get_x();
        while(!border_hadlock.empty()){
            // testCode();
            // print_grid();
            NodeFull* front = border_hadlock.front();
            border_hadlock.front()->set_status(kVisited);
            border_hadlock.pop_front();
            // start searching
            if(search_north(front, type, detour, sink_y) || search_east(front, type, detour, sink_x) 
            || search_south(front, type, detour, sink_y) || search_west(front, type, detour, sink_x)){
                    return true;
                    claim("Found Path!", kNote);
                    break;    
            }
        }
    }else{
        while(!border.empty()){
            NodeFull* front = border.front();
            // start searching
            if(search_north(front, type) || search_east(front, type) || search_south(front, type)
                || search_west(front, type)){
                    return true;
                    claim("Found Path!", kNote);
                    break;    
            }
            border.front()->set_status(kVisited);
            border.pop();
        }
    }
    return false;
}

double Utilities::GridFull::distance(NodeFull* first, NodeFull* second){
    return sqrt(pow((first->get_x() - second->get_x()), 2) + pow((first->get_y() - second->get_y()),2));
}

void Utilities::GridFull::run_algorithm_step(Type type){
    // input the first node (source) into queue
    claim("Load connection", kNote);
    NodeFull* source_node = load_connection();
    if(source_node == 0){
        return;
    }
    if(type == kHadlock){
        border_hadlock.push_back(source_node);
    } else{
        border.push(source_node);
    }
    
    // // k2bit uses a different type of queue that sections off nodes using null-nodes 
    // // hence push(0) which signals for a break
    // if(type == k2bit || type == kHadlock){
    //     border.push(0);
    // }
    
    claim("Begin Search", kNote);
    bool path_found = search(type);
    
    // reset source to the source node
    claim("Reset source's status", kNote);
    source_node->set_status(kSource);
    
    print_grid();
    
    // Re-Trace path
    if(!path_found){
        print_grid();
        claim("No path found", kWarning);
        
        // convert source and sink into paths
        claim("Converting to kPath", kNote);
        source_node->set_status(kFree);
        connections.front().second->set_status(kFree);
    } else{
        claim("Begin tracing", kNote);
        
        // if(type == k2bit){
        //     claim("Need to work on tracing", kDebug);
        // }
        
        backtrack(type);
        
        //TEST
        print_grid();
        
        // convert source and sink into paths
        claim("Converting to kPath", kNote);
        source_node->set_status(kPath);
        connections.front().second->set_status(kPath);
        
        // make stack into vector of paths
        claim("Adding to vector or Paths", kNote);
        convert_to_path();
    }
    // all border and visited nodes cleared status and sinks
    claim("Cleaning up grid", kNote);
    clean_up_grid();
    
    claim("Clean up border queues", kNote);
    while(!border.empty()){
        border.pop();
    }
    border_hadlock.clear();
    
    // pop the finished connection
    claim("Pop connection", kNote);
    connections.pop();
    
    claim("Finished connection!\n", kNote);
}

void Utilities::GridFull::clean_up_grid(){
    int width = get_width();    // decrease call cost
    for(int y = get_height() - 1; y >= 0; --y){
        for(int x = 0; x < width; ++x){
            NodeFull* current_node = grid.at(y).at(x);
            int status = current_node->get_status();
            if(status == kBorder || status == kVisited){
                current_node->set_status(kFree);
                current_node->set_cost(0);
            }
        }
    }
}

void Utilities::GridFull::convert_to_path(){
    Path* new_path = new Path();
    while(!trace_path.empty()){
        int x = trace_path.top()->get_x();
        int y = trace_path.top()->get_y();
        Point head(x, y);
        trace_path.pop();
        if(trace_path.empty()){
            break;
        }
        x = trace_path.top()->get_x();
        y = trace_path.top()->get_y();
        Point tail(x, y);
        PathSegment* new_segment = new PathSegment(head, tail);
        new_path->add_segment(new_segment);
    }
    paths.push_back(new_path);
}

void Utilities::GridFull::print_path(){
	for(unsigned i = 0; i < paths.size(); i++) {
    	std::cout << "Path " << i << ": ";
    	paths.at(i)->print();
    	std::cout << endl;
    }
}

bool Utilities::GridFull::backtrack_pick(int dir, Type type){
    switch (dir){
        std::cout << "test" << endl;
        case kNorth:
            if(backtrack_north(type)){
                return true;
            }
            break;
        case kEast:
            if(backtrack_east(type)){
                return true;
            }
            break;
        case kSouth:
            if(backtrack_south(type)){
                return true;
            }
            break;
        case kWest:
            if(backtrack_west(type)){
                return true;
            }
            break;
    }
    return false;
}

bool Utilities::GridFull::backtrack_direction(int &orig, const int dir1, const int dir2, Type type){
    // std::cout<< "s: " << orig << " " << dir1 << " " << dir2 << " " << dir3 << endl;
    int size = trace_path.size();
    if(orig == dir1){
        // std::cout << "orig same" << endl;
        orig = 0;
        if(backtrack_pick(dir2, type)){ //2nd best option
            return true;
        }
        // if no change
        if(trace_path.size() == size){
            if(backtrack_pick(dir2 * -1, type)){ // 3rd best option
                return true;
            }
        }
        //if there is still no change
        if(trace_path.size() == size){
            if(backtrack_pick(dir1 * -1, type)){ // 4rd best option
                return true;
            }
        }
    }else{
        orig = dir1;
        if(backtrack_pick(dir1, type)){
            return true;
        }
    }
    return false;
}

void Utilities::GridFull::backtrack(Type type){
    int past_direction = 0;
    while(trace_path.top()->get_status() != kSource){
        // if(backtrack_west(type) || backtrack_south(type) || backtrack_east(type) || backtrack_north(type)){
        //     return;
        // }
        // if(type == kHadlock){
        int diff_x = connections.front().first->get_x() - trace_path.top()->get_x();
        int diff_y = connections.front().first->get_y() - trace_path.top()->get_y();
        double direction = atan2(diff_y, diff_x) * 180 / 3.14159;
        // testCode();
        // std::cout << direction << endl;
        // std::cout << past_direction << endl;
        if(direction < -135){
            if(backtrack_direction(past_direction, kEast, kNorth, type)) return;
        }else if(direction < -45){
            if(backtrack_direction(past_direction, kNorth, kEast, type)) return;
        }else if(direction < 45){
            if(backtrack_direction(past_direction, kWest, kNorth, type)) return;
        }else if(direction < 135){
            if(backtrack_direction(past_direction, kSouth, kEast, type)) return;
        }else{
            if(backtrack_direction(past_direction, kEast, kSouth, type)) return;
        }
    }
}

bool Utilities::GridFull::backtrack_checker(NodeFull* node, Type type){
    int cost = node->get_cost();
    int top_cost = trace_path.top()->get_cost();
    int status = node->get_status();
    if(type == kNormal){
        if(cost + 1 == top_cost){
            return true;
        }
    } else if(type == k3bit){
        if(status == kSource){
            return true;
        }else if(cost != 0 && cost % 3 + 1 == top_cost && status > kBlock){
            return true;
        }
    } else if(type == k2bit){
        claim("Needs fixing", kError);
        if(top_cost == 1){
            if(cost == 2){
                return true;
            }
        }else if(top_cost == 2){
            if(cost == 1){
                return true;
            }
        }else{
            claim("Unusual cost", kWarning);
        }
    } else if(type == kHadlock){
        if(status == kSource){
            return true;
        }
        if((cost == top_cost || cost + 1 == top_cost) && (status == kVisited)){
            return true;
        }
    }
    
    return false;
}

bool Utilities::GridFull::backtrack_add_path(NodeFull* node){
    // testCode();
    if(node->get_status() == kSource){
        trace_path.push(node);
        // print_grid();
        return true;
    } else{
        trace_path.push(node);
        node->set_status(kPath);
        // print_grid();
    }
    return false;
}

bool Utilities::GridFull::backtrack_north(Type type){
    int current_y = trace_path.top()->get_y() - 1;
    // if(type == k2bit){
    //     current_y -= 1;
    // }
    int current_x = trace_path.top()->get_x();
    if(current_y >= 0){
        NodeFull* current = grid.at(current_y).at(current_x);
        if(type == k2bit){
            if(backtrack_checker(grid.at(current_y - 1).at(current_x), type)){
                if(backtrack_add_path(current)){
                    return true;
                }else if(backtrack_add_path(grid.at(current_y - 1).at(current_x))){
                    return true;
                }else{
                    backtrack_north(type);
                }
            }
        }else if(backtrack_checker(current, type)){
            if(backtrack_add_path(current)){
                return true;
            } else{
                backtrack_north(type);
            }
        }
    }
    return false;
}

bool Utilities::GridFull::backtrack_east(Type type){
    int current_y = trace_path.top()->get_y();
    int current_x = trace_path.top()->get_x() - 1;
    // if(type == k2bit){
    //     current_x -= 1;
    // }
    if(current_x >= 0){
        NodeFull* current = grid.at(current_y).at(current_x);
        if(type == k2bit){
            if(backtrack_checker(grid.at(current_y).at(current_x - 1), type)){
                if(backtrack_add_path(current)){
                    return true;
                }else if(backtrack_add_path(grid.at(current_y).at(current_x - 1))){
                    return true;
                }else{
                    backtrack_east(type);
                }
            }
        }else if(backtrack_checker(current, type)){
            if(backtrack_add_path(current)){
                return true;
            }else{
                backtrack_east(type);
            }
        }
    }
    return false;
}

bool Utilities::GridFull::backtrack_south(Type type){
    int current_y = trace_path.top()->get_y() + 1;
    // if(type == k2bit){
    //     current_y += 1;
    // }
    int current_x = trace_path.top()->get_x();
    if(current_y < get_height()){
        NodeFull* current = grid.at(current_y).at(current_x);
        if(type == k2bit){
            if(backtrack_checker(grid.at(current_y + 1).at(current_x), type)){
                if(backtrack_add_path(current)){
                    return true;
                }else if(backtrack_add_path(grid.at(current_y + 1).at(current_x))){
                    return true;
                }else{
                    backtrack_south(type);
                }
            }
        }else if(backtrack_checker(current, type)){
            if(backtrack_add_path(current)){
                return true;
            }else{
                backtrack_south(type);
            }
        }
    }
    return false;
}

bool Utilities::GridFull::backtrack_west(Type type){
    int current_y = trace_path.top()->get_y();
    int current_x = trace_path.top()->get_x() + 1;
    // if(type == k2bit){
    //     current_x += 1;
    // }
    if(current_x < get_width()){
        NodeFull* current = grid.at(current_y).at(current_x);
        if(type == k2bit){
            if(backtrack_checker(grid.at(current_y).at(current_x + 1), type)){
                if(backtrack_add_path(current)){
                    return true;
                }else if(backtrack_add_path(grid.at(current_y).at(current_x + 1))){
                    return true;
                }else{
                    backtrack_west(type);
                }
            }
        }else if(backtrack_checker(current, type)){
            if(backtrack_add_path(current)){
                return true;
            } else{
                backtrack_west(type);
            }
        }
    }
    return false;
}

bool Utilities::GridFull::is_closer(int first, int second, int sink){
    if(abs(sink - first) > abs(sink - second)){
        return true;
    }
    return false;
}

bool Utilities::GridFull::search_north(NodeFull* border_node, Type type, int custom_cost, int sink){
    int north_y = border_node->get_y() + 1;
    int north_x = border_node->get_x();
    int cost = border_node->get_cost() + 1;
    if(north_y < get_height()){
        // node is in grid range
        if(type == k2bit){
            return increment_path(grid.at(north_y).at(north_x), custom_cost, type);
        }else if(type == kHadlock){
            bool is_detour = true;
            if(is_closer(north_y - 1, north_y, sink)){
                cost -= 1;
                is_detour = false;
            }
            return increment_path(grid.at(north_y).at(north_x), cost, type, is_detour);
        } else return increment_path(grid.at(north_y).at(north_x), cost, type);
    }
    return false;
}

bool Utilities::GridFull::search_west(NodeFull* border_node, Type type, int custom_cost, int sink){
    int west_y = border_node->get_y();
    int west_x = border_node->get_x() - 1;
    int cost = border_node->get_cost() + 1;
    if(west_x >= 0){
        // Western node is within grid limits
        if(type == k2bit){
            return increment_path(grid.at(west_y).at(west_x), custom_cost, type);
        }else if(type == kHadlock){
            bool is_detour = true;
            if(is_closer(west_x + 1, west_x, sink)){
                cost -= 1;
                is_detour = false;
            }
            return increment_path(grid.at(west_y).at(west_x), cost, type, is_detour);
        } else return increment_path(grid.at(west_y).at(west_x), cost, type);
    }
    return false;
}

bool Utilities::GridFull::search_south(NodeFull* border_node, Type type, int custom_cost, int sink){
    int south_y = border_node->get_y() - 1;
    int south_x = border_node->get_x();
    int cost = border_node->get_cost() + 1;
    if(south_y >= 0){
        // Southern node is within grid 
        if(type == k2bit){
            return increment_path(grid.at(south_y).at(south_x), custom_cost, type);
        }else if(type == kHadlock){
            bool is_detour = true;
            if(is_closer(south_y + 1, south_y, sink)){
                cost -= 1;
                is_detour = false;
            }
            return increment_path(grid.at(south_y).at(south_x), cost, type, is_detour);
        } else return increment_path(grid.at(south_y).at(south_x), cost, type);
    }
    return false;
}

bool Utilities::GridFull::search_east(NodeFull* border_node, Type type, int custom_cost, int sink){
    int east_y = border_node->get_y();
    int east_x = border_node->get_x() + 1;
    int cost = border_node->get_cost() + 1;
    if(east_x < get_width()){
        // Eastern node is within grid limits
        if(type == k2bit){
            return increment_path(grid.at(east_y).at(east_x), custom_cost, type);
        }else if(type == kHadlock){
            bool is_detour = true;
            if(is_closer(east_x - 1, east_x, sink)){
                cost -= 1;
                is_detour = false;
            }
            return increment_path(grid.at(east_y).at(east_x), cost, type, is_detour);
        } else return increment_path(grid.at(east_y).at(east_x), cost, type);
    }
    return false;
}

bool Utilities::GridFull::increment_path(NodeFull* new_border_node, int cost, Type type, bool is_detour){
    if(type == k3bit){
        if(cost > 3){
            cost = 1;
        }
    }
    if(new_border_node->get_status() == kSink){
        new_border_node->set_cost(cost);
        trace_path.push(new_border_node);
        return true;
    }
    if(new_border_node->get_status() >= kFree){
        new_border_node->set_status(kBorder);
        new_border_node->set_cost(cost);
        if(type == kHadlock){
            if(is_detour){
                border_hadlock.push_back(new_border_node);
            }else{
                border_hadlock.push_front(new_border_node);
            }
            return false;
        }
        border.push(new_border_node);
    }
    return false;
}

