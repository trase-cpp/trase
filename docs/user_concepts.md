# User Concepts

Trase roughly follows the same concepts as described in the Grammer of Graphics. 
In general, three different items must be defined to create each plot, these 
are:

1. **Dataset with Aesthetics** - A  dataset is a $n \cross m$ matrix of data. 
   Each column of this matrix is labelled with a given Aesthetic. There are 
   $N=4$ different aesthetics in Trase: x, y, size, color. A user will create a 
   dataset by passing in columns of data using a `std::vector`, labelled with a 
   given Aesthetic.
2. **Geometry** - A Geometry defines how the data is displayed in the plot. It 
   takes columns of data corresponding to specific Aesthetics and draws them in 
   a certain way. A simple example of this is a line plot, which draws linear 
   line segments between 2D points given by the x and y Aesthetics.
3. **Transform** - A Transform maps an input Dataset to an output Dataset. Each 
   Geometry has a default Transform, which it applies to the input Dataset prior 
   to displaying the output Dataset. For example, the 1D histogram geometry has 
   as its default Transform the BinX Transform, which takes the x Aesthetic from 
   the input dataset and bins these data points into $N$ bins defined from the 
   minimum x point to the maximum x point. The number of input data points in 
   each bin is written into the output y Aesthetic. The line Geometry has the 
   Identity Transform as its default, which simply passes the input unchanged to 
   the output.
