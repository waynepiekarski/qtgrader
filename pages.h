#pragma once

#include <QImage>
#include <QPixmap>
#include <QDir>
#include "debug.h"

class Pages {

friend class Global;

private:
  Pages(QString path)
  {
    /* Scan the directory for images */
    _path = path;
    QDir dir (path, "*.jpg");
    dir.setFilter(QDir::Files);
    GDEBUG("Scanning for images in dir [%s]",qPrintable(dir.canonicalPath()));
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++)
    {
      QFileInfo f = list.at(i);
      GDEBUG("Found image %d from file [%s]", i, qPrintable(f.fileName()));
      filenames.push_back(f.filePath());
      images.push_back(NULL);
      pixes.push_back(NULL);
    }
    if (list.size() == 0)
      GEXITDIALOG(QString("No images found in scan directory [%1]").arg(qPrintable(path)));
    GASSERT(size() > 0, "Size %zu is invalid", size());
  }

private:
  QString& getFilename(size_t elem)
  {
    GASSERT(elem < size(), "Requested elem %zu not less than %zu", elem, size());
    return filenames[elem];
  }

  QPixmap& getQPixmap(size_t elem)
  {
    GASSERT(elem < size(), "Requested elem %zu not less than %zu", elem, size());
    if (images[elem] == NULL)
    {
      GDEBUG("Caching image %zu from file [%s]", elem, qPrintable(getFilename(elem)));
      images[elem] = new QImage (getFilename(elem));
      pixes[elem] = QPixmap::fromImage (*images[elem]);
    }
    return pixes[elem];
  }

  size_t size()
  {
    GASSERT(filenames.size() > 0, "Filenames size %zu is not greater than zero", filenames.size());
    GASSERT(filenames.size() == images.size(), "Filenames size %zu does not match images size %zu", filenames.size(), images.size());
    GASSERT(filenames.size() == pixes.size(), "Filenames size %zu does not match pixes size %zu", filenames.size(), pixes.size());
    return filenames.size();
  }

  QString& getPath() { return _path; }

private:
  std::vector<QString> filenames;
  std::vector<QImage*> images;
  std::vector<QPixmap> pixes;
  QString _path;
};
