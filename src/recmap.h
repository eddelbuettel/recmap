#ifndef RECMAP_H
#define RECMAP_H
  
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <vector>
#include <stack>
#include <iostream>
#include <string>
#include <iterator>
#include <sstream>
  
  /*
  
  Authors   :  <Christian.Panse@gmail.com>
  
  $HeadURL: $
  $Id: $
  
  */
  
  
namespace crecmap{


  typedef struct {
    double x, y, dx, dy, z;
    int id;
    double area_desired;
    std::string name;
    std::vector<int> connected;
  } map_region;

// TODO(cp): unify graph and map_region
typedef std::vector<std::vector<int>> graph;

  class Crecmap{
  
    typedef std::vector<map_region> recmapvector; 
    
    graph PD0;
    graph PD1;
    recmapvector RecMap;
    recmapvector Cartogram;
    int num_regions;
    
  public:
  
    //template < class Tdouble > 
    
    Crecmap() { num_regions = 0;}
    
    void push(double x, double y, double dx, double dy, double z){
      
      map_region R, R1; 
      
      R.x=x; R.y=y; R.dx=dx; R.dy = dy; R.z =z;
      R.id = num_regions;
      R.area_desired = -1;
      
      R1.x=-1; R1.y=-1; R1.dx = dx; R1.dy = dy; R1.z =z;
      R1.id = num_regions;
      R1.area_desired = -1;
      // R.name = name;
      // not needed for the algorithm
      PD0.push_back({});
      PD1.push_back({});
      
      RecMap.push_back(R);
      Cartogram.push_back(R1);
      num_regions++;
      
      if (num_regions != RecMap.size()){
        
      }
    }
    
    int get_size(){
      return num_regions;
    }
    
    /*
     * BEGIN DEBUG FUNCTIONS
     */
    void fun(){
      std::for_each(RecMap.begin(), RecMap.end(), 
                    [](map_region &r){std::swap(r.x, r.y);});
    }
    
    void print_pseudo_dual(graph &G, recmapvector &M){
      for (map_region r: M){
        std::cout << std::endl << r.id << "\t(" << r.x << ", " << r.y << ")" << std::endl;
        for (int i: G[r.id]){
          std::cout << " " <<  i ;
        }
      }
      
      for (int i = 0; i <= M.size(); i++){
        map_region r = RecMap[i];
        std::cout << r.id << " ";
      }
      std::cout << std::endl;
    }
    
    
    /* 
     * END OF DEBUG FUNCTIONS
     * 
     */
    
    
    /*  TODO: this can not work 
     *  map_region& operator[](const int i){
     * return (i * sizeof(map_region))}
     */
    map_region& get_map_region(int i){
        return(Cartogram[i]);
    }
    
    // http://stackoverflow.com/questions/17787410/nested-range-based-for-loops
    template<typename C, typename Op1, typename C1>
    void each_unique_pair(C& container, C1& container1, Op1 fun1){
      for(auto it = container.begin(); it != container.end() - 1; ++it)
        for(auto it2 = std::next(it); it2 != container.end(); ++it2)
          fun1(*it, *it2, container1);
    }
    
    
    void ComputePseudoDual(recmapvector &M, graph &G){
      each_unique_pair(M, G,
                       [](map_region &a, map_region &b, graph &G){
         /*
          *  http://gamemath.com/2011/09/detecting-whether-two-boxes-overlap/
          */
                         if (a.x + a.dx < b.x - b.dx) return false; // a is left of b
                         else if (a.x - a.dx > b.x + b.dx) return false; // a is right of b
                         else if (a.y + a.dy < b.y - b.dy) return false; // a is above b
                         else if (a.y - a.dy > b.y + b.dy) return false; // a is below b
                         // add edges tp pseudo dual graph iff boxes are connected 
                         G[a.id].push_back(b.id);
                         G[b.id].push_back(a.id);
                         return true;
                       });
    }
    // http://stackoverflow.com/questions/17787410/nested-range-based-for-loops
    template<typename C, typename Op1>
    void each_unique_pair__(C& container, Op1 fun1){
      for(auto it = container.begin(); it != container.end() - 1; ++it)
        for(auto it2 = std::next(it); it2 != container.end(); ++it2)
          fun1(*it, *it2, container);
    }
    
    void ComputePseudoDual__(recmapvector &M){
      each_unique_pair__(M, [](map_region &a, map_region &b, recmapvector &M){
                         /*
                         *  http://gamemath.com/2011/09/detecting-whether-two-boxes-overlap/
                         */
                         if (a.x + a.dx < b.x - b.dx) return false; // a is left of b
                         else if (a.x - a.dx > b.x + b.dx) return false; // a is right of b
                         else if (a.y + a.dy < b.y - b.dy) return false; // a is above b
                         else if (a.y - a.dy > b.y + b.dy) return false; // a is below b
                         // add edges tp pseudo dual graph iff boxes are connected 
                         M[a.id].connected.push_back(b.id);
                         M[b.id].connected.push_back(a.id);
                         return true;
                       });
    }
    

    void ComputeDesiredArea(recmapvector &M, recmapvector &C){
      double sum_z = 0.0;
      double sum_area = 0.0;
      
      std::for_each(M.begin(), M.end(), [&] (map_region &r) {sum_z += r.z;});
      std::for_each(M.begin(), M.end(), [&] (map_region &r) {sum_area += (4 * r.dx * r.dy);});
      
      std::for_each(C.begin(), C.end(), [&] (map_region &r) {
        double area_desired = r.z * sum_area / sum_z;
        double ratio = r.dy / r.dx;
        r.dx = std::sqrt(area_desired / (4 * ratio));
        r.dy = r.dx * ratio;
        });
    }
    
    // TODO(cp): has to be implemented
    int ComputeCoreRegion(recmapvector &M, recmapvector &C){
      int core_region_id = num_regions / 2;
      C[core_region_id].x = M[core_region_id].x;
      C[core_region_id].y = M[core_region_id].y;
        
      return core_region_id;
    }
    
    
    // place rectangle around predecessor_region_id if this violates the 
    // constrain do a bfs until the box can be placed. 
    // TODO(cp): 'spatial bfs' or later try several alternatives and evaluate
    void PlaceRectangle(int predecessor_region_id, int current_region_id, recmapvector &C, graph &PD1){
      std::cout << predecessor_region_id << '\t' << current_region_id << std::endl;
    }
    
    // expore existing map and places the rectangles acc. specification using dfs 
    void DrawCartogram(recmapvector &M, recmapvector &C, int start_region_id){
      std::stack<int> stack;
      std::vector<int> visited(num_regions, 0);
      std::vector<int> dfs_num(num_regions, 0);

      int dfs_num_counter = 0;
      int current_region_id = start_region_id;
      stack.push(current_region_id);
      visited[current_region_id]++;
      dfs_num[current_region_id] = dfs_num_counter++;

      int predecessor_region_id;
        
      while (stack.size()  > 0){
        predecessor_region_id = current_region_id;
        current_region_id = stack.top() ; stack.pop();

        if (predecessor_region_id != current_region_id)
          PlaceRectangle(predecessor_region_id, current_region_id, C, PD1);

        for(int adj_region_id: M[current_region_id].connected){
          if (visited[adj_region_id] == 0) {
            visited[adj_region_id]++;
            stack.push(adj_region_id);
            dfs_num[adj_region_id] = dfs_num_counter++;
          }
        }
      } // while
    }
  
    
    void run(){
      //ComputePseudoDual(RecMap, PD0);
      ComputePseudoDual__(RecMap);
      
      ComputeDesiredArea(RecMap, Cartogram);
      
      int core_region_id = ComputeCoreRegion(RecMap, Cartogram);
      
      DrawCartogram(RecMap, Cartogram, core_region_id);
  
      // determine core region to start
      // dfs 
      
    }//run  
  };
}// namespace
  
#endif  