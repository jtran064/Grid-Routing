#ifndef _GRID_FULL_H_
#define _GRID_FULL_H_

#include "nodefull.h"
#include "path.h"
#include "problem_object.h"
#include <vector>
#include <queue>
#include <stack>
#include <deque>
// #include <utility>

namespace Utilities {
    class GridFull{
        
        private:
            vector<vector<NodeFull*> > grid;   
            std::queue<std::pair<NodeFull*, NodeFull*> > connections;
            std::queue<NodeFull*> border;
            std::deque<NodeFull*> border_hadlock;
            std::stack<NodeFull*> trace_path;
            vector<Path*> paths;
            
        public:
            enum Type{kNormal, k3bit, k2bit, kHadlock};
        
            // Constructor
            GridFull(ProblemObject* problem_object);
            
            // Accessors
            int get_width();
            int get_height();
            
            // object checker
            void check_connections(ProblemObject* problem_object);
            void include_blockers(ProblemObject* problem_object);
            
            // algorithm
            vector<Path*> run_algorithm(Type type = kNormal);
            void run_algorithm_step(Type type = kNormal);
            NodeFull* load_connection();
            void convert_to_path();
            void clean_up_grid();
            //search functions
            bool search(Type);
            bool increment_path(NodeFull*, int, Type type = kNormal, bool = false);
            bool search_north(NodeFull*, Type type = kNormal, int = 0, int = 0);
            bool search_west(NodeFull*, Type type = kNormal, int  = 0, int = 0);
            bool search_south(NodeFull*, Type type = kNormal, int = 0, int = 0);
            bool search_east(NodeFull*, Type type = kNormal, int  = 0, int = 0);
            bool is_closer(int, int, int);
            //path functions
            void backtrack(Type);
            bool backtrack_checker(NodeFull*, Type);
            bool backtrack_add_path(NodeFull*);
            bool backtrack_direction(int&, const int, const int, Type);
            bool backtrack_pick(int, Type);
            bool backtrack_north(Type);
            bool backtrack_south(Type);
            bool backtrack_west(Type);
            bool backtrack_east(Type);
            
            // Hadlock
            double distance(NodeFull*, NodeFull*);
            
            // testers
            void print_grid();
            void print_path();
    };
}


#endif