/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — Help Browser Dialog implementation
*/

#include "HelpBrowser.hpp"

#include <QDialogButtonBox>
#include <QFile>
#include <QTextBrowser>
#include <QVBoxLayout>

#ifdef HELPDIR
static const QString kHelpDir = QStringLiteral(HELPDIR);
#else
static const QString kHelpDir = QStringLiteral("/usr/local/share/doc/rakarrack");
#endif

HelpBrowser::HelpBrowser(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
}

void HelpBrowser::setupUi()
{
    setWindowTitle(tr("Rakarrack Help"));
    resize(700, 520);
    setMinimumSize(480, 360);

    auto* layout = new QVBoxLayout(this);

    m_browser = new QTextBrowser(this);
    m_browser->setOpenExternalLinks(true);
    layout->addWidget(m_browser, 1);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::accept);
    layout->addWidget(buttons);
}

void HelpBrowser::loadFile(const QString& path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_browser->setHtml(QString::fromUtf8(file.readAll()));
    }
    else
    {
        m_browser->setPlainText(
            tr("Could not open help file:\n%1").arg(path));
    }
}

void HelpBrowser::showHelp()
{
    setWindowTitle(tr("Rakarrack Help"));
    loadFile(kHelpDir + QStringLiteral("/html/help.html"));
    show();
    raise();
    activateWindow();
}

void HelpBrowser::showLicense()
{
    setWindowTitle(tr("License"));
    loadFile(kHelpDir + QStringLiteral("/html/license.html"));
    show();
    raise();
    activateWindow();
}
