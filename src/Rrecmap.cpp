#include <Rcpp.h>
#include "recmap.h"

using namespace Rcpp;

// [[Rcpp::export]]
double get_angle(double x0, double y0, double x1, double y1){
  crecmap::map_region a, b;
  a.x = x0; a.y = y0;
  b.x = x1; b.y = y1;
  
  
  return (crecmap::get_angle(a,b));
}


// [[Rcpp::export]]
DataFrame place_rectanle(double x0, double y0, double dx0, double dy0, double dx1, double dy1, double alpha){
  crecmap::map_region a, b, c;
  
  a.x = x0; a.y = y0; a.dx = dx0; a.dy = dy0;
  c.dx = dx1; c.dy = dy1;
  
  crecmap::place_rectanle(a, alpha, c);
  return DataFrame::create(_["x"]= c.x, _["y"]= c.y, _["dx"]= dx1, _["dy"]= dy1);
}

// [[Rcpp::export]]
DataFrame recmap(DataFrame df) {
  
  // access the columns
  NumericVector x = df["x"];
  NumericVector y = df["y"];
  NumericVector dx = df["dx"];
  NumericVector dy = df["dy"];
  NumericVector z = df["z"];
  CharacterVector name = df["name"];
  
  NumericVector dfs_num(x.size()); 
  //crecmap::crecmap X(Rcpp::as<double>(x));
  crecmap::RecMap X;
  
  // TODO(cp@fgcz.ethz.ch): setting and gettings are pain of the art; fix that asap;
  for (int i=0; i<x.size(); i++){
    std::string sname = Rcpp::as<std::string>(name[i]);
    X.push(x[i], y[i], dx[i], dy[i], z[i], sname);
  }
  
  //X.fun();
  X.run();
  
  for (int i=0; i<x.size(); i++){
    crecmap::map_region r = X.get_map_region(i);
    
    x[i] = r.x;
    y[i] = r.y;
    dx[i] = r.dx;
    dy[i] = r.dy;
    //z[i] = r.z;
    dfs_num[i] = r.dfs_num;
  }

  
  // return a new data frame
  return DataFrame::create(_["x"]= x, _["y"]= y, _["dx"]= dx, _["dy"]= dy,_["z"]= z, _["name"]= name, _["dfs_num"] = dfs_num);
}


/*** R
r <- recmap:::.checker_board(2)
class(r) <- "data.frame"
r2 <- recmap(r)
#class(r2) <- "recmapFrame"
#plot(r2)
*/

// sourceCpp("./src/Rrecmap.cpp"); plot(r2)
