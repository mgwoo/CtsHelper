#include "lefdefIO.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::unordered_map;
    
using CtsHelper::Circuit;
using CtsHelper::NetInfo;

Circuit::Circuit() : lefManufacturingGrid(DBL_MIN) {}
Circuit::Circuit(vector<string>& lefStor, 
    string defFilename, string verilogFilename )
: lefManufacturingGrid(DBL_MIN) {
  Init( lefStor, defFilename, verilogFilename );
}
        
void Circuit::Init( vector<string>& lefStor, string defFilename, 
    string verilogFilename ) {
  ParseLef(lefStor);
  ParseDef(defFilename);
  if( verilogFilename != "" ) {
    // ParseVerilog(verilogFilename);   
  }
};
    
NetInfo::NetInfo( int _macroIdx, int _compIdx, int _pinIdx) 
  : macroIdx(_macroIdx), compIdx(_compIdx), pinIdx(_pinIdx) {};
