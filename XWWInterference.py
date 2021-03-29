from array import array
import CombineHarvester.CombineTools.plotting as plot
from HiggsAnalysis.CombinedLimit.PhysicsModel import *
import ROOT
 
 
### This is the base python class to study the Higgs width
 
class XWWInterference(PhysicsModel):
    def __init__(self):
        #self.categories=["of0j", "of1j", "of2j", "ofVBF", "ee", "mm"]
        pass
    
    def setModelBuilder(self, modelBuilder):
        PhysicsModel.setModelBuilder(self,modelBuilder)
        #self.modelBuilder.doModelBOnly = False
        #print "setModelBuilder"
        #for cat in self.categories:
        #  self.modelBuilder.doVar("r"+cat+"[1,0,10]")
        #  self.modelBuilder.factory_( "expr::s_scaling_ggH_"+cat+"(\"@0 - TMath::Sqrt(@0)\", r"+cat+")")
        #  self.modelBuilder.factory_( "expr::s_scaling_qqH_"+cat+"(\"@0 - TMath::Sqrt(@0)\", r"+cat+")")
        #  self.modelBuilder.factory_( "expr::b_scaling_ggH_"+cat+"(\"1.-TMath::Sqrt(@0)\",r"+cat+")")
        #  self.modelBuilder.factory_( "expr::b_scaling_qqH_"+cat+"(\"1.-TMath::Sqrt(@0)\",r"+cat+")")
        #  self.modelBuilder.factory_( "expr::sbi_scaling_ggH_"+cat+"(\"TMath::Sqrt(@0)\",r"+cat+")")
        #  self.modelBuilder.factory_( "expr::sbi_scaling_qqH_"+cat+"(\"TMath::Sqrt(@0)\",r"+cat+")")

    def preProcessNuisances(self,nuisances):
        if self.noInterference:
          print "No Interference: remove SBI Uncertainties"
          nuisancesToRemove = []
          for n in nuisances:
            #print "-",n[0],"---",n
            if "SBI" in n[0]:
              print "removing nuisance", n[0], "(Whole thing:)",n
              nuisancesToRemove.append(n[0])
        
          for nr in nuisancesToRemove:
            for n in nuisances:
              if n[0] == nr:
                nuisances.remove(n)
                break

    def getYieldScale(self,bin,process):
        #scaling=1.
        #postfix=""
        #self.categories.append(bin)
        #if self.manyR:
        #  for cat in self.categories:
        #    if cat in bin:
        #      postfix="_"+cat
        #      break
        #if "hwwlnuqq" in bin and self.muAsPOI == False :
        # postfix="2"
        addsemilep="2" if "qq" in bin else ""
        if "ggH_HWW" in process and "SBI" not in process:
          scaling = "S_SCALE_ggH" +addsemilep
        elif "qqH_HWW" in process and "SBI" not in process:
          scaling = "S_SCALE_qqH" +addsemilep
        elif process in ["ggWW", "ggH_hww"]: # and not ("dy" in bin or "top" in bin)
          scaling = "B_SCALE_ggH" +addsemilep
        elif process in ["qqWWqq", "qqH_hww"]: # and not ("dy" in bin or "top" in bin)
          scaling = "B_SCALE_qqH" +addsemilep
        elif "ggH_HWWSBI" in process:
          scaling = "SBI_SCALE_ggH" +addsemilep
        elif "qqH_HWWSBI" in process:
          scaling = "SBI_SCALE_qqH" +addsemilep
        else:
          scaling = 1

        print "Will scale",process,"in bin",bin,"by",scaling  
        return scaling

    def setPhysicsOptions(self,physOptions):
        self.muAsPOI = True
        self.noInterference = False
        self.NoSMXsecAdded = False
        #self.SemiLep = False
        #self.manyR = False  
        for po in physOptions:
            if po == "FloatVBFFraction":
              print "Will float the VBF fraction, the limit is on the total cross section"
              self.muAsPOI = False
            if po == "noInterference":
              print "will neglect interference"
              self.noInterference = True
            if po == "NoSMXsecAdded":
              self.NoSMXsecAdded = True
        #    if po == "SemiLep":
        #      self.SemiLep = True
        #    if po == "ManyR":
        #      self.manyR = True

           
    def doParametersOfInterest(self):
        """Create POI and other parameters, and define the POI set."""
        if self.muAsPOI:
          poi="r"
          self.modelBuilder.doVar("r[1,0,10]")

          if self.NoSMXsecAdded:
            # Adjust range to match as defined in setXsec
            self.modelBuilder.out.var("MH").setMin(17.5)
            self.modelBuilder.out.var("MH").setMax(5502.5)
            self.setXsec()

            if self.noInterference:
       
              self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0*@1\", r,xsec_ggH)")
              self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0*@1\", r,xsec_qqH)")
              self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.\", r)")
              self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.\", r)")
              self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"0\", r)")
              self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"0\", r)")

            else:

              self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0*@1 - TMath::Sqrt(@0)*@1\", r,xsec_ggH)")
              self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0*@1 - TMath::Sqrt(@0)*@1\", r,xsec_qqH)")
              self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.-TMath::Sqrt(@0)*@1\", r,xsec_ggH)")
              self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.-TMath::Sqrt(@0)*@1\", r,xsec_qqH)")
              self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"TMath::Sqrt(@0)*@1\", r,xsec_ggH)")
              self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"TMath::Sqrt(@0)*@1\", r,xsec_qqH)")

              self.modelBuilder.factory_("expr::S_SCALE_ggH2(\"@0*@1*{0} - TMath::Sqrt(@0)*@1*{0}\", r,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
              self.modelBuilder.factory_("expr::S_SCALE_qqH2(\"@0*@1*{0} - TMath::Sqrt(@0)*@1*{0}\", r,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))
              self.modelBuilder.factory_("expr::B_SCALE_ggH2(\"1.-TMath::Sqrt(@0)*@1*{0}\", r,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
              self.modelBuilder.factory_("expr::B_SCALE_qqH2(\"1.-TMath::Sqrt(@0)*@1*{0}\", r,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))
              self.modelBuilder.factory_("expr::SBI_SCALE_ggH2(\"TMath::Sqrt(@0)*@1*{0}\", r,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
              self.modelBuilder.factory_("expr::SBI_SCALE_qqH2(\"TMath::Sqrt(@0)*@1*{0}\", r,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))

              # Old: Had xsec implemented in r, but for very small r*xsec the interference is too large wrt signal (because sqrt(r)>>r)
              #self.modelBuilder.factory_("expr::rgg(\"@0*@1\", r,xsec_ggH)")
              #self.modelBuilder.factory_("expr::rqq(\"@0*@1\", r,xsec_qqH)")
              #self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0 - TMath::Sqrt(@0)\", rgg)")
              #self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0 - TMath::Sqrt(@0)\", rqq)")
              #self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.-TMath::Sqrt(@0)\", rgg)")
              #self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.-TMath::Sqrt(@0)\", rqq)")
              #self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"TMath::Sqrt(@0)\", rgg)")
              #self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"TMath::Sqrt(@0)\", rqq)")
              #
              #self.modelBuilder.factory_("expr::rgg2(\"@0*@1*{0}\", r,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
              #self.modelBuilder.factory_("expr::rqq2(\"@0*@1*{0}\", r,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))
              #self.modelBuilder.factory_("expr::S_SCALE_ggH2(\"@0 - TMath::Sqrt(@0)\", rgg2)")
              #self.modelBuilder.factory_("expr::S_SCALE_qqH2(\"@0 - TMath::Sqrt(@0)\", rqq2)")
              #self.modelBuilder.factory_("expr::B_SCALE_ggH2(\"1.-TMath::Sqrt(@0)\", rgg2)")
              #self.modelBuilder.factory_("expr::B_SCALE_qqH2(\"1.-TMath::Sqrt(@0)\", rqq2)")
              #self.modelBuilder.factory_("expr::SBI_SCALE_ggH2(\"TMath::Sqrt(@0)\", rgg2)")
              #self.modelBuilder.factory_("expr::SBI_SCALE_qqH2(\"TMath::Sqrt(@0)\", rqq2)")

          else:

            if self.noInterference:

              self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0\", r)")
              self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0\", r)")
              self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.\", r)")
              self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.\", r)")
              self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"0\", r)")
              self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"0\", r)")

            else:

              self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0 - TMath::Sqrt(@0)\", r)")
              self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0 - TMath::Sqrt(@0)\", r)")
              self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.-TMath::Sqrt(@0)\", r)")
              self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.-TMath::Sqrt(@0)\", r)")
              self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"TMath::Sqrt(@0)\", r)")
              self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"TMath::Sqrt(@0)\", r)")

        else:
          poi='sigma'
          self.modelBuilder.doVar("sigma[1,0,10]")
          self.modelBuilder.doVar("fvbf[0.5,0,1]")

          print 'MH will be assumed to be', 125
          self.modelBuilder.out.var("MH").setVal(125)
          # Adjust range to match as defined in setXsec
          self.modelBuilder.out.var("MH").setMin(17.5)
          self.modelBuilder.out.var("MH").setMax(5502.5)
          self.setXsec()

          # Here: Assuming self.NoSMXsecAdded == True! If False, would need to remove (divide) xsec again.
          # Still, need to "pseudo" add and remove xsec: Does nothing for signal, but interference.
          # See https://gitlab.cern.ch/cms-hcg/cadi/hig-17-012/-/blob/master/2l2nu/HiggsWidth.py#L94-95 and https://gitlab.cern.ch/cms-hcg/cadi/hig-17-033/blob/master/HIG-17-033/XWWInterference.py#L147-148
          if self.noInterference:

            self.modelBuilder.factory_("expr::rgg(\"(1.-@1)*@0\", sigma,fvbf)")
            self.modelBuilder.factory_("expr::rqq(\"@1*@0\", sigma,fvbf)")
            self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0\", rgg)")
            self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0\", rqq)")
            self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.\", rgg)")
            self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.\", rqq)")
            self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"0\", rgg)")
            self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"0\", rqq)")
            self.modelBuilder.out.function('rgg').Print('')
            self.modelBuilder.out.function('rqq').Print('')

          else:
            self.modelBuilder.factory_("expr::rgg(\"(1.-@1)*@0\", sigma,fvbf)")
            self.modelBuilder.factory_("expr::rqq(\"@1*@0\", sigma,fvbf)")
            self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0 - TMath::Sqrt(@0/@1)*@1\", rgg,xsec_ggH)")
            self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0 - TMath::Sqrt(@0/@1)*@1\", rqq,xsec_qqH)")
            self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.-TMath::Sqrt(@0/@1)*@1\", rgg,xsec_ggH)")
            self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.-TMath::Sqrt(@0/@1)*@1\", rqq,xsec_qqH)")
            self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"TMath::Sqrt(@0/@1)*@1\", rgg,xsec_ggH)")
            self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"TMath::Sqrt(@0/@1)*@1\", rqq,xsec_qqH)")


            self.modelBuilder.factory_("expr::S_SCALE_ggH2(\"@0*{0} - TMath::Sqrt(@0/@1)*@1*{0}\", rgg,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
            self.modelBuilder.factory_("expr::S_SCALE_qqH2(\"@0*{0} - TMath::Sqrt(@0/@1)*@1*{0}\", rqq,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))
            self.modelBuilder.factory_("expr::B_SCALE_ggH2(\"1.-TMath::Sqrt(@0/@1)*@1*{0}\", rgg,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
            self.modelBuilder.factory_("expr::B_SCALE_qqH2(\"1.-TMath::Sqrt(@0/@1)*@1*{0}\", rqq,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))
            self.modelBuilder.factory_("expr::SBI_SCALE_ggH2(\"TMath::Sqrt(@0/@1)*@1*{0}\", rgg,xsec_ggH)".format(self.BRlnuqq/self.BR2l2nu))
            self.modelBuilder.factory_("expr::SBI_SCALE_qqH2(\"TMath::Sqrt(@0/@1)*@1*{0}\", rqq,xsec_qqH)".format(self.BRlnuqq/self.BR2l2nu))

            # Old: For self.NoSMXsecAdded == False
            #self.modelBuilder.factory_("expr::rgg(\"(1.-@1)*@0/@2\", sigma,fvbf,xsec_ggH)")
            #self.modelBuilder.factory_("expr::rqq(\"@1*@0/@2\", sigma,fvbf,xsec_qqH)")
            #self.modelBuilder.factory_("expr::S_SCALE_ggH(\"@0 - TMath::Sqrt(@0)\", rgg)")
            #self.modelBuilder.factory_("expr::S_SCALE_qqH(\"@0 - TMath::Sqrt(@0)\", rqq)")
            #self.modelBuilder.factory_("expr::B_SCALE_ggH(\"1.-TMath::Sqrt(@0)\", rgg)")
            #self.modelBuilder.factory_("expr::B_SCALE_qqH(\"1.-TMath::Sqrt(@0)\", rqq)")
            #self.modelBuilder.factory_("expr::SBI_SCALE_ggH(\"TMath::Sqrt(@0)\", rgg)")
            #self.modelBuilder.factory_("expr::SBI_SCALE_qqH(\"TMath::Sqrt(@0)\", rqq)")
            #self.modelBuilder.out.function('rgg').Print('')
            #self.modelBuilder.out.function('rqq').Print('')

            #self.modelBuilder.factory_( "expr::rgg2(\"(1.-@1)*@0*%f/%f\", sigma,fvbf)" %(self.BRlnuqq, self.xsec_ggH_lnuqq))
            #self.modelBuilder.factory_( "expr::rqq2(\"@1*@0*%f/%f\", sigma,fvbf)" %(self.BRlnuqq, self.xsec_qqH_lnuqq))      
            #self.modelBuilder.factory_( "expr::s_scaling_ggH2(\"@0 - TMath::Sqrt(@0)\", rgg2)")
            #self.modelBuilder.factory_( "expr::s_scaling_qqH2(\"@0 - TMath::Sqrt(@0)\", rqq2)")
            #self.modelBuilder.factory_( "expr::b_scaling_ggH2(\"1.-TMath::Sqrt(@0)\",  rgg2)")
            #self.modelBuilder.factory_( "expr::b_scaling_qqH2(\"1.-TMath::Sqrt(@0)\", rqq2)" )
            #self.modelBuilder.factory_( "expr::sbi_scaling_ggH2(\"TMath::Sqrt(@0)\", rgg2)")
            #self.modelBuilder.factory_( "expr::sbi_scaling_qqH2(\"TMath::Sqrt(@0)\", rqq2)")



        #self.modelBuilder.out.var("MH").Print('')
        self.modelBuilder.out.function('S_SCALE_ggH').Print('')
        self.modelBuilder.out.function('S_SCALE_qqH').Print('')
        self.modelBuilder.out.function('B_SCALE_ggH').Print('')
        self.modelBuilder.out.function('B_SCALE_qqH').Print('')
        self.modelBuilder.out.function('SBI_SCALE_ggH').Print('')
        self.modelBuilder.out.function('SBI_SCALE_qqH').Print('')
        # if self.NoSMXsecAdded:
        self.modelBuilder.out.function('rgg2').Print('')
        self.modelBuilder.out.function('rqq2').Print('')
        self.modelBuilder.out.function('S_SCALE_ggH2').Print('')
        self.modelBuilder.out.function('S_SCALE_qqH2').Print('')
        self.modelBuilder.out.function('B_SCALE_ggH2').Print('')
        self.modelBuilder.out.function('B_SCALE_qqH2').Print('')
        self.modelBuilder.out.function('SBI_SCALE_ggH2').Print('')
        self.modelBuilder.out.function('SBI_SCALE_qqH2').Print('')

        self.modelBuilder.doSet("POI",poi)

        #print "How about now?:"
        #self.modelBuilder.out.var("MH").getBinning().Print()
        #self.modelBuilder.out.var("MH").setVal(3000)
        #self.modelBuilder.out.function("xsec_qqH").Print()


    def doHistFunc(self, name, hist, interpolate=0):
        varlist=self.modelBuilder.out.var("MH")
        "method to conveniently create a RooHistFunc from a TH1/TH2 input"
        print 'Doing histFunc %s...' % name
        #if self.dbg_file:
        #    self.dbg_file.WriteTObject(hist, name)
        if True:#self.mk_plots:
            canv = ROOT.TCanvas(name, name)
            pads = plot.OnePad()
            hist.GetXaxis().SetTitle(varlist.GetTitle())
            #hist.GetYaxis().SetTitle(varlist[1].GetTitle())
            hist.Draw('COLZ')
            plot.DrawTitle(pads[0], name, 3)
            #canv.Print('model_'+name+'.pdf')
            canv.Print('model_'+name+'.png')
            pads[0].SetLogy(True)
            #canv.Print('model_'+name+'_log.pdf')
            canv.Print('model_'+name+'_log.png')
            hist.SaveAs('root_'+name+'.root')
        dh = ROOT.RooDataHist('dh_%s'%name, 'dh_%s'%name, ROOT.RooArgList(varlist), ROOT.RooFit.Import(hist))
        hfunc = ROOT.RooHistFunc(name, name, ROOT.RooArgSet(varlist), dh)
        hfunc.setInterpolationOrder(interpolate)
        self.modelBuilder.out._import(hfunc, ROOT.RooFit.RecycleConflictNodes())
        return self.modelBuilder.out.function(name)

    def setXsec(self):
      #masses = ['200','210','230','250','270','300','350','400','450','500','550','650','700','750','800','900','1000','1500','2000','2500','3000' ]  
      #xsec_ggH = [1.4056999, 1.2602001, 1.0583491, 0.9183804,0.8139251,0.7125448,0.7252267,0.5753961,0.3880926,0.257722169161,0.17240922153,0.0800471380353,0.0556466244161,0.0390892401338,0.0278153866529,0.0145896067843,0.00794733408839,0.000959999975748,0.000410000007832,0.000159999995958,7.7999997302e-05]
      #xsec_qqH = [0.177031323314,0.161820292473,0.139826774597,0.122818663716,0.108498997986,0.0911082476377,0.0683906450868,0.0458333604038,0.0346079431474,0.0276690702885,0.0227834656835,0.0160982366651,0.013708290644,0.0116998897865,0.0100459931418,0.00748184928671,0.0056282337755,0.00289999996312,0.00230000005104,0.00150000001304,0.000920000020415]

      # Used for 2016
      #gghxseclist = [0.31279999017715454, 0.7674585580825806, 1.0240892171859741, 1.0913430452346802, 1.15627920627594,
      #               1.4364516735076904, 1.5966429710388184, 1.859334945678711, 2.078367233276367, 2.2598729133605957,
      #               2.4666330814361572, 2.5944933891296387, 2.5601279735565186, 2.4353036880493164, 2.285888671875,
      #               2.0814220905303955, 1.558440089225769, 1.3141587972640991, 1.150452971458435, 0.9180770516395569,
      #               0.7505993843078613, 0.6217686533927917, 0.47802814841270447, 0.3169771432876587, 0.13182885944843292,
      #               0.09705755859613419, 0.07434001564979553, 0.05831994116306305, 0.046382345259189606, 0.03739501163363457,
      #               0.024867240339517593, 0.01702437736093998, 0.01189199648797512, 0.0023783992416, 0.0006187705344,
      #               0.00018627571296, 6.058794816e-05]
      #vbfxseclist = [0.03689999878406525, 0.057831279933452606, 0.07923336327075958, 0.08496211469173431, 0.09059008955955505,
      #               0.1153029054403305, 0.14933550357818604, 0.1803554892539978, 0.20858122408390045, 0.23393188416957855,
      #               0.2632441222667694, 0.2853279113769531, 0.2897418439388275, 0.28325948119163513, 0.2729797661304474,
      #               0.25500351190567017, 0.20055875182151794, 0.17703132331371307, 0.16182029247283936, 0.13982677459716797,
      #               0.10849899798631668, 0.09110824763774872, 0.06839064508676529, 0.03460794314742088, 0.027669070288538933,
      #               0.022783465683460236, 0.019074873998761177, 0.01609823666512966, 0.01370829064399004, 0.010045993141829967,
      #               0.0074818492867052555, 0.005628233775496483, 0.00147473644032, 0.000454538521728, 0.00015211442304,
      #               5.31949293792e-05]

      #N3LO
      #gghxseclist = [0.46321814736, 0.7674585408, 1.02408916896, 1.09134309312, 1.1562791472,
      #               1.43645169312, 1.5966429696, 1.859334912, 2.078367336, 2.25987293952,
      #               2.46663319508, 2.59449338736, 2.5601278658, 2.43530358192, 2.28588856256,
      #               2.0814221376, 1.55844010368, 1.31415885216, 1.1504529792, 0.91807705584,
      #               0.7505993952, 0.62176864896, 0.47802816144, 0.31697713152, 0.19107311616,
      #               0.1318288608, 0.097057555344, 0.074340014112, 0.05831994168, 0.0463823438976,
      #               0.0373950126144, 0.0303586183008, 0.02486723976, 0.017024377824, 0.011891996208,
      #               0.0023783992416, 0.0006187705344, 0.00018627571296, 6.058794816e-05, 7.347900096e-06,
      #               9.02373696e-07

      #gmasses = [115, 120, 124, 125, 126,
      #           130, 135, 140, 145, 150,
      #           155, 160, 165, 170, 175,
      #           180, 190, 200, 210, 230,
      #           250, 270, 300, 350, 400,
      #           450, 500, 550, 600, 650,
      #           700, 750, 800, 900, 1000,
      #           1500, 2000, 2500, 3000] # 4000 and 5000 need to be fixed still
      #vmasses = [115, 120, 124, 125, 126,
      #           130, 135, 140, 145, 150,
      #           155, 160, 165, 170, 175,
      #           180, 190, 200, 210, 230,
      #           250, 270, 300, 350, 400,
      #           450, 500, 550, 600, 650,
      #           700, 750, 800, 900, 1000,
      #           1500, 2000, 2500, 3000, 4000,
      #           5000]

      #gghxseclist = [0.424747487664, 0.7674585408, 1.02408916896, 1.09134309312, 1.1562791472,
      #               1.43645169312, 1.50570016128, 1.77050002176, 1.99709386704, 2.18764945152,
      #               2.40793120025, 2.55657605616, 2.54655698873, 2.44541277072, 2.31794561765,
      #               2.1321885312, 1.63077276672, 1.40570002368, 1.2602001384, 1.05834913632,
      #               0.91838043648, 0.81392511744, 0.712544860368, 0.725226696, 0.575396130816,
      #               0.388092702816, 0.257722168608, 0.172409223168, 0.11687293008, 0.080047139328,
      #               0.055646622864, 0.0390892412736, 0.027815385744, 0.0145896064704, 0.0079473340512,
      #               0.000574489768032, 6.9869506176e-05, 1.14601459392e-05, 2.25722334528e-06]
      #vbfxseclist = [0.037118516328, 0.0578312784, 0.079233365376, 0.084962115648, 0.09059008896,
      #               0.115302909024, 0.149335498368, 0.180355486464, 0.208581223392, 0.233931877632,
      #               0.2632441239, 0.28532791728, 0.289741853918, 0.283259470176, 0.27297976375,
      #               0.25500350016, 0.20055874752, 0.177031316448, 0.161820294048, 0.139826772288,
      #               0.122818665744, 0.10849899456, 0.091108250496, 0.0683906462784, 0.045833361408,
      #               0.0346079418048, 0.0276690701952, 0.02278346616, 0.019074874032, 0.0160982375616,
      #               0.01370829096, 0.011699890128, 0.010045993248, 0.007481849472, 0.00562823365248,
      #               0.00147473644032, 0.000454538521728, 0.00015211442304, 5.31949293792e-05, 6.7742482464e-06,
      #               8.4823127424e-07]

      masses = [20, 25, 30, 35, 40,
                45, 50, 55, 60, 65,
                70, 75, 80, 85, 90,
                95, 100, 105, 110, 115,
                120, 125, 130, 135, 140,
                145, 150, 155, 160, 165,
                170, 175, 180, 190, 200,
                210, 220, 230, 240, 250,
                260, 270, 280, 290, 300,
                310, 320, 330, 340, 350,
                360, 370, 380, 390, 400,
                410, 420, 430, 440, 450,
                460, 470, 480, 490, 500,
                550, 600, 650, 700, 750,
                800, 850, 900, 950, 1000,
                1050, 1100, 1150, 1200, 1250,
                1300, 1350, 1400, 1450, 1500,
                1550, 1600, 1650, 1700, 1750,
                1800, 1850, 1900, 1950, 2000,
                2050, 2100, 2150, 2200, 2250,
                2300, 2350, 2400, 2450, 2500,
                2550, 2600, 2650, 2700, 2750,
                2800, 2850, 2900, 2950, 3000]

      # Values are XS(ggH/VBF)*BR(H->WW)*BR(2L2Nu)
      gghxseclist = [1.2716635176e-05, 2.7789876576000003e-05, 5.41337927328e-05, 9.8690457024e-05, 0.00017204044248,
                     0.000287922609072, 0.00046715789664, 0.0007397028863999999, 0.00114266376, 0.0017455829184000002,
                     0.0026205788735999998, 0.0039260604096, 0.0058780681344, 0.009063711820799998, 0.01576109664,
                     0.03231240012, 0.06937968816, 0.13930735104, 0.254939254848, 0.42474748766399995,
                     0.7674585408000001, 1.0913430931199999, 1.43645169312, 1.5057001612800003, 1.7705000217600002,
                     1.9970938670399996, 2.1876494515199996, 2.407931200245026, 2.55657605616, 2.5465569887323936,
                     2.44541277072, 2.317945617650813, 2.1321885311999997, 1.63077276672, 1.40570002368,
                     1.2602001384, 1.15041203856, 1.05834913632, 0.9836503142399999, 0.9183804364799999,
                     0.8629279142399998, 0.81392511744, 0.77370251328, 0.7405783862399998, 0.712544860368,
                     0.6923912529599998, 0.6783229992959999, 0.6742359686880001, 0.6846413997600002, 0.725226696,
                     0.7230998822400001, 0.69697345536, 0.660508992, 0.61830864, 0.5753961308159999,
                     0.534264014592, 0.49380552936000005, 0.455880219984, 0.420769947024, 0.388092702816,
                     0.357174331488, 0.32931978969600006, 0.30332615745600006, 0.27964388678400004, 0.257722168608,
                     0.172409223168, 0.11687293007999999, 0.08004713932799999, 0.05564662286399999, 0.0390892412736,
                     0.027815385744000002, 0.020023279704000002, 0.0145896064704, 0.01070833932, 0.0079473340512,
                     0.005941002248707944, 0.004472884675915511, 0.0033889856156445377, 0.00258415320591698, 0.001983119307226211,
                     0.0015295911243778508, 0.0011874408216328454, 0.0009270175520040614, 0.0007274654922012501, 0.000574489768032,
                     0.0004561446016102703, 0.0003641842951587896, 0.0002920322930728838, 0.00023521641016418797, 0.00019049072167127482,
                     0.00015478124143351258, 0.00012626079605748634, 0.00010337479610225888, 8.483425959540811e-05, 6.986950617599999e-05,
                     5.7733222805896574e-05, 4.7829443423729876e-05, 3.963701080737855e-05, 3.295452341650291e-05, 2.7527105632826273e-05,
                     2.3021194983475585e-05, 1.9270159571462655e-05, 1.6177787400645236e-05, 1.3602394305291812e-05, 1.1460145939200002e-05,
                     9.66061620711293e-06, 8.196982957651012e-06, 6.940053007761552e-06, 5.87737826376299e-06, 5.007530876332102e-06,
                     4.260184024393003e-06, 3.6320606736285847e-06, 3.0992865078737716e-06, 2.650246812033816e-06, 2.25722334528e-06]

      vbfxseclist = [6.25158324e-08, 2.36892935664e-07, 7.08700744224e-07, 1.80172198368e-06, 4.07428547184e-06,
                     8.384398477919999e-06, 1.60931042352e-05, 2.9281753497599997e-05, 5.087976768e-05, 8.5951409472e-05,
                     0.000140889654288, 0.00022798435737599997, 0.000365661641088, 0.00060002601984, 0.0011039276160000001,
                     0.0023833058198399998, 0.0053656592832, 0.011259053913600001, 0.0214578712224, 0.037118516328,
                     0.057831278400000005, 0.08496211564799999, 0.11530290902399999, 0.149335498368, 0.18035548646400001,
                     0.208581223392, 0.23393187763199996, 0.26324412390018803, 0.28532791727999995, 0.2897418539183547,
                     0.28325947017599995, 0.2729797637498278, 0.25500350016000006, 0.20055874752, 0.17703131644800002,
                     0.16182029404799997, 0.15014486332799998, 0.139826772288, 0.13095630028799998, 0.12281866574400001,
                     0.115423946352, 0.10849899456, 0.10221324163199999, 0.096464335968, 0.09110825049599999,
                     0.08619579359999999, 0.08146809446399998, 0.077238926352, 0.07313100552, 0.06839064627840001,
                     0.0625416774912, 0.057329997004799994, 0.052948687176, 0.049136987620799995, 0.045833361408,
                     0.043063727111999996, 0.040543106385600006, 0.038325750825600004, 0.0363772073088, 0.03460794180480001,
                     0.0329357371104, 0.0314811056736, 0.030105447681600006, 0.0288560233296, 0.027669070195200002,
                     0.022783466160000005, 0.019074874032000002, 0.016098237561599997, 0.013708290960000001, 0.011699890128,
                     0.010045993248, 0.008648552736, 0.007481849471999999, 0.0064761478992, 0.0056282336524799996,
                     0.004897970081885979, 0.004265782560752271, 0.003718551904090618, 0.0032448816153126305, 0.0028344254572032525,
                     0.0024789925119227236, 0.0021715616338723208, 0.001905212173615615, 0.0016745919678380463, 0.00147473644032,
                     0.0013017076161060473, 0.0011511720307613698, 0.0010201216044868882, 0.0009056666803154152, 0.0008050080076023856,
                     0.0007166988546443792, 0.0006384435362540038, 0.000569591264451363, 0.0005086190689182554, 0.00045453852172800004,
                     0.0004064475632140803, 0.000363671479464584, 0.00032553443446294304, 0.0002916101132392192, 0.0002613365477351283,
                     0.0002343401551554127, 0.0002101788823406854, 0.000188601148453118, 0.00016935303241706913, 0.00015211442304,
                     0.00013675668840222572, 0.00012295474436476515, 0.00011064132789532576, 9.95565115394564e-05, 8.963422273810261e-05,
                     8.067993342217577e-05, 7.269276862762676e-05, 6.547879168225727e-05, 5.901181860647282e-05, 5.3194929379199995e-05]

      self.BR2l2nu = (3*.108)*(3*.108)
      self.BRlnuqq = (3*.108)*.676*2
      masses += [3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000, 5100, 5200, 5300, 5400, 5500]
      # NOTE: Above 1TeV all BR(H->WW) are 0.614! No widths are available to calculate BR beyond 1TeV
      gghxseclist += [2.67562e-05*self.BR2l2nu*0.614, 1.96927e-05*self.BR2l2nu*0.614, 1.4553e-05*self.BR2l2nu*0.614, 1.07944e-05*self.BR2l2nu*0.614, 8.03565e-06*self.BR2l2nu*0.614, 6.00075e-06*self.BR2l2nu*0.614, 4.49424e-06*self.BR2l2nu*0.614, 3.37508e-06*self.BR2l2nu*0.614, 2.54085e-06*self.BR2l2nu*0.614, 1.91717e-06*self.BR2l2nu*0.614, 1.44961e-06*self.BR2l2nu*0.614, 1.09811e-06*self.BR2l2nu*0.614, 8.33307e-07*self.BR2l2nu*0.614, 6.33285e-07*self.BR2l2nu*0.614, 4.81973e-07*self.BR2l2nu*0.614, 3.67255e-07*self.BR2l2nu*0.614, 2.80135e-07*self.BR2l2nu*0.614, 2.13883e-07*self.BR2l2nu*0.614, 1.63444e-07*self.BR2l2nu*0.614, 1.24976e-07*self.BR2l2nu*0.614, 9.56175e-08*self.BR2l2nu*0.614, 7.31853e-08*self.BR2l2nu*0.614, 5.60359e-08*self.BR2l2nu*0.614, 4.29138e-08*self.BR2l2nu*0.614, 3.28674e-08*self.BR2l2nu*0.614] #TODO, Remember that ggH xsecs above 3000GeV are temporary until new official number are out!
      vbfxseclist += [6.684155e-04*self.BR2l2nu*0.614, 5.437613e-04*self.BR2l2nu*0.614, 4.424411e-04*self.BR2l2nu*0.614, 3.601540e-04*self.BR2l2nu*0.614, 2.933644e-04*self.BR2l2nu*0.614, 2.389263e-04*self.BR2l2nu*0.614, 1.945763e-04*self.BR2l2nu*0.614, 1.585186e-04*self.BR2l2nu*0.614, 1.290556e-04*self.BR2l2nu*0.614, 1.051662e-04*self.BR2l2nu*0.614, 8.559694e-05*self.BR2l2nu*0.614, 6.968611e-05*self.BR2l2nu*0.614, 5.666955e-05*self.BR2l2nu*0.614, 4.608781e-05*self.BR2l2nu*0.614, 3.747052e-05*self.BR2l2nu*0.614, 3.042152e-05*self.BR2l2nu*0.614, 2.469815e-05*self.BR2l2nu*0.614, 2.004009e-05*self.BR2l2nu*0.614, 1.624521e-05*self.BR2l2nu*0.614, 1.315317e-05*self.BR2l2nu*0.614, 1.064451e-05*self.BR2l2nu*0.614, 8.607313e-06*self.BR2l2nu*0.614, 6.951409e-06*self.BR2l2nu*0.614, 5.607474e-06*self.BR2l2nu*0.614, 4.517709e-06*self.BR2l2nu*0.614]

      #if semilep:
      #  for b,x in enumerate(gghxseclist):
      #    gghxseclist[b] = x*(self.BRlnuqq/self.BR2l2nu)
      #  for b,x in enumerate(vbfxseclist):
      #    vbfxseclist[b] = x*(self.BRlnuqq/self.BR2l2nu)

      gghxsec_graph = ROOT.TGraph(len(masses), array('d', masses), array('d', gghxseclist))
      vbfxsec_graph = ROOT.TGraph(len(masses), array('d', masses), array('d', vbfxseclist))

      topmass = 5500.0
      doingmass = 20.0
      stepmass = 5.0
      steplist = [doingmass-stepmass/2]
      newgghxseclist = []
      newvbfxseclist = []
      while doingmass < topmass+stepmass:
        steplist.append(doingmass+stepmass/2)
        newgghxseclist.append(gghxsec_graph.Eval(doingmass,0,""))
        newvbfxseclist.append(vbfxsec_graph.Eval(doingmass,0,""))
        doingmass += stepmass
        if doingmass==125.0:
          steplist.append(124.5)
          newgghxseclist.append(gghxsec_graph.Eval(124.0,0,""))
          newvbfxseclist.append(vbfxsec_graph.Eval(124.0,0,""))
          steplist.append(125.5)
          newgghxseclist.append(gghxsec_graph.Eval(125.0,0,""))
          newvbfxseclist.append(vbfxsec_graph.Eval(125.0,0,""))
          steplist.append(127.5)
          newgghxseclist.append(gghxsec_graph.Eval(126.0,0,""))
          newvbfxseclist.append(vbfxsec_graph.Eval(126.0,0,""))
          doingmass = 130.0

      gghxsec = ROOT.TH1F("gghxsec", "gghxsec", len(steplist)-1, array('d', steplist))
      for b,x in enumerate(newgghxseclist):
        # if fVBF and SM:    xsec=xsec -> Remove xsec from shapes
        # if fVBF and noSM:  xsec=1    -> xsec won't have to be removed if it's already not included
        # if SMVBF and SM:   xsec=1    -> Don't add xsec again
        # if SMVBF and noSM: xsec=xsec -> Add xsec now if it wasn't before
        # ==> Set xsec=1 if SMVBF XOR noSM
        #if self.NoSMXsecAdded != self.muAsPOI: x=1.0    # <-- Old generallization
        gghxsec.SetBinContent(b+1,x)
      vbfxsec = ROOT.TH1F("vbfxsec", "vbfxsec", len(steplist)-1, array('d', steplist))
      for b,x in enumerate(newvbfxseclist):
        #if self.NoSMXsecAdded != self.muAsPOI: x=1.0    # <-- Old generallization
        vbfxsec.SetBinContent(b+1,x)

      self.doHistFunc('xsec_ggH', gghxsec)
      self.doHistFunc('xsec_qqH', vbfxsec)

      #for index in range(len(masses)):
      #if self.mass == masses[index]:
      #self.xsec_ggH_2l2nu = xsec_ggH[index]
      #self.xsec_qqH_2l2nu = xsec_qqH[index]
      #if semilep:
      #  self.xsec_ggH_lnuqq = xsec_ggH[index]*(self.BRlnuqq/self.BR2l2nu)
      #  self.xsec_qqH_lnuqq = xsec_qqH[index]*(self.BRlnuqq/self.BR2l2nu)

      return
 
XWW = XWWInterference()

