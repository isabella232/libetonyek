/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTYPES_H_INCLUDED
#define KEYTYPES_H_INCLUDED

#include <deque>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKObject.h"
#include "IWORKPath_fwd.h"
#include "IWORKStyle_fwd.h"
#include "IWORKText_fwd.h"
#include "IWORKTypes_fwd.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class IWORKTransformation;

struct KEYLayer
{
  boost::optional<std::string> type;
  IWORKObjectList_t objects;

  KEYLayer();
};

struct KEYPlaceholder
{
  boost::optional<bool> title;
  boost::optional<bool> empty;
  IWORKStylePtr_t style;
  IWORKGeometryPtr_t geometry;
  IWORKTextPtr_t text;

  KEYPlaceholder();
};

struct KEYStickyNote
{
  IWORKGeometryPtr_t geometry;
  IWORKTextPtr_t text;

  KEYStickyNote();
  KEYStickyNote(const IWORKGeometryPtr_t &geometry, const IWORKTextPtr_t &text);
};

typedef std::deque<KEYStickyNote> KEYStickyNotes_t;

IWORKObjectPtr_t makeObject(const KEYPlaceholderPtr_t &placeholder, const IWORKTransformation &trafo);

}

#endif //  KEYTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
