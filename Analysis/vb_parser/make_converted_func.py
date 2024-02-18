from smbx_vb_tools import *

funcList, funcArgStrs = getFuncList()
translatedFuncs = {
    "Int": "::floor",                # VB6 Int(x) is floor(x)
    "Rnd": "VB6RNG::generateNumber", # VB6 Rnd() is VB6RNG::generateNumber()
}

class VBTranslator:
    parser : VisualBasic6Parser = None
    callInSegment : [bool]
    def __init__(self , parser : VisualBasic6Parser):
        self.parser = parser
        self.callInSegment = [False]

    def typeToC(self, asTypeClause : VisualBasic6Parser.AsTypeClauseContext) -> str:
        if asTypeClause.fieldLength():
            raise NotImplementedError()
        type_ = asTypeClause.type_().getText()
        if type_ in basicTypes:
            return basicTypes[type_].cName, basicTypes[type_].cInit
        else:
            return f'SMBX13::Types::{type_}_t', '{}'

    def parseArgList(self, argList : VisualBasic6Parser.ArgListContext) -> str:
        argsC = []
        for arg in argList.arg():
            # Get by val or reference
            byVal = arg.BYVAL()
            if byVal != None:
                byVal = True
            else:
                byVal = False
            byRef = not byVal

            if arg.asTypeClause():
                varType, cInit = self.typeToC(arg.asTypeClause())
                if byRef:
                    varType += "&"
            else:
                varType = "void*"
            varName = arg.ambiguousIdentifier().getText()
            if varName == "bool":
                varName += "_"

            argsC.append(f'{varType} {varName}')
        argListC = ", ".join(argsC)
        
        return argListC

    def parseArgDefaults(self, argStr):
        inputStream = antlr4.InputStream(
            "Function Foobar" + argStr + "\nEnd Function"
            )
        lexer = VisualBasic6Lexer(inputStream)
        tokens = antlr4.CommonTokenStream(lexer)
        parser = VisualBasic6Parser(tokens)
        functionStmt = parser.functionStmt()
        argList = functionStmt.argList()
        asTypeClause = functionStmt.asTypeClause()

        defaults = []
        for arg in argList.arg():
            defValStr = None
            argDefault = arg.argDefaultValue()
            if argDefault:
                defValStr = self.translateValueStmt(argDefault.valueStmt())
            defaults.append(defValStr)
        return defaults

    def translateVariableStmt(self, stmt : VisualBasic6Parser.VariableStmtContext):
        isStatic = bool(stmt.STATIC())
        if stmt.WITHEVENTS():
            raise NotImplementedError()
        for var in stmt.variableListStmt().variableSubStmt():
            if var.subscripts():
                raise NotImplementedError()
            if var.typeHint():
                raise NotImplementedError()

            # Get variable name
            name = var.ambiguousIdentifier().getText()

            # Get variable type
            varType, cInit = self.typeToC(var.asTypeClause())

            # Remove prefix unnecessary due to 'using'
            varType = varType.removeprefix('SMBX13::Types::')

            # Static prefix
            staticPrefix = ""
            if isStatic:
                staticPrefix = "static "

            # Return variable decleration string
            yield f'{staticPrefix}{varType} {name} = {cInit};'

    def translateWithStmt(self, stmt : VisualBasic6Parser.WithStmtContext):
        # WITH WS (NEW WS)? implicitCallStmt_InStmt NEWLINE+ (block NEWLINE+)? END_WITH

        val = stmt.implicitCallStmt_InStmt()
        val = self.translateICS_InStmt(val)

        yield '{'
        yield f'    auto& _ = {val};'
        yield from self.translateBlock(stmt.block())
        yield '}'

    def translateOperator(self, op : str, stmt : VisualBasic6Parser.ValueStmtContext) -> str:
        children = stmt.valueStmt()
        return f'({self.translateValueStmt(children[0])} {op} {self.translateValueStmt(children[1])})'

    def recognizeFuncName(self, val) -> str | None:
        funcName = None
        if val in basicTypes:
            funcName = basicTypes[basicTypes].cName
        elif val in translatedFuncs:
            funcName = translatedFuncs[val]
        elif val in funcList:
            funcName = val
        return funcName

    def translateICS_VariableOrProcedureCall(self, varProcCall : VisualBasic6Parser.ICS_S_VariableOrProcedureCallContext) -> str:
        if varProcCall.typeHint():
            raise NotImplementedError()
        if varProcCall.dictionaryCallStmt():
            raise NotImplementedError()
        
        # Get name
        varName = varProcCall.ambiguousIdentifier()
        varName = varName.getText()

        # See if the value is a function name we know about
        funcName = self.recognizeFuncName(varName)

        if funcName is None:
            # Doesn't seem to be a function, treat as variable
            return varName
        else:
            # Seems to be a call?
            self.callInSegment[-1] = True
            return f'{funcName}()'

    def translateICS_ProcedureOrArrayCall(self, procOrArray : VisualBasic6Parser.ICS_S_ProcedureOrArrayCallContext) -> str:
        varName = procOrArray.ambiguousIdentifier()
        if varName is None:
            raise NotImplementedError()

        # Get name
        varName = varName.getText()

        # See if the value is a function name we know about
        funcName = self.recognizeFuncName(varName)

        # Translate args
        argsList = []
        for argsCall in procOrArray.argsCall():
            argsList.extend(self.translateArgsCallToList(argsCall, funcName))

        if funcName is None:
            # Doesn't seem to be a function, treat as array
            # NOTE: VB6 array indicies seem to be reversed
            val = varName
            for argCall in reversed(argsList):
                val += f'[{argCall}]'
            return val
        else:
            # Seems to be a call?
            self.callInSegment[-1] = True
            args = ", ".join(argsList)
            return f'{funcName}({args})'

    def translateICS_MemberCall(self, memberCall : VisualBasic6Parser.ICS_S_MemberCallContext) -> str:
        val = "_"
        if memberCall.iCS_S_VariableOrProcedureCall():
            val = self.translateICS_VariableOrProcedureCall(memberCall.iCS_S_VariableOrProcedureCall())
        elif memberCall.iCS_S_ProcedureOrArrayCall():
            procOrArray = memberCall.iCS_S_ProcedureOrArrayCall()
            val = self.translateICS_ProcedureOrArrayCall(procOrArray)
        else:
            raise NotImplementedError()
        return val

    def translateICS_MembersCall(self, membersCall : VisualBasic6Parser.ICS_S_MembersCallContext) -> str:
        val = "_"
        if membersCall.iCS_S_VariableOrProcedureCall():
            val = self.translateICS_VariableOrProcedureCall(membersCall.iCS_S_VariableOrProcedureCall())
        elif membersCall.iCS_S_ProcedureOrArrayCall():
            procOrArray = membersCall.iCS_S_ProcedureOrArrayCall()
            val = self.translateICS_ProcedureOrArrayCall(procOrArray)
        if membersCall.dictionaryCallStmt():
            raise NotImplementedError()
        for memberCall in membersCall.iCS_S_MemberCall():
            val += f'.{self.translateICS_MemberCall(memberCall)}'
        return val

    def translateICS_InStmt(self, ics : VisualBasic6Parser.ImplicitCallStmt_InStmtContext) -> str:
        if ics.iCS_S_MembersCall():
            # Implement membercall syntax
            return self.translateICS_MembersCall(ics.iCS_S_MembersCall())
        elif ics.iCS_S_VariableOrProcedureCall():
            varProcCall = ics.iCS_S_VariableOrProcedureCall()
            return self.translateICS_VariableOrProcedureCall(varProcCall)
        elif ics.iCS_S_ProcedureOrArrayCall():
            procOrArray = ics.iCS_S_ProcedureOrArrayCall()
            return self.translateICS_ProcedureOrArrayCall(procOrArray)
        else:
            raise NotImplementedError(f'{ics.toStringTree(recog=self.parser)}')    
        #return f'{ics.getText()}'
        #return f'{ics.toStringTree(recog=self.parser)}'

    def translateLiteral(self, lit : VisualBasic6Parser.LiteralContext) -> str:
        if lit.TRUE():
            return 'true'
        elif lit.FALSE():
            return 'false'
        elif lit.STRINGLITERAL():
            return 'L' + lit.getText()
        elif lit.doubleLiteral():
            return lit.getText()
        elif lit.integerLiteral():
            return lit.getText()
        raise NotImplementedError()

    def translateAndOrOperator(self, op : str, func : str, stmt : VisualBasic6Parser.ValueStmtContext) -> str:
        children = stmt.valueStmt()
        # Translate left side arg
        left = self.translateValueStmt(children[0])

        # Translate right side arg, AND make note of if it contains a function call...
        # If it has a function call, we need to use VBAnd/VBOr instead of &&/|| so that potential
        # side effects may take place.
        self.callInSegment.append(False)
        right = self.translateValueStmt(children[1])
        haveCall = self.callInSegment.pop()
        if haveCall:
            # Use a function call for and/or if we have a function happening inside
            self.callInSegment[-1] = True
            return f'{func}({left}, {right})'
        else:
            # If no risk of side effects, just uses && or ||
            return f'({left} {op} {right})'

    def translateValueStmt(self, stmt : VisualBasic6Parser.ValueStmtContext) -> str:
        if isinstance(stmt, VisualBasic6Parser.VsAndContext):
            return self.translateAndOrOperator("&&", "VBAnd", stmt)
        elif isinstance(stmt, VisualBasic6Parser.VsOrContext):
            return self.translateAndOrOperator("||", "VBOr", stmt)
        elif isinstance(stmt, VisualBasic6Parser.VsNotContext):
            return '!' + self.translateValueStmt(stmt.valueStmt())
        elif isinstance(stmt, VisualBasic6Parser.VsCompContext):
            op = None
            if stmt.EQ():
                op = '=='
            elif stmt.NEQ():
                op = '!='
            elif stmt.LT():
                op = '<'
            elif stmt.GT():
                op = '>'
            elif stmt.LEQ():
                op = '<='
            elif stmt.GEQ():
                op = '>='
            else:
                raise NotImplementedError(f'Haven\'t implemented operator for "{stmt.getText()}"')
            return self.translateOperator(op, stmt)
        elif isinstance(stmt, VisualBasic6Parser.VsAddSubContext):
            op = None
            if stmt.PLUS():
                op = '+'
            elif stmt.MINUS():
                op = '-'
            else:
                raise NotImplementedError(f'Haven\'t implemented operator for "{stmt.getText()}"')
            return self.translateOperator(op, stmt)
        elif isinstance(stmt, VisualBasic6Parser.VsMultDivContext):
            op = None
            if stmt.MULT():
                op = '*'
            elif stmt.DIV():
                op = '/'
            else:
                raise NotImplementedError(f'Haven\'t implemented operator for "{stmt.getText()}"')
            return self.translateOperator(op, stmt)
        elif isinstance(stmt, VisualBasic6Parser.VsPlusMinusContext):
            op = None
            if stmt.PLUS():
                op = '+'
            elif stmt.MINUS():
                op = '-'
            else:
                raise NotImplementedError()
            return op + self.translateValueStmt(stmt.valueStmt())
        elif isinstance(stmt, VisualBasic6Parser.VsAmpContext):
            # String concat. Let's assume + will do the job for now
            return self.translateOperator('+', stmt)
        elif isinstance(stmt, VisualBasic6Parser.VsICSContext):
            return self.translateICS_InStmt(stmt.implicitCallStmt_InStmt())
        elif isinstance(stmt, VisualBasic6Parser.VsLiteralContext):
            return self.translateLiteral(stmt.literal())
        elif isinstance(stmt, VisualBasic6Parser.VsStructContext):
            children = stmt.valueStmt()
            if len(children) > 1:
                raise NotImplementedError()
            return self.translateValueStmt(children[0])
        else:
            raise NotImplementedError(f'Haven\' implemented {type(stmt)}\n\n{stmt.getText()}\n\n{stmt.toStringTree(recog=self.parser)}')
        return f'{stmt.getText()}'
        #return f'{stmt.toStringTree(recog=self.parser)}'

    def translateIfConditionStmt(self, stmt : VisualBasic6Parser.IfConditionStmtContext) -> str:
        condition = self.translateValueStmt(stmt.valueStmt())
        if not condition.startswith('('):
            condition = f'({condition})'
        return condition

    def translateIfBlockStmt(self, stmt : VisualBasic6Parser.IfBlockStmtContext | VisualBasic6Parser.IfElseIfBlockStmtContext | VisualBasic6Parser.IfElseBlockStmtContext):
        if stmt is None:
            yield from []
            return
        
        # Comment on this line
        initLineEnd = None
        if isinstance(stmt, VisualBasic6Parser.IfBlockStmtContext):
            initLineEnd = stmt.THEN()
        elif isinstance(stmt, VisualBasic6Parser.IfElseIfBlockStmtContext):
            initLineEnd = stmt.THEN()
        elif isinstance(stmt, VisualBasic6Parser.IfElseBlockStmtContext):
            initLineEnd = stmt.ELSE()
        yield from self.translateCommentTokensRight(initLineEnd)
        
        if isinstance(stmt, VisualBasic6Parser.IfBlockStmtContext):
            condition = self.translateIfConditionStmt(stmt.ifConditionStmt())
            yield f'if {condition} {{'
        elif isinstance(stmt, VisualBasic6Parser.IfElseIfBlockStmtContext):
            condition = self.translateIfConditionStmt(stmt.ifConditionStmt())
            yield f'else if {condition} {{'
        elif isinstance(stmt, VisualBasic6Parser.IfElseBlockStmtContext):
            yield 'else {'
        yield from self.translateBlock(stmt.block())
        yield '}'

    def translateIfThenElseStmt(self, stmt : VisualBasic6Parser.IfThenElseStmtContext):
        if isinstance(stmt, VisualBasic6Parser.InlineIfThenElseContext):
            # Inline if
            condition = self.translateIfConditionStmt(stmt.ifConditionStmt())
            
            ret = f'if {condition} '
            blocks = stmt.blockStmt()
            ret += '{ '
            for line in self.translateBlockStmt(blocks[0], ignoreComment=True):
                ret += line
            ret += ' }'
            if len(blocks) > 1:
                ret += ' else { '
                for line in self.translateBlockStmt(blocks[1], ignoreComment=True):
                    ret += line
                ret += ' }'
            yield ret
        else:
            # Block if
            yield from self.translateIfBlockStmt(stmt.ifBlockStmt())
            for elseif in stmt.ifElseIfBlockStmt():
                yield from self.translateIfBlockStmt(elseif)
            yield from self.translateIfBlockStmt(stmt.ifElseBlockStmt())
        
    def translateLetStmt(self, stmt : VisualBasic6Parser.LetStmtContext):
        leftSide = self.translateICS_InStmt(stmt.implicitCallStmt_InStmt())

        op = None
        if stmt.EQ():
            op = '='
        elif stmt.PLUS_EQ():
            op = '+='
        elif stmt.MINUS_EQ():
            op = '-='
        else:
            raise NotImplementedError()

        rightSide = self.translateValueStmt(stmt.valueStmt())

        yield f'{leftSide} {op} {rightSide};'

    def translateArgCall(self, var : VisualBasic6Parser.ArgCallContext) -> str:
        if var.BYVAL():
            raise NotImplementedError()
        if var.BYREF():
            raise NotImplementedError()
        if var.PARAMARRAY():
            raise NotImplementedError()
        return self.translateValueStmt(var.valueStmt())

    def translateArgsCallToList(self, var : VisualBasic6Parser.ArgsCallContext, funcName : str) -> [str]:
        args = []
        needDefaults = False
        sepCount = 1

        # Get correctly aligned list of args
        for child in var.children:
            if isinstance(child, VisualBasic6Parser.ArgCallContext):
                if sepCount > 1:
                    # If we had too many seperators, append blanks
                    needDefaults = True
                    for i in range(sepCount-1):
                        args.append(None)
                args.append(child)
                sepCount = 0
            elif isinstance(child, antlr4.tree.Tree.TerminalNodeImpl):
                if child.symbol.type in (VisualBasic6Lexer.COMMA, VisualBasic6Lexer.SEMICOLON):
                    # Count seperators
                    sepCount += 1
        
        # Get default args if necessary
        defaultArgs = None
        if needDefaults:
            defaultArgs = self.parseArgDefaults(funcArgStrs[funcName])

        # Parse pargs
        for i in range(len(args)):
            if args[i] is not None:
                # Parse arg
                args[i] = self.translateArgCall(args[i])
            else:
                # Take default arg, because we specify a non-default arg afterwards
                args[i] = defaultArgs[i]
        return args

    def translateArgsCall(self, var : VisualBasic6Parser.ArgsCallContext, funcName : str) -> str:
        return ", ".join(self.translateArgsCallToList(var, funcName))

    def translateICS_InBlock(self, stmt : VisualBasic6Parser.ImplicitCallStmt_InBlockContext):
        if stmt.iCS_B_ProcedureCall():
            procCall = stmt.iCS_B_ProcedureCall()
            funcName = procCall.certainIdentifier().getText()
            argsList = self.translateArgsCall(procCall.argsCall(), funcName)
            self.callInSegment[-1] = True
            yield f'{funcName}({argsList});'
        elif stmt.iCS_B_MemberProcedureCall():
            # implicitCallStmt_InStmt? DOT ambiguousIdentifier typeHint? (WS argsCall)? dictionaryCallStmt?
            procCall = stmt.iCS_B_MemberProcedureCall()
            leftSide = self.translateICS_InStmt(procCall.implicitCallStmt_InStmt())
            funcName = procCall.ambiguousIdentifier().getText()
            if procCall.typeHint():
                raise NotImplementedError()
            argsList = self.translateArgsCall(procCall.argsCall(), funcName)
            if procCall.dictionaryCallStmt():
                raise NotImplementedError()
            if leftSide == "Netplay":
                yield f'// {stmt.getText()}'
            else:
                self.callInSegment[-1] = True
                yield f'{leftSide}.{funcName}({argsList});'
        else:
            raise NotImplementedError()

    def translateForNextStmt(self, stmt : VisualBasic6Parser.ForNextStmtContext):
        # : FOR WS iCS_S_VariableOrProcedureCall typeHint? (WS asTypeClause)? WS? EQ WS? valueStmt WS TO WS valueStmt (
        #     WS STEP WS valueStmt
        # )? NEWLINE+ (block NEWLINE+)? NEXT (WS ambiguousIdentifier typeHint?)?

        # Get variable name
        varName = self.translateICS_VariableOrProcedureCall(stmt.iCS_S_VariableOrProcedureCall())

        # Get type
        if len(stmt.typeHint()) > 0:
            raise NotImplementedError()
        varType = ""
        if stmt.asTypeClause():
            varType, cInit = self.typeToC(stmt.asTypeClause.type_().getText()) + " "

        # Get from/to values, and optional step value
        values = stmt.valueStmt()
        fromVal = self.translateValueStmt(values[0])
        toVal = self.translateValueStmt(values[1])
        stepVal = None
        if len(values) >= 3:
            stepVal = self.translateValueStmt(values[2])
        
        # Verify ending identifier
        if stmt.ambiguousIdentifier():
            if stmt.ambiguousIdentifier().getText() != varName:
                raise Exception("Wrong var name?")

        # Get increment op
        incOp = '++'
        if stepVal is not None:
            incOp = f' += {stepVal}'

        yield f'for ({varType}{varName} = {fromVal}; {varName} <= {toVal}; {varName}{incOp}) {{'
        yield from self.translateBlock(stmt.block())
        yield '}'

    def translateCommentTokens(self, tokens):
        if tokens is not None:
            for tk in tokens:
                cmt = tk.text.strip().lstrip("'")
                yield f'// {cmt}'

    def translateCommentTokensRight(self, ctx):
        if ctx is not None:
            if isinstance(ctx, antlr4.tree.Tree.TerminalNodeImpl):
                tkn = ctx.symbol
            else:
                tkn = ctx.stop
            tokenStream = self.parser.getTokenStream()
            yield from self.translateCommentTokens(tokenStream.getHiddenTokensToRight(tkn.tokenIndex))

    def translateCommentTokensLeft(self, ctx):
        if ctx is not None:
            if isinstance(ctx, antlr4.tree.Tree.TerminalNodeImpl):
                tkn = ctx.symbol
            else:
                tkn = ctx.start
            tokenStream = self.parser.getTokenStream()
            yield from self.translateCommentTokens(tokenStream.getHiddenTokensToLeft(tkn.tokenIndex))

    def translateBlockStmt(self, blockStmt : VisualBasic6Parser.BlockStmtContext, ignoreComment:bool = False):

        # Comments if single line statement
        if not ignoreComment:
            txt = blockStmt.getText().strip("\n")
            if "\n" not in txt:
                yield from self.translateCommentTokensRight(blockStmt)

        # Translate a block statement
        stmt = blockStmt.children[0]
        if isinstance(stmt, VisualBasic6Parser.VariableStmtContext):
            yield from self.translateVariableStmt(stmt)
        elif isinstance(stmt, VisualBasic6Parser.WithStmtContext):
            yield from self.translateWithStmt(stmt)
        elif isinstance(stmt, VisualBasic6Parser.IfThenElseStmtContext):
            yield from self.translateIfThenElseStmt(stmt)
        elif isinstance(stmt, VisualBasic6Parser.LetStmtContext):
            yield from self.translateLetStmt(stmt)
        elif isinstance(stmt, VisualBasic6Parser.ImplicitCallStmt_InBlockContext):
            yield from self.translateICS_InBlock(stmt)
        elif isinstance(stmt, VisualBasic6Parser.ForNextStmtContext):
            yield from self.translateForNextStmt(stmt)
        elif isinstance(stmt, VisualBasic6Parser.ExitStmtContext):
            yield 'return;' # Note: Not yet supporting returning values
        else:
            raise NotImplementedError(stmt.toStringTree(recog=self.parser))

    def translateBlock(self, block : VisualBasic6Parser.BlockContext):
        # Iterate over block statements
        if block is not None:
            for blockStmt in block.blockStmt():
                for line in self.translateBlockStmt(blockStmt):
                    yield f'    {line}'
        yield from []

    def translateSubStmt(self, subStmt : VisualBasic6Parser.SubStmtContext | VisualBasic6Parser.FunctionStmtContext):
        # Get function name
        funcName = subStmt.ambiguousIdentifier().getText()
        
        # Get return type
        retType = "void"
        if isinstance(subStmt, VisualBasic6Parser.FunctionStmtContext):
            asTypeClause = subStmt.asTypeClause()
            if asTypeClause:
                retType, cInit = self.typeToC(asTypeClause.type_().getText())

        # Get argument list string
        argList = self.parseArgList(subStmt.argList())
        
        # Put together function decleration
        funcDecl = f'{retType} __stdcall SMBX13::Ports::{funcName}({argList})'

        # First line
        yield f'{funcDecl} {{'

        # Using
        yield '    using namespace SMBX13::Types;'
        yield '    using namespace SMBX13::Vars;'
        yield '    using namespace SMBX13::Functions;'
        yield '    #pragma warning( push )'
        yield '    #pragma warning( disable: 4244 ) // Disable loss of precision warning'
        # yield '    #pragma warning( disable: 4101 ) // Disable unused variable warning'

        # Yield lines from the block
        yield from self.translateBlock(subStmt.block())

        yield '    #pragma warning( pop )'

        # End of function
        yield f'}}'

        #DEBUG: yield(subStmt.toStringTree(recog=self.parser))

def main(argv):
    # Test file if unspecified
    if len(argv) < 2:
        argv = [argv[0], "test_func.bas"]

    # Read from stdin or specified file
    parser, name = getParserForArgv(argv)

    # Run parsing
    module = parser.module()

    # Get first body element, assume it's a subStmt
    subStmt = module.moduleBody().moduleBodyElement()[0].subStmt()

    # Make translator object
    translator = VBTranslator(parser)

    print('#include "../Types.h"')
    print('#include "../Variables.h"')
    print('#include "../Functions.h"')
    print('#include "../Ports.h"')
    print('#include "../../Misc/VB6RNG.h"')
    print('#include "../../Misc/VB6Logic.h"')
    print()
    for line in translator.translateSubStmt(subStmt):
        print(line)
    

if __name__ == '__main__':
    main(sys.argv)