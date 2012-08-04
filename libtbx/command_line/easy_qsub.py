"""
libtbx.qsub(
 where=path_to_where_you_want_to_run_your_jobs,
 source="/net/chevy/raid1/afonine/build/setpaths.csh",
 commands=[list of commands])

where [list of commands]) is

["python run.py model1.pdb data1.mtz",
 "python run.py model2.pdb data2.mtz",
...,
"python run.py modelN.pdb dataN.mtz" ]
"""

import os, sys
from libtbx import easy_run
from libtbx.utils import Sorry

key_words = {
  "phenix_source"    : str,
  "where"            : str,
  "commands"         : str,
  "number_of_chunks" : int,
  "size_of_chunks"   : int,
  "code"             : str,
  "js"               : int,
  "qsub_cmd"         : str,
  "start"            : int,
  "end"              : int,
  "host_scratch_dir" : str,
  }

script_file = """
import os, sys
from libtbx import easy_run

cmds = [
%s
]

def run(only_i=None,
        chunk_n=1,
        chunk_size=len(cmds),
        ):
  try: only_i = int(only_i)
  except ValueError: only_i=None

  try: chunk_n = int(chunk_n)
  except ValueError: chunk_n=1
  try: chunk_size = int(chunk_size)
  except ValueError: chunk_size=len(cmds)

  assert chunk_size==len(cmds) or chunk_n==1
  assert chunk_n>0
  assert chunk_size>0
  if chunk_n!=1:
    chunk_size = (len(cmds)-1)//chunk_n+1
  elif chunk_size!=1:
    chunk_n = len(cmds)%%chunk_size+1

  for i, cmd in enumerate(cmds):
    if only_i is None:
      print i, cmd
      continue
    else:
      if chunk_n!=1 or chunk_size!=len(cmds):
        if only_i!=i//chunk_size+1: continue
      else:
        if only_i!=i+1: continue
    print 'Running', i+1
    print 'Command', cmd
    easy_run.call(cmd)

if __name__=="__main__":
  run(*tuple(sys.argv[1:]))
"""

host_scratch_dir_pre = """
set work_dir=$PWD

set host_scratch_dir=/net/$host/scratch1/$user
if (! -d $host_scratch_dir) then
  echo "================================"
  echo " Creating host scratch directory $host_scratch_dir"
  echo "================================"
  mkdir $host_scratch_dir
endif

set host_scratch_dir=/net/$host/scratch1/$user/%s
if (! -d $host_scratch_dir) then
  echo "================================"
  echo " Creating host scratch directory $host_scratch_dir"
  echo "================================"
  mkdir $host_scratch_dir
endif

set host_scratch_dir=/net/$host/scratch1/$user/%s/$SGE_TASK_ID
if (! -d $host_scratch_dir) then
  echo "================================"
  echo " Creating host scratch directory $host_scratch_dir"
  echo "================================"
  mkdir $host_scratch_dir
endif

echo "=============="
echo " Changing into $host_scratch_dir"
echo "=============="
cd $host_scratch_dir
"""
host_scratch_dir_post = """
echo "==================="
echo " Move files back to $work_dir"
echo "==================="
mv * $work_dir
"""

run_file = """#! /bin/csh -q
#$ -cwd
#$ -o %s_queue.output -j y -N %s
limit datasize 2000000

source %s

%s

phenix.python %s $SGE_TASK_ID $SGE_TASK_LAST >& %s.$SGE_TASK_ID.out

%s

exit

"""

test_run_script = """
import os, sys

def run(only_i=None):
  print "%s " % only_i
  f=file("test_%s.output" % only_i, "wb")
  f.write("Testing %s\\n" % only_i)
  f.close()

if __name__=="__main__":
  run(*tuple(sys.argv[1:]))
"""

def process_args(args):
  kwds = {}
  for t in args:
    for key in key_words:
      if t.find("%s=" % key)==0:
        kwds[key]=t.split("=")[1]
        kwds[key] = key_words[key](kwds[key])
        break
    else:
      print '\n  failed to process "%s"\n' % t
      assert 0
  return kwds

def run(phenix_source=None,
        where=None,
        commands=None,
        size_of_chunks=1,
        number_of_chunks=None,
        code=None,
        js=1,
        qsub_cmd="qsub",
        start=1,
        end=None,
        host_scratch_dir=None,
        ):
  if not phenix_source:
    print '-'*80
    print "\n  Automatically setting phenix_source to current $PHENIX/phenix_env\n"
    phenix_source = "%s/phenix_env" % os.environ.get("PHENIX", "")

  if not commands:
    print '-'*80
    print "\n  Generating a test run script and queuing 10 times\n"
    f=file("easy_qsub_test_script.py", "wb")
    f.write(test_run_script)
    f.close()
    commands = []
    for i in range(10):
      commands.append("phenix.python %s %s" % (os.path.join(
                                               os.getcwd(),
                                               "easy_qsub_test_script.py"),
                                               i+100,
                                              )
        )


  print '-'*80
  print '  Inputs'
  print '    phenix_source',phenix_source
  if phenix_source.find("phenix_env")==-1 and phenix_source.find("setpath")==-1:
    print '  Need to supply file to source. e.g. phenix_env'
    return False
  if not os.path.exists(phenix_source):
    raise Sorry('source file for PHENIX environment not found "%s"' % phenix_source)
  print '    where',where
  if type(commands)==type([]):
#  if commands is None:
#    print '  Need to supply a list of commands, either by file or python list'
#    return False
#  elif type(commands)==type([]):
    if code is None: code = "easy_qsub"
    print '    commands',len(commands),
    if len(commands)>1:
      print 'similar to\n\n> %s\n' % (commands[0])
  else:
    print '    commands',commands
    assert os.path.exists(commands)
    if code is None: code = commands[:8]
  print '    size_of_chunks',size_of_chunks
  print '    number_of_chunks',number_of_chunks
  if number_of_chunks==1:
    print '\n  Need to choose number_of_chunks>1'
    return
  print '-'*80
  if where is None:
    where = os.getcwd()

  assert phenix_source

  if type(commands)==type([]):
    lines = commands
  else:
    f=file(commands, "rb")
    lines = f.readlines()
    f.close()
  number_of_jobs = len(lines)
  print '\n  Number of lines in command file',number_of_jobs
  if number_of_chunks is None:
    if size_of_chunks==1:
      number_of_chunks=len(lines)
    else:
      number_of_chunks=number_of_jobs//size_of_chunks
      if number_of_jobs%size_of_chunks: number_of_chunks+=1
  else:
    size_of_chunks = number_of_jobs//number_of_chunks
    if number_of_jobs%size_of_chunks: size_of_chunks+=1
  number_of_jobs = number_of_chunks

  print '\n  Number of queue jobs',number_of_jobs
  print '  Number of command in each queue job',size_of_chunks

  #print '\n  Changing to work directory : %s' % where
  os.chdir(where)

  if host_scratch_dir:
    print '\n  Setting up scratch directories on queue hosts'
    pre = host_scratch_dir_pre % (host_scratch_dir, host_scratch_dir)
    post = host_scratch_dir_post
  else:
    pre = ""
    post = ""

  python_run_filename = "easy_qsub_python_script.py"
  qsub_run_filename = "easy_qsub_qsub_script.sh"

  outl = ""
  for line in lines:
    if line[-1]=="\n": line = line[:-1]
    outl += "  '''%s ''',\n" % line
  python_run_filename = os.path.join(os.getcwd(), python_run_filename)
  print "  Writing queue python script:\n    %s" % python_run_filename
  f=file(python_run_filename, "wb")
  f.write(script_file % outl)
  f.close()

  qsub_run_filename = os.path.join(os.getcwd(), qsub_run_filename)
  print "  Writing queue command script:\n    %s" % qsub_run_filename
  f=file(qsub_run_filename, "wb")
  f.write(run_file % (
    code,
    code,
    phenix_source,
    pre,
    python_run_filename,
    code,
    post,
    )
    )
  f.close()

  if end is None:
    end = number_of_jobs

  cmd = "%s -t %d-%d -js %d %s" % (
    qsub_cmd,
    start,
    end, #number_of_jobs,
    js,
    qsub_run_filename,
    )
  print '\n  Queue command\n'
  print "> %s\n" % cmd
  easy_run.call(cmd)


if __name__=="__main__":
  args=sys.argv[1:]
  kwds = process_args(args)
  run(**kwds)
