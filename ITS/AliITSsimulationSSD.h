#ifndef ALIITSSIMULATIONSSD_H
#define ALIITSSIMULATIONSSD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id$ */

#include "AliITSsimulation.h"
#include "AliITSsegmentationSSD.h" // function used in inline functions

class AliITSMapA2;
class AliITSpList;
class AliITSTableSSD;
class AliITSdcsSSD;
class AliITSsegmentationSSD;
class AliITSresponseSSD;

class AliITSsimulationSSD: public AliITSsimulation {

 public:
    AliITSsimulationSSD(); // Default constructor
    AliITSsimulationSSD(const AliITSsimulationSSD &source); // copy constructor
    // operator =
    AliITSsimulationSSD& operator=(const AliITSsimulationSSD &source);
    //Standard Constructor
    AliITSsimulationSSD(AliITSsegmentation *seg,AliITSresponse *resp);
    //Destructor
    virtual ~AliITSsimulationSSD();
    // Initilize variables for this simulation
    void Init(AliITSsegmentationSSD *seg,AliITSresponseSSD *resp);
    //Digitizes all of the hits in a module
    void DigitiseModule(AliITSmodule *mod,Int_t dummy0,Int_t dummy1);
    // Computes the Summable Digits
    void SDigitiseModule(AliITSmodule *mod,Int_t module,Int_t dummy);
    // Computes the Charge on each Strip/ Analog/summable digits
    void HitsToAnalogDigits(AliITSmodule *mod,AliITSpList *pList);
    //Computes the signal from one hit
    void HitToDigit(Int_t module,Double_t x0,Double_t y0,Double_t z0, 
		    Double_t x,Double_t y,Double_t z,Double_t de,
		    AliITSTableSSD *tav);
    //returns a pointer to the SSD segmentation.
    AliITSsegmentationSSD *GetSegmentation() {
	return (AliITSsegmentationSSD*) fSegmentation;}
    //Returns the ionization energy for Si in GeV.
    Double_t GetIonizeE() const {return fIonE;}
    //Sets the ionization energy for Si in GeV.
    void SetIonizeE(Double_t e=3.62E-09){fIonE = e;}
    //Returns the Diffusion constant h in cm**2/sec
    Double_t GetDiffConst(Int_t i) const {return fDifConst[i];}
    //Sets the Diffusion constant h in cm**2/sec
    void SetDiffConst(Double_t h=11.0,Double_t e=30.0)
	{fDifConst[0] = h;fDifConst[1]=e;}
    //Returns the Drift velocity for the side i
    Double_t GetDriftVelocity(Int_t i) const {return fDriftVel[i];}
    //Sets the Drift velocity for the P and N sides
    void SetDriftVelocity(Double_t v0=0.86E+06,Double_t v1=2.28E+06)
	{fDriftVel[0] = v0;fDriftVel[1] = v1;}
    // Standard ascii class print function
    void Print(ostream *os);
    // Standard ascii class read function
    void Read(istream *is);

 private:
    // returns the number of steps needed to proplerly distribute the charge
    // in a step
    Int_t NumOfSteps(Double_t x,Double_t y,Double_t z,
		     Double_t  &dex,Double_t &dey,Double_t &dez);
    // Keepts track and orders tracks for a give strip.
    void GetList(Int_t trk,Int_t ht,Int_t mod,AliITSpList *pLt,
		 AliITSTableSSD *tav);
    // sets thresholds and fills digits
    void ChargeToSignal(AliITSpList *pList);
    // Writes Summable Digits to a root file for later use.
    void WriteSDigits(AliITSpList *pList);
    // ReadSDigits and create Digits
    void SDigitToDigit(Int_t module,AliITSpList *pList);
    // Fills fMapA2 from pList AliITSpList
    void FillMapFrompList(AliITSpList *pList);
    // Diffuses the charge onto neighboring strips.
    void    IntegrateGaussian(Int_t k,Double_t par,Double_t av,Double_t sigma, 
			      Double_t inf, Double_t sup,
			      AliITSTableSSD *tav);
     // Applies noise to strips randomly
    void    ApplyNoise(AliITSpList *pList,Int_t mod);
     // Applies posible signal coupling between strips
    void    ApplyCoupling(AliITSpList *pList,Int_t mod);
    // Computes the integral of a gaussian using Error Function
    Float_t F(Float_t av, Float_t x, Float_t s);
    // returns, from the segmentation, the number of stips
    Int_t GetNStrips() {return GetSegmentation()->Npx();}
    // returns, from the segmentation, the strip pitch
    Float_t GetStripPitch() {return GetSegmentation()->Dpx(0);}

    // Data members
 protected:
    AliITSdcsSSD *fDCS;   // Class containing detector controle paramters

 private:
    AliITSMapA2 *fMapA2;      //! Map of ionization, used localy only
    Double_t    fIonE;        // ionization energy of Si in GeV
    Double_t    fDifConst[2]; // Diffusion constants [h,e] in cm**2/sec
    Double_t    fDriftVel[2]; // Drift velocities [P,N sides] cm/sec

    ClassDef(AliITSsimulationSSD,2) // SSD signal simulation class

};
// Input and output functions for standard C++ input/output.
ostream &operator<<(ostream &os,AliITSsimulationSSD &source);
istream &operator>>(istream &is,AliITSsimulationSSD &source);
#endif
