# Module: SeqOpcode.py / version: 1.5 / build: 20211223
# Purpose: Definition of low-level opcodes for parser
# Harris Georgiou (c) 2021, Email:hgeorgiou@unipi.gr

# (no imports necessary)


class SeqOpcode():
    '''Sequoia class: Definition of low-level opcodes for compiler-oriented output'''

    version = '0.0.1'       # core engine/opcodes version

    opcodes = {
        'NOTHING' : 'NOP',		# no-operation
        'USE'     : 'USE',		# use data source (file)
        'CONNECT' : 'CONN',		# connect to data source (database)
        'ASSIGN'  : 'SET',		# variable assignment
        'RETURN'  : 'RET'		# return-exit
    }

    stmtsepar = ';'				# statement separator (for same line)


    def __init__(self):
        '''Default constructor-initializer'''
        pass		# nothing special to do
