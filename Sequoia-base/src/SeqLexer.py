# Module: SeqLexer.py / version: 1.5 / build: 20211223
# Purpose: Lexical analyzer based on regular expressions
# Harris Georgiou (c) 2021, Email:hgeorgiou@unipi.gr

# necessary imports
from sly import Lexer				# lexical analyzer
import re							# regular expressions
from SeqOpcode import SeqOpcode		# local module


class SeqLexer(Lexer):
    '''Sequoia class: Lexical analyzer-tokenizer'''
    
	# define constants for tokens and whitespaces
    tokens = { IDENT, FLOAT, INT, ASSIGN, LPAREN, RPAREN, QUIT, RESET, CLEAR, LIST, CONNECT, USE, RETURN }#, SEMICOL }
    ignore = ' \t'     # skip by default

    # Tokens
    IDENT = r'[a-zA-Z_][.a-zA-Z0-9_]*'      # valid identifiers and keywords
    #NUMBER = r'\d+'						# (depricated)
    #FLOAT = r'/^(?!0\d)\d*(\.\d+)?$/mg'	# (depricated)
    FLOAT = r'[-]*\d+[.]\d+'                # signed decimal numbers (preceeding)
    INT = r'[-]*\d+'                        # signed integers (after floats)

    # Special symbols
    ASSIGN  = r'='
    LPAREN  = r'\('
    RPAREN  = r'\)'
    #SEMICOL = r';'         # (depricated) default statement end, now defined in 'literals' below

    # literals (chars)
    literals = { SeqOpcode.stmtsepar }		# default statement end

    # Ignored patterns
    ignore_newline = r'\n+'        	# skip all newlines
    ignore_comment = r'\#.*'       	# skip single-line comments

    # Special cases
    IDENT['QUIT']    = QUIT         # default exit (interactive mode)
    IDENT['RESET']   = RESET        # default reset (interactive mode)
    IDENT['CLEAR']   = CLEAR        # default clear (interactive mode)
    IDENT['LIST']    = LIST         # default clear (interactive mode)
    IDENT['CONNECT'] = CONNECT      # connect to DB file/url
    IDENT['USE']     = USE          # import from local file (.csv)
    IDENT['RETURN']  = RETURN       # pipeline output (identifier)


    def __init__(self):
        '''Default constructor-initializer'''
        self.reset()    # reset all internal state/counters
        #print('interactive: ',self.interactive,' , verbose: ',self.verbose)


    def reset(self, interactive=True, verbose=True):
        '''Reset lexer status'''
        self.hasErrors=False
        self.verbose=verbose
        self.interactive=interactive


    def ignore_newline(self, t):
        '''Extra action for newlines (keep track)'''
        'update line count, ignore as token'
        self.lineno += t.value.count('\n')   


    def error(self, t):
        '''Default error handler for illegal tokens (characters)'''
        print('Error (line %d): Illegal character %r' % (self.lineno, t.value[0]))
        self.index += 1     	# skip current position, continue with input
        self.hasErrors=True		# flag to invalidate further processing by parser
