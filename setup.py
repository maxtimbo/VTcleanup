import sys
from cx_Freeze import setup, Executable

build_exe_options = {"packages" : ["os"]}

base = None
if sys.platform == "win32":
    base = "Win32GUI"

setup(  name = "VTcleanup",
        version = "0.1",
        description = "VT Cleanup Tool",
        options = {"build_exe" : build_exe_options},
        executables = [Executable("VTcleanup.pyw", base=base)])
