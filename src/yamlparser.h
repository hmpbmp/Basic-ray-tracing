#ifndef YAMLPARSER_H_INCLUDED
#define YAMLPARSER_H_INCLUDED
#define  YAML_DECLARE_STATIC
#include <yaml.h>
#include <string>
#include <map>
#include <vector>
#include <stack>

enum class YAMLObjType {Value, Map, Sequence};

struct YAMLObject {
  void release();
  std::string name;
  YAMLObjType type;
  std::string value;
  std::map<std::string,YAMLObject*> map;
  std::vector<YAMLObject*> array;
};


class YAMLParser {
public:
  YAMLParser ( std::string &scene );
  ~YAMLParser();
  YAMLObject *getCameraObject();
  YAMLObject *getHeadObject();
private:
  yaml_parser_t parser;
  yaml_event_t event;
  std::stack<YAMLObject*> st;
  YAMLObject *main;
};





#endif