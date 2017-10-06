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
#ifndef __PARSEX3D_H__

#include <limits.h>
#include <istream>
#include <stack>
//#include <hash_map>
#include <string>

#include "scene.h"

#ifndef _MSC_VER

#include <ext/hash_map>
#include <ext/hash_set>

namespace __gnu_cxx {
  template<> struct hash< std::string >
    {
      size_t operator()( const std::string& x ) const
      {
        return hash< const char* >()( x.c_str() );
      }
    };
}

#else

#include <hash_map>
#include <hash_set>

#define __gnu_cxx stdext

const char* dirname(const char* path);

#endif

class X3Reader {
 public:
  enum {NAME_NONE, NAME_USE, NAME_DEF};
 public:
  X3Reader();
  X3Scene* Read(std::istream& ist);
  void StartElement(const char *name, const char **atts);
  void EndElement(const char *name);

  int ProcessNames(const char** atts, std::string* name);

  static X3NodeType GetType(const char* name);

  void set_dirname(const char* dirname) {
    dirname_.assign(dirname);
  }
 private:
  void ResetSkip() {
    skip_level_ = INT_MAX;
  }
  bool ProcessLinkAttributes(const char **atts, 
                             X3InterpolatorNode** src_node,
                             X3Timer** timer_node, 
                             X3Node** dest_node, std::string* dest_field_name);
 private:
  int level_, skip_level_;
  X3Scene* scene_;
  std::stack<X3Node*> node_stack_;
  static __gnu_cxx::hash_map<std::string, X3NodeType> name_hash;
  __gnu_cxx::hash_map<std::string, X3Node*> def_nodes_;
  std::string dirname_; // scene file directory path
};

#endif  // __PARSEX3D_H__
