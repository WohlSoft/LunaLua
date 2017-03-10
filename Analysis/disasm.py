#!/usr/bin/python3
import pefile
import capstone as cs

# Fake inst for reg name fetching
_fakeinst = list(cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32).disasm(b"\x90", 0))[0]
def reg_name(reg):
	return _fakeinst.reg_name(reg)

def _getReadWriteOps(i, t=cs.CS_AC_READ):
	opList = []
	idx = 0
	op_str_list = tuple(x.strip() for x in i.op_str.split(","))
	for op in i.operands:
		if ((op.access & t) == 0):
			pass
		elif (op.type == cs.x86.X86_OP_REG):
			opList.append(reg_name(op.reg))
		else:
			opList.append(op_str_list[idx])
		idx += 1
	return opList

def _getFullReadRegs(i):
	regSet = set(reg_name(x) for x in i.regs_read)
	for op in i.operands:
		if ((op.access & cs.CS_AC_READ) == 0):
			continue
		elif (op.type == cs.x86.X86_OP_REG):
			regSet.add(reg_name(op.reg))
	return regSet
	
def _getFullWriteRegs(i):
	regSet = set(i.reg_name(x) for x in i.regs_write)
	for op in i.operands:
		if ((op.access & cs.CS_AC_WRITE) == 0):
			continue
		elif (op.type == cs.x86.X86_OP_REG):
			regSet.add(i.reg_name(op.reg))
	return regSet

def _cleanDeadEndCalls(instructionList):
	for i in instructionList:
		# We're looking for calls
		if (i.mnemonic != "call"):
			continue

		# Only calls related to 0x4010f4
		if not ("dword ptr [0x4010f4]" in i.resolveCall()):
			continue

		#print("Remove link after 0x%08X" % i.address)

		# Remove link after dead end, and disconnect
		if (i.nextAddr in i.tbl):
			child = i.tbl[i.nextAddr]
			child.srcAddr.remove(i.address)
		i.nextAddr = None

class InstTracer:
	def __init__(self, tbl=None, toVisit=[], revFrom=None, fwdFrom=None):
		self.tbl = tbl
		self.toVisit = list(toVisit)
		self.visited = set(toVisit)
		if (revFrom != None):
			self.addRevPaths(revFrom)
		if (fwdFrom != None):
			self.addRevPaths(fwdFrom)
	def iter(self):
		while (len(self.toVisit) > 0):
			nextAddr = self.toVisit.pop()
			if (nextAddr not in self.tbl):
				continue
			next = self.tbl[nextAddr]
			yield next
	def addAddrs(self, addrs):
		for addr in addrs:
			if (addr != None) and (addr not in self.visited):
				self.toVisit.append(addr)
				self.visited.add(addr)
	def addRevPaths(self, inst):
		self.tbl = inst.tbl
		self.addAddrs(inst.srcAddr)
	def addFwdPaths(self, inst):
		self.tbl = inst.tbl
		self.addAddrs((inst.nextAddr, inst.jmpAddr))

class InstTracerWithContext:
	def __init__(self, tbl=None, revFrom=None, fwdFrom=None, ctx=None):
		self.tbl = tbl
		self.toVisit = list()
		self.visited = set()
		if (revFrom != None):
			self.addRevPaths(revFrom, ctx=ctx)
		if (fwdFrom != None):
			self.addRevPaths(fwdFrom, ctx=ctx)
	def iter(self):
		while (len(self.toVisit) > 0):
			nextAddr, ctx = self.toVisit.pop()
			if (nextAddr not in self.tbl):
				continue
			next = self.tbl[nextAddr]
			yield next, ctx
	def addAddrs(self, ctx, addrs):
		for addr in addrs:
			if (addr != None) and (addr not in self.visited):
				self.toVisit.append((addr, ctx))
				self.visited.add(addr)
	def addRevPaths(self, inst, ctx):
		self.tbl = inst.tbl
		self.addAddrs(ctx, inst.srcAddr)
	def addFwdPaths(self, inst, ctx):
		self.tbl = inst.tbl
		self.addAddrs(ctx, (inst.nextAddr, inst.jmpAddr))

class Inst:
	tbl    =None
	address=None
	id     =None
	groups =None
	def __init__(self, tbl, inst=None, prev=None, fromDict=None):
		self.tbl = tbl
		self.address  = inst.address
		self.bytes    = inst.bytes
		self.id       = inst.id
		self.mnemonic = inst.mnemonic
		self.groups   = inst.groups
		self.op_str   = inst.op_str
		self.op_str_list = tuple(x.strip() for x in inst.op_str.split(","))
		self.full_regs_read = _getFullReadRegs(inst)
		self.full_regs_write = _getFullWriteRegs(inst)
		self.read_ops = _getReadWriteOps(inst, cs.CS_AC_READ)
		self.write_ops = _getReadWriteOps(inst, cs.CS_AC_WRITE)
		self.operands = inst.operands
		self.srcAddr = []
		self.nextAddr = None
		self.jmpAddr = None

	def printInst(self, intents = 0):
		print("\t" * intents + "%06x | %10s | %s %s" % (
			self.address,
			" ".join("%02X" % (b,) for b in self.bytes),
			self.mnemonic,
			self.op_str,
			#" ".join(i.full_regs_write),
			#" ".join(i.full_regs_read)
		))
	
	def tracebackToConditionalBranch(self):
		tracer = InstTracer(revFrom=self)
		found = set()
		for i in tracer.iter():
			# Check if it jumps as we want...
			if (i.jmpAddr != None):
				found.add(i)
				continue
			
			# Otherwise recurse further
			tracer.addRevPaths(i)
		
		# Return set of instructions which are conditional jumps prior to this
		return found
	
	def tracebackToRegWrite(self, reg):
		tracer = InstTracer(revFrom=self)
		found = set()
		for i in tracer.iter():
			# Check if it writes to the register...
			if (reg in i.full_regs_write):
				# If so, end search here
				found.add(i)
				continue
			
			# Otherwise recurse further
			tracer.addRevPaths(i)
		
		# Return set of instructions which may have written this register
		return found
	
	def traceMovAliases(self, op):
		aliases = {}
		if (op.type == cs.x86.X86_OP_REG):
			firstReg = reg_name(op.reg)
			aliases[firstReg] = self
		else:
			return [(self.op_str_list[0], self)]
		
		tracer = InstTracerWithContext(revFrom=self, ctx=firstReg)
		found = set()
		for next, currentReg in tracer.iter():
			# Check if it writes to the register...
			if (currentReg in next.full_regs_write):
				if (next.mnemonic.startswith("mov")):
					
					if (next.operands[1].type == cs.x86.X86_OP_REG) or (next.operands[1].type == cs.x86.X86_OP_MEM):
						nextStrOp = next.op_str_list[1]
						oldAliasAddr = None
						if (nextStrOp in aliases):
							oldAliasAddr = aliases[nextStrOp].address
						if (oldAliasAddr == None) or (next.address > oldAliasAddr):
							aliases[nextStrOp] = next
					
					if (next.operands[1].type == cs.x86.X86_OP_REG):
						tracer.addRevPaths(next, next.op_str_list[1])
				continue
			
			# Recurse backward
			tracer.addRevPaths(next, currentReg)
		
		aliases = list(aliases.items())
		aliases = sorted(aliases, key=lambda alias: -alias[1].address)
		return aliases
		
	def traceforwardToUsage(self, value, badPath=None):
		tracer = InstTracer(fwdFrom=self)
		found = []
		for next in tracer.iter():
			
			# Ignore this path...
			if (next == badPath):
				continue
			
			idx = 0
			matchedOp = None
			for op in next.operands:
				if ((op.access & cs.CS_AC_READ) == 0):
					pass
				else:
					opName = next.op_str_list[idx]
					if (opName == value):
						matchedOp = op
						break
					if (op.type == cs.x86.X86_OP_MEM):
						if (((op.mem.segment != 0) and (reg_name(op.mem.segment) == value)) or
							((op.mem.base != 0) and (reg_name(op.mem.base) == value)) or
							((op.mem.index != 0) and (reg_name(op.mem.index) == value))):
							matchedOp = op
							break
				idx += 1
			
			if (matchedOp != None):
				found.append(next)
				continue
			
			# Path overwrote
			if (value in next.full_regs_write) or (value in next.write_ops):
				continue
			
			if (value in next.read_ops):
				found.append(next)
				continue
			
			# Recurse deeper
			tracer.addFwdPaths(next)
		return found

	def resolveCall(self):
		op = self.operands[0]
		targets = set()
		if (op.type == cs.x86.X86_OP_REG):
			reg = reg_name(op.reg)
			srcs = self.tracebackToRegWrite(reg)
			for src in srcs:
				if (src.mnemonic == "mov"):
					targets.add(src.op_str_list[1])
		elif (op.type == cs.x86.X86_OP_MEM):
			targets.add(self.op_str_list[0])
		else:
			targets.add(self.op_str_list[0])
		
		return targets
		

def generateInstructionLookup(filename, startAddr=None):
	# READ DATA
	f = pefile.PE(filename)
	section = None
	for s in f.sections:
		if s.Name.strip(b"\0") == b".text":
			section = s
	if (section == None):
		raise "Couldn't find .text section"
	offset = f.OPTIONAL_HEADER.ImageBase + section.VirtualAddress
	data = section.get_data()

	# Process startAddr
	if (startAddr == None):
		startAddr = offset
	data = data[startAddr-offset:]
	offset = startAddr

	md = cs.Cs(cs.CS_ARCH_X86, cs.CS_MODE_32)
	md.detail = True
	md.skipdata = False
	instructionLookup = {}
	instructionList = []
	pathToLookup = {}
	prevAddr = None
	for i in md.disasm(data, offset):
		# Consider invalid instructions, NOPs, or INT3 instructions to be invalid and don't store or trace
		if (i.id in (0, cs.x86_const.X86_INS_NOP)) or (cs.x86_const.X86_GRP_INT in i.groups):
			prevAddr = None
			continue
		
		# Create instruction object and store
		i = Inst(instructionLookup, i)
		instructionLookup[i.address] = i
		instructionList.append(i)
		
		# Assign nextAddr to the prior instruction
		if (prevAddr in instructionLookup) and (instructionLookup[prevAddr].nextAddr == None):
			instructionLookup[prevAddr].nextAddr = i.address
		
		# Handle Jumps
		if (i.id == cs.x86_const.X86_INS_JMP) and (i.op_str.startswith("0x")):
			# For non-conditional jumps, record as nextAddr
			i.nextAddr = int(i.op_str[2:], 16)
		elif (cs.x86_const.X86_GRP_JUMP in i.groups) and (i.mnemonic != "jo") and (i.op_str.startswith("0x")):
			# For conditional jumps, record as jmpAddr... but ignore overflow check jumps
			i.jmpAddr = int(i.op_str[2:], 16)
		
		# If the instruction is a non-conditional jump, or a return, we cannot
		# simply proceed to the next statement
		if ((i.id == cs.x86_const.X86_INS_JMP) or
			(cs.x86_const.X86_GRP_RET in i.groups) or
			(cs.x86_const.X86_GRP_IRET in i.groups)):
			prevAddr = None
		else:
			prevAddr = i.address
	
	# Resolve source address lists
	for i in instructionList:
		for addr in (i.nextAddr, i.jmpAddr):
			if (addr not in instructionLookup):
				continue
			instructionLookup[addr].srcAddr.append(i.address)
	
	return instructionList

def loadInstructionList():
	import os.path
	
	filename = "smbx.exe.legacy"
	startAddr = 0x8BD770
	
	cacheFilename = "tmp.dat"
	
	if os.path.isfile(cacheFilename): # and (os.path.getmtime(cacheFilename) > os.path.getmtime(__file__)):
		# Cache exists and is new enough
		print("Reading cached 1st pass data...")
		
		import pickle
		with open(cacheFilename, "rb") as f:
			instructionList = pickle.load(f)
	else:
		# Non cached case
		print("Generating 1st pass data...")
		
		instructionList = generateInstructionLookup(filename, startAddr)
		_cleanDeadEndCalls(instructionList)
		
		for i in instructionList:
			i.tbl = None
		import pickle
		with open(cacheFilename, "wb") as f:
			pickle.dump(instructionList, f)
	
	# Reconstruct lookup table
	tbl = {}
	for i in instructionList:
		i.tbl = tbl
		tbl[i.address] = i
	
	return instructionList

if __name__ == "__main__":
	instructionList = loadInstructionList()

	