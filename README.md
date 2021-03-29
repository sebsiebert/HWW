Setup for running high mass semileptonic HWW limits.

The script `makeLimits.py` prepares all the needed commands for morphing, datacards, workspaces, and running the limits.
The option -dryrun allows for testing, the script will then only print the commands without running them.

For cross section limits, specify `-kind indep` as well as the desired scenarios: `-scenario sm vbf ggf float` (any combination of these should work). The year can be given as single year or all three years like `-years 16` or `-years 16 17 18`. Finally specify the next step, it will be one of `datacards`, `workspaces`, `limits`, `collect`, `plot`.

Example usage for cross section limits is:
1. Go to parent directory of HWW (this is currently a hardcoded requirement for `makeLimits.py` to work)

  ```cd ..```

2. Run morphing and fix rateparam

```python HWW/makeLimits.py -kind indep -cats res bos -years 18 -scenario sm vbf ggf float -step datacards```

3. Make workspaces

```python HWW/makeLimits.py -kind indep -cats res bos -years 18 -scenatio sm vbf ggf float -step workspaces```

4. Run limits

```python HWW/makeLimits.py -kind indep -cats res bos -years 18 -scenatio sm vbf ggf float -step limits```

5. After all the limits have been computed and the jobs finished, collect the outputs.

```python HWW/makeLimits.py -kind indep -cats res bos -years 18 -scenatio sm vbf ggf float -step collect```

6. Make plots out of the collected outputs:

```python HWW/makeLimits.py -kind indep -cats res bos -years 18 -scenatio sm vbf ggf float -step plot```

<br/><br/>

The idea is the same for MSSM limits, only `-kind mssm`, the year(s) and the step need to be given.


<br/><br/><br/><br/><br/><br/>

How to run this (old text):

1. Run `MorphingMSSMFullRun2_HWW`. Add the options `--output_folder someoutputdirectory` and add what year to run, like `--do2016semi 1`. For model independent limits also add the option `--mass MH`.

  NOTE: The following are hardcoded into the script: Path to root files containing shapes, List of backgrounds, List of signal masses, and Path to the shapes inside the root file.
        When changing any of these, don't forget to do `scram b -j4` before executing the script.

2. I run `python FixRateparam.py output/someoutputdirectory` to give the rateParam a range between 0 and 5, simply to avoid it becoming negative.

3. Make workspaces. These are the commands I use for myself:
- Model independent limits (SM fVBF):
`combineTool.py -M T2W -i "output/Full2016_indep/cmb/combined.txt" -o "../../WS2016.indepSM.root" -P CombineHarvester.CombinePdfs.XWWInterference:XWW --PO NoSMXsecAdded`
It requires the physics model "XWW" which I load from "CombineHarvester/CombinePdfs/XWWInterference" (I added this file here). NOTE THE OPTION `--PO NoSMXsecAdded`: The signal/interference shapes I use are NOT scaled by their xsec! This is done in the physics model instead.

- Model independent limits (other fVBF scenarios):
`combineTool.py -M T2W -i "output/Full2016_indep/cmb/combined.txt" -o "../../WS2016.indep.root" -P CombineHarvester.CombinePdfs.XWWInterference:XWW --PO FloatVBFFraction --PO NoSMXsecAdded`

- Model dependent limits (Mh125 scenario):
`combineTool.py -M T2W -i "output/Full2016/cmb/combined.txt" -o "../../WS2016.mh125.root" -P CombineHarvester.CombinePdfs.MSSM_HWW:MSSM --PO filePrefix=$CMSSW_BASE/src/CombineHarvester/Models/ --PO modelFiles=13TeV,mh125_13_VBF_810.root,2`
Aside from the physics model, this also need the file to read the xsec/BR from. I provide these files here as well, as I manually added the VBF xsec to them.

4. Now run the limits. These are the commands I use for myself:
- Model independent limits (SM fVBF):
`combineTool.py -m "115, 120, 125, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3200, 3400, 3600, 3800, 4000, 4200, 4400, 4600, 4800, 5000" -M AsymptoticLimits --rAbsAcc 0 --rRelAcc 0.0005 --setParameters r=0 --run expected -d ../../HWW/output/WS2016.indep.root --job-mode 'condor' --sub-opts '+JobFlavour = "tomorrow"' --task-name 'indepSM'`

- Model independent limits (fVBF=1):
`combineTool.py -m "115, 120, 125, 130, 140, 160, 180, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500, 2600, 2700, 2800, 2900, 3000, 3200, 3400, 3600, 3800, 4000, 4200, 4400, 4600, 4800, 5000" -M AsymptoticLimits --rAbsAcc 0 --rRelAcc 0.0005 --setParameters sigma=0,fvbf=1 --freezeParameters fvbf --run expected -d ../../HWW/output/WS2016.indep.root --job-mode 'condor' --sub-opts '+JobFlavour = "tomorrow"' --task-name 'indepVBF' --boundlist ../../CombinePdfs/scripts/mssm_HWW_boundaries.json`
Note the option `--boundlist ../../CombinePdfs/scripts/mssm_HWW_boundaries.json`. For SM fVBF the limit is on r (around 1.0), but here it is on sigma (much lower for high masses). The values in this json set the approximate limits for combine on where to scan, such that the limit on sigma can be found with better precision.

- Model dependent limits (Mh125 scenario):
`combineTool.py -M AsymptoticGrid ../scripts/FullRun2/mssm_asymptotic_grid_mh125.json -d ../../HWW/output/WS2016.mh125.root --job-mode 'condor' --sub-opts '+JobFlavour = "workday"' --task-name 'mh125' --merge=8`
The json file here sets the grid in which points of the two-dimensional plane the exclusion is tested.

5. Plot the limits
- Model independent:
`combineTool.py -M CollectLimits -i higgsCombine.Test.Asymptotic*.root -o indep.json`
This collects the result of all mass point into `indep.json`. For the SM fVBF scenario, we translate the limit on r into the limit on sigma. Run `python addxstoindep.py` with the directory containing the limits as argument (The script contains some paths with need to be fixed on your side). Do this also for other fVBF scenarios to produce the expected SM xsec curve.
The you can make the limit using something like `python ../scripts/plotMSSMLimits.py --logy --show exp indep.json --cms-sub "Work in progress" --scenario-label "SM f_{VBF}" --addSM  --title-right "35.9 fb^{-1} (13 TeV)" --y-axis-max 20 --y-axis-min 0.000008`

- Model dependent:
Run the same command as you use to submit the jobs. If all jobs are done, it will create a file `asymptotic_grid.root`.
Then just run `combineTool.py -M AsymptoticGrid ../scripts/FullRun2/mssm_asymptotic_grid_mh125.json python ../../CombineTools/scripts/plotLimitGrid.py asymptotic_grid.root --cms-sub "Work in progress" --contours "exp-2,exp-1,exp0,exp+1,exp+2" --logy --title-right "35.9 fb^{-1} (13 TeV)" --scenario-label "M_{h}^{125}"` using the script form `CombineHarvester/CombineTools/scripts/plotLimitGrid.py`


