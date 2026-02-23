/*
  rakarrack - guitar multi-effects processor
  SPDX-License-Identifier: GPL-2.0-only

  Qt6 GUI entry point.  Mirrors the FLTK main.cpp init sequence:
    1. Parse command line
    2. Construct RKR engine
    3. Start JACK
    4. Optionally start MIDI
    5. Load files from command line
    6. Run QApplication event loop
*/

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QMessageBox>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#ifndef WIN32
#include <sys/mman.h>
#endif

#include "MainWindow.hpp"
#include "config.hpp"
#include "EngineController.hpp"
#include "global.hpp"
#include "jack.hpp"

int main(int argc, char* argv[])
{
    // ── High-DPI support (must be set before QApplication) ─────────
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(PACKAGE));
    app.setApplicationVersion(QStringLiteral(VERSION));

    // Application-wide icon (used by window managers, task bars, etc.)
    QIcon appIcon;
    appIcon.addFile(QStringLiteral(":/icons/rakarrack-32.png"),  QSize(32, 32));
    appIcon.addFile(QStringLiteral(":/icons/rakarrack-64.png"),  QSize(64, 64));
    appIcon.addFile(QStringLiteral(":/icons/rakarrack-128.png"), QSize(128, 128));
    app.setWindowIcon(appIcon);

    fprintf(stderr,
            "\n%s %s - Copyright (c) Josep Andreu - Ryan Billing - "
            "Douglas McClendon - Arnout Engelen\n",
            PACKAGE, VERSION);

    // ── Command-line parsing ───────────────────────────────────────
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Guitar effects processor"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption loadOpt(
        {QStringLiteral("l"), QStringLiteral("load")},
        QStringLiteral("Load preset file"), QStringLiteral("file"));
    QCommandLineOption bankOpt(
        {QStringLiteral("b"), QStringLiteral("bank")},
        QStringLiteral("Load bank file"), QStringLiteral("file"));
    QCommandLineOption presetOpt(
        {QStringLiteral("p"), QStringLiteral("preset")},
        QStringLiteral("Set preset number"), QStringLiteral("number"));
    QCommandLineOption noguiOpt(
        {QStringLiteral("n"), QStringLiteral("no-gui")},
        QStringLiteral("Disable GUI"));
    QCommandLineOption dumpOpt(
        {QStringLiteral("x"), QStringLiteral("dump-preset-names")},
        QStringLiteral("Print preset names and exit"));

    parser.addOption(loadOpt);
    parser.addOption(bankOpt);
    parser.addOption(presetOpt);
    parser.addOption(noguiOpt);
    parser.addOption(dumpOpt);

    parser.process(app);

    // Map to legacy globals (engine still uses them)
    Pexitprogram = 0;
    preset       = 1000;
    commandline  = 0;
    gui          = parser.isSet(noguiOpt) ? 0 : 1;
    needtoloadbank  = 0;
    needtoloadstate = 0;

    int needtoloadfile = 0;

    if (parser.isSet(loadOpt))
    {
        commandline    = 1;
        needtoloadfile = 1;
        filetoload     = strdup(parser.value(loadOpt).toLocal8Bit().constData());
    }
    if (parser.isSet(bankOpt))
    {
        needtoloadbank = 1;
        banktoload     = strdup(parser.value(bankOpt).toLocal8Bit().constData());
    }
    if (parser.isSet(presetOpt))
    {
        preset = parser.value(presetOpt).toInt();
    }

    // ── Engine init ────────────────────────────────────────────────
    RKR rkr;

    if (nojack)
    {
        if (gui)
            QMessageBox::critical(
                nullptr, QStringLiteral("Rakarrack Error"),
                QStringLiteral("Cannot create JACK client. Is jackd running?"));
        return 1;
    }

    if (parser.isSet(dumpOpt))
    {
        rkr.dump_preset_names();
        return 0;
    }

    JACKstart(&rkr, rkr.jack.client);
#ifdef ENABLE_MIDI
    rkr.InitMIDI();
    rkr.ConnectMIDI();
#endif

    if (needtoloadfile)  rkr.loadfile(filetoload);
    if (needtoloadbank)  rkr.loadbank(banktoload);

    // ── EngineController bridge ────────────────────────────────────
    EngineController controller(rkr);

    // ── GUI or headless ────────────────────────────────────────────
    if (gui)
    {
        MainWindow window(controller);
        window.show();

#ifndef WIN32
        mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

        app.exec();

#ifndef WIN32
        munlockall();
#endif
    }
    else
    {
        // Headless mode (no GUI)
        rkr.Bypass = 1;
        rkr.calculavol(1);
        rkr.calculavol(2);
        rkr.booster = 1.0f;

#ifndef WIN32
        mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

        while (Pexitprogram == 0)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1500));
            if (preset != 1000)
            {
                if (preset > 0 && preset < 61)
                    rkr.Bank_to_Preset(preset);
                preset = 1000;
            }

            if (!rkr.jdis && rkr.jshut)
            {
                rkr.jdis = 1;
                rkr.Message(1, rkr.jack.name.data(),
                            "Jack Shut Down, try to save your work");
            }

#ifdef ENABLE_MIDI
            rkr.miramidi();
#endif
        }

#ifndef WIN32
        munlockall();
#endif
    }

    JACKfinish();
    return 0;
}
