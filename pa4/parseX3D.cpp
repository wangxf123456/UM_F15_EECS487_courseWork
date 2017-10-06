/*
 * Copyright (c) 2007 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University 
 * may not be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Igor Guskov
 *
 */
#include <string.h>
#include <limits.h>
#include <iostream>
#include <string>
using namespace std;

#include "expat.h"
#include "parseX3D.h"
#include "scene.h"

#ifndef _WIN32
#include <libgen.h>
#else
extern const char *dirname(const char* path);
extern const char *basename(const char* path);
#endif

__gnu_cxx::hash_map<std::string, X3NodeType> X3Reader::name_hash;

struct str2int {
  const char* s;
  X3NodeType n;
};

str2int name2type[] =
  {
    {"X3D", X3NODE_X3D},
    {"Scene", X3NODE_SCENE},
    {"Transform", X3NODE_TRANSFORM},
    {"Group", X3NODE_GROUP},
    {"Shape", X3NODE_SHAPE},
    {"Viewpoint", X3NODE_VIEWPOINT},
    {"Box", X3NODE_BOX},
    {"Cylinder", X3NODE_CYLINDER},
    {"Cone", X3NODE_CONE},
    {"IndexedFaceSet", X3NODE_INDEXEDFACESET},
    {"Material", X3NODE_MATERIAL},
    {"Appearance", X3NODE_APPEARANCE},
    {"Coordinate", X3NODE_COORDINATE},
    {"PointLight", X3NODE_POINTLIGHT},
    {"ImageTexture", X3NODE_IMAGETEXTURE},
    {"Link", X3NODE_LINK},
    {"PositionInterpolator", X3NODE_POSITIONINTERPOLATOR},
    {"OrientationInterpolator", X3NODE_ORIENTATIONINTERPOLATOR},
    {"ScalarInterpolator", X3NODE_SCALARINTERPOLATOR},
    {"Timer", X3NODE_TIMER},
    {"TextureCoordinate", X3NODE_TEXTURECOORDINATE},
    {"TextureTransform", X3NODE_TEXTURETRANSFORM},
    {"Curve", X3NODE_CURVE},
  };

#ifndef XMLCALL
#define XMLCALL 
#endif

// This is just a callback wrapper for the appropriate X3Reader call.
static void XMLCALL
startElement(void *userData, const char *name, const char **atts)
{
  X3Reader* reader = static_cast<X3Reader*>(userData);
  reader->StartElement(name, atts);
}

// This is just a callback wrapper for the appropriate X3Reader call.
static void XMLCALL
endElement(void *userData, const char *name)
{
  X3Reader* reader = static_cast<X3Reader*>(userData);
  reader->EndElement(name);
}

// For X3D models, there is nothing to parse, as everything is 
// passed in the attributes.
static void XMLCALL
char_cb(void *userData, const XML_Char *s, int len)
{
}

X3Reader::X3Reader() 
  : level_(-1),
    skip_level_(INT_MAX),
    scene_(NULL)
{
  if(name_hash.empty()) {
    for (int k = 0; k < (int)(sizeof(name2type)/sizeof(str2int)); ++k) {
      name_hash.insert(make_pair(string(name2type[k].s), name2type[k].n));
    }
  }
}

X3NodeType
X3Reader::GetType(const char* name)
{
  __gnu_cxx::hash_map<string, X3NodeType>::const_iterator mi =
    name_hash.find(string(name));
  return mi==name_hash.end() ? X3NODE_UNKNOWN : (*mi).second;
}

bool
X3Reader::ProcessLinkAttributes(const char **atts, 
                                X3InterpolatorNode** src_node,
                                X3Timer** timer_node,
                                X3Node** dest_node,
                                string* dest_field_name)
{
  if(atts==0)
    return false;
  __gnu_cxx::hash_map<string, X3Node*>::iterator hi; 
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    const char* name = *ref;
    ++ref;
    if(*ref==NULL) {
      break;
    }
    if(strcmp(name, "INTERPOLATOR")==0) {
      hi = def_nodes_.find(string(*ref)); 
      if(hi!=def_nodes_.end()) {
        // need to check if this is an interpolation node
        if((*hi).second==NULL) {
          return false;
        }
        if(!(*hi).second->IsInterpolator()) {
          cerr << "WARNING: FROM_NODE " << *ref << " cannot interpolate" << endl;
          return false;
        }
        *src_node = static_cast<X3InterpolatorNode*>((*hi).second);
      } else {
        cerr << "WARNING: FROM_NODE not found: " << *ref << endl;
        return false;
      }
    } else if(strcmp(name, "TIMER")==0) {
      hi = def_nodes_.find(string(*ref)); 
      if(hi!=def_nodes_.end()) {
        // need to check if this is a timer node
        if((*hi).second==NULL) {
          return false;
        }
        if(!(*hi).second->IsTimer()) {
          cerr << "WARNING: node " << *ref 
               << " is not a timer." << endl;
          return false;
        }
        *timer_node = static_cast<X3Timer*>((*hi).second);
      } else {
        cerr << "WARNING: TIMER not found: " << *ref << endl;
        return false;
      }
    } else if(strcmp(name, "TO_NODE")==0) {
      hi = def_nodes_.find(string(*ref)); 
      if(hi!=def_nodes_.end()) {
        *dest_node = (*hi).second;
      } else {
        cerr << "WARNING: TO_NODE not found: " << *ref << endl;
        return false;
      }
    } else if(strcmp(name, "TO_FIELD")==0) {
      dest_field_name->assign(*ref);
    }
  }
  return *src_node && *dest_node && 
    *timer_node && !dest_field_name->empty();
}

void
X3Reader::StartElement(const char *name, const char **atts)
{
  ++level_;
  if(level_>skip_level_)
    return;

  X3Node* node = NULL;
  X3NodeType node_type = GetType(name);

  string defname;
  int def_or_use = ProcessNames(atts, &defname);

  if(def_or_use==NAME_USE) {
    // For the node with the USE attribute, find the corresponding DEF node
    // in the table and use a pointer to the old node at the current position in
    // the scene graph.
    __gnu_cxx::hash_map<string, X3Node*>::iterator hi = def_nodes_.find(defname); 

    if(hi!=def_nodes_.end()) {
      node = (*hi).second;
    }

  } else {
    // Otherwise allocate a new node usually.
    switch(node_type) {
    case X3NODE_X3D:
      // Can just skip this.
      break;
    case X3NODE_SCENE:
      // The scene node is the root of our scene graph hierarchy. We shall
      // only process the first scene in the file.
      if(scene_) {
        // If we already have the scene, we skip all the other ones.
        skip_level_ = level_;
        return;
      }
      // 
      scene_ = new X3Scene;
      node = scene_;
      break;
    case X3NODE_LINK:
      // this is a non-standard node -- it sets up a link between 
      // interpolator nodes
      // and their consumer node-fields which are given in the attributes
      if(scene_) {
        // get the from and to nodes from the def_nodes_ table
        // the destination node should provide linking by name method
        // with type checking. This will not happen too often.
        // the source should provide type id and destination should check that
        // against the field 
        //scene_->AddLink(atts);
        X3InterpolatorNode* src_node;
        X3Timer* timer_node;
        X3Node* dest_node;
        string dest_field_name;
        if(ProcessLinkAttributes(atts, &src_node, &timer_node, 
                                 &dest_node, &dest_field_name)) {
          scene_->AddLink(src_node, timer_node, dest_node, dest_field_name);
        }
      }
    case X3NODE_TRANSFORM:
      node = new X3Transform(atts);
      break;
    case X3NODE_GROUP:
      node = new X3Group;
      break;
    case X3NODE_VIEWPOINT:
      node = new X3Viewpoint(atts);
      break;
    case X3NODE_SHAPE:
      node = new X3Shape;
      break;
    case X3NODE_BOX:
      node = new X3Box(atts);
      break;
    case X3NODE_CYLINDER:
      node = new X3Cylinder(atts);
      break;
    case X3NODE_CONE:
      node = new X3Cone(atts);
      break;
    case X3NODE_INDEXEDFACESET:
      node = new X3IndexedFaceSet(atts);
      break;
    case X3NODE_APPEARANCE:
      node = new X3Appearance;
      break;
    case X3NODE_MATERIAL:
      node = new X3Material(atts);
      break;
    case X3NODE_COORDINATE:
      node = new X3Coordinate(atts);
      break;
    case X3NODE_TEXTURECOORDINATE:
      node = new X3TextureCoordinate(atts);
      break;
    case X3NODE_POINTLIGHT:
      node = new X3PointLight(atts);
      break;
    case X3NODE_IMAGETEXTURE:
      node = new X3ImageTexture(dirname_, atts);
      break;
    case X3NODE_POSITIONINTERPOLATOR:
      node = new X3PositionInterpolator(atts);
      break;
    case X3NODE_ORIENTATIONINTERPOLATOR:
      node = new X3OrientationInterpolator(atts);
      break;
    case X3NODE_SCALARINTERPOLATOR:
      node = new X3ScalarInterpolator(atts);
      break;
    case X3NODE_TIMER:
      node = new X3Timer(atts);
      break;
    case X3NODE_TEXTURETRANSFORM:
      node = new X3TextureTransform(atts);
      break;
    case X3NODE_CURVE:
      node = new X3Curve(atts);
      break;
    default:
      // This is how we skip everything we do not process: set the skip level
      // and wait for the end element at this level. Then start processing again.
      skip_level_ = level_;
      return;
    }
    if(def_or_use==NAME_DEF) {
      // If there is a DEF attribute, we need to insert the node into
      // the def node table to be used later.
      def_nodes_.insert(make_pair(defname, node)); 
    }
    // remember the node to later delete it
    if(scene_) {
      if(node != scene_) {
        scene_->Register(node);
      }
    }
  }

  if(node) {
    // If a valid node is present we push it onto the stack (linking 
    // with its parent if that is present.
    // Note that the Scene node should appear before any other meaningful
    // node for this to work.
    if(!node_stack_.empty())
      node_stack_.top()->Add(node_type, node);
    node_stack_.push(node);
  }

  //cout << level_ << name << endl;
}

// Searches for DEF/USE attributes.
int
X3Reader::ProcessNames(const char** atts, string* name)
{
  for (const char** ref = atts; *ref!=NULL; ++ref) {
    if(strcmp(*ref, "USE")==0) {
      ++ref;
      if(ref)
        name->assign(*ref);
      return NAME_USE;
    }
    if(strcmp(*ref, "DEF")==0) {
      ++ref;
      if(ref)
        name->assign(*ref);
      return NAME_DEF;
    }
    ++ref;
    if(*ref==NULL) {
      //cout << "UNDEFINED";
      break;
    }
  }
  return NAME_NONE;
}

// This method handles skipping and pops elements off the stack as needed.
void
X3Reader::EndElement(const char *name)
{
  if(level_ <= skip_level_) {
    if(level_ == skip_level_) {
      ResetSkip();
    } else {
      if(!node_stack_.empty()) {
        // pop the stack
        node_stack_.pop();
      }
      //cout << level_ << "/" << name << endl;
    }
  }
  --level_;
}

X3Scene*
X3Reader::Read(std::istream& ist)
{
  XML_Parser parser = XML_ParserCreate(NULL);
  
  XML_SetStartElementHandler(parser, startElement);
  XML_SetEndElementHandler(parser, endElement);
  XML_SetCharacterDataHandler(parser, char_cb);
  XML_SetUserData(parser, this);

  level_ = -1;

  const int BUFSIZE = 32;
  char buf[BUFSIZE];

  bool done;

  do {
    ist.read(buf, BUFSIZE);
    done = ist.gcount() < BUFSIZE;
    if (XML_Parse(parser, buf, (int) ist.gcount(), done) == XML_STATUS_ERROR) {
      cerr << XML_ErrorString(XML_GetErrorCode(parser))
           << " at line " << XML_GetCurrentLineNumber(parser) << endl;
      delete scene_;
      return NULL;
    }
  } while (!done);

  XML_ParserFree(parser);
  return scene_;
}

