#!/bin/csh -f
perl -0 -pe 's/,([ \t\r\n]*[\]\}])/\1/g; s/,\Z//' 
