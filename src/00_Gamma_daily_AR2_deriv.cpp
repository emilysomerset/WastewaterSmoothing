#include <TMB.hpp>                                // Links in the TMB libraries
//#include <fenv.h>

template<class Type>
Type objective_function<Type>::operator() ()
{
  DATA_VECTOR(y); // response variable
  // DATA_VECTOR(t); // need this to calculate delta
  DATA_VECTOR(denom); // response variable
  DATA_SPARSE_MATRIX(X); // Design matrix (for fixed effects)
  DATA_SPARSE_MATRIX(B); // Design matrix (for random effects)
  DATA_SPARSE_MATRIX(P); // Penalty matrix
  DATA_SPARSE_MATRIX(daily);
  DATA_SPARSE_MATRIX(obs); //observed data indexes 
  DATA_IVECTOR(stationsizes);
  DATA_IVECTOR(n1);
  DATA_IVECTOR(y_ind_obs);
  // DATA_IVECTOR(y_ind_cens);
  // DATA_IVECTOR(cens_dir); // 1 is <= detection. 0 is >= detection. 
  
  int d = P.cols(); // Number of B-Spline coefficients
  DATA_SCALAR(logPdet); // Determinant of (fixed) penalty matrix
  DATA_SCALAR(u1); // pc prior, u1 param
  DATA_SCALAR(alpha1); // pc prior, alpha1 param
  DATA_SCALAR(u2); // pc prior, u2 param
  DATA_SCALAR(alpha2); // pc prior, alpha2 param
  DATA_SCALAR(u3); // pc prior, u2 param
  DATA_SCALAR(alpha3); // pc prior, alpha2 param
  DATA_SCALAR(u4); // pc prior, u2 param
  DATA_SCALAR(alpha4); // pc prior, alpha2 param
  DATA_SCALAR(betaprec); // beta ~iid N(0,1/betaprec)
  
  // Parameter
  PARAMETER_VECTOR(W); // W = c(U,beta,Z), eta = B * U + X * beta + daily * Z
  // int Wdim = W.size();
  int betadim = X.cols();
  int ndays = daily.cols();
  int nstation = stationsizes.size();
  int nfull = obs.cols();
  // int ncens = y_ind_cens.size();
  vector<Type> U(d);
  vector<Type> beta(betadim);
  vector<Type> Z(ndays);
  vector<Type> IS(nfull);
  matrix<Type> alpha(2,nfull); // alpha
  for (int i=0;i<d;i++) U(i) = W(i);
  for (int i=0;i<betadim;i++) beta(i) = W(i+d);
  for (int i=0;i<ndays;i++) Z(i) = W(i+d + betadim);
  for (int i=0;i<nfull;i++) alpha(0,i) = W(i + d + ndays + betadim);
  for (int i=0;i<nfull;i++) alpha(1,i) = W(i + d + ndays + betadim + nfull);
  
  PARAMETER(theta1); // theta1 = -2log(sigma1)
  PARAMETER(theta2); // theta2 = -2log(sigma1)
  PARAMETER(theta3); //
  PARAMETER(theta4);
  PARAMETER(c_log);
  
  
  // Transformations
  vector<Type> alpha_0 = alpha.row(0);
  vector<Type> eta = X * beta + B * U + daily * Z + obs * alpha_0;
  // Type sigma1 = exp(-0.5*theta1);
  // Type sigma2 = exp(-0.5*theta2);
  Type sigmastat = exp(-0.5*theta3);
  Type shape = exp(-0.5*theta4);
  Type c = exp(c_log);
  vector<Type> scale = exp(eta)/shape * denom;
  Type delta = 1;
  
  // sigma1 = W( d + ndays + betadim + nfull);
  
  // Log likelihood
  Type ll = 0;
  ll += sum(dgamma(y(y_ind_obs), shape, scale(y_ind_obs), TRUE));
  // for (int i=0;i<ncens;i++) {
  //   ll += log(pgamma(y(y_ind_cens(i)), shape, scale(y_ind_cens(i))))*cens_dir(i);
  //   ll += log(1-pgamma(y(y_ind_cens(i)), shape, scale(y_ind_cens(i))))*(1-cens_dir(i));
  // }
  REPORT(ll);
  REPORT(ll);
  
  // Log prior on W
  Type lpW = 0;
  
  // Cross product
  vector<Type> PU = P*U;
  Type UPU = (U * PU).sum();
  lpW += -0.5 * exp(theta1) * UPU; // U part
  Type bb = (beta * beta).sum();
  lpW += -0.5 * betaprec * bb; // Beta part
  Type zz = (Z * Z).sum();
  lpW += -0.5 * exp(theta2) * zz; // Z part
  
  // Log determinant
  Type logdet1 = d * theta1 + logPdet;
  lpW += 0.5 * logdet1; // P part
  
  Type logdet2 = ndays * theta2;
  lpW += 0.5 * logdet2; // Z part
  Rcout << "lpW: " << lpW << "\n";
  
  REPORT(lpW);
  
  // Log prior on alpha
  Type lpA = 0;
  
  //prior for the first column of alpha
  matrix<Type> T(2,2);
  matrix<Type> Sigma(2,2);
  matrix<Type> Sigma_inv(2,2);
  
  T(0,0) = exp(-c*delta)*(1+c*delta);
  T(0,1) = exp(-c*delta)*delta;
  T(1,0) = -exp(-c*delta)*pow(c,2)*delta;
  T(1,1) = exp(-c*delta)*(1-c*delta);
  
  matrix<Type> alpha_mu = T * alpha; // T*alpha
  
  for (int j=0; j<nstation; j++){
  Sigma(0,0) = pow(sigmastat,2)/(4*pow(c,3));
  Sigma(0,1) = 0;
  Sigma(1,0) = 0;
  Sigma(1,1) = pow(sigmastat,2)/(4*c);
  
  Sigma_inv(0,0) = Sigma(1,1);
  Sigma_inv(1,1) = Sigma(0,0);
  Sigma_inv(1,0) = 0;
  Sigma_inv(0,1) = 0;
  vector<Type> PU = Sigma_inv*alpha.col(n1(j));
  vector<Type> alpha_col0 = alpha.col(n1(j));
  Type UPU = (alpha_col0 * PU).sum();
  lpA += -0.5*log(Sigma(0,0)*Sigma(1,1))-0.5*pow(Sigma(0,0)*Sigma(1,1),-1)*UPU;
  Rcout << "lpAInitial: " << lpA << "\n";
  
  for (int i=(n1(j)+1); i < n1(j+1); i++){
    Sigma(0,0) = pow(sigmastat,2)/(4*pow(c,3))*(1-exp(-2*c*delta)*(-2*c*delta*(-delta*c-1)+1));
    Sigma(0,1) = pow(sigmastat,2)*0.5*pow(delta,2)*exp(-2*delta*c);
    Sigma(1,0) = pow(sigmastat,2)*0.5*pow(delta,2)*exp(-2*delta*c);
    Sigma(1,1) = pow(sigmastat,2)/(4*c)*(1-exp(-2*delta*c)*(-2*delta*c*(-delta*c+1)+1));
    
    Sigma_inv(0,0) = Sigma(1,1);
    Sigma_inv(1,1) = Sigma(0,0);
    Sigma_inv(1,0) = -Sigma(1,0);
    Sigma_inv(0,1) = -Sigma(0,1);
    
    vector<Type> resid_alpha = alpha.col(i)-alpha_mu.col(i-1);
    vector<Type> PU = Sigma_inv*resid_alpha;
    Type UPU = (resid_alpha * PU).sum();
    lpA += -0.5*log(Sigma(0,0)*Sigma(1,1)-Sigma(1,0)*Sigma(0,1))-0.5*pow(Sigma(0,0)*Sigma(1,1)-Sigma(1,0)*Sigma(0,1),-1)*UPU;
  }}
  
  
  
  // Log prior for theta
  Type lpT = 0;
  Type phi1 = -log(alpha1) / u1;
  lpT += log(0.5 * phi1) - phi1*exp(-0.5*theta1) - 0.5*theta1;
  Type phi2 = -log(alpha2) / u2;
  lpT += log(0.5 * phi2) - phi2*exp(-0.5*theta2) - 0.5*theta2;
  Type phi3 = -log(alpha3) / u3;
  lpT += log(0.5 * phi3) - phi3*exp(-0.5*theta3) - 0.5*theta3;
  Type phi4 = -log(alpha4) / u4;
  lpT += log(0.5 * phi4) - phi4*exp(-0.5*theta4) - 0.5*theta4;
  Rcout << "lpT: " << lpT << "\n";
  
  // Lop prior for c_log
  Type prior_c_log = dnorm(c_log, Type(0), Type(1), true);
  Rcout << "prior_c_log: " << prior_c_log << "\n";
  
  // Final result!
  Type logpost = -1 * (ll + lpW + lpT + lpA + prior_c_log);
  Rcout << "ll: " << ll << "\n";
  Rcout << "logpost: " << logpost << "\n";
  
  return logpost;
}