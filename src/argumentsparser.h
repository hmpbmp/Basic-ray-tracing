#ifndef ARGUMENTSPARSER_H_INCLUDED
#define ARGUMENTSPARSER_H_INCLUDED
#include <string>

class ArgumentsParser {
public:
  ArgumentsParser ( int argc , char ** argv );

  std::string getScene();
  std::string getOutput();
  int getXRes();
  int getYRes();
  int getTraceDepth();

private:
  void parseStr ( std::string str );
  std::string scene;
  std::string output;
  int x_res, y_res;
  int traceDepth;
};


#endif