#ifndef AliAnalysisTaskCheckV0_cxx
#define AliAnalysisTaskCheckV0_cxx

/*  See cxx source for full Copyright notice */

//-----------------------------------------------------------------
//                 AliAnalysisTaskCheckV0 class
//            This task is for QAing the V0s from ESD/AOD
//              Origin: B.H. Nov2007, hippolyt@in2p3.fr
//-----------------------------------------------------------------

class TString;
class TList;
class TH1F;
class AliESDEvent;
class AliAODEvent;

#include "AliAnalysisTaskSE.h"

class AliAnalysisTaskCheckV0 : public AliAnalysisTaskSE {
 public:
  AliAnalysisTaskCheckV0();
  AliAnalysisTaskCheckV0(const char *name);
  virtual ~AliAnalysisTaskCheckV0() {}
  
  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);

  void   SetCollidingSystems(Int_t collidingSystems = 0) {fCollidingSystems = collidingSystems;}
  void   SetAnalysisType(const char* analysisType) {fAnalysisType = analysisType;}
  
 private:
  AliESDEvent *fESD;                            //! ESD object
  AliAODEvent *fAOD;                            //! AOD object
  TString      fAnalysisType;                   //  ESD or AOD
  Int_t        fCollidingSystems;               //  Colliding systems 0/1 for pp/PbPb  
  TList       *fListHist;                       //! List of histograms
  TH1F        *fHistPrimaryVertexPosX;          //! Primary vertex position in X
  TH1F        *fHistPrimaryVertexPosY;          //! Primary vertex position in Y
  TH1F        *fHistPrimaryVertexPosZ;          //! Primary vertex position in Z
  TH1F        *fHistTrackMultiplicity;          //! Track multiplicity distribution
  TH1F        *fHistV0Multiplicity;             //! V0 multiplicity distribution
  TH1F        *fHistV0OnFlyStatus;              //! V0 on fly status distribution

              // V0 offline distributions
  TH1F        *fHistV0MultiplicityOff;          //! V0 multiplicity distribution offline
  TH1F        *fHistV0Chi2Off;                  //! V0 chi2 distribution
  TH1F        *fHistDcaV0DaughtersOff;          //! Dca between V0 daughters
  TH1F        *fHistV0CosineOfPointingAngleOff; //! Cosine of V0 pointing angle
  TH1F        *fHistV0RadiusOff;                //! V0 radial distance distribution
  TH1F        *fHistDcaV0ToPrimVertexOff;       //! Dca of V0 to primary vertex
  TH1F        *fHistDcaPosToPrimVertexOff;      //! Dca of V0 positive daughter to primary vertex
  TH1F        *fHistDcaNegToPrimVertexOff;      //! Dca of V0 negative daughter to primary vertex

  TH1F        *fHistMassK0Off;                  //! Invariant Mass of K0s
  TH1F        *fHistMassLambdaOff;              //! Invariant Mass of Lambda
  TH1F        *fHistMassAntiLambdaOff;          //! Invariant Mass of Anti-Lambda

              // V0 on-the-fly distributions
  TH1F        *fHistV0MultiplicityOn;           //! V0 multiplicity distribution offline
  TH1F        *fHistV0Chi2On;                   //! V0 chi2 distribution
  TH1F        *fHistDcaV0DaughtersOn;           //! Dca between V0 daughters
  TH1F        *fHistV0CosineOfPointingAngleOn;  //! Cosine of V0 pointing angle
  TH1F        *fHistV0RadiusOn;                 //! V0 radial distance distribution
  TH1F        *fHistDcaV0ToPrimVertexOn;        //! Dca of V0 to primary vertex
  TH1F        *fHistDcaPosToPrimVertexOn;       //! Dca of V0 positive daughter to primary vertex
  TH1F        *fHistDcaNegToPrimVertexOn;       //! Dca of V0 negative daughter to primary vertex

  TH1F        *fHistMassK0On;                   //! Invariant Mass of K0s
  TH1F        *fHistMassLambdaOn;               //! Invariant Mass of Lambda
  TH1F        *fHistMassAntiLambdaOn;           //! Invariant Mass of Anti-Lambda
   
  AliAnalysisTaskCheckV0(const AliAnalysisTaskCheckV0&);            // not implemented
  AliAnalysisTaskCheckV0& operator=(const AliAnalysisTaskCheckV0&); // not implemented
  
  ClassDef(AliAnalysisTaskCheckV0, 0);
};

#endif
