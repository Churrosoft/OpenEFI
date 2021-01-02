Import("env")
import subprocess

git_tag = str(subprocess.check_output("git rev-parse --short HEAD", shell=True))[2:9]

# access to global construction environment
env.Replace(PROGNAME="firmware_%s" % git_tag) 