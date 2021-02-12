#pragma once
#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "mana/core/memoryallocator.h"
//
#include <ctype.h>
#include <stdbool.h>

#include "mana/core/corecommon.h"
#include "mana/core/fileio.h"
#include "mana/core/xmlnode.h"
#include "mana/graphics/render/vulkanrenderer.h"

struct XmlNode* xml_parser_load_xml_file(char* xml_file_path);
struct XmlNode* xml_parser_load_node(char** xml_file_data);
void xml_parser_delete(struct XmlNode* xml_node);

#endif  // XML_PARSER_H
