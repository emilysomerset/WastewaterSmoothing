
<!-- README.md is generated from README.Rmd. Please edit that file -->

# WastewaterSmoothing

<!-- badges: start -->
<!-- badges: end -->

The goal of the `WastewaterSmoothing` package is

## Installation

You can install the development version of

## Example

We consider the following data set of respiratory syncytial viral (RSV)
in California, which is available from the WastewaterSCAN Dashboard

``` r
#library(WastewaterSmoothing)
load('./data/california_rsv.rda')

head(work_d)
#>   sample_id sample_date dilution bcov_recovery bcov_recovery_lci
#> 1 COD110188  2021-11-15        1         0.857             0.802
#> 2 COD110149  2021-11-20        1         0.709             0.662
#> 3 COD110156  2021-11-27        1         0.919             0.754
#> 4 COD110172  2021-12-06        1         0.909             0.842
#> 5 COD110206  2021-12-13        1         0.598             0.554
#> 6 COD110187  2021-12-18        1         1.289             1.171
#>   bcov_recovery_uci                 name site_id sewershed_pop RSV_num_wells
#> 1             0.912 CODIGA, Stanford, CA  CODIGA         10000             8
#> 2             0.756 CODIGA, Stanford, CA  CODIGA         10000             8
#> 3             1.107 CODIGA, Stanford, CA  CODIGA         10000             1
#> 4             0.979 CODIGA, Stanford, CA  CODIGA         10000             8
#> 5             0.643 CODIGA, Stanford, CA  CODIGA         10000             8
#> 6             1.401 CODIGA, Stanford, CA  CODIGA         10000             8
#>   RSV_gc_g_dry_weight RSV_gc_g_dry_weight_lci RSV_gc_g_dry_weight_uci
#> 1           13862.522                9726.777               19031.930
#> 2            1874.510                 972.549                3215.686
#> 3           19799.325               10288.686               33898.213
#> 4           62085.470               54427.350               70427.350
#> 5            8589.436                5856.904               11134.393
#> 6           62494.118               49694.118               73662.745
#>   RSV_gc_g_dry_weight_pmmov RSV_gc_g_dry_weight_pmmov_lci
#> 1              2.100839e-05                  1.250497e-05
#> 2              8.883698e-07                  2.323271e-07
#> 3              8.186462e-06                  2.202204e-06
#> 4              1.055539e-04                  8.648869e-05
#> 5              1.665014e-05                  1.032970e-05
#> 6              9.438260e-05                  7.068959e-05
#>   RSV_gc_g_dry_weight_pmmov_uci RSV_gc_g_dry_weight_trimmed5
#> 1                  2.951180e-05                     13862.52
#> 2                  1.544412e-06                     16830.92
#> 3                  1.417072e-05                     14083.76
#> 4                  1.246192e-04                     30158.08
#> 5                  2.297058e-05                     32347.49
#> 6                  1.180756e-04                     28610.86
#>   RSV_gc_g_dry_weight_trimmed5_pmmov
#> 1                       8.186462e-06
#> 2                       1.459742e-05
#> 3                       1.528166e-05
#> 4                       3.973973e-05
#> 5                       4.005351e-05
#> 6                       4.005351e-05
```
