from __future__ import print_function
from tempfile import mkstemp
from shutil import move
from os import fdopen, remove
import os
import argparse
import re
import subprocess
import time

def listAllFiles(directory):
  files = []
  for dirpath,_,filenames in os.walk(directory):
    for f in filenames:
      files.append(os.path.abspath(os.path.join(dirpath, f)))
  return files

def main(args):

  ## check xml file exists
  xml_file = os.path.join(os.getcwd(), args.submitscript)
  if not os.path.isfile(xml_file) :
    print('xmlfile doesnt exist!')
    return

  ## create output from input variables
  #param_string = "geantid_{}_dca_{}_nhit_{}_nhitfrac_{}".format(args.geantid, args.dca, args.nhit, args.nhitfrac)
  print('TRAZAN: ',args.outputroot)
  out_base = os.path.join(args.outputroot, args.production, args.outputtag)#, param_string)
  print('TRAZODAN: ',out_base)
  log_dir = os.path.join(out_base, "log")
  out_dir = os.path.join(out_base, "out")

  ## create log and output directories
  if not os.path.exists(log_dir):
    os.makedirs(log_dir)
  if not os.path.exists(out_dir):
    os.makedirs(out_dir)

  ## get the list of input files
  filelist = listAllFiles(args.listdir)

  ## sort the files into mu and mc lists
  find_mu = re.compile('MuDsts\d+_\d+.list')
  #find_mu = re.compile('MuDsts_\d+.list')#nominal
  mu_list = []
  find_mc = re.compile('minimcs\d+_\d+.list')
  #find_mc = re.compile('minimcs_\d+.list')#nominal
  mc_list = []
  print(find_mu)
  print(find_mc)
  #print(filelist)
  print('huluuuuuuuuuuu')
  for file in filelist :
    if find_mu.search(file) :
      mu_list.append(file)
    elif find_mc.search(file) :
      mc_list.append(file)
  mu_list.sort()
  mc_list.sort()
  
  if len(mc_list) != len(mu_list):
    print("Error: different number of mu and minimc list files")

  print('Submitting jobs - parameters')
  print('number of jobs: ', len(mc_list))
  print('library: ', args.library)
#  print('DCA: ', args.dca)
#  print('nhit: ', args.nhit)
#  print('nhitposs: ', args.nhitfrac)
#  print('geant ID', args.geantid)
  print('log directory: ', log_dir)
  print('output directory: ', log_dir)

  for i in range(len(mu_list)) :
    mu_file = mu_list[i]
    mc_file = mc_list[i]

    print("submitting job: ")
    print("muDst file list: " + mu_file)
    print("minimc file list: " + mc_file)

    print("slice: " + mc_file[mc_file.find('minimcs')+len('minimcs'):mc_file.find('.list')])
    
    slice_name = 'pt-hat' + mc_file[mc_file.find('minimcs')+len('minimcs'):mc_file.find('.list')]

    submit_args = 'lib=' + args.library
    submit_args = submit_args + ',mulist=' + mu_file
    submit_args = submit_args + ',mclist=' + mc_file
    submit_args = submit_args + ',log=' + log_dir
    submit_args = submit_args + ',out=' + out_dir
    submit_args = submit_args + ',outfile=' + slice_name

    star_submit = 'star-submit-template '
    star_submit = star_submit + '-template ' + xml_file
    star_submit = star_submit + ' -entities ' + submit_args
    
    print('submit command: ', star_submit)

    ret = subprocess.Popen(star_submit, shell=True)
    ret.wait()
    if ret.returncode != 0:
      print('warning: job submission failure')
    
if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='Submit jobs to generate embedding picos', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('--submitscript', default='submit/jetPicoProduction_example.xml', help='the xml file for star-submit-template')
  parser.add_argument('--listdir', default='list_20235003', help='the xml file for star-submit-template') #'list_pAu2015embed_2022fullstats', help='directory to lists of mu and mc lists')#list_forRaghav_sample_2535
  parser.add_argument('--production', default='P12id', help='production tag associated with the data embedded into') #'P18ih', help='production tag associated with the data embedded into')#P12id
  #parser.add_argument('--outputtag', default='picos/reprod_2/', help='output directory name (appended to outputroot/production')#TEST_pAu_200_production_2015#nominal
  parser.add_argument('--outputtag', default='picos/20235003', help='output directory name (appended to outputroot/production') #'picos/pAu2015embed_2022fullstats/', help='output directory name (appended to outputroot/production')#pp2012embed_2022sample_2535
  parser.add_argument('--library', default='SL12d', help='STAR library version to run analysis with')#pro
  parser.add_argument('--outputroot', default='/gpfs01/star/pwg/youqi/run12/embedding', help='root directory for all output and logs')
  args = parser.parse_args()
  main( args )

