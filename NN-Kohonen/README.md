Title:<br/>
<b>NN-Kohonen: Neural Networks - Kohonen model in 'vanilla' C/C++</b>

Description:<br/>
<p>This package implements the basic Kohonen ANN model. It contains all source code to define the structures and functionality of such ANN. It also contains some tools to test the behaviour of the constructed networks.</p>
<p>The program can be used to implement any Kohonen network, with one "dummy" input layer, one neuron grid (X*Y neurons) and a "dummy" output grid to carry out values from the "hidden" neuron grid. Parameters define the grid size, training coefficients and cycles. During training there is an "ETA" displayed value to show the estimated time remaining to complete training, together with some other training parameters as they change throughout the process. Network status (weight values) are periodically dumped into an output file, used to determine the network throughput after the training has completed.</p>
<p>The initial values for weights can be random or normalized. The success of the training and the quality of the final network is dependent by various factors such as the parameters of learning and distance coefficients, the number of training cycles (repetitions) and of course the network's structure and the quality of random input.</p>
<p>Although such networks normally work fine when they are used for "mapping" the input vector's space into a two-dimensional space, it has some difficulties with some problems such as the mapping of a 2-dim vector set into the grid. The reason is because some times the input vectors are very close to the initial values of some weights and as a result these weights are continiously updated during training, although others remain at short "distant" values. This problem can be (partially) solved with the use of conscience technics (although tests show minimum gain) and mainly by initializing weights with normalized values. For example, for 2-dim input vectors (so, 2 weights per neuron) the initial values of weights for each neuron must be random points from a circle of radius 1 (that is:  w = ( x , sqrt(1-x^2)).</p>
<p>The package has been originally developed for the Solaris/SparcOS, but it is strictly a 'vanilla' C/C++ implementation and should compile with any compiler and any platform. The source files contained in this package (mainly the "kohmem.c" file) can be used to construct a more abstruct model for ANN implementation, from one neuron layer (such as Perceptron) to multi-level neuron grids (such as Neocognitron).</p>
<p>For more details about the program structure and functionality, see the source code (and inline comments) in the "src" directory.</p>
<p>Files listing:</p>
<pre>
src/			KOHONEN main source directory
src/exitcode.h		Program exit codes definition
src/koh.c		Main program source
src/koh.h		Main program parameters
src/kohmem.c		ANN network memory structures definition
src/kohnet.c		Kohonen ANN network functional definition
src/kohnet.h		Kohonen ANN network parameters
src/kohx.c		Program variation - Input is diagonal array with 1 and 0 (Aii=1)
src/gencrnd.c		Input file creation tool - Normalized pairs
src/genrnd.c		Input file creation tool - Random NxM numbers
src/dview.c		Create graphs from XY-series of data, in a text window
src/datanal.c		Procedures to calculate basic statistical variables (E[x],Var[x])

bin/			Binaries for Windows (CLI)

examples/rndcinp100		Sample input file - 100 normalized pairs
examples/rndcinp1000		Sample input file - 1000 normalized pairs
examples/rndinp100		Sample input file - 100 random pairs
examples/rndinp1000		Sample input file - 1000 random pairs
examples/rndcinp100.plot	Sample text-plot from 'dview' tool
examples/dmpdat.1000i10n10000c.nn		Network statistics (1000x2 inputs, 10x10 neurons, 10000 cycles)
examples/dmpdat.1000i10n10000c.nr		Network statistics (1000x2 inputs, 10x10 neurons, 10000 cycles)
examples/dmpdat.1000i20n10000c.nn		Network statistics (1000x2 inputs, 10x10 neurons, 10000 cycles)
examples/dmpdat.1000i20n10000c.nr		Network statistics (1000x2 inputs, 20x20 neurons, 10000 cycles)
examples/res.1000i20n10000c.nn		Program output (1000x2 inputs, 20x20 neurons, 10000 cycles)
examples/res.1000i20n10000c.nr		Program output (1000x2 inputs, 10x10 neurons, 10000 cycles)
examples/res.2000i15n20000c.rr		Program output (2000x2 inputs, 15x15 neurons, 20000 cycles)
</pre>
