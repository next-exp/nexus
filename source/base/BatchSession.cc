// ----------------------------------------------------------------------------
//  $Id$
//
//  Author : <justo.martin-albo@ific.uv.es>
//  Created: 22 Mar 2013
//
//  Copyright (c) 2013 NEXT Collaboration. All rights reserved.
// ----------------------------------------------------------------------------

#include "BatchSession.h"

#include <G4UImanager.hh>


using namespace nexus;



static void Tokenize(const G4String& str, std::vector<G4String>& tokens)
{
  const char* delimiter= " ";

  G4String::size_type pos0= str.find_first_not_of(delimiter);
  G4String::size_type pos = str.find_first_of(delimiter, pos0);

  while (pos != G4String::npos || pos0 != G4String::npos) {
    if (str[pos0] == '\"') {
      pos = str.find_first_of("\"", pos0+1);
      if (pos != G4String::npos) pos++;
    }

    if (str[pos0] == '\'') {
      pos = str.find_first_of("\'", pos0+1);
      if (pos != G4String::npos) pos++;
    }

    tokens.push_back(str.substr(pos0, pos-pos0));
    pos0 = str.find_first_not_of(delimiter, pos);
    pos = str.find_first_of(delimiter, pos0);
  }
}


BatchSession::BatchSession(G4String filename, G4UIsession* previous_session):
  G4UIsession(), opened_(false), prev_(previous_session)
{
  _macrostream.open(filename.data(), std::ios::in);

  if (_macrostream.fail()) {
    G4String msg = "Cannot open macro file " + filename;
    G4Exception("[BatchSession]", "BatchSession()", FatalException, msg);
  }
  else {
    _opened = true;
  }

  G4UImanager::GetUIpointer()-> SetSession(this);
}



BatchSession::~BatchSession()
{
  if (opened_) macrostream_.close();
}



G4String BatchSession::ReadCommand()
{
  enum { BUFSIZE = 4096 };
  static char linebuf[BUFSIZE];
  const char ctrM = 0x0d;

  G4String cmdtotal = "";
  G4bool qcontinued = false;

  while (_macrostream.good()) {

    _macrostream.getline(linebuf, BUFSIZE);

    G4String cmdline(linebuf);

    // TAB-> ' ' conversion
    str_size nb=0;
    while ((nb= cmdline.find('\t',nb)) != G4String::npos) {
      cmdline.replace(nb, 1, " ");
    }

    // strip
    cmdline = cmdline.strip(G4String::both);
    cmdline = cmdline.strip(G4String::trailing, ctrM);

    // skip null line if single line
    if (!qcontinued && cmdline.size()==0) continue;

    // '#' is treated as echoing something
    if(cmdline[(size_t)0]=='#') return cmdline;

    // tokenize...
    std::vector<G4String> tokens;
    Tokenize(cmdline, tokens);
    qcontinued= false;
    for (G4int i=0; i< G4int(tokens.size()); i++) {
      // string after '#" is ignored
      if(tokens[i][(size_t)0] == '#' ) break;
      // '\' or '_' is treated as continued line.
      if(tokens[i] == '\\' || tokens[i] == '_' ) {
        qcontinued= true;
        // check nothing after line continuation character
        if( i != G4int(tokens.size())-1) {
          G4Exception("[BatchSession]", "ReadCommand()", JustWarning,
            "Unexpected character after line continuation character.");
        }
        break; // stop parsing
      }
      cmdtotal+= tokens[i];
      cmdtotal+= " ";
    }

    if(qcontinued) continue; // read the next line

    if(cmdtotal.size() != 0) break;
    if(_macrostream.eof()) break;
  }

  // strip again
  cmdtotal= cmdtotal.strip(G4String::both);

  // finally,
  if(_macrostream.eof() && cmdtotal.size()==0) {
    return "exit";
  }

  return cmdtotal;
}



G4int BatchSession::ExecCommand(const G4String& command)
{
  G4UImanager* UI = G4UImanager::GetUIpointer();
  G4int rc = UI->ApplyCommand(command);

  G4String msg;

  switch(rc) {
    case fCommandSucceeded:
      break;
    case fCommandNotFound:
      msg = "Command not found: " + command;
      G4Exception("[BatchSession]", "ExecCommand()", FatalException, msg);
      break;
    case fIllegalApplicationState:
      msg = "Illegal application state: " + command;
      G4Exception("[BatchSession]", "ExecCommand()", FatalException, msg);
      break;
    default:
      G4int pn = rc%100;
      msg = "Illegal parameter: " + command;
      G4Exception("[BatchSession]", "ExecCommand()", FatalException, msg);
      G4cerr << "(" << pn << ") <" << command << ">" << G4endl;
  }

  return rc;
}



G4UIsession * BatchSession::SessionStart()
{
  if(!_opened) return _prev;

  while(1) {

    G4String newCommand = ReadCommand();

    if (newCommand == "exit") break;

    // just echo something if #
    if (newCommand[(size_t)0] == '#') {
      if(G4UImanager::GetUIpointer()-> GetVerboseLevel() == 2) {
        G4cout << newCommand << G4endl;
      }
      continue;
    }

    // execute command
    G4int rc= ExecCommand(newCommand);
    if(rc != fCommandSucceeded) {
      G4Exception("[BatchSession]", "SessionStart()", JustWarning, "A problem occurred with the previous command. Keep reading the macro.");
      continue;
    }
  }

  return _prev;
}



void BatchSession::PauseSessionStart(const G4String& Prompt)
{
  G4cout << "Pause session <" << Prompt << "> start." << G4endl;

  SessionStart();

  G4cout << "Pause session <" << Prompt << "> Terminate." << G4endl;
}
