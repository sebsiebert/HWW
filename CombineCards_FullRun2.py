import sys
import os

directory = sys.argv[1] # output/FullRun2_indep/
if directory[-1] != "/": directory = directory+"/"
dirlist = ["hww_res+bos_13TeV/", "hww_resolved_13TeV/", "hww_boosted_13TeV/"]
directory_list = [directory+x for x in dirlist]

for path in  directory_list:
    files = sorted([file for file in os.listdir(path) if file.endswith('.txt')])
    cc_cards = [os.path.splitext(file)[0]+'='+file for file in files]
    cmd = 'combineCards.py %(CARDS)s &> %(COMBINED)s' % ({
                        'CARDS': ' '.join(cc_cards),
                        'COMBINED': 'combined.txt'
                        })
    os.system("pushd "+path+"; "+cmd+"; popd")
    print "Done!"