#include "yamlparser.h"


YAMLParser::YAMLParser ( std::string scene ) {

  if ( ! ( scene.size() == 0 ) ) {
    yaml_parser_initialize ( &parser );
    FILE *input = fopen ( scene.c_str(),"rb" );
    if ( input ) {
      int done = 0;
      yaml_parser_set_input_file ( &parser, input );
      std::string scalar;
      while ( !done ) {
        if ( !yaml_parser_parse ( &parser,&event ) ) {
          break;
        }
        switch ( event.type ) {
          case YAML_MAPPING_START_EVENT:  {
              YAMLObject *other = new YAMLObject();
              other->type = YAMLObjType::Map;
              st.push ( other );
              break;
            }
          case YAML_MAPPING_END_EVENT: {
              if ( st.size() == 1 && st.top()->type == YAMLObjType::Map ) {
                break;
              }
              YAMLObject *top = st.top();
              st.pop();
              if ( st.top()->type == YAMLObjType::Sequence ) {
                st.top()->array.push_back ( top );
              } else {
                YAMLObject *utop = st.top();
                st.pop();
                st.top()->map.insert ( std::pair<std::string,YAMLObject*> ( utop->value, top ) );
              }
              break;
            }
          case YAML_SEQUENCE_START_EVENT: {
              YAMLObject *other = new YAMLObject();
              other->type = YAMLObjType::Sequence;
              st.push ( other );
              break;
            }
          case YAML_SEQUENCE_END_EVENT: {
              if ( st.size() == 1 && st.top()->type == YAMLObjType::Sequence ) {
                break;
              }
              YAMLObject *top = st.top();
              st.pop();
              if ( st.top()->type == YAMLObjType::Sequence ) {
                st.top()->array.push_back ( top );
              } else {
                YAMLObject *utop = st.top();
                st.pop();
                st.top()->map.insert ( std::pair<std::string,YAMLObject*> ( utop->value, top ) );
              }
              break;
            }
          case YAML_SCALAR_EVENT: {
              scalar = ( char* ) event.data.scalar.value;
              YAMLObject *other = new YAMLObject();
              other->type = YAMLObjType::Value;
              other->value = scalar;
              switch ( st.top()->type ) {
                case YAMLObjType::Map:
                  st.push ( other );
                  break;
                case YAMLObjType::Sequence:
                  st.top()->array.push_back ( other );
                  break;
                case YAMLObjType::Value: {
                    YAMLObject * top = st.top();
                    st.pop();
                    st.top()->map.insert ( std::pair<std::string, YAMLObject*> ( top->value, other ) );
                    break;
                  }
              }
              break;
            }
          default:
            break;
        }
        done = ( event.type == YAML_STREAM_END_EVENT );
        yaml_event_delete ( &event );
      }
    }
  }
  yaml_parser_delete ( &parser );
  main = st.top();
  st.pop();
}


YAMLObject * YAMLParser::getCameraObject() {
  if ( main ) {
    return main->array[0]->map["camera"];
  }
  return NULL;
}

YAMLObject *YAMLParser::getHeadObject() {
  if ( main ) {
    return main->array[1]->map["node"];
  }
  return NULL;
}