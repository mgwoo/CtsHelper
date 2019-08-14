///////////////////////////////////////////////////////////////////////////////
// Authors: Mingyu Woo
//
// BSD 3-Clause License
//
// Copyright (c) 2019, The Regents of the University of California
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include "lefdefIO.h"

#define compileDate __DATE__
#define compileTime __TIME__

vector<string> lefStor;
unordered_map<string, int> clockPinMap;
string defStr;
string ctsStr;
string blkStr;

using std::ofstream;
using std::stringstream;
using std::string;

bool argument(int argc, char *argv[]);
void PrintHelp();
void WriteCts(Replace::Circuit& ckt, const string& ctsStr);
void WriteBlk(Replace::Circuit& ckt, const string& blkStr);

int main(int argc, char *argv[]) {

  if( !argument(argc, argv) ) {
    PrintHelp();
    cout << "input param error!! " << endl;
    exit(1);
  }

  Replace::Circuit ckt;
  ckt.Init( lefStor, defStr );

  float cellHeight = FLT_MIN;
  for(auto& curSite : ckt.lefSiteStor) {
    if( strcmp( curSite.siteClass(), "CORE" ) == 0 ) {
      cellHeight = curSite.sizeY();
      break;
    }
  }
  cout << "normal cellHeight: " << cellHeight << endl;
  float defScale = ckt.defUnit;
  cout << "def scale down: " << defScale << endl;


  // defiPoints points = ckt.defDieArea.getPoint();
  // cout << points.numPoints << endl;
  // for(int i=0; i<points.numPoints; i++ ) {
  //   cout << 1.0*points.x[i]/defScale << " " 
  //   << 1.0*points.y[i]/defScale << endl;
  // }
  // cout << ckt.defDieArea.xl() << " " << ckt.defDieArea.yl() << " "
  //   << ckt.defDieArea.xh() << " " << ckt.defDieArea.yh() << endl;


  // for CTS
  WriteCts( ckt, ctsStr );

  // for BLK
  WriteBlk( ckt, blkStr );
}

void PrintHelp() {
  cout << "./lefdef2macro -lef <lef1> -lef <lef2> -def <def> -cts <cts> -blk <blk>" << endl;
}

void WriteCts( Replace::Circuit& ckt, const string& ctsStr ) {
  ofstream ctsFile(ctsStr);
  if( !ctsFile.good() ) {
    cout << "** ERROR : Cannot Open CTS file to write : " << ctsStr << endl;
    exit(1); 
  }
  stringstream feed;
  float defScale = ckt.defUnit;
  for(auto& curComp : ckt.defComponentStor) {
    auto macroPtr = ckt.lefMacroMap.find( string(curComp.name()) );
    if( macroPtr == ckt.lefMacroMap.end() ) {
      cout << "\n** ERROR : Cannot find MACRO cell in lef files: " 
        << curComp.name() << endl;
      exit(1);
    }

    lefiMacro& curMacro = ckt.lefMacroStor[ macroPtr->second ];

    if( !curMacro.hasSize() ) {
      cout << "\n** ERROR : Cannot find MACRO SIZE in lef files: " 
        << curComp.name() << endl;
      exit(1);
    }


    for(auto& curPin: ckt.lefPinStor[&curMacro - &ckt.lefMacroStor[0]]) {
      for(int i=0; i<curPin.numPorts(); i++) {
        lefiGeometries* geom = curPin.port(i);
        //                prtGeometry(geom);
        for(int j=0; j<geom->numItems(); j++) {
          if( geom->itemType(j) != lefiGeomLayerE ) {
            continue;
          }
          // cout << geom->getLayer(j) << " " << string( ckt.lefLayerStor[ 
          //    ckt.lefLayerMap[string(geom->getLayer(j))] ].type() ) << endl;
          if( string( ckt.lefLayerStor[ 
                ckt.lefLayerMap[string(geom->getLayer(j))] ].type() ) != "ROUTING") {
            continue;
          }

          string pinName = curPin.name();
          auto cpinPtr = clockPinMap.find( pinName) ;

          if( cpinPtr == clockPinMap.end() ) {
            continue;
          }

          feed << curComp.id() << "/" << pinName << " " 
            << curComp.placementX()/(defScale*15.0f) << " " 
            << curComp.placementY()/(defScale*15.0f) << " " 
            << geom->getLayer(j) << endl;

          break;
        }
      }
    }
  }
  ctsFile << feed.str();
  ctsFile.close();
  feed.clear();
}

void WriteBlk( Replace::Circuit& ckt, const string& blkStr) {
  ofstream blkFile(blkStr);
  if( !blkFile.good() ) {
    cout << "** ERROR : Cannot Open CTS file to write : " << ctsStr << endl;
    exit(1); 
  }
    
  float cellHeight = FLT_MIN;
  for(auto& curSite : ckt.lefSiteStor) {
    if( strcmp( curSite.siteClass(), "CORE" ) == 0 ) {
      cellHeight = curSite.sizeY();
      break;
    }
  }

  stringstream feed;
  float defScale = ckt.defUnit;
  for(auto& curComp : ckt.defComponentStor) {
    auto macroPtr = ckt.lefMacroMap.find( string(curComp.name()) );
    if( macroPtr == ckt.lefMacroMap.end() ) {
      cout << "\n** ERROR : Cannot find MACRO cell in lef files: " 
        << curComp.name() << endl;
      exit(1);
    }

    lefiMacro& curMacro = ckt.lefMacroStor[ macroPtr->second ];

    if( !curMacro.hasSize() ) {
      cout << "\n** ERROR : Cannot find MACRO SIZE in lef files: " 
        << curComp.name() << endl;
      exit(1);
    }

    // std cell skip
    if( abs( curMacro.sizeY() - cellHeight) <= std::numeric_limits<float>::epsilon() ) {
      continue;
    }
    feed << 1.0*curComp.placementX()/defScale << " " 
         << 1.0*curComp.placementY()/defScale << " "
         << curMacro.sizeX() << " " << curMacro.sizeY() << endl;

    /*
    for(auto& curObs: ckt.lefObsStor[&curMacro - &ckt.lefMacroStor[0]]) {
      lefiGeometries* geom = curObs.geometries();
      string layerName;

      for(int j=0; j<geom->numItems(); j++) {
        if( geom->itemType(j) == lefiGeomLayerE ) {
          layerName = geom->getLayer(j);
        }
        else if( geom->itemType(j) == lefiGeomRectE ) {
          lefiGeomRect* rect = geom->getRect(j); 

          feed << 1.0*curComp.placementX()/defScale + rect->xl << " " 
            << 1.0*curComp.placementY()/defScale + rect->yl << " " 
            << rect->xh << " " 
            << rect->yh << endl;
        }
      }
    }
    */
  // now macro cell
  //  cout << curComp.id() << " " 
  //    << 1.0*curComp.placementX()/defScale << " " 
  //    << 1.0*curComp.placementY()/defScale << " "
  //    << curMacro.sizeX() << " " << curMacro.sizeY() << " " 
  //    << curComp.name() << endl;
  }
  blkFile << feed.str();
  blkFile.close();
  feed.clear();
}


bool argument(int argc, char *argv[]) {
  if( argc <= 1 ) {
    return false;
  }

  defStr = ctsStr = blkStr = "";
  for(int i = 1; i < argc; i++) {
    if(!strcmp(argv[i], "-lef")) {
      i++;
      if(argv[i][0] != '-') {
        lefStor.push_back( string(argv[i]) );
      }
      else {
        printf("\n**ERROR: Option %s requires *.lef.\n",
            argv[i - 1]);
        return false;
      }
    }
    else if(!strcmp(argv[i], "-def")) {
      i++;
      if(argv[i][0] != '-') {
        defStr = argv[i];
      }
      else {
        printf("\n**ERROR: Option %s requires *.def.\n",
            argv[i - 1]);
        return false;
      }
    }
    else if(!strcmp(argv[i], "-cpin")) {
      i++;
      if(argv[i][0] != '-') {
        clockPinMap[ string(argv[i]) ] = 0;
      }
      else {
        printf("\n**ERROR: Option %s requires *.lef.\n",
            argv[i - 1]);
        return false;
      }
    }
    else if(!strcmp(argv[i], "-cts")) {
      i++;
      if(argv[i][0] != '-') {
        ctsStr = argv[i];
      }
      else {
        printf("\n**ERROR: Option %s requires *.cts.\n",
            argv[i - 1]);
        return false;
      }
    }
    else if(!strcmp(argv[i], "-blk")) {
      i++;
      if(argv[i][0] != '-') {
        blkStr = argv[i];
      }
      else {
        printf("\n**ERROR: Option %s requires *.blk.\n",
            argv[i - 1]);
        return false;
      }
    }
  }
  if( lefStor.size() == 0 || defStr == "" ||
      ctsStr == "" || blkStr == "") {
    return false;
  }
}

