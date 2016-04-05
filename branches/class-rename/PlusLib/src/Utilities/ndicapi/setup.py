from distutils.core import setup, Extension


module1 = Extension('polaris',
                    sources = [
'ndicapi.c',
'ndicapi_math.c',
'ndicapi_serial.c',
'ndicapi_thread.c',
'polarismodule.c',
])

setup (name = 'polaris',
       version = '3.2',
       description = 'This is a demo package',
       ext_modules = [module1])
