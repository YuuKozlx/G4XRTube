// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#ifndef G4XRTUBE_EVENT_ACTION_HH
#define G4XRTUBE_EVENT_ACTION_HH

#include "G4UserEventAction.hh"

class EventAction final : public G4UserEventAction
{
public:
  EventAction();
  ~EventAction() override = default;
};

#endif
