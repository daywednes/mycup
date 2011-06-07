Date : 01/27/2011
 
Developer:	Noam Koenigstein
Email:		noam{dot}kdd{at}gmail{dot}com
  
Copyright (c) 2011 Yahoo! Inc. 
  

The project should be compiled using a C++ compiler. We tried the Visual C++ and the g++ compilers.
The compiled program assumes that all the track-1 data files (listed below) reside in the current directory, from which the executable is run.
It assumes the following filenames:

File                        name
TRACK1 TRAINING FILE        "trainIdx1.txt"
TRACK1 VALIDATION FILE      "validationIdx1.txt"
TRACK1 TEST FILE            "testIdx1.txt"
TRACK1 STATS FILE           "stats1.txt"

Notice that the sample code makes no use of the items hierarchy, hence album, artist and genres data are not read.

When running the executable, the program searches for binary files (with extension .dat) which contain processed data structures.  
In case the binary files are not found, the text files listed above are read and processed, and the binary files are written to the same folder. 
On subsequent executions, the program will locate those binary files and read them instead of reading the original text files (much faster).

A model is then trained and predictions written to "track1_results.txt". The sample model learns items and users biases, using a simple model:
r(u,i) = mue + bi + bu

Where r(u,i) is the predicted rating of user 'u' to item 'i'. 
mue – is a constant the is equal to the overall average rating. 
bi – is item's 'i' bias
bu – is user's 'u' bias

The biases 'bi' and 'bu' are learned using a Stochastic Gradient Descent (SGD) algorithm. The validation dataset is used by SGD for early stopping. 