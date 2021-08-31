// Dear emacs, this is -*-c++-*-
#ifndef CxAODMaker_TrackJetHandler_VBFHbb_H
#define CxAODMaker_TrackJetHandler_VBFHbb_H

#include "CxAODMaker/TrackJetHandler.h"

class TrackJetHandler_VBFHbb : public TrackJetHandler {

public:

private:
  // selection functions
  bool passTrackJet(xAOD::Jet* jet);
};

#endif
