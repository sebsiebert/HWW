import sys
import os

directory = sys.argv[1] # output/Full2018_indep/

path1 = directory #"output/"+directory+"/"
if path1[-1] != "/": path1 = path1+"/"
firstdirs = [f for f in os.listdir(path1)]

for dirs in firstdirs:
  path = path1 + dirs + "/"
  files = [f for f in os.listdir(path) if f.endswith(".txt")]
  for f in files:
    if ("comb" in f) and not (os.path.isfile(path+f+"_orig")): os.system("cp "+path+f+" "+path+f+"_orig")
    #listofchannels = []

    with open(path+f, "r") as file:
      content = file.readlines()

    alreadydone = False
    for idx,line in enumerate(content):
      
      if ("rateParam" in line) and not ('1 [0,5]' in line): content[idx] = '1 [0,5]'.join(line.rsplit('1', 1)) # Setting range for rateParam
      #if line.startswith("bin") and listofchannels == []: listofchannels = line.split()[1:] # Getting list of channels

    # Don't do this! autoMCstats doesn't work at all when Morphed PDFs are present. Use do_bbb in CH instead!
    #for chan in listofchannels:
    #  if "top" in chan or "dy" in chan: continue
    #  content.append(chan + " autoMCStats 10 0 1\n") # Adding autoMCStats for all channels

    with open(path+f, "w") as file:
      for line in content:
        file.write(line)

print "Done!"
exit(0)
