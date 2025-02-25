#include "workmode-default.h"
#include "qdebug.h"

namespace {
const QStringList emissionStrings{"F1B", "F7B", "G1B", "J3E", "B8E", "R3E", "H3E", "F3EJ", "F3EA", "A1A", "A3E", "A2A"};

}

WorkModeDefault getDefault(EmissionType emissionClass)
{
    WorkModeDefault result;
    switch (emissionClass) {
    case EmissionType::F1B:
        return {emissionClass, {6, 75, 100, 125, 200, 250, 500, 1000}, {50, 100, 150, 300, 500}};
    case EmissionType::F7B:

        return {emissionClass, {125, 200, 250, 400, 500, 1000}, {50, 100, 150, 200, 300, 500}};
    case EmissionType::G1B:
        return {emissionClass, {}, {100, 300, 500, 600, 1200, 2400, 4800, 9600, 16000}};
    case EmissionType::J3E:
        return {emissionClass, {}, {}};
    case EmissionType::B8E:
        return {emissionClass, {}, {}};
    case EmissionType::R3E:
        return {emissionClass, {}, {}};
    case EmissionType::H3E:
        return {emissionClass, {}, {}};
    case EmissionType::F3EJ:
        return {emissionClass, {}, {}};
    case EmissionType::F3EA:
        return {emissionClass, {}, {}};
    case EmissionType::A1A:
        return {emissionClass, {160, 300, 600, 1200, 2200}, {}};
    case EmissionType::A3E:
        return {emissionClass, {}, {}};
    case EmissionType::A2A:
        return {emissionClass, {}, {}};
        break;
    }
    return {};
}

QString emissionToString(const EmissionType emission)
{
    return emissionStrings[static_cast<int>(emission)];
}

EmissionType stringToEmission(const QString& str)
{
    auto i = emissionStrings.indexOf(str);
    if (i == -1) {
        qWarning() << "invalid emission type string" << str;
        return {};
    }
    return static_cast<EmissionType>(i);
}

QStringList emissionTextList()
{
    return emissionStrings;
}
