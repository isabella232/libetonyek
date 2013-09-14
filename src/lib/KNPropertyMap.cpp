/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNPropertyMap.h"

namespace libkeynote
{

KNPropertyMap::KNPropertyMap()
  : m_map()
  , m_parent(0)
{
}

KNPropertyMap::KNPropertyMap(const KNPropertyMap *const parent)
  : m_map()
  , m_parent(parent)
{
}

void KNPropertyMap::setParent(const KNPropertyMap *const parent)
{
  m_parent = parent;
}

boost::any KNPropertyMap::get(const std::string &key, const bool lookInParent) const
{
  const Map_t::const_iterator it = m_map.find(key);
  if (m_map.end() != it)
    return it->second;

  if (lookInParent && m_parent)
    return m_parent->get(key, lookInParent);

  return boost::any();
}

void KNPropertyMap::set(const std::string &key, const boost::any &value)
{
  m_map[key] = value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
