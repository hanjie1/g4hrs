#include "CLHEP/Random/Random.h"

#include "remollRunAction.hh"
#include "remollPrimaryGeneratorAction.hh"
#include "remollEventAction.hh"
#include "remollSteppingAction.hh"

#include "G4StepLimiterBuilder.hh"

//------------
// Geometries:
//------------
#include "remollDetectorConstruction.hh"

#include "remollIO.hh"
#include "remollMessenger.hh"

//-----------------------------------
#include "LHEP.hh"

#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4RunManagerKernel.hh"

#include "G4UnitsTable.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

int main(int argc, char** argv)
{
  //-------------------------------
  // Initialize the CLHEP random engine used by
  // "shoot" type functions
  //-------------------------------

    CLHEP::HepRandom::createInstance();
    CLHEP::HepRandom::setTheSeed(time(0));

    remollIO *io = new remollIO();



  //-------------------------------
  // Initialization of Run manager
  //-------------------------------
  G4cout << "RunManager construction starting...." << G4endl;
  G4RunManager * runManager = new G4RunManager;

  remollMessenger *rmmess = new remollMessenger();

  // Detector/mass geometry and parallel geometry(ies):
  G4VUserDetectorConstruction* detector = new remollDetectorConstruction();

  runManager->SetUserInitialization(detector);
  
  // PhysicsList (including G4FastSimulationManagerProcess)
  G4VModularPhysicsList* physicsList = new LHEP();
  physicsList->RegisterPhysics(new G4StepLimiterBuilder());
  runManager->SetUserInitialization(physicsList);

  //-------------------------------
  // UserAction classes
  //-------------------------------
  G4UserRunAction* run_action = new remollRunAction;
  ((remollRunAction *) run_action)->SetIO(io);
  runManager->SetUserAction(run_action);
  //
  G4VUserPrimaryGeneratorAction* gen_action = new remollPrimaryGeneratorAction;
  ((remollPrimaryGeneratorAction *) gen_action)->SetIO(io);
  runManager->SetUserAction(gen_action);
  //
  G4UserEventAction* event_action = new remollEventAction;
  ((remollEventAction *) event_action)->SetIO(io);
  ((remollEventAction *) event_action)->SetEvGen(((remollPrimaryGeneratorAction *) gen_action)->GetEvGen());
  runManager->SetUserAction(event_action);
  //
  G4UserSteppingAction* stepping_action = new remollSteppingAction;
  runManager->SetUserAction(stepping_action);

  // Initialize Run manager
  runManager->Initialize();
  
    // New units


  //----------------
  // Visualization:
  //----------------
#ifdef G4VIS_USE
  G4cout << "Instantiating Visualization Manager......." << G4endl;
  G4VisManager* visManager = new G4VisExecutive;
  visManager -> Initialize ();
#endif

  // Setup commands
  //
  G4UImanager * UImanager = G4UImanager::GetUIpointer();
  /*
  UImanager->ApplyCommand("/Step/Verbose 0");
  UImanager->ApplyCommand("/tracking/Verbose 1");
  UImanager->ApplyCommand("/gun/particle e-");
  UImanager->ApplyCommand("/gun/energy 100 MeV");
  UImanager->ApplyCommand("/gun/direction 0 0 1");
  UImanager->ApplyCommand("/gun/position 0 0 0");
  UImanager->ApplyCommand("/gun/direction 0 .3 1.");
  */

  if(argc==1)
  {
    //--------------------------
    // Define (G)UI
    //--------------------------
#ifdef G4UI_USE
    G4UIExecutive * ui = new G4UIExecutive(argc,argv);


    ui->SessionStart();

    delete ui;
#endif
  }
  else
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

#ifdef G4VIS_USE
//  delete visManager;
#endif
//  delete runManager;

  return 0;
}