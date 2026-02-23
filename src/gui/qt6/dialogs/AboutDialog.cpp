/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI — About Dialog implementation
*/

#include "AboutDialog.hpp"

#ifdef VERSION
static constexpr const char* kVersion = VERSION;
#else
static constexpr const char* kVersion = "0.6.2";
#endif

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About Rakarrack"));
    setFixedSize(420, 340);

    auto* layout = new QVBoxLayout(this);
    layout->setSpacing(8);
    layout->setContentsMargins(20, 16, 20, 16);

    // Title
    auto* title = new QLabel(QStringLiteral("Rakarrack"), this);
    auto titleFont = title->font();
    titleFont.setPointSize(32);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    // Subtitle
    auto* subtitle = new QLabel(QStringLiteral("Audio F/X"), this);
    auto subFont = subtitle->font();
    subFont.setPointSize(14);
    subFont.setBold(true);
    subtitle->setFont(subFont);
    subtitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(subtitle);

    // Version
    auto* version = new QLabel(
        tr("Version %1").arg(QString::fromLatin1(kVersion)), this);
    version->setAlignment(Qt::AlignCenter);
    layout->addWidget(version);

    layout->addSpacing(8);

    // URL
    auto* url = new QLabel(
        QStringLiteral("<a href=\"http://rakarrack.sourceforge.net\">"
                       "http://rakarrack.sourceforge.net</a>"),
        this);
    url->setOpenExternalLinks(true);
    url->setAlignment(Qt::AlignCenter);
    layout->addWidget(url);

    layout->addSpacing(8);

    // Credits
    auto* credits = new QLabel(this);
    credits->setWordWrap(true);
    credits->setAlignment(Qt::AlignCenter);
    credits->setText(
        tr("Copyright Josep Andreu, Ryan Billing,\n"
           "Douglas McClendon, Arnout Engelen (2007-2010)\n\n"
           "Some effects based on ZynAddSubFX by Paul Nasca Octavian\n"
           "Some effects based on swh-plugins LADSPA by Steve Harris\n"
           "Tuner code from tuneit by Mario Lang\n"
           "Compressor based on ArtsCompressor by Matthias Kretz & Stefan Westerfeld"));
    layout->addWidget(credits);

    layout->addStretch();

    // Buttons
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    layout->addWidget(buttons);
}
