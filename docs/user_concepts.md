# User Concepts

Trase roughly follows the same concepts as described in the Grammer of Graphics 
\cite wilkinson2006grammar. In general, three different items must be defined to 
create each plot, these are:

1. [A Dataset with Aesthetics](\ref trase::DataWithAesthetic) - A dataset is a 
   \f$n \times m\f$ matrix of data, where \f$n\f$ is the number of data points 
   (i.e. rows) and \f$m\f$ is the number of columns. Each column of this matrix 
   is labelled with a given \ref trase::Aesthetic. Currently there are 4 
   different aesthetics in Trase: \ref trase::Aesthetic::x, \ref 
   trase::Aesthetic::y, \ref trase::Aesthetic::size, and \ref
   trase::Aesthetic::color. 
   
   A user will normally create a dataset using the \ref trase::create_data() 
   function, by passing in columns of data using a `std::vector` labelled with a 
   given Aesthetic. For example:

   \snippet tests/TestUserConcepts.cpp data_set_with_aesthetic

   \note When creating a [DataWithAesthetic](\ref trase::DataWithAesthetic) 
   object using [create_data](\ref trase::create_data()), the Aesthetic labels 
   are set using member functions of the [DataWithAesthetic](\ref 
   trase::DataWithAesthetic) class. These return references to the 
   [DataWithAesthetic](\ref trase::DataWithAesthetic) object and can thus be 
   chained together.

2. **A Geometry** (Plot1D) - A Geometry defines how the data is displayed in the 
   plot. It takes columns of data corresponding to specific Aesthetics and draws 
   them in a certain way. A simple example of this is a line plot, which draws 
   linear line segments between 2D points given by the x and y Aesthetics.
3. **A Transform** (Transform)- A Transform maps an input Dataset to an output 
   Dataset. Each Geometry has a default Transform, which it applies to the input 
   Dataset prior to displaying the output Dataset. For example, the 1D histogram 
   geometry has as its default Transform the BinX Transform, which takes the x 
   Aesthetic from the input dataset and bins these data points into $N$ bins 
   defined from the minimum x point to the maximum x point. The number of input 
   data points in each bin is written into the output y Aesthetic. The line 
   Geometry has the Identity Transform as its default, which simply passes the 
   input unchanged to the output.
