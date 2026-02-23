/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Help Browser Dialog

  Shows embedded HTML help files (help.html, license.html)
  using QTextBrowser.
*/

#pragma once

#include <QDialog>

class QTextBrowser;

class HelpBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit HelpBrowser(QWidget* parent = nullptr);
    ~HelpBrowser() override = default;

    /// Load an HTML file from disk and display it.
    void loadFile(const QString& path);

    /// Load and display the main help file.
    void showHelp();

    /// Load and display the license file.
    void showLicense();

private:
    void setupUi();

    QTextBrowser* m_browser{nullptr};
};
