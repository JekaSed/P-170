#include "workmode-default.h"

WorkModeDefault getDefault(EmissionType emissionClass)
{
    WorkModeData result;
    switch (emissionClass) {
    case EmissionType::F1B:
        return {emissionClass, {6, 75, 100, 125, 200, 250, 500, 1000}, {4, 50, 100, 150, 300, 500}};
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
        return {emissionClass, {}, {}};
    case EmissionType::A3E:
        return {emissionClass, {}, {}};
    case EmissionType::A2A:
        return {emissionClass, {}, {}};
        break;
    }
    return {};
}
