// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

///  @file   AliHLTTPCFastdEdxComponent.cxx
///  @author David Rohr <drohr@cern.ch>
///  @date   May 2017
///  @brief  dEdx calculation component for the HLT TPC


#include "AliHLTTPCFastdEdxComponent.h"
#include "AliCDBEntry.h"
#include "AliCDBManager.h"
#include "AliHLTDataTypes.h"
#include "AliHLTExternalTrackParam.h"
#include "AliHLTGlobalBarrelTrack.h"
#include "AliHLTTPCGeometry.h"
#include "AliHLTTPCRawCluster.h"
#include "AliHLTTPCDefinitions.h"
#include "AliTPCcalibDB.h"
#include "AliTPCRecoParam.h"
#include "AliHLTTPCdEdxData.h"
#include "AliHLTTPCRawCluster.h"
#include <algorithm>

ClassImp( AliHLTTPCFastdEdxComponent )
AliHLTTPCFastdEdxComponent::AliHLTTPCFastdEdxComponent() : AliHLTProcessor(), fBz(0.), fBufMax(NULL), fBufTot(NULL), fMaxClusterCount(1000)
{
  for (int i = 0;i < 36 * 6;i++)
  {
    fNPatchClusters[0][i] = 0;    
    fPatchClusters[0][i] = NULL;
  }
}

AliHLTTPCFastdEdxComponent::AliHLTTPCFastdEdxComponent( const AliHLTTPCFastdEdxComponent& ) : AliHLTProcessor(), fBz(0.), fBufMax(NULL), fBufTot(NULL), fMaxClusterCount(1000)
{
  for (int i = 0;i < 36 * 6;i++)
  {
    fPatchClusters[0][i] = 0;    
    fPatchClusters[0][i] = NULL;
  }
}

AliHLTTPCFastdEdxComponent& AliHLTTPCFastdEdxComponent::operator=( const AliHLTTPCFastdEdxComponent& )
{
  fBz = 0;
  fBufMax = NULL;
  fBufTot = NULL;
  fMaxClusterCount = 1000;
  for (int i = 0; i < 36*6;i++)
  {
    fPatchClusters[0][i] = 0;    
    fPatchClusters[0][i] = NULL;
  }
  return *this;
}

AliHLTTPCFastdEdxComponent::~AliHLTTPCFastdEdxComponent()
{
  DoDeinit();
}

const char* AliHLTTPCFastdEdxComponent::GetComponentID()
{
  return "FastTPCdEdx";
}

void AliHLTTPCFastdEdxComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list )
{
  list.clear();
  list.push_back( kAliHLTDataTypeTrack|kAliHLTDataOriginTPC );
  list.push_back( AliHLTTPCDefinitions::RawClustersDataType() );
}

AliHLTComponentDataType AliHLTTPCFastdEdxComponent::GetOutputDataType()
{
  return AliHLTTPCDefinitions::TPCdEdxNew();
}

void AliHLTTPCFastdEdxComponent::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  constBase = 200;
  inputMultiplier = 0.1;
}

AliHLTComponent* AliHLTTPCFastdEdxComponent::Spawn()
{
  return new AliHLTTPCFastdEdxComponent;
}

void AliHLTTPCFastdEdxComponent::SetDefaultConfiguration()
{
}

int AliHLTTPCFastdEdxComponent::ReadConfigurationString(  const char* arguments )
{
  int iResult = 0;
  if ( !arguments ) return iResult;

  TString allArgs = arguments;
  TString argument;
  int bMissingParam = 0;

  TObjArray* pTokens = allArgs.Tokenize( " " );

  int nArgs =  pTokens ? pTokens->GetEntries() : 0;

  for ( int i = 0; i < nArgs; i++ ) {
    argument = ( ( TObjString* )pTokens->At( i ) )->GetString();
    if ( argument.IsNull() ) continue;

    HLTError( "Unknown option \"%s\"", argument.Data() );
    iResult = -EINVAL;
  }
  delete pTokens;

  if ( bMissingParam ) {
    HLTError( "Specifier missed for parameter \"%s\"", argument.Data() );
    iResult = -EINVAL;
  }

  return iResult;
}

int AliHLTTPCFastdEdxComponent::Configure( const char* cdbEntry, const char* chainId, const char *commandLine )
{
  SetDefaultConfiguration();

  int iResult = 0;
  if ( commandLine && commandLine[0] != 0 )
  {
    HLTInfo( "received configuration string from HLT framework: \"%s\"", commandLine );
    iResult = ReadConfigurationString( commandLine );
  }

  return iResult;
}



int AliHLTTPCFastdEdxComponent::DoInit( int argc, const char** argv )
{
  // Configure the component

  TString arguments = "";
  for ( int i = 0; i < argc; i++ ) {
    if ( !arguments.IsNull() ) arguments += " ";
    arguments += argv[i];
  }

  int ret = Configure( NULL, NULL, arguments.Data() );
  
  fBz = GetBz();
  fBufMax = new float[fMaxClusterCount];
  fBufTot = new float[fMaxClusterCount];
  if (fBufMax == NULL || fBufTot == NULL)
  {
    HLTError("Memory allocation failed");
    return(-ENOSPC);
  }

  return ret;
}


int AliHLTTPCFastdEdxComponent::DoDeinit()
{
  return 0;
  delete[] fBufMax;
  delete[] fBufTot;
  fBufMax = fBufTot = NULL;
}



int AliHLTTPCFastdEdxComponent::Reconfigure( const char* cdbEntry, const char* chainId )
{
  return Configure( cdbEntry, chainId, NULL );
}



int AliHLTTPCFastdEdxComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size, vector<AliHLTComponentBlockData>& outputBlocks)
{
  AliHLTUInt32_t maxBufferSize = size;
  size = 0; // output size

  if (!IsDataEvent()) return 0;

  for(int i = 0;i < 36 * 6;i++)
  {
    fNPatchClusters[0][i] = 0;
  }

  int nBlocks = (int) evtData.fBlockCnt;

  int nInputClusters = 0;
  int nInputTracks = 0;

  // first read all raw clusters

  for (int ndx = 0;ndx < nBlocks;ndx++)
  {
    const AliHLTComponentBlockData* iter = blocks+ndx;
    if (iter->fDataType != AliHLTTPCDefinitions::RawClustersDataType()) continue;

    Int_t slice=AliHLTTPCDefinitions::GetMinSliceNr(iter->fSpecification);
    Int_t patch=AliHLTTPCDefinitions::GetMinPatchNr(iter->fSpecification);

    fPatchClusters[slice][patch] = (AliHLTTPCRawClusterData*) iter->fPtr;
    nInputClusters += fPatchClusters[slice][patch]->fCount;
    fNPatchClusters[slice][patch] = fPatchClusters[slice][patch]->fCount;
  }

  // loop over the input tracks: calculate dEdx and write output
  for (const AliHLTComponentBlockData* pBlock = GetFirstInputBlock(kAliHLTDataTypeTrack|kAliHLTDataOriginTPC);pBlock != NULL;pBlock=GetNextInputBlock())
  {
    AliHLTTracksData* dataPtr = (AliHLTTracksData*) pBlock->fPtr;
    int nTracks = dataPtr->fCount;

    const int outSize = sizeof(AliHLTTPCdEdxData) + nTracks * 10 * sizeof(float);
    AliHLTTPCdEdxData* outPtr = (AliHLTTPCdEdxData*) (outputPtr + size);
    if (size + outSize > maxBufferSize) //Check output size for current fValuesPerTrack = 10 (see below)
    {
	HLTWarning("Output buffer size exceeded");
	return(-ENOSPC);
    }
    outPtr->fVersion = 1;
    outPtr->fValuesPerTrack = 10;
    outPtr->fCount = nTracks;
    float* outFill = outPtr->fdEdxInfo;
    
    AliHLTComponentBlockData outBlock;
    FillBlockData(outBlock);
    outBlock.fOffset = size;
    outBlock.fSize = outSize;
    outBlock.fDataType = AliHLTTPCDefinitions::TPCdEdxNew();
    outBlock.fSpecification = pBlock->fSpecification;

    AliHLTExternalTrackParam* currTrack = dataPtr->fTracklets;
    nInputTracks+=nTracks;

    for(int itr = 0;itr < nTracks && ((AliHLTUInt8_t*) currTrack < ((AliHLTUInt8_t*) pBlock->fPtr) + pBlock->fSize);itr++)
    {
      // create an off-line track
      AliHLTGlobalBarrelTrack gb(*currTrack);
      AliExternalTrackParam& track = gb;

      int count = 0;
      int countIROC = 0;
      int countOROC1 = 0;
      
      int lastPatch = AliHLTTPCGeometry::CluID2Partition(currTrack->fPointIDs[currTrack->fNPoints - 1]);
      for(int ic = currTrack->fNPoints;--ic;)
      {
	UInt_t id = currTrack->fPointIDs[ic];
	int iSlice = AliHLTTPCGeometry::CluID2Slice(id);
	int iPatch = AliHLTTPCGeometry::CluID2Partition(id);
	int iCluster = AliHLTTPCGeometry::CluID2Index(id);

	AliHLTTPCRawCluster& cluster = fPatchClusters[iSlice][iPatch]->fClusters[iCluster];
	if (cluster.GetFlagSplitAnyOrEdge()) continue;
	
	float chargeTot = cluster.GetCharge();
	float chargeMax = cluster.GetQMax();
	int padRow = cluster.GetPadRow() + AliHLTTPCGeometry::GetFirstRow(iPatch);
	float padPitchWidth = AliHLTTPCGeometry::GetPadPitchWidth(iPatch);
	float padLength = AliHLTTPCGeometry::GetPadLength(padRow);
	
	if (lastPatch != iPatch)
	{
	  int sec = iSlice;
	  if (sec > 18) sec -= 18;
	  float alpha = 0.174533 + 0.349066 * sec;
	  track.RotateParamOnly(alpha);
	  lastPatch = iPatch;
	}
	int ret = track.PropagateParamOnlyTo(AliHLTTPCGeometry::Row2X(padRow), fBz);
	if (ret == kFALSE) break;
	
	float factor = sqrt((1 - track.GetSnp() * track.GetSnp()) / (1 + track.GetTgl() * track.GetTgl()));
	factor /= padLength;
	
	chargeTot *= factor;
	chargeMax *= factor / padPitchWidth;
	
	fBufTot[count] = chargeTot;
	fBufMax[count++] = chargeMax;
	if (padRow < AliHLTTPCGeometry::GetNRowLow()) countIROC++;
	else if (padRow < AliHLTTPCGeometry::GetNRowLow() + AliHLTTPCGeometry::GetNRowUp1()) countOROC1++;
	
	if (count >= fMaxClusterCount) break;
      }

      int countOROC2 = count - countIROC - countOROC1;
      int countOROC = countOROC1 + countOROC2;
      int truncLow = 5;
      int truncHigh = 40;
      outFill[0] = GetSortTruncMean(fBufTot                         , countIROC , countIROC  * 100 / truncLow, countIROC  * 100 / truncHigh);
      outFill[1] = GetSortTruncMean(fBufTot + countIROC             , countOROC1, countOROC1 * 100 / truncLow, countOROC1 * 100 / truncHigh);
      outFill[2] = GetSortTruncMean(fBufTot + countIROC + countOROC1, countOROC2, countOROC2 * 100 / truncLow, countOROC2 * 100 / truncHigh);
      outFill[3] = GetSortTruncMean(fBufTot + countIROC             , countOROC , countOROC  * 100 / truncLow, countOROC  * 100 / truncHigh);
      outFill[4] = GetSortTruncMean(fBufTot                         , count     , count      * 100 / truncLow, count      * 100 / truncHigh);
      outFill[5] = GetSortTruncMean(fBufMax                         , countIROC , countIROC  * 100 / truncLow, countIROC  * 100 / truncHigh);
      outFill[6] = GetSortTruncMean(fBufMax + countIROC             , countOROC1, countOROC1 * 100 / truncLow, countOROC1 * 100 / truncHigh);
      outFill[7] = GetSortTruncMean(fBufMax + countIROC + countOROC1, countOROC2, countOROC2 * 100 / truncLow, countOROC2 * 100 / truncHigh);
      outFill[8] = GetSortTruncMean(fBufMax + countIROC             , countOROC , countOROC  * 100 / truncLow, countOROC  * 100 / truncHigh);
      outFill[9] = GetSortTruncMean(fBufMax                         , count     , count      * 100 / truncLow, count      * 100 / truncHigh);

      unsigned int step = sizeof(AliHLTExternalTrackParam) + currTrack->fNPoints * sizeof(unsigned int);
      currTrack = (AliHLTExternalTrackParam*) (((Byte_t*) currTrack) + step);  
    }

    outputBlocks.push_back( outBlock );
    size += outSize;
  }

  return 0;
}

float AliHLTTPCFastdEdxComponent::GetSortTruncMean(float* array, int count, int trunclow, int trunchigh)
{
  if (count - trunclow - trunchigh <= 0) return(0.);
  std::sort(array, array + count);
  float mean = 0;
  for (int i = trunclow;i < count - trunchigh;i++) mean += array[i];
  return(mean / (count - trunclow - trunchigh));
}
