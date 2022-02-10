#ifndef _MOVEINFO_H_
#define _MOVEINFO_H_

#include "Data.h"
#include "Utils.h"
struct MoveInfo {
    NEIGHBORHOOD neighborhood;
    int route_1;
    int route_2;
    int route_1_pos_1;
    int route_1_pos_2;
    int route_2_pos_1;
    int route_2_pos_2;

    MoveInfo() = default;

    MoveInfo(NEIGHBORHOOD neighborhood) : neighborhood(neighborhood) {}

    MoveInfo(
        NEIGHBORHOOD neighborhood, int route_1,
        int route_2, int route_1_pos_1,
        int route_1_pos_2, int route_2_pos_1,
        int route_2_pos_2) : neighborhood(neighborhood),
                             route_1(route_1),
                             route_2(route_2),
                             route_1_pos_1(route_1_pos_1),
                             route_1_pos_2(route_1_pos_2),
                             route_2_pos_1(route_2_pos_1),
                             route_2_pos_2(route_2_pos_2) {
    }

    // TWO_OPT_INTER MoveInfo
    MoveInfo(
        NEIGHBORHOOD neighborhood, int route_1,
        int route_2, int route_1_pos_1,
        int route_2_pos_1) : neighborhood(neighborhood),
                             route_1(route_1),
                             route_2(route_2),
                             route_1_pos_1(route_1_pos_1),
                             route_2_pos_1(route_2_pos_1) {
    }

    // EXCHANGE_VHC_INTER MoveInfo
    // UNCHANGEABLE MoveDesc in two routes
    MoveInfo(
        NEIGHBORHOOD neighborhood, int route_1,
        int route_2) : neighborhood(neighborhood),
                       route_1(route_1),
                       route_2(route_2),
                       route_1_pos_1(0),
                       route_2_pos_1(0) {
    }

    // RELOCATE_INTRA MoveInfo
    MoveInfo(
        NEIGHBORHOOD neighborhood, int route_1,
        int route_1_pos_1, int route_1_pos_2,
        int route_2_pos_1, int route_2_pos_2) : neighborhood(neighborhood),
                                                route_1(route_1),
                                                route_1_pos_1(route_1_pos_1),
                                                route_1_pos_2(route_1_pos_2),
                                                route_2_pos_1(route_2_pos_1),
                                                route_2_pos_2(route_2_pos_2) {
    }

    // EXCHANGE_INTRA MoveInfo
    MoveInfo(
        NEIGHBORHOOD neighborhood, int route_1,
        int route_1_pos_1, int route_1_pos_2) : neighborhood(neighborhood),
                                                route_1(route_1),
                                                route_1_pos_1(route_1_pos_1),
                                                route_1_pos_2(route_1_pos_2) {
    }

    MoveInfo(NEIGHBORHOOD neighborhood,
             MoveInfo move_info_1, MoveInfo move_info_2) : neighborhood(neighborhood),
                                                           route_1(move_info_1.route_1),
                                                           route_2(move_info_2.route_1),
                                                           route_1_pos_1(move_info_1.route_1_pos_1),
                                                           route_1_pos_2(move_info_1.route_1_pos_2),
                                                           route_2_pos_1(move_info_2.route_1_pos_1),
                                                           route_2_pos_2(move_info_2.route_1_pos_2) {
    }

    void setRoutes(int r1, int r2) {
        route_1 = r1;
        route_2 = r2;
    }

    void setRoute(int r) {
        route_1 = r;
    }

    MoveInfo invertedMoveInfo() {
        return MoveInfo(neighborhood,
                        route_2, route_1,
                        route_2_pos_1, route_2_pos_2,
                        route_1_pos_1, route_1_pos_2);
    }

    bool operator!=(const MoveInfo move_info) const {
        return !(neighborhood == move_info.neighborhood &&
                 route_1 == move_info.route_1 &&
                 route_2 == move_info.route_2 &&
                 route_1_pos_1 == move_info.route_1_pos_1 &&
                 route_1_pos_2 == move_info.route_1_pos_2 &&
                 route_2_pos_1 == move_info.route_2_pos_1 &&
                 route_2_pos_2 == move_info.route_2_pos_2);
    }

    void print() {
        string move_name;
        if (route_2 == INFEASIBLE) {
            switch (neighborhood) {
                case RELOCATE_INTRA:
                    move_name = "EXCHANGE_INTRA";
                    break;
                case EXCHANGE_INTRA:
                    move_name = "RELOCATE_INTRA";
                    break;
                default:
                    move_name = "ERROR";
                    break;
            }
        } else {
            switch (neighborhood) {
                case EXCHANGE_INTER:
                    move_name = "EXCHANGE_INTER";
                    break;
                case RELOCATE_INTER:
                    move_name = "RELOCATE_INTER";
                    break;
                case TWO_OPT:
                    move_name = "TWO_OPT";
                    break;
                case EXCHANGE_VHC:
                    move_name = "EXCHANGE_VHC";
                    break;
                case EXCHANGE_BLOCK:
                    move_name = "EXCHANGE_BLOCK";
                    break;
                case RELOCATE_BLOCK:
                    move_name = "RELOCATE_BLOCK";
                    break;
                default:
                    move_name = "ERROR";
                    break;
            }
        }

        cout << "\n>>> MOVE INFO: <<< \nMOVE NAME: " << move_name << endl;
        printf("Route 1: %d\nRoute 2: %d\nRoute_1_Pos_1:%d\nRoute_1_Pos_2:%d\nRoute_2_Pos_1:%d\nRoute_2_Pos_2:%d\n",
               route_1, route_2, route_1_pos_1, route_1_pos_2, route_2_pos_1, route_2_pos_2);
    }
};

enum MOVEDESCSTATUS {
    UNVISITED    = -1,
    UNCHANGEABLE = 0,
    CHANGEABLE   = 1
};

struct MoveDesc {
    MOVEDESCSTATUS status;  //-1 not visited, 0 visited but not valid, 1 visited and valid
    MoveInfo moveInfo;
    MoveDesc() : status(UNVISITED), moveInfo(MoveInfo()) {}
    MoveDesc(MOVEDESCSTATUS status, MoveInfo moveInfo) : status(status), moveInfo(moveInfo) {}
};

#endif