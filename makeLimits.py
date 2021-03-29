import os
import sys
import argparse
import exceptions
import subprocess

dry = False

def run(command):
    print "\n",command,"\n"
    if not dry:
        return subprocess.call(command, shell=True)
    else: return 0


def mkdir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)



def indep_datacards(years, cats):
    os.chdir('/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/datacards')
    if len(years) == 1:
        morph = 'MorphingMSSMFullRun2_HWW --output_folder indep_semi20{}'.format(years[0])\
                    +' --do20{}semi 1 --mass MH'.format(years[0])
        fixrate = 'python /afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/'\
                    +'HWW/FixRateparam.py output/indep_semi20{}'.format(years[0])

    else:
        morph = 'MorphingMSSMFullRun2_HWW --output_folder indep_semi_allyears'\
                    +' --do2016semi 1 --do2017semi 1 --do2018semi 1 --mass MH'
        fixrate = 'python /afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/'\
                    +'HWW/FixRateparam.py output/indep_semi_allyears'

    error = run(morph)
    if error!=0:
        return -1
    return run(fixrate)



def indep_workspaces(years, cats, scenario):
    os.chdir('/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/datacards')
    commands = []
    if len(years) == 1:
        if scenario == 'sm':
            template = 'combineTool.py -M T2W -i "output/indep_semi20'+years[0]+'/hww_{long}_13TeV/combined.txt" -o "../../semi20'+years[0]+'.indepSM_{short}.root" -P CombineHarvester.CombinePdfs.XWWInterference:XWW --PO NoSMXsecAdded'
        else:
            template = 'combineTool.py -M T2W -i "output/indep_semi20'+years[0]+'/hww_{long}_13TeV/combined.txt" -o "../../semi20'+years[0]+'.indep_{short}.root" -P CombineHarvester.CombinePdfs.XWWInterference:XWW --PO FloatVBFFraction --PO NoSMXsecAdded'
    else:
        if scenario == 'sm':
            template = 'combineTool.py -M T2W -i "output/FullRun2_indep/hww_{long}_13TeV/combined.txt" -o "../../semi_allyears.indepSM_{short}.root" -P CombineHarvester.CombinePdfs.XWWInterference:XWW --PO NoSMXsecAdded'
        else:
            template = 'combineTool.py -M T2W -i "output/FullRun2_indep/hww_{long}_13TeV/combined.txt" -o "../../semi_allyears.indep_{short}.root" -P CombineHarvester.CombinePdfs.XWWInterference:XWW --PO FloatVBFFraction --PO NoSMXsecAdded'

    if 'res' in cats:
        commands.append(template.format(long='resolved', short='res'))

    if 'bos' in cats:
        commands.append(template.format(long='boosted', short='bos'))

    if 'res' in cats and 'bos' in cats:
        commands.append(template.format(long='res+bos', short='br'))

    if 'hm' in cats:
        commands.append(template.format(long='hm', short='hm'))

    error = 0
    while commands and error==0:
        error = run(commands.pop())



def indep_limits(years, cats, scenario):

    if scenario != 'sm':
        value = 0 if scenario == 'ggf' else (1 if scenario == 'vbf' else 0.5)
        freeze = '' if scenario == 'float' else '--freezeParameters fvbf'

    res_masses = "115, 120, 125, 130, 140, 160, 180, 200, 250, 300"
    # bos_masses = "700, 750, 800, 850, 900, 950, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3200, 3400, 3600, 3800, 4000, 4200, 4400, 4600, 4800, 5000"
    bos_masses = "700, 750, 800, 900, 950, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3200, 3400, 3600, 3800, 4000, 4200, 4400, 4600, 4800, 5000"
    comb_masses = "400, 450, 500, 550, 600"
    hm_masses = "600, 700, 750, 800, 850, 900, 950, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3200, 3400, 3600, 3800, 4000, 4200, 4400, 4600, 4800, 5000"

    if len(years) == 1:
        if scenario == 'sm':
            dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/20{y}_semi_indepSM'.format(y=years[0])
            command_template = """combineTool.py -m "{masses}" -M AsymptoticLimits --rAbsAcc 0 --rRelAcc 0.0005 --setParameters r=0 --run expected -d ../datacards/output/semi20"""+years[0]+""".indepSM_{short}.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'indepSM_{short}_"""+years[0]+"'"
        else:
            dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/20{y}_semi_indep{s}'.format(y=years[0],s=scenario)
            command_template = """combineTool.py -m "{masses}" -M AsymptoticLimits --rAbsAcc 0 --rRelAcc 0.0005"""+""" --setParameters sigma=0,fvbf={value} {freeze} --run expected -d ../datacards/output/semi20""".format(value=value, freeze=freeze)+years[0]+""".indep_{short}.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'indepVBF_{short}_"""+years[0]+"' --boundlist ../HWW/mssm_HWW_boundaries.json"
    else:
        if scenario == 'sm':
            dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/allyears_semi_indepSM'
            command_template = """combineTool.py -m "{masses}" -M AsymptoticLimits --rAbsAcc 0 --rRelAcc 0.0005 --setParameters r=0 --run expected -d ../datacards/output/semi_allyears.indepSM_{short}.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'indepSM_{short}_allyears'"""
        else:
            dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/allyears_semi_indep'+scenario
            command_template = """combineTool.py -m "{masses}" -M AsymptoticLimits --rAbsAcc 0 --rRelAcc 0.0005"""+" --setParameters sigma=0,fvbf={value} {freeze}".format(value=value, freeze=freeze)+""" --run expected -d ../datacards/output/semi_allyears.indep_{short}.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'indepSM_{short}_allyears' --boundlist ../HWW/mssm_HWW_boundaries.json"""

    if 'res' in cats:
        mkdir(dir_template)
        os.chdir(dir_template)
        run(command_template.format(masses=res_masses, short='res'))

    if 'bos' in cats:
        mkdir(dir_template)
        os.chdir(dir_template)
        run(command_template.format(masses=bos_masses, short='bos'))

    if 'res' in cats and 'bos' in cats:
        mkdir(dir_template)
        os.chdir(dir_template)
        run(command_template.format(masses=comb_masses, short='br'))

    if 'hm' in cats:
        mkdir(dir_template+'_hm')
        os.chdir(dir_template+'_hm')
        run(command_template.format(masses=hm_masses, short='hm'))



def indep_collect(years, cats, scenario):
    base_dir = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/'
    if scenario == 'sm':
        dir_template = '{y}_semi_indepSM'
    else:
        dir_template = '{y}_semi_indep'+scenario

    if len(years) == 1:
        dir_template = dir_template.format(y='20'+years[0])

    else:
        dir_template = dir_template.format(y='allyears')
    
    if 'res' in cats or 'bos' in cats:
        os.chdir(base_dir+dir_template)
        run('combineTool.py -M CollectLimits -i higgsCombine.Test.Asymptotic*.root -o indep.json')
        run('python ../HWW/addxstoindep.py {}'.format(dir_template))
    if 'hm' in cats:
        os.chdir(base_dir+dir_template+'_hm')
        run('combineTool.py -M CollectLimits -i higgsCombine.Test.Asymptotic*.root -o indep.json')
        run('python ../HWW/addxstoindep.py {}'.format(dir_template+'_hm'))



def indep_plot(years, cats, scenario):
    suffix = 'SM' if scenario == 'sm' else scenario
    dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/{y}_semi_indep'+suffix
    lumi_dict = {'16': 35.9, '17': 41.5, '18': 59.7, 'all': 137.1}

    if len(years) == 1:
        dir_template = dir_template.format(y='20'+years[0])
        lumi = lumi_dict[years[0]]
    else:
        dir_template = dir_template.format(y='allyears')
        lumi = lumi_dict['all']

    if scenario == 'sm':
        addPaper = '--addPaper'
    else:
        addPaper = ''
    
    if scenario == 'sm':
        label = 'SM f_{VBF}'
    elif scenario == 'ggf':
        label = 'f_{VBF} = 0'
    elif scenario == 'vbf':
        label = 'f_{VBF} = 1'
    elif scenario == 'float':
        label = 'floating f_{VBF}'

    if 'res' in cats or 'bos' in cats:
        os.chdir(dir_template)
        run('python ../HWW/plotMSSMLimits.py --logy --show exp indep.json --cms-sub "Private work" --scenario-label "{label}" --title-right "{lumi} fb^{{-1}} (13 TeV)" --y-axis-max 40 --y-axis-min 0.00001 --addSM {paper}'.format(lumi=lumi, paper=addPaper, label=label))

    if 'hm' in cats:
        os.chdir(dir_template+'_hm')
        run('python ../HWW/plotMSSMLimits.py --logy --show exp indep.json --cms-sub "Private work" --scenario-label "{label}" --title-right "{lumi} fb^{{-1}} (13 TeV)" --y-axis-max 40 --y-axis-min 0.00001 --addSM {paper}'.format(lumi=lumi, paper=addPaper, label=label))









def mssm_datacards(years):
    os.chdir('/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/datacards')

    if len(years) == 1:
        morph = 'MorphingMSSMFullRun2_HWW --output_folder mh125_semi20{}'.format(years[0])\
                    +' --do20{}semi 1 --mass mH'.format(years[0])
        fixrate = 'python /afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/'\
                    +'HWW/FixRateparam.py output/mh125_semi20{}'.format(years[0])

    else:
        morph = 'MorphingMSSMFullRun2_HWW --output_folder mh125_semi_allyears'\
                    +' --do2016semi 1 --do2017semi 1 --do2018semi 1 --mass mH'
        fixrate = 'python /afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/'\
                    +'HWW/FixRateparam.py output/mh125_semi_allyears'

    error = run(morph)
    if error!=0:
        return -1
    return run(fixrate)
        


def mssm_workspaces(years):
    os.chdir('/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/datacards')
    commands = []
    if len(years) == 1:
        template = 'combineTool.py -M T2W -i "$CMSSW_BASE/src/CombineHarvester/datacards/output/mh125_semi20'+years[0]+'/hww_{long}_13TeV/combined.txt" -o "../../semi20'+years[0]+'_{short}.mh125.root" -P CombineHarvester.CombinePdfs.MSSM_HWW:MSSM --PO filePrefix=$CMSSW_BASE/src/CombineHarvester/HWW/Models/ --PO modelFiles=13TeV,mh125_13_VBF_810.root,2'
    else:
        template = 'combineTool.py -M T2W -i "$CMSSW_BASE/src/CombineHarvester/datacards/output/FullRun2_mh125/hww_{long}_13TeV/combined.txt" -o "../../semi_allyears_{short}.mh125.root" -P CombineHarvester.CombinePdfs.MSSM_HWW:MSSM --PO filePrefix=$CMSSW_BASE/src/CombineHarvester/HWW/Models/ --PO modelFiles=13TeV,mh125_13_VBF_810.root,2'


    commands.append(template.format(long='resolved', short='res'))

    commands.append(template.format(long='res+bos', short='br'))

    error = 0
    while commands and error==0:
        error = run(commands.pop())



def mssm_limits(years):
    dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/{y}_mh125_semi'

    res_grid  = "../HWW/mssm_asymptotic_grid_mh125_lo.json"
    comb_grid = "../HWW/mssm_asymptotic_grid_mh125_hi_morebin.json"

    if len(years) == 1:
        dir_template = dir_template.format(y='20'+years[0])
        command_template = """combineTool.py -M AsymptoticGrid {grid} -d ../datacards/output/semi20"""+years[0]+"""_{short}.mh125.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'mh125_{short}_"""+years[0]+"""' --merge=8"""
    else:
        dir_template = dir_template.format(y='allyears')
        command_template = """combineTool.py -M AsymptoticGrid {grid} -d ../datacards/output/semi_allyears_{short}.mh125.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'mh125_{short}_allyears' --merge=8"""


    mkdir(dir_template)
    os.chdir(dir_template)
    run(command_template.format(grid=res_grid, short='res'))

    run(command_template.format(grid=comb_grid, short='br'))



def mssm_collect(years):
    dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/{y}_mh125_semi'

    complete_grid = '../HWW/mssm_asymptotic_grid_mh125_comb.json'

    if len(years) == 1:
        dir_template = dir_template.format(y='20'+years[0])
        command_template = """combineTool.py -M AsymptoticGrid {grid} -d ../datacards/output/semi20"""+years[0]+"""_{short}.mh125.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'mh125_"""+years[0]+"""' --merge=8"""
    else:
        dir_template = dir_template.format(y='allyears')
        command_template = """combineTool.py -M AsymptoticGrid {grid} -d ../datacards/output/semi_allyears_{short}.mh125.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'mh125_allyears' --merge=8"""
    os.chdir(dir_template)
    run(command_template.format(grid=complete_grid, short='res'))



def mssm_plot(years):
    dir_template = '/afs/cern.ch/work/s/ssiebert/CMSSW_10_2_13/src/CombineHarvester/{y}_mh125_semi'

    lumi_dict = {'16': 35.9, '17': 41.5, '18': 59.7, 'all': 137.1}

    if len(years) == 1:
        dir_template = dir_template.format(y='20'+years[0])
        lumi = lumi_dict[years[0]]
    else:
        dir_template = dir_template.format(y='allyears')
        lumi = lumi_dict['all']

    command_template = """python ../CombineTools/scripts/plotLimitGrid.py asymptotic_grid.root --cms-sub "Private work" --contours "exp-2,exp-1,exp0,exp+1,exp+2" --logy --title-right "{lumi} fb^{{-1}} (13 TeV)" --scenario-label "M_{{h}}^{{125}}" """

    os.chdir(dir_template)
    run(command_template.format(lumi=lumi))







def make_indep(step, years, cats, scenario):
    if step == 'datacards':
        indep_datacards(years, cats)

    elif step == 'workspaces':
        for s in scenario:
            indep_workspaces(years, cats, s)

    elif step == 'limits':
        for s in scenario:
            indep_limits(years, cats, s)

    elif step == 'collect':
        for s in scenario:
            indep_collect(years, cats,s)

    elif step == 'plot':
        for s in scenario:
            indep_plot(years, cats, s)


def make_mssm(step, years):
    if step == 'datacards':
        mssm_datacards(years)

    elif step == 'workspaces':
        mssm_workspaces(years)

    elif step == 'limits':
        mssm_limits(years)

    elif step == 'collect':
        mssm_collect(years)

    elif step == 'plot':
        mssm_plot(years)




if __name__ == "__main__":
    
    parser = argparse.ArgumentParser(description='An interface to run the different combine steps.')
    parser.add_argument('-step', type=str, choices=['datacards', 'workspaces',
                        'limits', 'collect', 'plot'], required=True,
                        help='step to run (datacards|workspaces|limits|collect|plot)')
    parser.add_argument('-years', type=str, nargs='+', required=True,
                        help='years to run. single year or all three combined (16, 17, 18)')
    parser.add_argument('-kind', type=str, choices=['indep', 'mssm'], required=True,
                        help='indep or mssm')
    parser.add_argument('-cats', type=str, nargs='+', choices=['res','bos','hm'],
                        help='categories to include')
    parser.add_argument('-scenario', type=str, nargs='*', choices=['sm', 'float', 'vbf', 'ggf'],
                        default=['sm',], help='indep vbf fraction scenarios')
    parser.add_argument('-dryrun', action='store_true',
                        help="only print commands but don't run")

    args = parser.parse_args()
    print(args)
    years = args.years
    if len(years)!=1 and ('16' not in years or '17' not in years or '18' not in years):
        raise ValueError("years must either be single year or all three")
    
    dry = args.dryrun

    if(args.kind == 'indep'):
        make_indep(args.step, args.years, args.cats, args.scenario)
    elif(args.kind == 'mssm'):
        make_mssm(args.step, args.years)