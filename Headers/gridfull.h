#ifndef _GRID_FULL_H_
#define _GRID_FULL_H_

#include "node.h"
#include "path.h"
#include "problem_object.h"
#include "grid.h"
#include <vector>
#include <map>
#include <set>

namespace Utilities {
    class GridFull : public Grid {
        private:
            std::map<Node*, int> grid_nodes;
            /*
                NODE-TYPE CONSTANTS:
                kBlock = -1
                kVistited = -2
                kBorder = -3
                kFree = -4
                
                All sinks and sources are non-negatives. 
                Sources are even
                Sinks are odds and sources + 1
                
                Ex. Source1 = 0, Sink1 = 1. Source2 = 2, Sink2 = 3
            */
            vector<Node* > border;
            
        public:
            // Constructor
            GridFull(ProblemObject* problem_object);
            
            // object checker
            void include_connections(ProblemObject* problem_object);
            void include_blockers(ProblemObject* problem_object);
            
            // testers
            void print_grid();
    };
}


#endif