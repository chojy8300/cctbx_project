import sys, os, shutil
PACKAGES = sys.argv[0]
for i in xrange(3): PACKAGES = os.path.dirname(PACKAGES)
while (PACKAGES[-1] == os.sep): PACKAGES = PACKAGES[:-1]
os.mkdir("vc60")
os.chdir("vc60")
os.mkdir("boost")
os.chdir("boost")
shutil.copy(PACKAGES + r"\boost\libs\python\build\vc60.mak", "Makefile")
os.system('nmake ROOT="%s" copy' % (PACKAGES,))
os.system('nmake ROOT="%s"' % (PACKAGES,))
os.system("nmake test")
os.chdir("..")
os.mkdir("cctbx")
os.chdir("cctbx")
shutil.copy(PACKAGES + r"\cctbx\build\configuration_vc60", "configuration")
os.system(r"python %s\cctbx\build\boot.py" % (PACKAGES,))
os.system("python make.py copy")
os.system("python make.py compile_all")
os.system("python test.py")
os.system(r"examples\cpp\getting_started.exe")
os.environ["PYTHONPATH"] = os.path.normpath(os.getcwd() + "/lib/python")
os.system(r"python %s\cctbx\examples\python\getting_started.py" % (PACKAGES,))
print "Done."
