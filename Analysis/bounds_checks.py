#!/usr/bin/python3
from disasm import *

def findBoundsChecks(instructionList):
	for i in instructionList:
		# We're looking for calls
		if (i.mnemonic != "call"):
			continue
		
		# Only calls related to 0x4010f4
		targets = i.resolveCall()
		targetMatch = False
		if "dword ptr [0x4010f4]" not in targets:
			continue
		
		yield i

def boundsAnalysis(instructionList):
	for i in findBoundsChecks(instructionList):
		# If we have a matching call, trace back to the last conditional branch, and then what affected that
		i.printInst()
		
		# Find the conditional branch for the bounds check
		for src in i.tracebackToConditionalBranch():
			aliases = []
			src.printInst(1)
			
			# Find the comparison instruction for the bounds check
			for src2 in src.tracebackToRegWrite("eflags"):
				src2.printInst(1)
				
				aliases = src2.traceMovAliases(src2.operands[0])
				print("\tAliases: " + str(tuple(x[0] for x in aliases)))
				
				if (len(aliases) > 0):
					usages = src2.traceforwardToUsage(aliases[0][0], badPath=i)
					print("\tUsages:")
					for usage in usages:
						usage.printInst(2)
	
	# NOTICE: THIS IS A WORK IN PROGRESS

if __name__ == "__main__":
	instructionList = loadInstructionList()
	
	boundsAnalysis(instructionList)