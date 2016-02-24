from distutils.core import setup  
import py2exe  
 
#setup(console=["client.py"])
setup(windows=["wavy_ppt_client.py"])
#setup(windows=["wavy_ppt_service.py"])

#class Target:
#    def __init__(self, **kw):
#        self.__dict__.update(kw)
#        # for the versioninfo resources
#        self.version = "1.1.8"
#        self.company_name = "Yovole Shanghai Co. Ltd."
#        self.copyright = "Copyright (c) 2013 Founder Software (Shanghai) Co., Ltd. "
#        self.name = "Guest Agent"
#myservice = Target(
#    description = 'Yovole Cloud Desktop Guest Agent',
#    modules = ['server'],
#    cmdline_style='pywin32'
#    #icon_resources=[(1, "cartrigde.ico")] 
#)
#options = {"py2exe":   
#            {   "compressed": 1,   
#                "bundle_files": 1 
#            }   
#          }  
#setup(
#    service=[myservice],
#    options = options, 
#    zipfile = None,
#    windows=[{"script": "server.py"}], 
#)