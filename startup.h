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

#include <QDialog>
#include "ui_startup.h"
#include "debug.h"

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
  Q_OBJECT
    
public:
  explicit StartupDialog(QWidget *parent = 0);
  ~StartupDialog();
  
public slots:
  void handleSelectImageDirectory();
  void handleLoadExistingProject();

private:
  Ui::StartupDialog *ui;
};
