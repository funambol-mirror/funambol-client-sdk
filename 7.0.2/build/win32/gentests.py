#
# Usage: gentest.py <library.lib> <dependencies.c>
#
# Finds all CPPUnit tests suites defined with
# FUNAMBOL_TEST_SUITE_REGISTRATION/FUNAMBOL_TEST_SUITE
# and generates a C source file which references all
# of them. Compiling that C source file and adding
# it to a test runner ensures that all tests are added
# to the resulting executable.
#

import sys, os, re

output = open(sys.argv[2], "w")


#import subprocess
#(dummy,input) = os.popen4("dumpbin " + sys.argv[1])
#p = subprocess.Popen("dumpbin " + sys.argv[1],
#                     shell=True,
#                     stdout=subprocess.PIPE)
# input = p.stdout

os.system("dumpbin /symbols /out:%s.tmp %s" % (sys.argv[1], sys.argv[1]))
input = open("%s.tmp" % sys.argv[1])

tests = []
pattern = re.compile(r'\| _(\S*funambolAutoRegisterRegistry\S*)')
for line in input:
	# output.write("/* " + line + "*/\n")	
	m = pattern.search(line)
	if m:
		print "test: %s" % m.group(1)
		tests.append(m.group(1))

output.write("/* automatically generated, do not edit */\n");
if len(tests) > 0:
	output.write("extern int " + ", ".join(tests) + ";\n")
	output.write("int *alltests[] = { &" + \
	             ", &".join(tests) + \
	             " };\n")

print "done"
