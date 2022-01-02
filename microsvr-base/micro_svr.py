# Module: wuase_svr.py  /  version: 1.5  /  build: 20211224
# Purpose: Retrieve application and system information
# Harris Georgiou (c) 2021, Email:hgeorgiou@unipi.gr

# use only standard modules for cross-platform portability
import configparser, logging, logging.handlers, argparse
from wsgiref.simple_server import make_server
from urllib.parse import parse_qs
import os, sys, time
import app_info         # local module


class AppServices():
    '''Class for automating command-line options, configuration file and logging'''

    PARAM_CMD = 'command'       # URL key for 'command' parameter
    PARAM_CONTENT = 'content'   # URL key for 'content' parameter

    CMD_HELLO = 'HELLO'         # HELLO: print a hello message (verify connection)
    CMD_ECHO  = 'ECHO'          # ECHO:  print back the 'content' argument
    CMD_VER   = 'VER'           # VER:   return service version
    CMD_QUIT  = 'QUIT'          # QUIT:  shutdown the server


    def __init__(self, app_name='<ApplicationName>', app_version='<0.0.0>', app_copyright='<copyright>', 
                 app_config='<default_cfg_filename>'):
        '''Initialize instance (constructor)'''
        self.app_name = app_name
        self.app_version = app_version
        self.app_copyright = app_copyright
        self.app_config = app_config
        self.ip_allow_list = ''   # to be updated after configuration fetching
        self.svr_port = -1        # to be updated after configuration fetching
        self.log_level = ''       # to be updated after configuration fetching


    def get_sys_info(self):
        '''Return system information'''
        return (app_info.get_sys_info())


    def get_app_info(self):
        '''Return application information'''
        # retrieve application and system info, return as string
        return (app_info.get_app_info( self.app_name, self.app_version, self.app_copyright))


    def init_opt(self):
        '''Command-line options handler (see 'argparse' for details)'''
        self.cmdopt = argparse.ArgumentParser(description=self.app_name+' core functionality.')
        # options '-h' and '--help' are implemented implicitly
        self.cmdopt.add_argument('--version', action='version', version='%(prog)s '+self.app_version)
        self.cmdopt.add_argument('-v', '--verbose', action='store_true', dest='verbose',
                           default=False, help='enable verbose mode, detailed logging is enabled')
        self.cmdopt.add_argument('-q', '--quiet', action='store_true', dest='quiet',
                           default=False, help='enable quiet mode, no output is printed')
        self.cmdopt.add_argument('-c', '--config', action='store', dest='cfgfile',
                          default=self.app_config, help=('configuration file (default=\'%s\')' % self.app_config))
        args = self.cmdopt.parse_args()
        self.opt = vars(args)


    def init_cfg(self, fname=None):
        '''Configuration file handler'''
        if (fname != None):
            cfg_filename = fname     # use the given filename if not null
        else:
            cfg_filename = self.opt['config']    # get from command-line options
        # open configuration file for all application parameters
        self.cfg = configparser.ConfigParser()
        self.cfg.read_file(open(cfg_filename))      # read configuration from file


    def init_log_timeR(self, fname, loglevel, logwhen, loginterval, logcycle):
        '''Logging handler, time-based rotation (see 'logging' for details)'''
        # initialize logger with time-based rotation
        self.log = logging.getLogger(__name__)      # use module name as logger id
        self.log.setLevel(loglevel)                 # set logging level (from string)
        fh = logging.handlers.TimedRotatingFileHandler(fname, when=logwhen, interval=int(loginterval), backupCount=int(logcycle))
        fmt = logging.Formatter('%(asctime)s; %(levelname)s; %(message)s')    # set default logging format
        fh.setLevel(loglevel)
        fh.setFormatter(fmt)
        self.log.addHandler(fh)


    def init_log_sizeR(self, fname, loglevel, loglimit, logcycle):
        '''Logging handler, size-based rotation (see 'logging' for details)'''
        # initialize logger with size-based rotation
        self.log = logging.getLogger(__name__)      # use module name as logger id
        self.log.setLevel(loglevel)                 # set logging level (from string)
        fh = logging.handlers.RotatingFileHandler(fname, maxBytes=int(loglimit), backupCount=int(logcycle))
        #fh = logging.handlers.TimedRotatingFileHandler(fname, when='m', interval=1, backupCount=3)
        fmt = logging.Formatter('%(asctime)s; %(levelname)s; %(message)s')    # set default logging format
        fh.setLevel(loglevel)
        fh.setFormatter(fmt)
        self.log.addHandler(fh)


    def wsgi_app(self, environ, start_response, exc_info=None):
        '''WSGI loop for handling HTTP requests
           Example call:   http://localhost:15387?command=ECHO&content=someone
           '''
        try:
            t0 = time.time_ns()    # start timer (optional)

            # input: get parameters from http query string
            qs = parse_qs(environ['QUERY_STRING'])
            client_cmd = qs.get(self.PARAM_CMD,['None'])[0]
            client_content = qs.get(self.PARAM_CONTENT,['None'])[0]
            client_ip = environ['REMOTE_ADDR']

            # process: based on input data
            client_cmd = client_cmd.upper()    # convert command tag to uppercase (optional)
            if client_ip not in self.ip_allow_list:
                print('Client dropped (connecting from: %s)' % client_ip)
                raise Exception    # drop client with no message in response
            else:    # client IP allowed, handle the request normally
                #time.sleep(1)   # do some real work here (testing)
                if (client_cmd == self.CMD_ECHO):
                    print('-> Request(1): %s=\'%s\' , %s=\'%s\'' % (self.PARAM_CMD, client_cmd, self.PARAM_CONTENT, client_content))
                    response = client_content
                elif (client_cmd == self.CMD_VER):
                    print('-> Request(2): %s=\'%s\' , %s=\'%s\'' % (self.PARAM_CMD, client_cmd, self.PARAM_CONTENT, client_content))
                    response = self.get_app_info()
                elif (client_cmd == self.CMD_HELLO):
                    print('-> Request(3): %s=\'%s\' , %s=\'%s\'' % (self.PARAM_CMD, client_cmd, self.PARAM_CONTENT, client_content))
                    response = 'Hello client connecting from: ' + client_ip + '\n'
                elif (client_cmd == self.CMD_QUIT):
                    print('-> Request(4): %s=\'%s\' , %s=\'%s\'' % (self.PARAM_CMD, client_cmd, self.PARAM_CONTENT, client_content))
                    response = 'Server shutdown\n'
                    self.log.critical('Server shutdown (QUIT command)')    # log only on QUIT request (not Ctrl+C)

                    # immediate shutdown without responding to 'quit' request
                    # shutdown('QUIT command')   # perform all internal cleanup
                    # os._exit(0)                # force exit Python immediately (WSGI loop catches sys.exit())
                else:
                    print('Client dropped (invalid request command)')
                    raise Exception    # drop client with no message in response
            
                # output: produce result (for valid requests)
                status = '200 OK'
                headers = [('Content-Type','text/plain'),('Content-Length',str(len(response)))]        
                start_response(status,headers)
                res = [bytes(response.encode('UTF-8'))]

                # report lap timer in console window and return result (http)
                print('-> Request served in ' + str((time.time_ns()-t0)/1e6) + '  msec')
                if (client_cmd == self.CMD_QUIT):
                    raise SystemExit    # raise 'exit' exception after returning the response
                else:
                    return res            

        except SystemExit:    # normal shutdown after responding to 'quit' request
            shutdown('QUIT command')   # perform all internal cleanup
            os._exit(0)                # force exit Python immediately (WSGI loop catches sys.exit())

        except Exception:
            status = '500 Internal Server Error'
            headers = [('Content-Type','text/plain')]
            start_response(status, headers, sys.exc_info())
            return [b'Server Error\n']


def shutdown( str_reason ):
    '''Cleanup actions before WSGI server shutdown'''
    print('\nShutting down (%s)...' % str_reason)


def main():
    '''Main WSGI server initialization and service'''
    # fetch application tag and version info
    ver_list = app_info.read_version_json('version.json')
    # initialize application server instance
    app = AppServices(app_name=ver_list[0], app_version=ver_list[1]+' (build: '+ver_list[2]+')', 
                      app_copyright='Harris Georgiou (c) 2021, Licence: CC-BY-NC-SA/4.0i',
                      app_config='cfg/settings.cfg')
    # fetch command-line options
    app.init_opt()
    # print full intro if not in 'help' or 'version' options
    print(app.get_app_info(),'\n')     
    print(app.get_sys_info(),'\n')    

    # fetch full configuration
    app.init_cfg(app.opt['cfgfile'])     # static argument (app-specific), everything else is in there
    print('configuration: ', app.opt['cfgfile'])

    # initialize logging
    if (app.opt['verbose'] and not app.opt['quiet']):     # only 'verbose' given
        app.log_level = 'DEBUG'
    elif (not app.opt['verbose'] and app.opt['quiet']):   # only 'quiet' given
        app.log_level = 'CRITICAL'
    else:  # ignore if both 'verbose' and 'quiet' are used
        app.log_level = app.cfg['logging']['level']
    #   size-based rotation:
    app.init_log_sizeR(app.cfg['logging']['filename'], app.log_level, app.cfg['logging']['maxBytes'], app.cfg['logging']['backupCount'])
    #   time-based rotation:
    #app.init_log_timeR(app.cfg['logging']['filename'], app.cfg['logging']['level'], app.cfg['logging']['when'], app.cfg['logging']['interval'], app.cfg['logging']['backupCount'])
    print('logging file:  %s (level=%s)' % (app.cfg['logging']['filename'], app.log_level))
    #print('logging level: ', app.log.getEffectiveLevel())

    #print('\ntype(ip_allow) = ',type(app.cfg['network']['ip_allow']))
    #print('len(ip_allow) = ',len(app.cfg['network']['ip_allow']))
    #print('ip_allow = ',app.cfg['network']['ip_allow'])
    #ip_allow_list = app.cfg['network']['ip_allow'].strip('][').split(',')
    app.ip_allow_list = app.cfg['network']['ip_allow'].replace(' ','').strip('][').split(',')
    #print('\ntype(ip_allow_list) = ',type(ip_allow_list))
    #print('len(ip_allow_list) = ',len(ip_allow_list))
    print('IP allowed list: ',app.ip_allow_list)
    app.svr_port = int(app.cfg['network']['svr_port'])
    print('Server listening at port %d\n' % app.svr_port)
    app.log.debug('Server initialized, listening port %d' % app.svr_port)

    # Main WSGI server loop
    try:
        httpd = make_server('localhost', app.svr_port, app.wsgi_app)
        print('Serving at port %d...' % app.svr_port)
        httpd.serve_forever()
    except KeyboardInterrupt:    # User break (Ctrl+C) in the console
        shutdown('KeyboardInterrupt')


if __name__ == '__main__':    
    main()     # stub for calling the main loop
