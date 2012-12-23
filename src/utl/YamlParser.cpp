#include <string>
#include <map>
#include <iostream>
#include <exception>
#include <stdio.h>
#include <YamlParser.h>

using namespace std;

YamlParser::YamlParser( const char *file ) throw (YamlException) {
  f = fopen ( file, "r" );

  /* Initialize parser */
  if(!yaml_parser_initialize(&parser)){
    throw YamlException ("Failed to initialize parser!");
  } else {
    /* Set input file */
    yaml_parser_set_input_file(&parser, f);
  }
}

YamlParser::~YamlParser(){

  /* Cleanup */
  yaml_token_delete(&token);
  yaml_parser_delete(&parser);
  fclose (f);
}

void YamlParser::parse ( string root ) {

  string key;
  string value;

  bool done = false ;
  
  do {

    yaml_parser_scan(&parser, &token);
    switch(token.type) {

    /* Stream start/end */
    case YAML_STREAM_START_TOKEN:
      node_type = YAML_NODE;
      break;
    case YAML_STREAM_END_TOKEN:
      node_type = YAML_NODE;
      done = true;
      break;

    /* Token types (read before actual token) */
    case YAML_KEY_TOKEN:
      node_type = YAML_NODE;
      break;

    case YAML_VALUE_TOKEN:
      node_type = YAML_LEAF;
      break;

    /* Block delimeters */
    case YAML_BLOCK_SEQUENCE_START_TOKEN:
      node_type = YAML_SEQ_LEAF;
      parse ( root + ( key.size()>0 ? "." + key : ""));
      break;

    case YAML_BLOCK_ENTRY_TOKEN:
      break;

    case YAML_BLOCK_END_TOKEN:
      if ( node_type == YAML_SEQ_LEAF ){
        properties [root]= value ;
      }

      node_type = YAML_NODE;
      done = true;
      break;

    case YAML_BLOCK_MAPPING_START_TOKEN:
      parse ( root + ( key.size()>0 ? "." + key : ""));
      break;

    /* Data */
    case YAML_SCALAR_TOKEN:
      switch ( node_type ){
        case YAML_NODE: 
        key = (char *)token.data.scalar.value;
        break;

        case YAML_SEQ_LEAF: 
        value += (char *)token.data.scalar.value ;
        value += " ";
        break;

        case YAML_LEAF: 
        value =  (char *)token.data.scalar.value;
        properties [string(root +"."+ key)]= value ;
        break;
      } 
      break;

    default:
      throw YamlParser ("Unexpected token value ?!" );
    }

    last_token = token.type;

    if(token.type != YAML_STREAM_END_TOKEN) yaml_token_delete(&token);

  } while( ! done );
}

const map<string,string> YamlParser::get_properties (){
  return properties;
}
