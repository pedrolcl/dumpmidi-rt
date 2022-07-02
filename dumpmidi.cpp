/*
    Multiplatform Command Line MIDI Monitor program using Drumstick::RT
    Copyright (C) 2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <iomanip>
#include <csignal>

#include <QObject>
#include <QCoreApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include <drumstick/backendmanager.h>
#include <drumstick/rtmidiinput.h>

class DumpMIDI : public QObject
{
    Q_OBJECT
public:    
    DumpMIDI(QObject *parent = nullptr): QObject(parent) {}

public slots:
    void noteOn(int chan, int note, int vel) {
        std::cout << std::setw(23) << std::left << "note on";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(3) << note << " ";
        std::cout << std::setw(3) << vel << std::endl;
    }
    void noteOff(int chan, int note, int vel) {
        std::cout << std::setw(23) << std::left << "note off";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(3) << note << " ";
        std::cout << std::setw(3) << vel << std::endl;
    }
    void keyPressure(const int chan, const int note, const int value) {
        std::cout << std::setw(23) << std::left << "polyphonic aftertouch";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(3) << note << " ";
        std::cout << std::setw(3) << value << std::endl;
    }
    void controller(const int chan, const int control, const int value) {
        std::cout << std::setw(23) << std::left << "control change";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(3) << control << " ";
        std::cout << std::setw(3) << value << std::endl;
    }
    void program(const int chan, const int program) {
        std::cout << std::setw(23) << std::left << "program change";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(3) << program << std::endl;
    }
    void channelPressure(const int chan, const int value) {
        std::cout << std::setw(23) << std::left << "channel aftertouch";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(3) << value << std::endl;
    }
    void pitchBend(const int chan, const int value) {
        std::cout << std::setw(23) << std::left << "pitch bend";
        std::cout << std::setw(2) << std::right << chan << " ";
        std::cout << std::setw(5) << value << std::endl;
    }
    void sysex(const QByteArray &data) {
        std::cout << std::setw(26) << std::left << "system exclusive" << std::setw(0);
        for (int i = 0; i < data.size(); ++i) {
            unsigned int digits = data[i] & 0xff;
            std::cout << std::hex << digits << " ";
        }
        std::cout << std::dec << std::endl;
    }
    void systemCommon(const int status) {
        std::cout << "system common: " << std::hex << status << std::dec << std::endl;
    }
    void systemRealtime(const int status) {
        std::cout << "system realtime: " << std::hex << status << std::dec << std::endl;
    }
};

void signalHandler(int sig)
{
    if (sig == SIGINT)
        std::cerr << "Received a SIGINT. Exiting" << std::endl;
    else if (sig == SIGTERM)
        std::cerr << "Received a SIGTERM. Exiting" << std::endl;
    qApp->quit();
}

int main(int argc, char **argv) {
    const QString PGM_NAME = QStringLiteral("dumpmidi-rt");
    const QString PGM_DESCRIPTION = QStringLiteral("Drumstick command line utility for decoding MIDI events");
    const QString DEFAULT_DRIVER = 
#if defined(Q_OS_LINUX)
        QStringLiteral("ALSA");
#elif defined(Q_OS_WINDOWS)
        QStringLiteral("Windows MM");
#elif defined(Q_OS_MACOS)
        QStringLiteral("CoreMIDI");
#elif defined(Q_OS_UNIX)
        QStringLiteral("OSS");
#else
        QStringLiteral("Network");
#endif
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(PGM_NAME);
    QCoreApplication::setApplicationVersion(QStringLiteral(QT_STRINGIFY(VERSION)));
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    QCommandLineParser parser;
    parser.setApplicationDescription(PGM_DESCRIPTION);
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();
    QCommandLineOption driverOption({"d", "driver"}, "MIDI Driver.", "driver");
    parser.addOption(driverOption);
    QCommandLineOption portOption({"p", "port"}, "MIDI Port.", "port");
    parser.addOption(portOption);
    parser.process(app);
    if (parser.isSet(versionOption) || parser.isSet(helpOption)) {
        return 0;
    }    
    drumstick::rt::BackendManager man;
    QString driverName = DEFAULT_DRIVER;
    if (parser.isSet(driverOption)) {
        driverName = parser.value(driverOption);
    }
    drumstick::rt::MIDIInput* input = man.inputBackendByName(driverName);
    DumpMIDI dmp;
    if (input == nullptr) {
        std::cerr << "Error: input driver (" << driverName.toStdString() << ") not found." << std::endl;
        std::cerr << "You may need to set the environment variable DRUMSTICKRT" << std::endl;
        return EXIT_FAILURE;
    } else {
        QObject::connect(input, &drumstick::rt::MIDIInput::midiNoteOn, &dmp, &DumpMIDI::noteOn);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiNoteOff, &dmp, &DumpMIDI::noteOff);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiKeyPressure, &dmp, &DumpMIDI::keyPressure);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiController, &dmp, &DumpMIDI::controller);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiProgram, &dmp, &DumpMIDI::program);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiChannelPressure, &dmp, &DumpMIDI::channelPressure);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiPitchBend, &dmp, &DumpMIDI::pitchBend);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiSysex, &dmp, &DumpMIDI::sysex);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiSystemCommon, &dmp, &DumpMIDI::systemCommon);
        QObject::connect(input, &drumstick::rt::MIDIInput::midiSystemRealtime, &dmp, &DumpMIDI::systemRealtime);
        drumstick::rt::MIDIConnection conn;
        if (parser.isSet(portOption)) {
            QString portName = parser.value(portOption);
            auto avail = input->connections(true);
            auto it = std::find_if(avail.begin(), avail.end(), 
                [portName](drumstick::rt::MIDIConnection m) { return m.first == portName; }
            );
            if (it != avail.end()) {
                conn = *it;
            } else {
                std::cerr << "Port " << portName.toStdString() << " not available." << std::endl;
                conn = avail.first();
            }
        }
        input->open(conn);
        std::cout << "driver: " << input->backendName().toStdString() << std::endl;
        std::cout << "port: " << conn.first.toStdString() << std::endl;
        std::cout << "Press ^C to stop the program..." << std::endl;
        std::cout << "Event_________________ Ch _Data__" << std::endl;
        std::cout.flush();
    }
    return app.exec();
}

#include "dumpmidi.moc"
