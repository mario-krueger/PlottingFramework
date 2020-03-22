ANALYSIS:
---------
- straighten up Fit, Simulation, Publication input names
- categorize plots, group them in functions
- add data specific colors and markers
- clean up file names and make useful sub-folders
- add more qa plots for unfolding

FRAMEWORK:
----------
Major Features:
- possibility to dump and load plot styles (manager should only read in required plot styles)
- make sure PlotStyle names are unique
- use TPad function void Range(float x1,float y1,float x2,float y2) to set user coordinate system independent of first histo, graph, etc
- normalizing should be part of histogram drawing options
- add some more formatting features for legend (floating point precision, exponent..)
- add graph division feature with tspline3
- add constructor to plotStyle that already gives useful default markers, colors, 2d style, etc
- use drawing option AXIS and lagrest axis range for ranges or find better way to define which histo should auto-define ranges
(set limits? is this by definition limited to original or can it be changed?)
- possibility to set alias for axis e.g. "ratio"?
- be clear about coordinate systems (fix relative positioning, maybe)
- find a general way to automatically determine optimal axis offsets (in particular for 2d and 3d views) (what unit is title offset?)
- it should be possible to conveniently pipe all root plotting functionality (drawing properties) to the plot
- get rid of tautological stuff for ratio, histo, graph in main loop over data (templates?)
- generalize legend function to handle text boxes in a similar manner
- pads must inherit plot properties (text size et al unless specified otherwise)
- text size and style settings should by default be inherited by each object with text but there should be a possiblity to override
- check if 2d hist is part of plot, then change style

Minor Features:
- option in main program to list loaded plots
- replace PlotGenerator class with namespace
- possibility to load all data from input files
- load only allowed datatypes
- setter for csv format string and delimiter
- would it be possible to define 'top left' 'bottom right' etc default positons for boxes in general manner? maybe with flexible minimal distance to ticks
- is it possible to set the order in multi column tlegends? left-right vs top-bottom
- pipe box line fill properties to drawing
- add possibility to make canvas intransparent (and colored)
- how to handle multi-plots if not all of them are available?
- possibility to scale histograms by factor
- improve use last color feature (color = -1)
- change only color not marker options?
- add line in ratio plots? in plot style? value of const line flexible?
- backward compatibility with "ratio" keyword in axis range and title?
- fix dirty 2d hacks
- fix dirty hacks for backward compatibility regarding ratio plots
- check if padID starts with 1 is a problem (vectors maybe)
- think about more placeholders for legends and texts and how to format them
- option to set n divisions of axes
- add option in padstyle that identifies the pad as ratio plot
- fix overlap between axis title and tick marks if they have too many digits
- increase number of color steps in 2d plots resp make flexible
- possibility to load and plot thstack and multigraphs
- add stack drawing option (thstack)
- add option to change grid styles
- add possibility for grey tilted overlayed text like "draft"
- write case-insensitive "contained in string" function for control string! as lambda function
- text align, angle features!
- possibility for user to grep and change specific plotstyle or plot that was already loaded in manager
- put in some feasible defaults for colors, markers etc
- for loading and saving styles: possibility for default values as fallback?
- possibility to specify use of only full or open markers in AddHisto
- implement check that ensures only valid drawing options are used for each data type (separate 1d 2d info)
- also define default line styles
- generalized version of white patch hiding the truncated zero
- possibility to split the legend?
- maybe linking axes should also be possible for different axes (x in pad1 to y in pad2)

Bugs:
- width and height calculation wrong for text boxes (ndc vs relative pad coordinates?)
- using user coordinates in legend box breaks something for text box?
- interactively moving text boxes in log scale plots has a bug (maybe already in root)
- when saving to macro or pdf, the boxes are slightly misplaced (some global setting missing?)
- make sure figure groups cannot contain '.'
- exponents on x axis are not in the proper position
- boxes are not transparent by default
- sometimes text boxes are randomly not drawn?
- zeroes in histograms should not be drawn if they have no error?
- if lables are 1, 2, 3 height is not calculated correctly?

Structural changes:
- replace couts with LOG macro expressions
- think about how things can be implemented in a more general way as templates
- change arguments referring to internal variables to const ref if possible to be more memory efficient, use lambdas
- add (copy-, move-) constructors and destructors; make sure all variables are initialized properly

Long term goals:
- add TView for 3d representations of th2
- add shape objects
- add functions, fitting
- add data input and stand-alone definable objects (shapes, arrows, functions)

Ideas:
- option for plotting projectins? is this reasonable?