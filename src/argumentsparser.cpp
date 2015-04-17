#include "argumentsparser.h"

ArgumentsParser::ArgumentsParser ( int argc, char** argv ) {
  for ( int i = 1; i < argc - 1; i++ ) {
    parseStr ( std::string ( argv[i] ) );
  }
}

void ArgumentsParser::parseStr ( std::string str ) {
  if ( str.substr ( 0,8 ) == "--scene=" ) {
    scene = str.substr ( 8,str.size() - 8 );
    return;
  } else if ( str.substr ( 0, 15 ) == "--resolution_x=" ) {
    x_res = std::stoi ( str.substr ( 15, str.size() - 15 ) );
    return;
  }  else if ( str.substr ( 0, 15 ) == "--resolution_y=" ) {
    y_res = std::stoi ( str.substr ( 15, str.size() - 15 ) );
    return;
  }  else if ( str.substr ( 0, 9 ) == "--output=" ) {
    output = str.substr ( 9, str.size() - 9 );
    return;
  }  else if ( str.substr ( 0, 14 ) == "--trace_depth=" ) {
    traceDepth = std::stoi ( str.substr ( 14, str.size() - 14 ) );
    return;
  }  else {
    printf ( "Unexpected argument: %s", str.c_str() );
  }
}

std::string ArgumentsParser::getScene() {
  return scene;
}
std::string ArgumentsParser::getOutput() {
  return output;
}
int ArgumentsParser::getXRes() {
  return x_res;
}

int ArgumentsParser::getYRes() {
  return y_res;
}

int ArgumentsParser::getTraceDepth() {
  return traceDepth;
}