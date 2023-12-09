The efficient frontier, depicted as a hyperbola, delineates the range of optimal portfolios attainable from given assets. 
The program identifies the least risky portfolios (with the lowest volatility) for various expected returns. 
A portfolio, characterized by a vector of weights denoting asset allocations, sums to 1 (or 100%). 
The program, tasked with estimating optimal weights for returns ranging from 1% to 26% in 1% intervals, leverages the Eigen Package.

An additional functionality, triggered by the "-r" option, enforces a constraint against short sales, ensuring non-negative weights. 
The resulting values for returns of 1-4%, for instance, are adjusted accordingly.

The program initiates by computing weights for the unrestricted case, subsequently addressing negative weights. 
While any negative weights persist, the program augments the linear system with equations constraining negative values to zero. 
This involves adding linear equations for each negative weight, and the weights are recalculated by solving the updated system.

In the process of identifying negative weights, the program introduces linear equations to the matrix, setting the corresponding variables to zero. 
A new row is appended to the matrix with a 1 in the relevant position, and a corresponding "0" row is added to the right-hand side vector. 
This iteration continues until no negative weights remain or the weights approach a negligible value close to zero, having a minimal impact on portfolio volatility. 
It ensures that each negative weight is added only once.
