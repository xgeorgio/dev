Title:<br/>
<b>microsvr-base: WSGI-based micro-server (REST API) skeleton using Python</b>

Description:<br/>
This is a small skeleton REST API micro-server, using GET query strings for command-content pairs as a request. 
* Available 'command' parameter (case-insensitive): 'ECHO', 'VER', 'HELLO", 'QUIT' (server shutdown). 
* Available 'content' parameter is used by 'HELLO' and 'ECHO' commands.
* All configuration is handled via external files (see 'cfg' folder).
* There is also automatic logging (see 'log' folder) and command-line options parsing.
* Clients are served only if they are in the allowed IP list, defined in the configuration file.
* At the console, the server can be shut down by pressing Ctrl+C (break).
<p>There is also a 'sql' folder with sample SQL scripts for implementing statistics retrieval from a database, when the host RDBMS does not have such primitives (e.g. variance, linear regression).</p>

Examples of client request, e.g. via web browser:
<pre>
http://localhost:15387/?command=VER
http://localhost:15387/?command=ECHO&content='hello there to you too'
http://localhost:15387/?command=HELLO&content=someone
http://localhost:15387/?command=QUIT
</pre>

Example of server console output:<br/>
<pre>
WSGI micro-service core, version 0.0.1 (build: 20211224)
Harris Georgiou (c) 2021, Licence: CC-BY-NC-SA/4.0i 

CPU type: AMD64 / CPU count: 4 / CPU class: Intel64 Family 6 Model 58 Stepping 9, GenuineIntel
OS platform: 64bit-NT-WindowsPE / OS version: Windows-8.1-6.3.9600-SP0
Python version: 3.10.1 (CPython) / Python build: MSC v.1929 64 bit (AMD64)
SQLite3 version: 2.6.0 (Python) / 3.35.5 (engine) 

configuration:  cfg/settings.cfg
logging file:  log/error.log (level=DEBUG)
IP allowed list:  ['127.0.0.1']
Server listening at port 15387

Serving at port 15387...
-> Request(2): command='VER' , content='someone'
-> Request served in 0.0  msec
-> Request(1): command='ECHO' , content='hello there to you too'
-> Request served in 0.0  msec
-> Request(3): command='HELLO' , content='someone'
-> Request served in 0.0  msec
Client dropped (invalid request command)
-> Request(4): command='QUIT' , content='someone'
-> Request served in 0.0  msec

Shutting down (QUIT command)...
</pre>
