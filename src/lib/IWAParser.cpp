/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAParser.h"

#include <cassert>
#include <deque>

#include <boost/optional.hpp>

#include "IWAMessage.h"

namespace libetonyek
{

using boost::optional;

using std::deque;
using std::make_pair;
using std::pair;
using std::string;

IWAParser::ObjectRecord::ObjectRecord(const RVNGInputStreamPtr_t &stream, const unsigned type,
                                      const long pos, const unsigned long headerLen, const unsigned long dataLen)
  : m_stream(stream)
  , m_type(type)
  , m_headerRange(pos, pos + long(headerLen))
  , m_dataRange(m_headerRange.second, m_headerRange.second + long(dataLen))
{
}

IWAParser::IWAParser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, IWORKCollector &collector)
  : m_fragments(fragments)
  , m_package(package)
  , m_collector(collector)
{
}

bool IWAParser::parse()
{
  parseObjectIndex();
  return parseDocument();
}

boost::optional<IWAMessage> IWAParser::queryObject(const unsigned id, const unsigned type) const
{
  const RecordMap_t::const_iterator recIt = m_fragmentObjectMap.find(id);
  if (recIt == m_fragmentObjectMap.end())
    return boost::none;
  if (!recIt->second.second.m_stream)
    const_cast<IWAParser *>(this)->scanFragment(recIt->second.first);
  if (recIt->second.second.m_stream)
  {
    const ObjectRecord &objRecord = recIt->second.second;
    if ((objRecord.m_type == type) || (type == 0))
      return IWAMessage(objRecord.m_stream, objRecord.m_dataRange.first, objRecord.m_dataRange.second);
  }
  return boost::none;
}

boost::optional<unsigned> IWAParser::readRef(const IWAMessage &msg, const unsigned field)
{
  if (msg.message(field))
    return msg.message(field).uint32(1);
  return boost::none;
}

void IWAParser::parseObjectIndex()
{
  if (!m_fragments->existsSubStream("Index/Metadata.iwa"))
  {
    ETONYEK_DEBUG_MSG(("Index/Metadata.iwa missing, can't create object index"));
    return;
  }

  const RVNGInputStreamPtr_t input(m_fragments->getSubStreamByName("Index/Metadata.iwa"));
}

void IWAParser::scanFragment(const unsigned id)
{
  // scan the fragment file
  const FileMap_t::iterator fragmentIt = m_fragmentMap.find(id);
  if (fragmentIt != m_fragmentMap.end())
  {
    assert(!fragmentIt->second.second); // this could only happen if the fragment file had already been scanned
    if (m_fragments->existsSubStream(fragmentIt->second.first.c_str()))
    {
      const RVNGInputStreamPtr_t stream(m_fragments->getSubStreamByName(fragmentIt->second.first.c_str()));
      assert(bool(stream));
      fragmentIt->second.second = stream;
      scanFragment(fragmentIt->first, fragmentIt->second.second);
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWAParser::scanFragment: file %s does not exist\n", fragmentIt->second.first.c_str()));
      m_fragmentMap.erase(fragmentIt); // avoid unnecessary repeats of the lookup
    }
  }
}

void IWAParser::scanFragment(const unsigned id, const RVNGInputStreamPtr_t &stream)
{
  try
  {
    while (!stream->isEnd())
    {
      // scan a single object
      const uint64_t headerLen = readUVar(stream);
      const long start = stream->tell();
      const IWAMessage header(stream, headerLen);
      if (!header.message(2) || !header.message(2).uint64(1))
        break;
      const uint64_t dataLen = header.message(2).uint64(1).get();
      if (header.uint32(1))
      {
        const RecordMap_t::iterator recIt = m_fragmentObjectMap.find(header.uint32(1).get());
        const optional<unsigned> type = header.uint32(1);
        if (recIt != m_fragmentObjectMap.end())
          recIt->second.second = ObjectRecord(stream, get_optional_value_or(type, 0), start, long(headerLen), long(dataLen));
      }
      if (stream->seek(start + long(headerLen) + long(dataLen), librevenge::RVNG_SEEK_CUR) != 0)
        break;
    }
  }
  catch (...)
  {
    // just read as much as possible
  }

  // remove all objects from the fragment that have not been found
  RecordMap_t::iterator it = m_fragmentObjectMap.begin();
  while (it != m_fragmentObjectMap.end())
  {
    const RecordMap_t::iterator curIt = it;
    ++it;
    if ((curIt->second.first == id) && !curIt->second.second.m_stream)
    {
      ETONYEK_DEBUG_MSG(("IWAParser::scanFragment: object with ID %u was not found\n", curIt->first));
      m_fragmentObjectMap.erase(curIt);
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */