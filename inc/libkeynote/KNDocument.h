/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBKEYNOTE_KNDOCUMENT_H_INCLUDED
#define LIBKEYNOTE_KNDOCUMENT_H_INCLUDED

#include <libwpd/libwpd.h>
#include <libwpg/libwpg.h>

#include <libkeynote/KNStringVector.h>

class WPXInputStream;

namespace libkeynote
{

/** The type of Keynote file.
  */
enum KNDocumentType
{
  KN_DOCUMENT_TYPE_UNKNOWN,
  /// A standalone APXL file.
  KN_DOCUMENT_TYPE_APXL_FILE,
  /// A complete package.
  KN_DOCUMENT_TYPE_PACKAGE,
};

class KNDocument
{
public:

  /** Detect if the stream contains a valid Keynote document.
    *
    * The function can also optionally classify the type of the input
    * file. This is needed for proper support of documents created by
    * Keynote versions older than 5. The "documents" produced by these
    * versions were in fact directories, with the main file called
    * index.apxl.gz. But KNDocument only works with a stream, so if a
    * file stream just for the main file is passed, it is not possible
    * to get to the rest of the files (i.e., the images used in the
    * document), because there is no way to get the path. Therefore we
    * provide the @c type as a hint to the application that it passed us
    * just the main file and it can try again with a stream wrapping the
    * whole directory :-)
    *
    * @arg[in] input the stream
    * @arg[out] type of document
    * @returns true if the stream contains a Keynote document, false
    * otherwise
    */
  static bool isSupported(WPXInputStream *input, KNDocumentType *type = 0);

  static bool parse(WPXInputStream *input, libwpg::WPGPaintInterface *painter);

  static bool generateSVG(WPXInputStream *input, KNStringVector &output);
};

} // namespace libkeynote

#endif // LIBKEYNOTE_KNDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
