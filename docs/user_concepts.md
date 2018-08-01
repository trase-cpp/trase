User Concepts
=============

Trase roughly follows the same concepts as described in the Grammer of Graphics
\cite wilkinson2006grammar. In general, three different items must be defined to
create each plot, these are:

1.  **A Dataset with Aesthetics** (\ref trase::DataWithAesthetic) - A dataset is
    a \f$n \times m\f$ matrix of data, where \f$n\f$ is the number of data
    points (i.e. rows) and \f$m\f$ is the number of columns. Each column of this
    matrix is labelled with a given \ref trase::Aesthetic. Currently there are 4
    different aesthetics in Trase: \ref trase::Aesthetic::x,
    \ref trase::Aesthetic::y, \ref trase::Aesthetic::size, and
    \ref trase::Aesthetic::color. A user will normally create a dataset using
    the \ref trase::create\_data() function, by passing in columns of data using
    a `std::vector` labelled with a given Aesthetic. For example:

    \snippet tests/TestUserConcepts.cpp user\_concepts\_data\_set

    \note When creating a \ref trase::DataWithAesthetic object using
    \ref trase::create\_data, the Aesthetic labels are set using member
    functions of the \ref trase::DataWithAesthetic class. These return
    references to the \ref trase::DataWithAesthetic object and can thus be
    chained together.

2.  **A Geometry** (\ref trase::Plot1D) - A Geometry defines how the data is
    displayed in the plot. It takes columns of data corresponding to specific
    Aesthetics and draws them in a certain way. A simple example of this is a
    line plot, which draws linear line segments between 2D points given by the x
    and y Aesthetics. A geometry is created by calling a specific member
    function of the figure axis, for example, the following code creates a
    figures and then creates a line geometry on that figure's (default) axis
    object

    \snippet tests/TestUserConcepts.cpp user\_concepts\_geometry

    Below is a table of currently implemented geometries, along with their default
    transforms and required Aesthetics.

Geometry               | Default Transform     | Required Aesthetics  | Creation function |
-----------------------| ----------------------| ---------------------|-------------------|
\ref trase::Line       | \ref trase::Identity  | x, y                 | trase::Axis::line |
\ref trase::Points     | \ref trase::Identity  | x, y, color, size    | trase::Axis::points |
\ref trase::Histogram  | \ref trase::BinX      | x                    | trase::Axis::histogram |


3.  **A Transform** (\ref trase::Transform)- A transform maps an input dataset
    to an output dataset. For example, the line geometry has the \ref trase::Identity transform as its
    default, which simply passes the input unchanged to the output. Each geometry has a default transform, which it
    applies to the input dataset prior to displaying the output dataset.

    \note if you wish to change the default transform for a given geometry, just pass in the new transform to the creation function
