# Module: SeqLexer.py / version: 1.5 / build: 20211223
# Purpose: Lexical analyzer based on regular expressions
# Harris Georgiou (c) 2021, Email:hgeorgiou@unipi.gr

# necessary imports
from sly import Lexer#, Parser       # lexical analyzer and syntax parser
from sly import Parser
from datetime import datetime       # datetime services
import re                           # regular expressions
from SeqOpcode import SeqOpcode     # local module
from SeqLexer import SeqLexer       # local module


class SeqParser(Parser):
    '''Sequoia class: Syntax analyzer-parser'''

    debugfile = 'seqparser.out'     # export rules/states in debugging mode
    tokens = SeqLexer.tokens        # get valid tokens from lexer

    # precedence = (
    #     ('left', PLUS, MINUS),
    #     ('left', TIMES, DIVIDE),
    #     ('right', UMINUS),
    #     )

    # define keywords in identifiers, treat as case-insensitive in grammar
    keywords = ('QUIT', 'RESET', 'CLEAR', 'LIST', 'CONNECT', 'USE', 'RETURN')


    def __init__(self):
        '''Default constructor-initializer'''
        self.reset()           # reset all internal state/counters
        #print('interactive: ',self.interactive,' , verbose: ',self.verbose)
        self.oplist_clear()    # clear current opcode listing


    def reset(self, interactive=True, verbose=True):
        '''Reset parser status, including identifiers and opcodes'''
        #self.names = { '$ans': None }  # dictionary for identifiers ($ans = reserved for latest result)
        self.clear_vars()               # clear all variables (workspace)
        self.oplist_clear()             # clear all opcodes (output)

        self.stmtno=0                   # reset statements counter

        self.hasErrors=False            # flag for parsing errors
        self.hasInput=False             # flag for defined input (yes/no)
        self.hasOutput=False            # flag for defined output (yes/no)
        self.verbose=verbose            # flag for verbose messages
        self.interactive=interactive    # flag for interactive/compile mode
        if (self.interactive):          # set appropriate 'mode' string
            self.mode='interactive'
        else:
            self.mode='compiler'

    
    # --- Section: Environment handling methods ---

    def clear_vars(self):
        '''Clear all identifiers only (opcodes unchanged)'''
        self.names = { '$ans': None }   # dictionary for identifiers ($ans = reserved for latest result)


    def list_vars(self):
        '''display currently declared identifiers (vars)'''
        for (k, v) in self.names.items():
            # keep loop regardless of verbose, for any other processing here
            if (self.verbose):
                print('\'%s\'\t:  %s' % (k, type(v)))       # display identifier name and type (not value)


    def oplist_clear(self):
        '''Clear all opcode data (reset)'''
        self.oplist = {             # main opcode listing, updated during parsing
            'meta'   : {            # oplist: metadata section (initialize)
                'name' : None,
                'version' : SeqOpcode.version,
                'mode'    : None,
                'author'  : None,
                'copyright' : None,
                'timestamp' : None,

            },
            'input'  : {            # oplist: input section (initialize)
                #'dbname'  : None,
                #'dbtype'  : None,

            },
            'steps'  : {            # oplist: pipeline section (initialize)

            },
            'output' : {            # oplist: output section (initialize)
                #'return'  : None,

            },
        }


    def oplist_meta(self, name, author, copyright):
        '''Populate the metadata section with actual data'''
        self.oplist['meta']['name'] = name
        self.oplist['meta']['author'] = author
        self.oplist['meta']['copyright'] = copyright
        self.oplist['meta']['version'] = SeqOpcode.version
        self.oplist['meta']['mode'] = self.mode
        self.oplist['meta']['timestamp'] = str(datetime.now())


    def oplist_validate(self):
        '''Validate pipeline definition'''
        # check if proper input has been defined
        if (not (self.hasInput and ('dbname' in self.oplist['input']))):
            print('Error: No \'USE\' or \'CONNECT\' input defined')
            self.hasErrors=True

        # check if proper output has been defined
        if (not (self.hasOutput and ('return' in self.oplist['output']))):
            print('Error: No \'RETURN\' output defined')
            self.hasErrors=True

        return (self.hasErrors)
        

    def cap_keywords(self, inptext):
        '''Capitalize keywords for case-insensitive matching in rules'''
        for kw in self.keywords:                    # convert all grammar keywords to upper case
            re_exp = re.compile(kw, re.IGNORECASE)  # find all keywords in current input
            inptext = re_exp.sub(kw, inptext)       # replace matches with uppercase keywords
            
        return(inptext)


    def done(self, errcode=0):
        '''Gracefully finish parsing process and exit'''
        if (self.verbose):
            print('Finished parsing %d statements:' % len(self.oplist['steps']))
            print('Identifiers: ',self.names)
            print('oplist:\n',self.oplist)
            if (self.hasErrors):
                print('Exiting with errors (code=%d)' % errcode)
            else:
                print('No errors found in input')

        if (self.hasErrors and errcode != 0):    # should also catch Lexer errors
            exit(errcode)
        elif (self.hasErrors):     # exit with errors, no code given
            exit(1)
        else:                      # no errors (errcode ignored)
            exit(0)        


    # --- Section: Grammar defition for the syntax parser ---

    @_('QUIT')
    def statement(self, p):
        '''Rule: 'QUIT' '''
        #self.stmtno += 1       # do not count 'quit' as statement (interactive mode)
        self.oplist_validate()
        self.done()


    @_('RESET')
    def statement(self, p):
        '''Rule: 'RESET' '''
        #self.stmtno += 1       # do not count 'quit' as statement (interactive mode)
        self.reset()
        if (self.verbose):
            print('Parser reset: OK')


    @_('CLEAR')
    def statement(self, p):
        '''Rule: 'CLEAR' '''
        #self.stmtno += 1       # do not count 'quit' as statement (interactive mode)
        self.clear_vars()
        if (self.verbose):
            print('Parser clear vars: OK')


    @_('LIST')
    def statement(self, p):
	    #'''Rule: 'LIST' '''        #self.stmtno += 1       # do not count 'quit' as statement (interactive mode)
        self.list_vars()
        if (self.verbose):
            print('Parser list vars: OK')


    @_('USE IDENT')
    def statement(self, p):
        '''Rule: 'USE <filename>' '''
        # if (p.IDENT not in self.names):
        #     print(f'Error (line %d): \'USE\' with undefined identifier {p.IDENT!r}' % p.lineno)
        # elif ('$dbname' in self.names):
        # elif (self.hasInput):
        
        if (self.hasInput):
            print(f'Error (line %d): Input already defined as {p.IDENT!r}' % p.lineno)
        else:
            self.names['$dbname'] = p.IDENT                                 # update identifiers dictionary (special $)
            self.stmtno += 1      # count as valid statement (in oplist)
            self.oplist['input']['dbname'] = self.names['$dbname']          # update oplist (input filename)
            self.oplist['input']['dbtype'] = 'csv/text'                     # update oplist (input type)
            #self.oplist['steps'][self.stmtno] = SeqOpcode.opcodes['USE']   # insert new pipeline step (opcode)
            #self.oplist['steps'][self.stmtno] = (SeqOpcode.opcodes['USE'], self.names['$dbname'])  # opcode content
            self.hasInput=True
            if (self.verbose):
                print('%s: \'%s\'' % (SeqOpcode.opcodes['USE'], self.names['$dbname']))
                

    @_('CONNECT IDENT')
    def statement(self, p):
        '''Rule: 'CONNECT <filename>' '''
        # if (p.IDENT not in self.names):
        #     print(f'Error (line %d): \'CONNECT\' with undefined identifier {p.IDENT!r}' % p.lineno)
        # elif ('$dbname' in self.names):
        # elif (self.hasInput):
        
        if (self.hasInput):
            print(f'Error (line %d): Input already defined as {p.IDENT!r}' % p.lineno)
        else:
            self.names['$dbname'] = p.IDENT                                 # update identifiers dictionary (special $)            
            self.stmtno += 1      # count as valid statement (in oplist)
            self.oplist['input']['dbname'] = self.names['$dbname']          # update oplist (input filename)
            self.oplist['input']['dbtype'] = 'db/binary'                    # update oplist (input type)
            #self.oplist['steps'][self.stmtno] = SeqOpcode.opcodes['CONNECT']    # insert new pipeline step (opcode)
            #self.oplist['steps'][self.stmtno] = (SeqOpcode.opcodes['CONNECT'], self.names['$dbname'])  # opcode content
            self.hasInput=True
            if (self.verbose):
                print('%s: \'%s\'' % (SeqOpcode.opcodes['CONNECT'], self.names['$dbname']))
                

    @_('RETURN IDENT')
    def statement(self, p):
        '''Rule: 'RETURN <identifier>' '''
        if (p.IDENT not in self.names):
            print(f'Error (line %d): \'RETURN\' with undefined identifier {p.IDENT!r}' % p.lineno)
        #elif ('$return' in self.names):
        elif (self.hasOutput):
            print(f'Error (line %d): \'RETURN\' already defined as {p.IDENT!r}' % p.lineno)
        else:
            self.names['$return'] = p.IDENT                         # set output identifiers from dictionary (special $)                    
            self.stmtno += 1      # count as valid statement (in oplist)
            self.oplist['output']['return'] = self.names['$return']          # update oplist (input)
            #self.oplist['steps'][self.stmtno] = SeqOpcode.opcodes['RETURN']    # insert new pipeline step (opcode)
            #self.oplist['steps'][self.stmtno] = (SeqOpcode.opcodes['RETURN'], self.names['$return'])  # opcode content
            self.hasOutput=True
            if (self.verbose):
                print('return: \'%s\'' % self.names['$return'])
                

    # @_('USE error')
    # def statement(self, p):
    #     '''Default syntax error handler for 'USE' rule'''
    #     line   = p.lineno       # line number of the recognized token
    #     index  = p.index        # Index of the recognized token in input text
    #     print('error in USE: \'%s\'' % p.error)
    #     print('(lineno %d, index %d)' % (line, index))
    

    @_('IDENT ASSIGN expr')
    def statement(self, p):
        '''Rule: '<identifier> = <expression>|<identifier>' '''
        self.names[p.IDENT] = p.expr        # update identifiers dictionary (new) and set value
        #self.names['$ans']=p.expr          # update latest-result identifier (auto)
        self.stmtno += 1                    # count as valid statement (in oplist)
        self.oplist['steps'][self.stmtno] = SeqOpcode.opcodes['ASSIGN']    # insert new pipeline step (opcode)
        self.oplist['steps'][self.stmtno] = (SeqOpcode.opcodes['ASSIGN'], p.IDENT, self.names[p.IDENT])  # opcode content
        # Note: on invalid <expr> the p.expr value becomes 'None', i.e. means 'unset <identifier>'
        if (self.verbose):
            print('set: \'%s\' = %s' % (p.IDENT,self.names[p.IDENT]))
            #print('(auto): $ans = %s' % self.names['$ans'])
            
            
    @_('expr')
    def statement(self, p):
        '''Rule: '<expression>|<identifier>' (print current value)'''
        if (p.expr != None):
            print(p.expr)       # useful only in interactive mode (no oplist update)
        #self.names['$ans']=p.expr
        #if (self.verbose):
        #    print('(auto): $ans = %s' % self.names['$ans'])


    @_('LPAREN expr RPAREN')    
    def expr(self, p):
        '''Rule: '( <expression>|<identifier> )' '''
        return (p.expr)         # useful only in interactive mode (no oplist update)


    @_('FLOAT')
    def expr(self, p):
        '''Rule: '<expression> as float' '''
        return float(p.FLOAT)    # already checked by the lexer, convert to value
        

    @_('INT')
    def expr(self, p):
        '''Rule: '<expression> as integer' '''
        return int(p.INT)        # already checked by the lexer, convert to value


    # @_('NUMBER')
    # def expr(self, p):
    #     '''Rule: '<expression> as number' (depricated)'''
    #     return int(p.NUMBER)
    

    @_('IDENT')
    def expr(self, p):
        '''Rule: <expression>|<identifier>'''
        #try:
        if (p.IDENT in self.names.keys()):
            self.stmtno += 1                    # useful only in interactive mode (no oplist update) 
            return (self.names[p.IDENT])        # valid identifier, return value
        #except LookupError:
        else:
            print(f'Error (line %d): Undefined identifier {p.IDENT!r}' % p.lineno)
            self.hasErrors=True
            self.stmtno += 1
            #return 0
            

    def error(self, p):
        '''Default syntax error handler for as catch-all global'''
        if (p==None):       # default error for incomplete rule matching
            print('Error: Invalid or incomplete statement %d' % self.stmtno)
        elif ((p.type=='INT') or (p.type=='FLOAT')):
            print('Error (line %d): Invalid number expression' % p.lineno)
        elif (p.type=='IDENT'):     # Note: keywords already catched by lexer/tokenizer
            print('Error (line=%d): Invalid identifier \'%s\'' % (p.lineno, p.value))
        else:               # default error for any other case
            #print('Error: ',p)     # should be catched earlier
            print('Error (line=%d): Invalid use of \'%s\'' % (p.lineno, p.value))

        self.hasErrors=True
        self.stmtno += 1            # count errors as statements


# --- Section: Main loop (entry point) for parser ---

if __name__ == '__main__':
    '''MAIN: stand-alone mode, used as an entry point (skeleton example)'''
    lexer = SeqLexer()          # initialize the lexer
    parser = SeqParser()        # initialize the parser

    #print('same-line statement separator: ',SeqOpcode.stmtsepar)
    parser.interactive=True     # enable by default (debug mode)
    parser.oplist_meta('SeqParser','Harris Georgiou','CC-BY-NC-SA (c) 2021')
    
    while (True):       # main loop, iterate forever (until end-of-text)
        try:
            text = input('seqp > ')                         # default prompt in interactive mode
            text = re.sub(SeqOpcode.stmtsepar,'\n',text)    # replace ';' with newline characters
            #print('input: ',text)                          # echo user's input (debug mode)
            for stmt in text.splitlines(True):              # split lines, input separately to parser
                #print('\tstmt: ',stmt)                     # show individual statements (debug mode)
                parser.parse( lexer.tokenize( parser.cap_keywords(stmt) ) )    # parse line, update internally
        except EOFError:
            break     # Ctrl+Z exits (end-of-text)

        # simpler input-to-parser handler for command-line parsing mode
        #if text:
        #   parser.parse( lexer.tokenize( parser.cap_keywords(text) ) )
