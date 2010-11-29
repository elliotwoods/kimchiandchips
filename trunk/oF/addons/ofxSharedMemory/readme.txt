This really isn't a proper addon.
It's a mess, is documented badly and doesn't have complete functionality.
It's more a proof of concept than anything, but I prefer to release things uncomplete otherwise i'd never release anything.
(p.s. hats off entirely to all the people out there that do release pretty complete code, I admire you very much)

One thing in particular to note is that for some reason the managed byte array grows by 28 bytes of crap at the front
so i've had to cut out the first few pixels and send that crap (for the time being).





Instructions (if you're making your own project):

For this to work you need to add a reference to the VVVV Utils
in your project.



1. Add the Utils project to your solution (look for _Utils on the VVVV
SVN. If you've written any plugins then chances are that you have this
already).

2. Right click on your Visual Studio project

	'Add New Reference...'
	'Projects' (tab)
	Choose Utils
	
	I also added the path to Utils project in my
	'Additional reference search paths'


Then also you need to setup CLR
I generally set all the flags in Project>Properties to be as similar as possible to a cpp VVVV plugin. 

The main step is to go to 'Configuration Properties>General' (top level of the properties tree)
and choose 'Common Language Runtime Support (/clr)' under the option box called the same thing.
Chances are your project wont build now, but will throw up lots of errors for things you can track
down in the c++ settings dialog. Lots of compile flags aren't compatible with clr, so hunt them down


Other notes (when using clr, that i've noticed)

In Configuration Properties>Linker>Input, add 'libcpmtd.lib' to Ignore Specific Library