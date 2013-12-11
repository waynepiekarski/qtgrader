// ---------------------------------------------------------------------
//
// qtgrader  -  the interactive high speed grading assistant
// Copyright (C) 2012-2013 Wayne Piekarski
// wayne@tinmith.net http://tinmith.net/wayne
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ---------------------------------------------------------------------

#pragma once

#include <QProcess>
#include <QImage>
#include <QPixmap>
#include <QDir>
#include "debug.h"

class Pages {

friend class Global;

private:
  Pages(QString path)
  {
    if(path.contains(' '))
      GEXITDIALOG(QString("Cannot process path %1 that contains a space in it").arg(path));
    if(path.contains('\''))
      GEXITDIALOG(QString("Cannot process path %1 that contains a single quote in it").arg(path));
    if(path.contains('"'))
      GEXITDIALOG(QString("Cannot process path %1 that contains a double quote in it").arg(path));
    if(path.contains('\\'))
      GEXITDIALOG(QString("Cannot process path %1 that contains a backslash in it").arg(path));

    /* See if we need to firstly convert PDF to JPG */
    preparePDFtoJPG(path);

    /* The JPEG files should be there, so we can load them in now */
    _path = path;
    QDir dir (path, "*.jpg");
    dir.setFilter(QDir::Files);
    GDEBUG("Loading JPG images from dir [%s]",qPrintable(dir.canonicalPath()));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++)
    {
      QFileInfo f = list.at(i);
      GDEBUG("Found image %d from file [%s]", i, qPrintable(f.fileName()));
      filenames.push_back(f.filePath());
    }
    if (list.size() == 0)
      GEXITDIALOG(QString("No images found in scan directory [%1]").arg(qPrintable(path)));
    GASSERT(size() > 0, "Size %zu is invalid", size());
  }

  void preparePDFtoJPG (QString path)
  {
    QDir dir (path, "*.jpg");
    dir.setFilter(QDir::Files);
    GDEBUG("Scanning for JPG images in dir [%s]",qPrintable(dir.canonicalPath()));
    if (dir.entryInfoList().size() != 0)
    {
      GDEBUG("Found %d JPG files in dir [%s], so will not perform another PDF conversion", dir.entryInfoList().size(), qPrintable(path));
      return;
    }

    QDir pdfs (path, "*.pdf");
    pdfs.setFilter(QDir::Files);
    QFileInfoList list = pdfs.entryInfoList();
    size_t found_pdfs = 0;
    for (int i = 0; i < list.size(); i++)
    {
      QString pdfname = list.at(i).absoluteFilePath();
      if (pdfname.contains("report-"))
        GEXITDIALOG(QString("Found generated PDF file %1 but there are no JPG images, and this should not be possible").arg(pdfname));
      found_pdfs++;
      QString jpegprefix = pdfname;
      jpegprefix.replace(".pdf", "");
      GASSERT(jpegprefix.size()+4 == pdfname.size(), "Was not able to replace .pdf in file name [%s]", qPrintable(pdfname));

      // Use pdfimages to do the work. Note that QProcess only executes a single program, and you
      // need to know the location of the executable. So we use a bash wrapper to do this. You also
      // need to include --login so the PATH is set correctly to find the pdfimages program.
      QString cmd = QString("/bin/bash --login -c \"pdfimages -j %1 %2\"").arg(pdfname).arg(jpegprefix);
      QProcess pdfimages;
      GDEBUG("Processing PDF file [%s] with cmd [%s]", qPrintable(pdfname), qPrintable(cmd));
      pdfimages.start(cmd);
      if (!pdfimages.waitForFinished())
        GEXITDIALOG(QString("An error %1 occurred running command [%2], make sure you have pdfimages installed in your PATH").arg(pdfimages.errorString()).arg(cmd));
    }
    if (found_pdfs == 0)
      GEXITDIALOG(QString("No PDF or JPG files found in scan directory [%1]").arg(qPrintable(path)));
  }

private:
  QString& getFilename(size_t elem)
  {
    GASSERT(elem < size(), "Requested elem %zu not less than %zu", elem, size());
    return filenames[elem];
  }

  QPixmap getQPixmap(size_t elem)
  {
    GASSERT(elem < size(), "Requested elem %zu not less than %zu", elem, size());
    GDEBUG("Loading image %zu from file [%s]", elem, qPrintable(getFilename(elem)));
    QImage img (getFilename(elem));
    QPixmap pix = QPixmap::fromImage (img);
    return pix;
  }

  size_t size()
  {
    GASSERT(filenames.size() > 0, "Filenames size %zu is not greater than zero", filenames.size());
    return filenames.size();
  }

  QString& getPath() { return _path; }

private:
  std::vector<QString> filenames;
  QString _path;
};
