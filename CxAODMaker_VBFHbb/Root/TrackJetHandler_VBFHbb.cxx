#include <iostream>

#include "CxAODMaker_VBFHbb/TrackJetHandler_VBFHbb.h"

#include "CxAODTools_VBFHbb/CommonProperties_VBFHbb.h"

bool TrackJetHandler::passTrackJet(xAOD::Jet* jet)
{
  Props::isTrackJet.set(jet, false);
  Props::passPreSel.set(jet, false);
 return false;
}

