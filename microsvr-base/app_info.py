# Module: app_info.py  /  version: 1.5  /  build: 20211223
# Purpose: Retrieve application and system information
# Harris Georgiou (c) 2021, Email:hgeorgiou@unipi.gr

# use only standard modules for cross-platform portability
import os, platform, sqlite3, json


def get_sys_info():
    '''Create a rich string with system-specific information (return: str)'''
    arch=platform.architecture()
    res = 'CPU type: %s / CPU count: %s / CPU class: %s\n' % (platform.machine(), os.cpu_count(), platform.processor())
    res += 'OS platform: %s-%s-%s / OS version: %s\n' % (arch[0], str(os.name).upper(), arch[1], platform.platform())
    res += 'Python version: %s (%s) / Python build: %s\n' % (platform.python_version(), platform.python_implementation(), platform.python_compiler())
    res += 'SQLite3 version: %s (Python) / %s (engine)' % (sqlite3.version, sqlite3.sqlite_version)
    return(res)


def get_app_info(str_appname, str_appversion, str_appcopyright):
    '''Create a plain application name and version string (return: str)'''
    res = '%s, version %s\n%s' % (str_appname, str_appversion, str_appcopyright)
    return(res)


def read_version_json( jfname ):
    '''Read application name and version from .json file (return: list)'''
    version_list = None

    with open(jfname, 'r') as jfile:
        jdata=jfile.read()
        jcont = json.loads(jdata)
        version_list = [ str(jcont['application']['name']),     # application name
                         str(jcont['application']['version']),  # full version tag
                         str(jcont['application']['build']),    # build tag
                         str(jcont['update']['last']) ]         # last updated

    return version_list


def main():
    '''Default function when running as script (testing mode)'''
    print(get_app_info('PyTest example','0.1 (beta)','Copyright (c) <author> 2020, Licence: CC-BY-SA/4.0i'))
    print(get_sys_info())
    print('json version info: ',read_version_json('../../version.json'))


if __name__ == "__main__":
    main()       # stub for calling main loop
