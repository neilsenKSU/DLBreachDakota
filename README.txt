Dakota User Guides:
https://snl-dakota.github.io/

-----

The configuration file which defines the parameters of the experiment is a ".in" file which will be referred to as "run.in" here. This is NOT to be confused with the params.in or newParams.in files which are intermediary files used in the background.

The different parts of the run.in file are described in the Dakota documentation, but the important parts for these experiments will be briefly discussed here.

The "variables," section defines what input values will be varied with each simulation run.
	The "id_variables" parameter defines what distribution will be used. A list of accepted distributions can be found in the Dakota documentation.
	
	Here we use a triangular distribution which comes with the "triangular_uncertain" parameter that defines how many variables there will be. The triangle distribution is described by a lower bound, upper bound, and mode. For the following parameters, each variable is defined on the same line and separated with spaces.
	
	The parameter that defines the names of variables is "descriptors". An example for three generic variables might be "x y z". For variables which have units, conversion from metric to US or vice versa is possible, by adding a prefix to the variable descriptor. The accepted prefixes are:
		t_	= s -> hr						t.	= hr -> s
		l_	= m -> ft						l.	= ft -> m
		l2_	= cm -> in						l2.	= in -> cm
		a_	= m^2 -> ac						a.	= ac -> m^2
		a2_	= m^2 -> ft^2					a2.	= ft^2 -> m^2
		v_	= m^3 -> ac*ft					v.	= ac*ft -> m^3
		f_	= m^3/s -> ft^3/s				f.	= ft^3/s -> m^3/s
		e_	= cm^3/(N*s) -> ft^3/(lbf*hr)	e.	= ft^3/(lbf*hr) -> cm^3/(N*s)
		p_	= N/m^2 -> lbf/ft^2				p.	= lbf/ft^2 -> N/m^2
		d_	= kN/m^3 -> lbf/ft^3			d.	= lbf/ft^3 -> kN/m^3
	
	This conversion is made possible thanks to the parseIn.exe program which we have created. Additional conversions can be added by altering the parseWinDamIn.c file and recompiling it, or by requesting that we add that conversion.
	
	The parameters that define the value ranges for variables are "tuv_modes", "tuv_lower_bounds", and "tuv_upper_bounds". The lower and upper bounds are self explanatory while the mode parameter is the value that appears most often in a set of values from that range. For our purposes it's fine to treat it as the expected value for that variable.

The "responses," section defines what Dakota will include in the output file, dakota_multidim.dat. It is formatted similar to the variables section.
	"num_response_functions" defines how many output variables will be tracked.
	"descriptors" defines how each of those variables will be labeled.

	Currently, what values are output is fixed by the parseRes.exe program for WinDAM C and the parseOut.py program for DLBreach. These programs scan the output files created by their respective simulatories and pull out the necessary values for Dakota to collect.

The "method," section defines what experimental method will be used to run the experiment. For our purposes the only value that matters is the "partitions" parameter which defines how many values will be tested for each variable. As an example, if I have variables with descriptors of "x y z" and partitions of "8 5 3" then there will be 8 values for x, 5 for y, and 3 for z. That would then be a total of 8 * 5 * 3 = 120 simulation runs.

-----

Another part of setting up an experiment is defining the template file that Dakota will be manipulating for each simulation run. For WinDAM C this will be a ".WDT" file while for DLBreach it'll be a ".txt" file. The specific file for each is hard-coded in the ".bat" file that accompanies the run.in file.

For WinDAM C, the line in the run.bat file is "perl dprepro newParams.in {your template}.WDT {your resulting config}.WDC"

For DLBreach, the line in the run.bat file is "perl dprepro params.in {your template}.txt {your resulting config}.txt"

Where {your template} is the name of your template file, and {your resulting config} is a name for the file created from filling in the "blanks" in the template.

Those blanks are defined by a set of curly braces {} with a variable name inside. The variable name should match one of the names used as a descriptor in your run.in file. For example, if you have the descriptor "l_cw" in your run.in file, then in your template you should have "{l_cw}".

Something important to note!!! In WinDAM C the number of letters/digits matters. Each value must be exactly 9 characters long. The template filler will insert 9 characters, but you must unsure your curly braces take up exactly 9 characters such as "{l_cw   }".

-----

To run the experiment defined in the run.in file, simply have it open in the editor and click the green circle with a play icon in it, or right-click on the file in the Project Explorer panel and select run As > Dakota.

-----

Tips for a successful run:
	Sometimes the newParams.in file gets clogged up with old values that can mess up your simulations. To ensure this doesn't happen: open newParams.in, clear out any contents, and save it.
	
	Sometimes Dakota gets confused about what experiment you are trying to run, this is because it saves run configurations and it may think you are trying to run an old one. If it seems like you're experiments are using an old configuration instead of your current one: click the arrow next to the green play icon in the top left, choose "Run configurations...", and in the dialog window that pops up, under the "Dakota" group in the side pane, select the previous configurations and delete them. Now when you run your experiment, it should be using your current configuration.