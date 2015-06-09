#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cassert>
#include <math.h>
#include <stdlib.h> 

using namespace std;

struct state {
   int x, y, dx, dy;
};

ostream& operator<<(ostream& os, state const& s) {
   return os << "(" << s.x << ", " << s.y << ") (dx=" << s.dx << ", dy=" << s.dy << ")";
}

struct solution {
   int nSteps;
   state s;
};

typedef map<pair<int, int>, solution> mapIItoS;

vector<vector<mapIItoS> > v;     // The s part points to the previous state.
queue<solution> q;

int width, height;
int goalX, goalY;
int circleX, circleY, circleRadius;
int nStatesSeen = 0;

// Assumes that the direction and length of the move would be valid if there
// were no obstacles and we were on an infinite grid.
//HACK: For now, we just check whether this move intersects a single user-provided circle.
bool isMoveValid(state from, state to) {
// return to.x >= 0 && to.x < width && to.y >= 0 && to.y < height;
   if (to.x < 0 || to.x >= width || to.y < 0 || to.y >= height) {
      return false;
   } else {
      if (circleRadius == -1) {
         return true;      // No obstacle
      } else {
         // There's a circular obstacle.
         int dx = to.x - from.x;
         int dy = to.y - from.y;
         
         if (!dx) {
            // Handle vertical lines specially.
            return to.x < circleX - circleRadius || to.x > circleX + circleRadius || max(to.y, from.y) < circleY - circleRadius || min(to.y, from.y) > circleY + circleRadius;
         } else {
            double m = (double) dy / dx;     // Slope
            double c = (to.y - circleY) - m * (to.x - circleX);      // Intercept, after translating so that circle is located at origin
            double discrim = circleRadius * circleRadius * (m * m + 1) - c * c;
                    
            if (discrim < 0) {
               return true;      // Line does not intersect circle anywhere
            } else {
               double sqrtD = sqrt(discrim);
               double x1 = (-m * c - sqrtD) / (m * m + 1);
               if (x1 > max(to.x - circleX, from.x - circleX)) {
                  return true;      // Intersection occurs to the right of the line segment's end
               }
               
               double x2 = (-m * c + sqrtD) / (m * m + 1);
               if (x2 < min(to.x - circleX, from.x - circleX)) {
                  return true;      // Intersection occurs to the left of the line segment's end
               }
               
               return false;     // Line either intersects or is totally contained.
            }
         }
      }
   }
}

// This could be defined in several ways, but currently we simply have
// a single target location, and we don't care about the velocity.
bool isGoalState(state s) {
   return s.x == goalX && s.y == goalY;
}

solution solve() {
   while (!q.empty()) {
      solution s(q.front());
      q.pop();
      ++nStatesSeen;
      
      // Invariant: We already have an optimal path to s stored in v.
      assert(v[s.s.x][s.s.y].find(make_pair(s.s.dx, s.s.dy)) != v[s.s.x][s.s.y].end());
      
      // Generate all valid moves we could make from here.
      for (int dy = -1; dy <= 1; ++dy) {
         for (int dx = -1; dx <= 1; ++dx) {
            solution ns;
            ns.s.dx = s.s.dx + dx;
            ns.s.dy = s.s.dy + dy;
            ns.s.x = s.s.x + ns.s.dx;
            ns.s.y = s.s.y + ns.s.dy;
            ns.nSteps = s.nSteps + 1;
            if (isMoveValid(s.s, ns.s)) {
               // If we haven't seen this new state before, then we have discovered
               // an optimal path to it.
               pair<int, int> newDxDy(make_pair(ns.s.dx, ns.s.dy));
               if (v[ns.s.x][ns.s.y].find(newDxDy) == v[ns.s.x][ns.s.y].end()) {
                  solution tmp(s);
                  ++tmp.nSteps;
                  v[ns.s.x][ns.s.y][newDxDy] = tmp;      // The s field points back to the previous state
                  
                  if (isGoalState(ns.s)) {
                     // We're done!
                     return ns;
                  }
                  
                  q.push(ns);
               }
            }
         }
      }
   }
   
   // We failed to find a path.
   solution s;
   s.nSteps = -1;
   return s;
}

//HACK: For now we just have a totally empty grid of fixed size!
void loadGrid() {
   width = 100;
   height = 100;
   
   // Need to make the 2D matrix large enough
   v.resize(width, vector<mapIItoS>(height));
}

int main(int argc, char **argv) {
   if (argc != 5 && argc != 8) {
      cerr << "Syntax: racetrack startX startY goalX goalY [circleX circleY radius]\n";
      return 1;
   }
   
   int startX = atoi(argv[1]), startY = atoi(argv[2]);
   goalX = atoi(argv[3]);
   goalY = atoi(argv[4]);
   
   loadGrid();
   
   cout << "Starting at (" << startX << ", " << startY << ").\n";
   cout << "Goal is (" << goalX << ", " << goalY << ").\n";
   cout << "Grid size is " << width << "*" << height << " (W*H).\n";
   
   if (argc == 8) {
      circleX = atoi(argv[5]);
      circleY = atoi(argv[6]);
      circleRadius = atoi(argv[7]);
      cout << "A circular obstacle of radius " << circleRadius << " is centred at (" << circleX << ", " << circleY << ").\n";
   } else {
      circleRadius = -1;
      cout << "No obstacle.\n";
   }
   
   solution start;
   start.nSteps = 0;
   start.s.x = startX;
   start.s.y = startY;
   start.s.dx = 0;
   start.s.dy = 0;
   v[startX][startY][make_pair(0, 0)] = start;
   
   q.push(start);
   
   solution final = solve();
   
   if (final.nSteps == -1) {
      cout << "No solution could be found!\n";  
   } else {
      cout << final.nSteps << "-step solution:\n";
      solution cur(final);
      while (cur.nSteps) {
         cout << cur.s << "\n";
         cur = v[cur.s.x][cur.s.y][make_pair(cur.s.dx, cur.s.dy)];
      }
   }
   
   cout << nStatesSeen << " states were examined in the process.\n";
   return 0;
}