// This file was generated by Rcpp::compileAttributes
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// recmap
DataFrame recmap(DataFrame df);
RcppExport SEXP recmap_recmap(SEXP dfSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< DataFrame >::type df(dfSEXP);
    __result = Rcpp::wrap(recmap(df));
    return __result;
END_RCPP
}
