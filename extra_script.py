import subprocess
Import("env")
from os import path

git_tag = str(subprocess.check_output(
    "git rev-parse --short HEAD", shell=True))[2:9]

if(path.isfile('TESTING_CI') | path.isfile('RELEASE_CI')):
    env.Replace(PROGNAME="firmware")
else:
    # access to global construction environment
    env.Replace(PROGNAME="firmware_%s" % git_tag)


# Add semihosting feature
env.Append(
    LINKFLAGS=["--specs=rdimon.specs"],
    LIBS=["rdimon"]
)
